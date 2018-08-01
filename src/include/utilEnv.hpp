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

   Source File Name = utilEnv.hpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef UTILENV_HPP_
#define UTILENV_HPP_

#include "utilCommon.hpp"
#include "ossAtomic.hpp"
#include "ossUtil.hpp"

using namespace bson ;

namespace engine
{

   /*
      Sync clock interval define
   */
   #define UTIL_SYNC_CLOCK_INTERVAL                ( 10 ) /// ms

   /*
      When recieve quit event or signal, will call
   */
   typedef  void (*UTIL_ON_QUIT_FUNC)() ;

   /*
      Be called when pmdEDU exit
   */
   typedef  void (*UTIL_ON_EDU_EXIT_FUNC)() ;

   UTIL_ON_EDU_EXIT_FUNC utilSetEDUHook( UTIL_ON_EDU_EXIT_FUNC hookFunc ) ;
   UTIL_ON_EDU_EXIT_FUNC utilGetEDUHook() ;

   /*
      util system info define
   */

   // save the number of error occured
   typedef struct _utilOccurredErr
   {
      UINT64 _oom ;
      UINT64 _noSpc ;
      UINT64 _tooManyOpenFD ;
      ossTimestamp _resetTimestamp ;

      _utilOccurredErr()
      : _oom( 0 ), _noSpc( 0 ), _tooManyOpenFD( 0 )
      {
         ossGetCurrentTime( _resetTimestamp ) ;
      }
   } utilOccurredErr ;

   typedef struct _utilSysInfo
   {
      SDB_ROLE                      _dbrole ;
      MsgRouteID                    _nodeID ;
      ossAtomic32                   _isPrimary ;
      SDB_TYPE                      _dbType ;
      UINT64                        _startTime ;
      UINT16                        _localPort ;

      BOOLEAN                       _quitFlag ;
      UTIL_ON_QUIT_FUNC             _pQuitFunc ;

      /// loop updated by syncClockEntryPoint
      volatile UINT64               _tick ;

      /// global id
      ossAtomic64                   _globalID ;

      /// ulimit info
      ossProcLimits                 _limitInfo ;

      /// number of occurred error
      utilOccurredErr               _numErr ;

      _utilSysInfo()
      :_isPrimary( 0 ), _globalID( 1 )
      {
         _dbrole        = SDB_ROLE_STANDALONE ;
         _nodeID.value  = MSG_INVALID_ROUTEID ;
         _quitFlag      = FALSE ;
         _dbType        = SDB_TYPE_DB ;
         _pQuitFunc     = NULL ;
         _startTime     = time( NULL ) ;
         _localPort     = 0 ;
         _tick          = 0 ;
      }
   } utilSysInfo ;

   SDB_ROLE       utilGetDBRole() ;
   void           utilSetDBRole( SDB_ROLE role ) ;
   SDB_TYPE       utilGetDBType() ;
   void           utilSetDBType( SDB_TYPE type ) ;
   MsgRouteID     utilGetNodeID() ;
   void           utilSetNodeID( MsgRouteID id ) ;
   BOOLEAN        utilIsPrimary() ;
   void           utilSetPrimary( BOOLEAN primary ) ;

   UINT64         utilGetStartTime() ;

   void           utilSetLocalPort( UINT16 port ) ;
   UINT16         utilGetLocalPort() ;

   void           utilSetQuit() ;
   BOOLEAN        utilIsQuitApp() ;

   void           utilUpdateDBTick() ;

   UINT64         utilGetDBTick() ;

   UINT64         utilGetTickSpanTime( UINT64 lastTick ) ;

   UINT64         utilDBTickSpan2Time( UINT64 tickSpan ) ;

   UINT64         utilAcquireGlobalID() ;

   ossProcLimits* utilGetLimit() ;

   void              utilIncErrNum( INT32 rc ) ;
   void              utilResetErrNum() ;
   utilOccurredErr   utilGetOccurredErr() ;

   utilSysInfo*   utilGetSysInfo () ;

   INT32&         utilGetSigNum() ;

   /*
      Env define
   */
   #define  UTIL_SIGNUM()  utilGetSigNum()

}

#endif //UTILENV_HPP_

