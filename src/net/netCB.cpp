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

   Source File Name = netCB.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          10/01/2019  Jiaqi Initial Draft

   Last Changed =

*******************************************************************************/

#include "netCB.hpp"
#include <iostream>

namespace engine
{
#if !defined _MSC_VER
   // auto register cb
   PMD_IMPLEMENT_CB_AUTO_REGISTER( _netCB ) ;
#endif
   /*
      _netCB implement
   */
   _netCB::_netCB()
   {
   }

   _netCB::~_netCB()
   {
   }

   PMD_CB_TYPE _netCB::cbType() const
   {
      return PMD_CB_NET ;
   }

   const CHAR* _netCB::cbName() const
   {
      return "NET" ;
   }

   INT32 _netCB::init()
   {
      cout << "init _netCB" << endl ;
      _netManager = SDB_OSS_NEW _netSvcManager() ;
      return SDB_OK ;
   }

   INT32 _netCB::active()
   {
      cout << "active net...does not need to start a new EDU" << endl ;
      return SDB_OK ;
   }

   INT32 _netCB::deactive()
   {
      cout << "deactive net...do nothing" << endl ;
      return SDB_OK ;
   }

   INT32 _netCB::fini()
   {
      cout << "fini net" << endl ;
      SDB_OSS_DEL _netManager ;
      return SDB_OK ;
   }


   OSS_INLINE IPmdRoot::IF_PUB_MODE _netCB::publishMode() const
   {
      return IPmdRoot::IF_LIST ;
   }

   void _netCB::publishList( vector<PMD_IF_TYPE> &vecIF ) const
   {
      vecIF.push_back(PMD_IF_GET_NETMGR) ;
   }

   void* _netCB::queryIF( PMD_IF_TYPE type )
   {
      void* pIF = NULL ;
      switch (type)
      {
         case PMD_IF_GET_NETMGR :
            pIF = (void *) _netManager ;
            break ;

         default:
            break ;
      }

         return pIF ;
   }


}


