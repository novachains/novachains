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

   Source File Name = pmdEDUMgr.hpp

   Descriptive Name = Process MoDel Engine Dispatchable Unit Manager Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure for EDU Pool, which
   include operations like creating a new EDU, reuse existing EDU, destroy EDU
   and etc...

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/08/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMDEDUMGR_HPP__
#define PMDEDUMGR_HPP__

#include "core.hpp"
#include "oss.hpp"
#include "pmdEDU.hpp"
#include "ossLatch.hpp"
#include "ossEvent.hpp"
#include "ossUtil.hpp"

#include <map>
#include <set>
#include <string>

namespace engine
{

   /*
      Property Define
   */
   #define PMD_EDU_SYSTEM           0x0001
   #define PMD_EDU_USER             0x0002
   #define PMD_EDU_ALL              ( PMD_EDU_SYSTEM | PMD_EDU_USER )

   #define PMD_MAX_POOLED_EDU_DFT   ( 10 )

   typedef boost::shared_ptr<ossEvent>       pmdEventPtr ;

   #define PMD_STOP_TIMEOUT               ( 600000 )  /// 10 mins
   #define PMD_STOP_DEADCHECK_TIMEOUT     ( PMD_STOP_TIMEOUT + 60000 )

   /*
      _pmdEDUMgr define
   */
   class _pmdEDUMgr : public IPmdExecutorMgr, public SDBObject
   {
      typedef vector< IPmdIOService* >    VEC_IOSERVICE ;
      typedef map<EDUID, pmdEDUCB*>       MAP_EDUCB ;
      typedef MAP_EDUCB::iterator         MAP_EDUCB_IT ;
      typedef map<UINT32,EDUID>           MAP_TID2EDU ;
      typedef MAP_TID2EDU::iterator       MAP_TID2EDU_IT ;
      typedef map<INT32,set<EDUID> >      MAP_SYSTEMEDU ;
      typedef MAP_SYSTEMEDU::iterator     MAP_SYSTEMEDU_IT ;

      public:
         _pmdEDUMgr () ;
         virtual ~_pmdEDUMgr () ;

         virtual INT32     startEDU( INT32 type,
                                     void *args,
                                     EDUID *pEDUID = NULL,
                                     const CHAR *pInitName = "" ) ;

         virtual INT32     forkEDU( void *args,
                                    EDUID *pEDUID = NULL,
                                    const CHAR *pInitName = "" ) ;

         virtual void      addIOService( IPmdIOService *pIOService ) ;
         virtual void      delIOSerivce( IPmdIOService *pIOService ) ;

         /*
            EDU Interfaces
         */
         virtual INT32     activateEDU( EDUID eduID ) ;
         virtual INT32     activateEDU( IPmdExecutor *cb ) ;

         virtual INT32     waitEDU( EDUID eduID ) ;
         virtual INT32     waitEDU( IPmdExecutor *cb ) ;

         virtual void      lockEDU( IPmdExecutor *cb ) ;
         virtual void      unlockEDU( IPmdExecutor *cb ) ;

         virtual INT32     postEDUPost ( EDUID eduID,
                                         pmdEDUEventTypes type,
                                         pmdEDUMemTypes dataMemType = PMD_EDU_MEM_NONE,
                                         void *pData = NULL,
                                         UINT64 usrData = 0 ) ;

         virtual INT32     forceUserEDU( EDUID eduID ) ;
         virtual INT32     interruptUserEDU( EDUID eduID ) ;
         virtual INT32     disconnectUserEDU( EDUID eduID ) ;

         /*
            Mon stat
         */
         virtual void      resetMon( EDUID eduID = PMD_INVALID_EDUID ) ;
         virtual void      resetIOService() ;

      public:
         INT32             init( IPmdResource *pResource,
                                 UINT32 maxPooledEDU = PMD_MAX_POOLED_EDU_DFT ) ;
         BOOLEAN           fini( INT64 timeout = PMD_STOP_TIMEOUT ) ;

         void              setMaxPooledEDU( UINT32 maxPooledEDU ) ;

         UINT32            dumpAbnormalEDU() ;
         INT32             startDeadCheck( INT64 timeout = PMD_STOP_DEADCHECK_TIMEOUT ) ;
         void              stopDeadCheck() ;

