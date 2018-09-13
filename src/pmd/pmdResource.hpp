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
#ifndef PMD_RESOURCE_HPP__
#define PMD_RESOURCE_HPP__

#include "pmdInterface.hpp"

namespace engine
{

   class _pmdIFAdapter ;
   class _pmdContainer ;

   /*
      _pmdResource define
   */
   class _pmdResource : public IPmdResource, public SDBObject
   {
      public:
         _pmdResource() ;
         virtual ~_pmdResource() ;

         INT32    init( IPmdParam *pParam,
                        IPmdCmdArg *pCmdArg,
                        IPmdEnv *pEnv,
                        IPmdExecutorMgr *pEDUMgr,
                        _pmdIFAdapter *pIFAdpter,
                        _pmdContainer *pContainer ) ;

      public:
         virtual IPmdParam*         getParam() ;
         virtual IPmdCmdArg*        getCmdArg() ;
         virtual IPmdEnv*           getEnv() ;
         virtual IPmdExecutorMgr*   getExecutorMgr() ;
      
         virtual IPmdCB*            getCB( PMD_CB_TYPE type ) ;
         virtual void*              queryIF( PMD_IF_TYPE type ) ;

      private:
         IPmdParam                  *_pParam ;
         IPmdCmdArg                 *_pArg ;
         IPmdEnv                    *_pEnv ;
         IPmdExecutorMgr            *_pEDUMgr ;
         _pmdIFAdapter              *_pIFAdapter ;
         _pmdContainer              *_pContainer ;

   } ;
   typedef _pmdResource pmdResource ;

}

#endif //PMD_RESOURCE_HPP__

