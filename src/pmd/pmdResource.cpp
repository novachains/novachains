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

   Source File Name = pmdResource.hpp

   Descriptive Name = Process MoDel Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure kernel control block,
   which is the most critical data structure in the engine process.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          06/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdResource.hpp"
#include "pmdIFAdapter.hpp"
#include "pmdContainer.hpp"
#include "pd.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"

namespace engine
{

   /*
      _pmdResource implement
   */
   _pmdResource::_pmdResource()
   {
      _pParam = NULL ;
      _pArg = NULL ;
      _pEnv = NULL ;
      _pEDUMgr = NULL ;
      _pIFAdapter = NULL ;
   }

   _pmdResource::~_pmdResource()
   {
   }

   INT32 _pmdResource::init( IPmdParam *pParam,
                             IPmdCmdArg *pCmdArg,
                             IPmdEnv *pEnv,
                             IPmdExecutorMgr *pEDUMgr,
                             _pmdIFAdapter *pIFAdpter,
                             _pmdContainer *pContainer )
   {
      INT32 rc = SDB_OK ;

      if ( !pParam || !pCmdArg || !pEnv || !pEDUMgr ||
           !pIFAdpter || !pContainer )
      {
         rc = SDB_SYS ;
         goto error ;
      }

      _pParam = pParam ;
      _pArg = pCmdArg ;
      _pEnv = pEnv ;
      _pEDUMgr = pEDUMgr ;
      _pIFAdapter = pIFAdpter ;
      _pContainer = pContainer ;

   done:
      return rc ;
   error:
      goto done ;
   }

   IPmdParam* _pmdResource::getParam()
   {
      return _pParam ;
   }

   IPmdCmdArg* _pmdResource::getCmdArg()
   {
      return _pArg ;
   }

   IPmdEnv* _pmdResource::getEnv()
   {
      return _pEnv ;
   }

   IPmdExecutorMgr* _pmdResource::getExecutorMgr()
   {
      return _pEDUMgr ;
   }

   IPmdCB* _pmdResource::getCB( PMD_CB_TYPE type )
   {
      if ( _pContainer )
      {
         return _pContainer->getCB( type ) ;
      }
      return NULL ;
   }

   void* _pmdResource::queryIF( PMD_IF_TYPE type )
   {
      if ( _pIFAdapter )
      {
         return _pIFAdapter->queryIF( type ) ;
      }
      return NULL ;
   }

}


