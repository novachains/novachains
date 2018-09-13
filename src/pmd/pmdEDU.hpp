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

   Source File Name = pmdEDU.hpp

   Descriptive Name = Process MoDel Engine Dispatchable Unit Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure for EDU Control
   Block.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          13/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMDEDU_HPP__
#define PMDEDU_HPP__

#include "pmdInterface.hpp"
#include "ossQueue.hpp"
#include "pmdDef.hpp"

namespace engine
{
   /*
      CONST VALUE DEFINE
   */
   #define PMD_EDU_NAME_LENGTH         ( 512 )
   #define PMD_EDU_ERROR_BUFF_SIZE     ( 1024 )

   /*
      EDU CONTROL FLAG DEFINE
   */
   #define PMD_EDU_CTRL_INTERRUPTED          ( 0x01 )
   #define PMD_EDU_CTRL_DISCONNECTED         ( 0x02 )
   #define PMD_EDU_CTRL_FORCED               ( 0x04 )

   /*
      PMD_EDU_INFO_TYPE DEFINE
   */
   enum PMD_EDU_INFO_TYPE
   {
      PMD_EDU_INFO_ERROR      = 1   //Error
   } ;

   /*
      Tool Functions Define
   */
   const CHAR *pmdGetEDUStatusDesp ( PMD_EDU_STATUS status ) ;

   /*
      Declare
   */
   class _pmdEDUMgr ;

   /*
      _pmdEDUCB define
   */
   class _pmdEDUCB : public IPmdExecutor, public SDBObject
   {
      friend class _pmdEDUMgr ;

      public:
         typedef multimap<UINT32,CHAR*>         CATCH_MAP ;
         typedef CATCH_MAP::iterator            CATCH_MAP_IT ;

         typedef map<CHAR*,UINT32>              ALLOC_MAP ;
         typedef ALLOC_MAP::iterator            ALLOC_MAP_IT ;

      public:
         _pmdEDUCB( _pmdEDUMgr *mgr, INT32 type ) ;
         virtual ~_pmdEDUCB() ;

      public:
         /*
            Base Function
         */
         virtual EDUID        getID() const { return _eduID ; }
         virtual UINT32       getTID() const { return _tid ; }
         virtual const CHAR*  getName () const { return _name ; } ;
         virtual INT32        getType () const { return _eduType ; }

         /*
            Session Related
         */
         virtual IPmdSession* getSession() { return _pSession ; }

         /*
            Status and Control
         */
         virtual BOOLEAN   isInterrupted () ;
         virtual BOOLEAN   isDisconnected () ;
         virtual BOOLEAN   isForced () ;

         virtual UINT32    getProcessedNum() const { return _processEventCount ; }
         virtual void      incEventCount( UINT32 step = 1 ) ;

         virtual UINT32    getQueSize() ;

         virtual void      resetForLoopOnce() ;

         /*
            Buffer Manager
         */
         virtual INT32     allocBuff( UINT32 len,
                                      CHAR **ppBuff,
                                      UINT32 *pRealSize = NULL ) ;

         virtual INT32     reallocBuff( UINT32 len,
                                        CHAR **ppBuff,
                                        UINT32 *pRealSize = NULL ) ;

         virtual void      releaseBuff( CHAR *pBuff ) ;

         virtual void*     getAlignedBuff( UINT32 size,
                                           UINT32 *pRealSize = NULL,
                                           UINT32 alignment =
                                           OSS_FILE_DIRECT_IO_ALIGNMENT ) ;

         virtual void      releaseAlignedBuff() ;

         /*
            Que functions
         */
         virtual BOOLEAN   waitEvent( pmdEDUEvent &data,
                                      INT64 millsec,
                                      BOOLEAN resetStat = FALSE ) ;

         virtual BOOLEAN   waitEvent( pmdEDUEventTypes type,
                                      pmdEDUEvent &data,
                                      INT64 millsec,
                                      BOOLEAN resetStat = FALSE ) ;

         virtual void      postEvent ( pmdEDUEvent const &data ) ;

