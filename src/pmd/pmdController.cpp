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

   Source File Name = pmdController.cpp

   Descriptive Name = Process MoDel

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains kernel control block object.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdController.hpp"
#include "pmdCBFactory.hpp"
#include "pmdTool.hpp"
#include "pmdTrace.hpp"
#include "pd.hpp"

namespace engine
{

   /*
      _pmdController implement
   */
   _pmdController::_pmdController()
   {
      _pParam = NULL ;
      _pMainCallBack = NULL ;
   }

   _pmdController::~_pmdController()
   {
      fini() ;
   }

   INT32 _pmdController::init( const IPmdCBConfig &config,
                               IPmdParam *pParam,
                               IPmdMainCallback *pMainCallBack,
                               INT32 argc,
                               const CHAR *argv[] )
   {
      INT32 rc = SDB_OK ;
      vector<PMD_CB_TYPE> vecType ;
      PMD_CB_TYPE type = PMD_CB_MIN ;
      IPmdCB *cb = NULL ;

      if ( !pParam )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      _pParam = pParam ;
      _pMainCallBack = pMainCallBack ;

      /// init cmd arg
      _cmdArg.init( argc, argv ) ;

      if ( IPmdCBConfig::INSTALL_ALL == config.getInstallMode() )
      {
         pmdGetCBFactory()->getAllCBType( vecType ) ;
      }
      else
      {
         config.getInstallList( vecType ) ;
      }

      /// register cb and interface
      for ( UINT32 i = 0 ; i < vecType.size() ; ++i )
      {
         type = vecType[ i ] ;

         if ( !pmdGetCBFactory()->hasCB( type ) )
         {
            ossPrintf( "*Error: CB[%d] is not implement"OSS_NEWLINE, type ) ;
            rc = SDB_SYS ;
            goto error ;
         }
         cb = pmdGetCBFactory()->createCB( type ) ;
         if ( !cb )
         {
            ossPrintf( "*Error: Create CB[%d] instance failed"OSS_NEWLINE,
                       type ) ;
            rc = SDB_OOM ;
            goto error ;
         }

         rc = _ifAdapter.registerRoot( cb ) ;
         if ( rc )
         {
            ossPrintf( "*Error: Reisget CB[%d]'s interface failed, "
                       "rc: %d"OSS_NEWLINE, type, rc ) ;
            goto error ;
         }

         rc = _container.registerCB( cb ) ;
         if ( rc )
         {
            ossPrintf( "*Error: Reister CB[%d] failed, rc: %d"OSS_NEWLINE,
                       type, rc ) ;
            goto error ;
         }
         cb = NULL ;
      }

   done:
      if ( cb )
      {
         pmdGetCBFactory()->releaseCB( cb ) ;
      }
      return rc ;
   error:
      goto done ;
   }

   void _pmdController::fini()
   {
      _ifAdapter.clear() ;
      IPmdCB *cb = NULL ;
      INT32 type = PMD_CB_MIN ;

      /// release all cb
      while( type < PMD_CB_MAX )
      {
         cb = _container.getCB( (PMD_CB_TYPE)type ) ;
         if ( cb )
         {
            pmdGetCBFactory()->releaseCB( cb ) ;
         }
         ++type ;
      }
      _container.clear() ;

      _pParam = NULL ;
      _pMainCallBack = NULL ;
   }

   INT32 _pmdController::run()
   {
      INT32 rc = SDB_OK ;

      rc = _resource.init( _pParam, &_cmdArg, &_env,
                           &_eduMgr, &_ifAdapter,
                           &_container ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Init resource failed, rc: %d" ) ;
         goto error ;
      }

      rc = _eduMgr.init( &_resource ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Init edu manager failed, rc: %d", rc ) ;
         goto error ;
      }

      /// declare
      pmdDeclareEDUCB( _eduMgr.getMainEDU() ) ;

      if ( _pMainCallBack )
      {
         _pMainCallBack->attach( &_resource ) ;

         rc = _pMainCallBack->onPreStart() ;
         if ( rc )
         {
            PD_LOG( PDERROR, "On pre-start failed, rc: %d", rc ) ;
            goto error ;
         }
      }

      /// init all cb
      rc = _container.initCB( &_resource ) ;
      if ( rc )
      {
         goto error ;
      }

      /// active all cb
      rc = _container.activeCB() ;
      if ( rc )
      {
         goto error ;
      }

      _env.setActive( TRUE ) ;

      if ( _pMainCallBack )
      {
         rc = _pMainCallBack->onPostStart() ;
         if ( rc )
         {
            PD_LOG( PDERROR, "On post start failed, rc: %d", rc ) ;
            goto error ;
         }
      }

      /// run loop
      while ( !_env.isShutdown() )
      {
         ossSleep( OSS_ONE_SEC ) ;

         if ( _pMainCallBack )
         {
            _pMainCallBack->onRunTimer( OSS_ONE_SEC ) ;
         }
      }

      /// get the shutdown code
      rc = _env.getShutdownCode() ;

   done:
      destory() ;
      if ( _pMainCallBack )
      {
         _pMainCallBack->detach() ;
      }
      return rc ;
   error:
      if ( !_env.isShutdown() )
      {
         _env.shutdownNode( rc ) ;
      }
      goto done ;
   }

   void _pmdController::destory()
   {
      BOOLEAN normalStop = TRUE ;
      INT64 stopTimeout = PMD_STOP_TIMEOUT ;

      /// set quit flag
      pmdSetQuit() ;

      /// set deactive
      _env.setActive( FALSE ) ;
      if ( _pMainCallBack )
      {
         stopTimeout = _pMainCallBack->getStopTimeout() ;
         _pMainCallBack->onPreStop() ;
      }

      /// start dead check
      if ( stopTimeout > 0 )
      {
         _eduMgr.startDeadCheck( stopTimeout + 60000 ) ;
      }

      /// deactive all cb
      _container.deactiveCB() ;

      /// stop all ioservice and edu
      if ( _pMainCallBack )
      {
         _pMainCallBack->onPreEDUStop() ;
      }
      normalStop = _eduMgr.fini( stopTimeout ) ;
      if ( _pMainCallBack )
      {
         _pMainCallBack->onPostEDUStop() ;
      }

      /// fini all cb
      _container.finiCB() ;
      if ( _pMainCallBack )
      {
         _pMainCallBack->onPostStop() ;
      }

      if ( !normalStop && _eduMgr.dumpAbnormalEDU() > 0 )
      {
         PD_LOG( PDSEVERE, "Stop all EDUs timeout, crashed." ) ;
         ossPanic() ;
      }

      /// undeclare
      pmdUndeclareEDUCB() ;

      /// stop dead check
      _eduMgr.stopDeadCheck() ;
   }

   INT32 _pmdController::onConfigInit()
   {
      return SDB_OK ;
   }

   void _pmdController::onConfigChange( UINT32 changeID )
   {
      _container.onConfigChange( changeID ) ;
   }

   void _pmdController::onConfigSave()
   {
      _container.onConfigSave() ;
   }

}

