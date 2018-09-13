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

   Source File Name = pmdCBMainEntry.cpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/08/2018  Xu Jianhui  Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdInterface.hpp"
#include "pmdEPFactory.hpp"
#include "pd.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"

namespace engine
{

   INT32 pmdCBMainEntry( IPmdResource *pResource,
                         IPmdExecutor *cb,
                         void *pData )
   {
      INT32 rc = SDB_OK ;
      IPmdExecutorMgr *pMgr = pResource->getExecutorMgr() ;
      IPmdCB *pCBItem = ( IPmdCB* )pData ;

      rc = pMgr->activateEDU( cb ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG ( PDERROR, "Failed to active EDU[type:%d,ID:%lld]",
                  cb->getType(), cb->getID() ) ;
         goto error ;
      }

      rc = pCBItem->runCBMain( pResource, cb ) ;
      if ( rc )
      {
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   /// Register
   PMD_DEFINE_ENTRYPOINT( PMD_EDU_CBMAIN, TRUE,
                          pmdCBMainEntry,
                          "CBMAIN" ) ;

}

