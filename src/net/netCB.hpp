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

   Source File Name = netCB.hpp

   Descriptive Name = net service CB

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          15/01/2019  Jiaqi Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_NET_CB_HPP__
#define PMD_NET_CB_HPP__

#include "pmdInterface.hpp"
#include "pmdCBFactory.hpp"
#include "netService.hpp"

namespace engine
{

   /*
      _netCB define
   */
   class _netCB : public _IPmdCB
   {
      PMD_DECLARE_CB_AUTO_REGISTER() ;

      public:
         _netCB() ;
         virtual ~_netCB() ;

      public:
         virtual PMD_CB_TYPE  cbType() const ;
         virtual const CHAR*  cbName() const ;


         virtual INT32  init () ;
         virtual INT32  active () ;
         virtual INT32  deactive () ;
         virtual INT32  fini () ;

	 virtual IPmdRoot::IF_PUB_MODE  publishMode() const ;

         virtual void publishList( vector<PMD_IF_TYPE> &vecIF ) const ;

	 virtual void* queryIF( PMD_IF_TYPE type ) ; 

      private:
	 _netSvcManager*  _netManager ;

   } ;
   typedef _netCB netCB ;

}

#endif //PMD_NET_CB_HPP__

