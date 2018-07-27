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

   Source File Name = utilEnv.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          22/04/2014  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "utilEnv.hpp"
#include "ossEDU.hpp"
#include "pdTrace.hpp"
#include "utilTrace.hpp"
#include "ossUtil.hpp"

using namespace bson ;

namespace engine
{
   static OSS_THREAD_LOCAL UTIL_ON_EDU_EXIT_FUNC __eduHookFunc = NULL ;

   UTIL_ON_EDU_EXIT_FUNC utilSetEDUHook( UTIL_ON_EDU_EXIT_FUNC hookFunc )
   {
      UTIL_ON_EDU_EXIT_FUNC oldFunc = __eduHookFunc ;
      __eduHookFunc = hookFunc ;
      return oldFunc ;
   }

   UTIL_ON_EDU_EXIT_FUNC utilGetEDUHook()
   {
      return __eduHookFunc ;
   }

   utilSysInfo* utilGetSysInfo()
   {
      static utilSysInfo s_sysInfo ;
      return &s_sysInfo ;
   }
   SDB_ROLE utilGetDBRole()
   {
      return utilGetSysInfo()->_dbrole ;
   }
   void  utilSetDBRole( SDB_ROLE role )
   {
      utilGetSysInfo()->_dbrole = role ;
      utilSetDBType( utilRoleToType( role ) ) ;
   }
   SDB_TYPE utilGetDBType()
   {
      return utilGetSysInfo()->_dbType ;
   }
   void utilSetDBType( SDB_TYPE type )
   {
      utilGetSysInfo()->_dbType = type ;
   }
   MsgRouteID utilGetNodeID()
   {
      return utilGetSysInfo()->_nodeID ;
   }
   void utilSetNodeID( MsgRouteID id )
   {
      utilGetSysInfo()->_nodeID = id ;
   }
   BOOLEAN utilIsPrimary ()
   {
      return utilGetSysInfo()->_isPrimary.peek() ;
   }
   void utilSetPrimary( BOOLEAN primary )
   {
      utilGetSysInfo()->_isPrimary.init( primary ) ;
   }

   UINT64 utilGetStartTime()
   {
      return utilGetSysInfo()->_startTime ;
   }

   void utilSetLocalPort( UINT16 port )
   {
      utilGetSysInfo()->_localPort = port ;
   }

   UINT16 utilGetLocalPort()
   {
      return utilGetSysInfo()->_localPort ;
   }

   BOOLEAN utilIsQuitApp()
   {
      return utilGetSysInfo()->_quitFlag ;
   }

   void utilSetQuit()
   {
      utilGetSysInfo()->_quitFlag = TRUE ;
   }

   INT32& utilGetSigNum()
   {
      static INT32 s_sigNum = -1 ;
      return s_sigNum ;
   }

   ossProcLimits* utilGetLimit()
   {
      return &( utilGetSysInfo()->_limitInfo ) ;
   }

   void utilIncErrNum( INT32 rc )
   {
      switch ( rc )
      {
         case SDB_OOM :
            utilGetSysInfo()->_numErr._oom++ ;
            break ;
         case SDB_NOSPC :
            utilGetSysInfo()->_numErr._noSpc++ ;
            break ;
         case SDB_TOO_MANY_OPEN_FD :
            utilGetSysInfo()->_numErr._tooManyOpenFD++ ;
            break ;
         default :
            break ;
      }
   }

   void utilResetErrNum()
   {
      utilGetSysInfo()->_numErr._oom = 0 ;
      utilGetSysInfo()->_numErr._noSpc = 0 ;
      utilGetSysInfo()->_numErr._tooManyOpenFD = 0 ;
      ossGetCurrentTime( utilGetSysInfo()->_numErr._resetTimestamp ) ;
   }

   utilOccurredErr utilGetOccurredErr()
   {
      return utilGetSysInfo()->_numErr ;
   }

   void utilUpdateDBTick()
   {
      ++(utilGetSysInfo()->_tick) ;
      return ;
   }

   UINT64 utilGetDBTick()
   {
      return utilGetSysInfo()->_tick ;
   }

   UINT64 utilAcquireGlobalID()
   {
      return utilGetSysInfo()->_globalID.inc() ;
   }

   UINT64 utilGetTickSpanTime( UINT64 lastTick )
   {
      UINT64 curTick = utilGetDBTick() ;
      if ( curTick > lastTick )
      {
         return ( curTick - lastTick ) * UTIL_SYNC_CLOCK_INTERVAL ;
      }
      return 0 ;
   }

   UINT64 utilDBTickSpan2Time( UINT64 tickSpan )
   {
      return tickSpan * UTIL_SYNC_CLOCK_INTERVAL ;
   }

}