         /*
            Mon stat
         */
         virtual void      resetMon() ;

      /// Other functions
      public:
         void              clear() ;
         string            toString() const ;
         PMD_EDU_STATUS    getStatus () const { return _status ; }
         BOOLEAN           isLocked() const { return _isLocked ; }

         _pmdEDUMgr*       getEDUMgr() { return _eduMgr ; }

      #if defined ( _LINUX )
         pthread_t         getThreadID () const { return _threadID ; }
         OSSTID            getThreadHandle() const { return _threadHdl ; }
      #elif defined ( _WINDOWS )
         HANDLE            getThreadHandle() const { return _threadHdl ; }
      #endif

      public :
         void        attachSession( IPmdSession *pSession ) ;
         void        detachSession() ;

         void        restoreBuffs( CATCH_MAP &catchMap ) ;
         void        saveBuffs( CATCH_MAP &catchMap ) ;

         void        interrupt () ;

         INT32       printInfo ( PMD_EDU_INFO_TYPE type, const CHAR *format, ... ) ;
         const CHAR* getInfo ( PMD_EDU_INFO_TYPE type ) ;
         void        resetInfo ( PMD_EDU_INFO_TYPE type ) ;
         void        resetInterrupt () ;
         void        resetDisconnect () ;

         void        setName ( const CHAR *name ) ;

      protected:
         void     setStatus ( PMD_EDU_STATUS status ) { _status = status ; }
         void     setID ( EDUID id ) { _eduID = id ; }
         void     setLock( BOOLEAN lock ) { _isLocked = lock ; }

         CHAR*    _getBuffInfo ( PMD_EDU_INFO_TYPE type, UINT32 &size ) ;
         BOOLEAN  _allocFromCatch( UINT32 len, CHAR **ppBuff, UINT32 *buffLen ) ;

         void     disconnect () ;
         void     force () ;

      private:
         void        setType( INT32 type ) ;
         void        setTID ( UINT32 tid ) { _tid = tid ; }

      #if defined ( _LINUX )
         void        setThreadID ( pthread_t id ) { _threadID = id ; }
         void        setThreadHdl( OSSTID hdl ) { _threadHdl = hdl ; }
      #elif defined ( _WINDOWS )
         void        setThreadHdl( HANDLE hdl ) { _threadHdl = hdl ; }
      #endif

      private :
         _pmdEDUMgr              *_eduMgr ;
         ossQueue<pmdEDUEvent>   _queue ;

         PMD_EDU_STATUS          _status ;
         INT32                   _eduType ;
         BOOLEAN                 _isLocked ;

         // buffer related
         CATCH_MAP               _catchMap ;
         ALLOC_MAP               _allocMap ;
         INT64                   _totalCatchSize ;
         INT64                   _totalMemSize ;

         // thread specific error message buffer, aka SQLCA
         CHAR                    *_pErrorBuff ;
      #if defined ( _WINDOWS )
         HANDLE                  _threadHdl ;
      #elif defined ( _LINUX )
         OSSTID                  _threadHdl ;
         pthread_t               _threadID ;
      #endif // _WINDOWS

         CHAR                    _name[ PMD_EDU_NAME_LENGTH + 1 ] ;

         /*
            Interace members
         */
         EDUID                   _eduID ;
         UINT32                  _tid ;
         IPmdSession             *_pSession ;

         UINT64                  _processEventCount ;

         INT32                   _ctrlFlag ;
         /// aligned memory.
         void                    *_alignedMem ;
         UINT32                   _alignedMemSize ;

   } ;
   typedef _pmdEDUCB pmdEDUCB ;

   /*
      Other functions
   */
   _pmdEDUCB*  pmdGetThreadEDUCB() ;
   _pmdEDUCB*  pmdDeclareEDUCB( _pmdEDUCB *p ) ;
   void        pmdUndeclareEDUCB() ;

   void        pmdEduEventRelase( pmdEDUEvent &event, _pmdEDUCB *cb ) ;

}

#endif // PMDEDU_HPP__

