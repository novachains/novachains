/*******************************************************************************


   Copyright (C) 2011-2019 NOVACHAIN Ltd.

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

   Source File Name = dpsLogMgr.hpp

   Descriptive Name = Data Protection Services Replica Log Manager Header

   When/how to use: this program may be used on binary and text-formatted
   versions of DPS component. This file contains code logic for replica manager

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          08/02/2019  jiaqi Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef DPSREPLICALOGMGR_H_
#define DPSREPLICALOGMGR_H_

#include "core.hpp"
#include "oss.hpp"
#include "dpsLogPage.hpp"
#include "ossLatch.hpp"
#include "dpsMergeBlock.hpp"
#include "ossAtomic.hpp"
#include "dpsLogFileMgr.hpp"
#include "ossUtil.hpp"
#include "ossEvent.hpp"
#include "ossQueue.hpp"

#include <vector>
using namespace std ;

namespace engine
{

   // we ALWAYS search for MEM first, because we may have LSN stay in buffer
   // but not on disk
   #define  DPS_SEARCH_MEM       0x01
   // indicating also search in file
   #define  DPS_SEARCH_FILE      0x10
   #define  DPS_SEARCH_ALL       (DPS_SEARCH_MEM|DPS_SEARCH_FILE)

   class _pmdEDUCB ;

   /*
      _dpsLogMgr define
   */
   class _dpsLogMgr : public SDBObject
   {
   private:
      ossQueue<_dpsLogPage *>    _queue;
      _dpsLogFileMgr             _logger;
      _dpsLogPage                *_pages;
      _ossSpinXLatch             _mtx ;
      _ossSpinXLatch             _writeMutex ;
      _ossAtomic32               _idleSize;
      UINT32                     _totalSize;
      UINT64                     _offset ;
      UINT32                     _work;
      UINT32                     _begin ;
      BOOLEAN                    _rollFlag ;
      UINT32                     _pageNum;
      BOOLEAN                    _restoreFlag ;
      ossAutoEvent               _allocateEvent ;
      _ossAtomic32               _queSize ;

      BOOLEAN                    _incVersion ;

      UINT64                     _pageFlushCount ;

   public:
      _dpsLogMgr();

      ~_dpsLogMgr();

   public:
      OSS_INLINE UINT32 idleSize()
      {
         return _idleSize.peek();
      }

   public:
      INT32 init( const CHAR *path, UINT32 pageNum );

      INT32 merge( _dpsMergeBlock &block ) ;

      // first step: allocate pages and product lsn
      INT32 preparePages ( dpsMergeInfo &info ) ;
      // secondary step: write data to pages
      void  writeData ( dpsMergeInfo &info ) ;

      INT32 run( _pmdEDUCB *cb ) ;

      INT32 tearDown();
      INT32 flushAll() ;

      void setLogFileSz ( UINT64 logFileSz )
      {
         _logger.setLogFileSz ( logFileSz ) ;
      }
      UINT32 getLogFileSz ()
      {
         return _logger.getLogFileSz () ;
      }
      void setLogFileNum ( UINT32 logFileNum )
      {
         _logger.setLogFileNum ( logFileNum ) ;
      }
      UINT32 getLogFileNum ()
      {
         return _logger.getLogFileNum () ;
      }

      BOOLEAN isInRestore ()
      {
         return _restoreFlag;
      }

      _dpsLogFile* getLogFile( UINT32 fileId )
      {
         return _logger.getLogFile( fileId ) ;
      }

      UINT32 calcFileID ( DPS_LSN_OFFSET offset )
      {
         return ( offset / getLogFileSz () ) % getLogFileNum () ;
      }

      UINT32 calcLogicalFileID( DPS_LSN_OFFSET offset )
      {
         return ( offset / getLogFileSz () ) ;
      }

      BOOLEAN isFirstPhysicalLSNOfFile( DPS_LSN_OFFSET offset )
      {
         return ( ( offset % getLogFileSz () ) == 0 ) ? TRUE : FALSE ;
      }

      DPS_LSN_OFFSET calcFirstPhysicalLSNOfFile( UINT32 logicalFileId )
      {
         // file id start from 0
         return ((UINT64)logicalFileId) * getLogFileSz () ;
      }

   private:
      void _allocate( UINT32 len,
                      dpsPageMeta &allocated ) ;
      void _push2SendQueue( const dpsPageMeta &allocated );
      void _mergeLogs( _dpsMergeBlock &block,
                       const dpsPageMeta &meta ) ;
      void _mergePage( const CHAR *src,
                       UINT32 len,
                       UINT32 &workSub,
                       UINT32 &offset );
      INT32 _flushPage( _dpsLogPage *page, BOOLEAN shutdown = FALSE );
      INT32 _flushAll() ;

      UINT32 _decPageID ( UINT32 pageID )
      {
         return pageID ? pageID - 1 : _pageNum - 1 ;
      }
      UINT32 _incPageID ( UINT32 pageID )
      {
         ++pageID ;
         return pageID >= _pageNum ? 0 : pageID ;
      }

      void _flushOldestTransBeginLSN() ;
   };
   typedef class _dpsLogMgr dpsLogMgr;
}

#endif //DPSREPLICALOGMGR_H_