         UINT32            countIOService() ;
         UINT32            size() ;
         UINT32            sizeRun() ;
         UINT32            sizeIdle() ;
         UINT32            sizeSystem() ;
         UINT32            sizeByType( INT32 type ) ;
         UINT32            sizeWithSession() ;
         void              sizeInfo( UINT32 &runSize,
                                     UINT32 &idleSize,
                                     UINT32 &sysSize ) ;

         EDUID             getFirstSystemEDU( INT32 eduType ) ;
         set<EDUID>        getSystemEDU( INT32 eduType ) ;
         BOOLEAN           isSystemEDU( EDUID eduID ) ;

         BOOLEAN           isQuiesced() ;
         BOOLEAN           isDestroyed() ;

         pmdEDUCB*         getEDUByID( EDUID eduID ) ;
         INT32             getEDUTypeByID( EDUID eduID ) ;
         pmdEDUCB*         getEDU() ;
         pmdEDUCB*         getEDU( UINT32 tid ) ;

         pmdEDUCB*         getMainEDU() ;

#if defined (_LINUX)
         void              killByThreadID( INT32 signo ) ;
         void              getEDUThreadID ( set<pthread_t> &tidList ) ;
#endif //_LINUX

      protected:
         void              forceIOSerivce() ;

         INT32             createNewEDU ( INT32 type,
                                          BOOLEAN isSystem,
                                          void* arg,
                                          EDUID *pEDUID,
                                          const CHAR *pInitName ) ;

         INT32             createIdleEDU( EDUID *pEDUID ) ;

         pmdEDUCB*         getFromPool( INT32 type ) ;

         void              _forceEDUs( INT32 property = PMD_EDU_ALL ) ;
         UINT32            _getEDUCount( INT32 property = PMD_EDU_ALL ) ;

         void              setDestroyed( BOOLEAN b ) ;
         void              setQuiesced( BOOLEAN b ) ;

         BOOLEAN           destroyAll( INT64 timeout = -1 ) ;

         void              setEDU ( UINT32 tid, EDUID eduid ) ;
         void              returnEDU( pmdEDUCB *cb, BOOLEAN &destroyed ) ;
         BOOLEAN           forceDestory( pmdEDUCB *cb, UINT32 idleTime ) ;

         pmdEDUCB*         findAndRemove( EDUID eduID ) ;

         UINT32            calIdleLowSize( UINT32 *pRunSize = NULL,
                                           UINT32 *pIdleSize = NULL,
                                           UINT32 *pSysSize = NULL,
                                           UINT32 *pPoolSize = NULL ) ;

      private:
         BOOLEAN           _isSystemEDU( pmdEDUCB *cb ) ;
         pmdEDUCB*         _getEDUByID( EDUID eduID ) ;
         void              _postDestoryEDU( pmdEDUCB *cb ) ;
         UINT32            _calIdleLowSize( UINT32 runSize,
                                            UINT32 idleSize,
                                            UINT32 sysSize,
                                            UINT32 poolSize ) ;

         void              _removeFromSystem( INT32 type, EDUID eduID ) ;
         void              _addToSystem( INT32 type, EDUID eduID ) ;
         UINT32            _systemSize( INT32 type = -1 ) ;

      private:
         VEC_IOSERVICE              _vecIOServices ;
         ossSpinSLatch              _latch ;
         EDUID                      _EDUID ;

         MAP_EDUCB                  _mapRuns ;
         MAP_EDUCB                  _mapIdles ;

         MAP_TID2EDU                _mapTid2Edu ;
         MAP_SYSTEMEDU              _mapSystemEdu ;

         BOOLEAN                    _isDestroyed ;
         BOOLEAN                    _isQuiesced ;

         IPmdResource               *_pResource ;
         boost::thread              *_pMonitorThd ;
         boost::thread              *_pDeadCheckThd ;

         ossAutoEvent               _monitorEvent ;
         ossAutoEvent               _deadCheckEvent ;

         UINT32                     _maxPooledEDU ;

         pmdEDUCB                   *_pMainEDU ;

      private:
         /*
            Entry Functions define
         */
         INT32 pmdEDUEntryPointWrapper( pmdEDUCB *cb, pmdEventPtr ePtr ) ;
         INT32 pmdEDUEntryPoint( pmdEDUCB *cb,
                                 pmdEventPtr ePtr,
                                 BOOLEAN &quitWithException ) ;
         void  monitor() ;
         void  deadCheck( INT64 timeout ) ;
   } ;
   typedef _pmdEDUMgr pmdEDUMgr ;

}

#endif // PMDEDUMGR_HPP__
