/*******************************************************************************


   Copyright (C) 2011-2018 SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = pmdEDU.cpp

   Descriptive Name = Process MoDel Agent Engine Dispatchable Unit

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains functions for EDU processing.
   EDU thread is a wrapper of all threads. It will call each entry function
   depends on the EDU type.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          13/08/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#include "pmdEDU.hpp"
#include "pmdEPFactory.hpp"
#include "pmdTool.hpp"
#include "pd.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"

namespace engine
{
   const UINT32 EDU_MEM_ALIGMENT_SIZE  = 1024 ; // must for times for 4
   const UINT32 EDU_MAX_CATCH_SIZE     = 16*1024*1024 ;

   /*
      TOOL FUNCTIONS
   */
   const CHAR * pmdGetEDUStatusDesp ( PMD_EDU_STATUS status )
   {
      const CHAR *desp = "Unknown" ;

      switch ( status )
      {
         case PMD_EDU_CREATING :
            desp = "Creating" ;
            break ;
         case PMD_EDU_RUNNING :
            desp = "Running" ;
            break ;
         case PMD_EDU_WAITING :
            desp = "Waiting" ;
            break ;
         case PMD_EDU_IDLE :
            desp = "Idle" ;
            break ;
         default :
            break ;
      }

      return desp ;
   }

   /*
      _pmdEDUCB implement
   */
   _pmdEDUCB::_pmdEDUCB( _pmdEDUMgr *mgr, INT32 type )
   {
      _eduMgr           = mgr ;
      _eduID            = PMD_INVALID_EDUID ;
      _tid              = 0 ;
      _status           = PMD_EDU_UNKNOW ;
      _eduType          = type ;
      _isLocked         = FALSE ;
      _ctrlFlag         = 0 ;
      _processEventCount= 0 ;
      ossMemset( _name, 0, sizeof( _name ) ) ;
      _threadHdl        = 0 ;
      _pSession         = NULL ;
      _totalCatchSize   = 0 ;
      _totalMemSize     = 0 ;

      _alignedMem       = NULL ;
      _alignedMemSize   = 0 ;

#if defined (_LINUX)
      _threadID         = 0 ;
#endif // _LINUX

      _pErrorBuff = (CHAR *)SDB_OSS_MALLOC( PMD_EDU_ERROR_BUFF_SIZE + 1 ) ;
      if ( _pErrorBuff )
      {
         ossMemset( _pErrorBuff, 0, PMD_EDU_ERROR_BUFF_SIZE + 1 ) ;
      }
   }

   _pmdEDUCB::~_pmdEDUCB ()
   {
      if ( _pErrorBuff )
      {
         SDB_OSS_FREE ( _pErrorBuff ) ;
         _pErrorBuff = NULL ;
      }

      clear() ;
   }

   void _pmdEDUCB::clear()
   {
      // clear all queue msg
      pmdEDUEvent data ;
      while ( _queue.try_pop( data ) )
      {
         pmdEduEventRelase( data, this ) ;
      }
      _processEventCount = 0 ;
      ossMemset( _name, 0, sizeof( _name ) ) ;
      _isLocked = FALSE ;

      _ctrlFlag = 0 ;
      releaseAlignedBuff() ;

      // clean catch
      CATCH_MAP_IT it = _catchMap.begin() ;
      while ( it != _catchMap.end() )
      {
         SDB_OSS_FREE( it->second ) ;
         _totalCatchSize -= it->first ;
         _totalMemSize -= it->first ;
         ++it ;
      }
      _catchMap.clear() ;

      // clean alloc memory
      ALLOC_MAP_IT itAlloc = _allocMap.begin() ;
      while ( itAlloc != _allocMap.end() )
      {
         SDB_OSS_FREE( itAlloc->first ) ;
         _totalMemSize -= itAlloc->second ;
         ++itAlloc ;
      }
      _allocMap.clear() ;

      SDB_ASSERT( _totalCatchSize == 0 , "Catch size is error" ) ;
      SDB_ASSERT( _totalMemSize == 0, "Memory size is error" ) ;
   }

   string _pmdEDUCB::toString() const
   {
      std::stringstream ss ;
      ss << "ID: " << _eduID << ", Type: " << _eduType << "["
         << getEDUName( _eduType ) << "], TID: " << _tid ;

      if ( _pSession )
      {
         ss << ", Session: " << _pSession->sessionName() ;
      }

      return ss.str() ;
   }

   void _pmdEDUCB::attachSession( IPmdSession *pSession )
   {
      _pSession = pSession ;
   }

   void _pmdEDUCB::detachSession()
   {
      _pSession = NULL ;
   }

   void _pmdEDUCB::setType ( INT32 type )
   {
      _eduType = type ;
   }

   void _pmdEDUCB::interrupt()
   {
      _ctrlFlag |= PMD_EDU_CTRL_INTERRUPTED ;
   }

   void _pmdEDUCB::disconnect ()
   {
      interrupt () ;
      _ctrlFlag |= PMD_EDU_CTRL_DISCONNECTED ;
      postEvent ( pmdEDUEvent ( PMD_EDU_EVENT_TERM ) ) ;
   }

   void _pmdEDUCB::force ()
   {
      disconnect () ;
      _ctrlFlag |= PMD_EDU_CTRL_FORCED ;
   }

   void _pmdEDUCB::resetInterrupt ()
   {
      _ctrlFlag &= ~PMD_EDU_CTRL_INTERRUPTED ;
   }

   void _pmdEDUCB::resetDisconnect ()
   {
      resetInterrupt () ;
      _ctrlFlag &= ~PMD_EDU_CTRL_DISCONNECTED ;
   }

   void _pmdEDUCB::resetForLoopOnce()
   {
      resetInterrupt() ;
      resetInfo( PMD_EDU_INFO_ERROR ) ;
   }

   void _pmdEDUCB::setName ( const CHAR * name )
   {
      ossStrncpy ( _name, name, PMD_EDU_NAME_LENGTH ) ;
      _name[ PMD_EDU_NAME_LENGTH ] = 0 ;
   }

   void _pmdEDUCB::postEvent ( pmdEDUEvent const &data )
   {
      _queue.push ( data ) ;
   }

   BOOLEAN _pmdEDUCB::waitEvent ( pmdEDUEvent &data,
                                  INT64 millsec,
                                  BOOLEAN resetStat )
   {
      BOOLEAN waitMsg   = FALSE ;
   
      if ( resetStat && PMD_EDU_IDLE != _status )
      {
         _status = PMD_EDU_WAITING ;
      }
   
      if ( 0 > millsec )
      {
         _queue.wait_and_pop ( data ) ;
         waitMsg = TRUE ;
      }
      else
      {
         waitMsg = _queue.timed_wait_and_pop ( data, millsec ) ;
      }
   
      if ( waitMsg )
      {
         ++_processEventCount ;
         if ( data._eventType == PMD_EDU_EVENT_TERM )
         {
            _ctrlFlag |= ( PMD_EDU_CTRL_DISCONNECTED|PMD_EDU_CTRL_INTERRUPTED ) ;
         }
         else if ( resetStat )
         {
            _status = PMD_EDU_RUNNING ;
         }
      }
   
      return waitMsg ;
   }

   BOOLEAN _pmdEDUCB::waitEvent( pmdEDUEventTypes type,
                                 pmdEDUEvent &data,
                                 INT64 millsec,
                                 BOOLEAN resetStat )
   {
      BOOLEAN ret = FALSE ;
      INT64 waitTime = 0 ;
      ossQueue< pmdEDUEvent > tmpQue ;
   
      if ( millsec < 0 )
      {
         millsec = 0x7FFFFFFF ;
      }
   
      while ( !isInterrupted() )
      {
         waitTime = millsec < OSS_ONE_SEC ? millsec : OSS_ONE_SEC ;
         if ( !waitEvent( data, waitTime, resetStat ) )
         {
            millsec -= waitTime ;
            if ( millsec <= 0 )
            {
               break ;
            }
            continue ;
         }
         if ( type != data._eventType )
         {
            tmpQue.push( data ) ;
            --_processEventCount ;
            continue ;
         }
         ret = TRUE ;
         break ;
      }
   
      pmdEDUEvent tmpData ;
      while ( !tmpQue.empty() )
      {
         tmpQue.try_pop( tmpData ) ;
         _queue.push( tmpData ) ;
      }
      return ret ;
   }

   CHAR *_pmdEDUCB::_getBuffInfo ( PMD_EDU_INFO_TYPE type, UINT32 & size )
   {
      CHAR *buff = NULL ;
      switch ( type )
      {
         case PMD_EDU_INFO_ERROR :
            buff = _pErrorBuff ;
            size = PMD_EDU_ERROR_BUFF_SIZE ;
            break ;
         default :
            break ;
      }

      return buff ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDEDUCB_PRINTINFO, "_pmdEDUCB::printInfo" )
   INT32 _pmdEDUCB::printInfo ( PMD_EDU_INFO_TYPE type, const CHAR * format, ... )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__PMDEDUCB_PRINTINFO );

      if ( !getInfo( type ) )
      {
         UINT32 buffSize = 0 ;
         CHAR *buff = _getBuffInfo ( type, buffSize ) ;

         if ( NULL == buff || buffSize == 0 )
         {
            rc = SDB_INVALIDARG ;
            goto error ;
         }

         va_list ap ;
         va_start ( ap, format ) ;
         vsnprintf ( buff, buffSize, format, ap ) ;
         va_end ( ap ) ;

         buff[ buffSize ] = 0 ;
      }

   done :
      PD_TRACE_EXITRC ( SDB__PMDEDUCB_PRINTINFO, rc );
      return rc ;
   error :
      goto done ;
   }

   const CHAR *_pmdEDUCB::getInfo( PMD_EDU_INFO_TYPE type )
   {
      UINT32 buffSize = 0 ;
      CHAR *buff = _getBuffInfo ( type, buffSize ) ;
      if ( buff && buff[0] == '\0' )
      {
         buff = NULL ;
      }
      return buff ;
   }

   void _pmdEDUCB::resetInfo( PMD_EDU_INFO_TYPE type )
   {
      UINT32 buffSize = 0 ;
      CHAR *buff = _getBuffInfo ( type, buffSize ) ;
      if ( buff )
      {
         buff[0] = 0 ;
      }
   }

   BOOLEAN _pmdEDUCB::_allocFromCatch( UINT32 len, CHAR **ppBuff,
                                       UINT32 *buffLen )
   {
      UINT32 tmpLen = 0 ;
      CATCH_MAP_IT it = _catchMap.lower_bound( len ) ;
      if ( it != _catchMap.end() )
      {
         *ppBuff = it->second ;
         tmpLen = it->first ;
         _catchMap.erase( it ) ;
         _allocMap[ *ppBuff ] = tmpLen ;
         _totalCatchSize -= tmpLen ;

         if ( buffLen )
         {
            *buffLen = tmpLen ;
         }
         return TRUE ;
      }
      return FALSE ;
   }

   void _pmdEDUCB::restoreBuffs( _pmdEDUCB::CATCH_MAP &catchMap )
   {
      CATCH_MAP_IT it = catchMap.begin() ;
      while ( it != catchMap.end() )
      {
         _catchMap.insert( std::make_pair( it->first, it->second ) ) ;
         _totalCatchSize += it->first ;
         _totalMemSize += it->first ;
         ++it ;
      }
      catchMap.clear() ;
   }

   void _pmdEDUCB::saveBuffs( _pmdEDUCB::CATCH_MAP &catchMap )
   {
      // clean alloc memory
      CHAR *pBuff = NULL ;
      ALLOC_MAP_IT itAlloc = _allocMap.begin() ;
      while ( itAlloc != _allocMap.end() )
      {
         pBuff = itAlloc->first ;
         ++itAlloc ;
         releaseBuff( pBuff ) ;
      }
      _allocMap.clear() ;

      // restore catch map
      CATCH_MAP_IT it = _catchMap.begin() ;
      while ( it != _catchMap.end() )
      {
         _totalCatchSize -= it->first ;
         _totalMemSize -= it->first ;
         catchMap.insert( std::make_pair( it->first, it->second ) ) ;
         ++it ;
      }
      _catchMap.clear() ;
   }

   /*
      Interface impelent
   */
   void *_pmdEDUCB::getAlignedBuff( UINT32 size,
                                    UINT32 *pRealSize,
                                    UINT32 alignment )
   {
      SDB_ASSERT( alignment == OSS_FILE_DIRECT_IO_ALIGNMENT,
                  "rewrite this function if u want to use new alignment" ) ;
      if ( _alignedMemSize < size )
      {
         if ( NULL != _alignedMem )
         {
            SDB_OSS_ORIGINAL_FREE( _alignedMem ) ;
            _alignedMemSize = 0 ;
            _alignedMem = NULL ; 
         }

         size = ossRoundUpToMultipleX( size, alignment ) ;
         _alignedMem = ossAlignedAlloc( alignment, size ) ;
         if ( NULL != _alignedMem )
         {
            _alignedMemSize = size ;
         }
      }

      if ( pRealSize )
      {
         *pRealSize = _alignedMemSize ;
      }

      return _alignedMem ;
   }

   void _pmdEDUCB::releaseAlignedBuff()
   {
      if ( NULL != _alignedMem )
      {
         SDB_OSS_ORIGINAL_FREE( _alignedMem ) ;
         _alignedMem = NULL ;
         _alignedMemSize = 0 ;
      }
   }

   void _pmdEDUCB::resetMon()
   {
   }

   INT32 _pmdEDUCB::allocBuff( UINT32 len,
                               CHAR **ppBuff,
                               UINT32 *pRealSize )
   {
      INT32 rc = SDB_OK ;

      // first alloc from catch
      if ( _totalCatchSize >= len &&
           _allocFromCatch( len, ppBuff, pRealSize ) )
      {
         goto done ;
      }

      // malloc
      len = ossRoundUpToMultipleX( len, EDU_MEM_ALIGMENT_SIZE ) ;
      *ppBuff = ( CHAR* )SDB_OSS_MALLOC( len ) ;
      if( !*ppBuff )
      {
         rc = SDB_OOM ;
         PD_LOG( PDERROR, "Edu[%s] malloc memory[size: %u] failed",
                 toString().c_str(), len ) ;
         goto error ;
      }

      // update meta info
      _totalMemSize += len ;
      _allocMap[ *ppBuff ] = len ;

      if ( pRealSize )
      {
         *pRealSize = len ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _pmdEDUCB::reallocBuff( UINT32 len,
                                 CHAR **ppBuff,
                                 UINT32 *pRealSize )
   {
      INT32 rc = SDB_OK ;
      CHAR *pOld = *ppBuff ;
      UINT32 oldLen = 0 ;

      ALLOC_MAP_IT itAlloc = _allocMap.find( *ppBuff ) ;
      if ( itAlloc != _allocMap.end() )
      {
         oldLen = itAlloc->second ;
         if ( pRealSize )
         {
            *pRealSize = oldLen ;
         }
      }
      else if ( *ppBuff != NULL )
      {
         PD_LOG( PDERROR, "EDU[%s] realloc input buffer error",
                 toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      if ( oldLen >= len )
      {
         goto done ;
      }
      len = ossRoundUpToMultipleX( len, EDU_MEM_ALIGMENT_SIZE ) ;
      *ppBuff = ( CHAR* )SDB_OSS_REALLOC( *ppBuff, len ) ;
      if ( !*ppBuff )
      {
         rc = SDB_OOM ;
         PD_LOG( PDERROR, "Failed to realloc memory, size: %d", len ) ;
         goto error ;
      }

      if ( pOld != *ppBuff )
      {
         /// the old pointer has release, so need del from map
         _allocMap.erase( pOld ) ;
      }

      // update meta info
      _totalMemSize += ( len - oldLen ) ;

      _allocMap[ *ppBuff ] = len ;

      if ( pRealSize )
      {
         *pRealSize = len ;
      }

   done:
      return rc ;
   error:
      if ( pOld )
      {
         releaseBuff( pOld ) ;
         *ppBuff = NULL ;
         if ( pRealSize )
         {
            *pRealSize = 0 ;
         }
      }
      goto done ;
   }

   void _pmdEDUCB::releaseBuff( CHAR *pBuff )
   {
      ALLOC_MAP_IT itAlloc = _allocMap.find( pBuff ) ;
      if ( itAlloc == _allocMap.end() )
      {
         SDB_OSS_FREE( pBuff ) ;
         return ;
      }
      INT32 buffLen = itAlloc->second ;
      _allocMap.erase( itAlloc ) ;

      if ( (UINT32)buffLen > EDU_MAX_CATCH_SIZE )
      {
         SDB_OSS_FREE( pBuff ) ;
         _totalMemSize -= buffLen ;
      }
      else
      {
         // add to catch
         _catchMap.insert( std::make_pair( buffLen, pBuff ) ) ;
         _totalCatchSize += buffLen ;

         // re-org catch
         while ( _totalCatchSize > EDU_MAX_CATCH_SIZE )
         {
            CATCH_MAP_IT it = _catchMap.begin() ;
            SDB_OSS_FREE( it->second ) ;
            _totalMemSize -= it->first ;
            _totalCatchSize -= it->first ;
            _catchMap.erase( it ) ;
         }
      }
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDEDUCB_ISINT, "_pmdEDUCB::isInterrupted" )
   BOOLEAN _pmdEDUCB::isInterrupted ()
   {
      PD_TRACE_ENTRY ( SDB__PMDEDUCB_ISINT );
      BOOLEAN ret = FALSE ;

      if ( _ctrlFlag & PMD_EDU_CTRL_INTERRUPTED )
      {
         ret = TRUE ;
         goto done ;
      }
      else if ( _pSession && _pSession->getClient() )
      {
         IPmdClient *pClient = _pSession->getClient() ;
         if ( pClient->isClosed() )
         {
            _ctrlFlag |= ( PMD_EDU_CTRL_INTERRUPTED | PMD_EDU_CTRL_DISCONNECTED ) ;
            ret = TRUE ;
         }
         else if ( pClient->isInterrupted() )
         {
            _ctrlFlag |= PMD_EDU_CTRL_INTERRUPTED ;
            ret = TRUE ;
         }
      }

   done :
      PD_TRACE1 ( SDB__PMDEDUCB_ISINT, PD_PACK_INT(ret) );
      PD_TRACE_EXIT ( SDB__PMDEDUCB_ISINT );
      return ret ;
   }

   BOOLEAN _pmdEDUCB::isDisconnected ()
   {
      return ( _ctrlFlag & PMD_EDU_CTRL_DISCONNECTED ) ? TRUE : FALSE ;
   }

   BOOLEAN _pmdEDUCB::isForced ()
   {
      return ( _ctrlFlag & PMD_EDU_CTRL_FORCED ) ? TRUE : FALSE ;
   }

   void _pmdEDUCB::incEventCount( UINT32 step )
   {
      _processEventCount += step ;
   }

   UINT32 _pmdEDUCB::getQueSize()
   {
      return _queue.size() ;
   }

   /*
      Other functions
   */

   static OSS_THREAD_LOCAL _pmdEDUCB *__eduCB ;

   _pmdEDUCB *pmdGetThreadEDUCB ()
   {
      return __eduCB ;
   }

   _pmdEDUCB *pmdDeclareEDUCB ( _pmdEDUCB *p )
   {
      __eduCB = p ;
      return __eduCB ;
   }

   void pmdUndeclareEDUCB ()
   {
      __eduCB = NULL ;
   }

}

