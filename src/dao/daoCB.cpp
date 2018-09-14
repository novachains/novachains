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

   Source File Name = daoCB.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          14/09/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "daoCB.hpp"
#include <iostream>

namespace engine
{

   /// auto register cb
   PMD_IMPLEMENT_CB_AUTO_REGISTER( _daoCB ) ;

   /// message map, use ON_MSG or ON_EVENT to map message and function
   BEGIN_OBJ_MSG_MAP( _daoCB, _pmdCBBase )
   END_OBJ_MSG_MAP()

   /*
      _daoCB implement
   */
   _daoCB::_daoCB()
   {
      _pResource = NULL ;
   }

   _daoCB::~_daoCB()
   {
   }

   PMD_CB_TYPE _daoCB::cbType() const
   {
      return PMD_CB_DAO ;
   }

   const CHAR* _daoCB::cbName() const
   {
      return "DAO" ;
   }

   INT32 _daoCB::init( _IPmdResource *pResource )
   {
      _pResource = pResource ;

      cout << "init dao" << endl ;

      return SDB_OK ;
   }

   INT32 _daoCB::active()
   {
      cout << "active dao" << endl ;
      return SDB_OK ;
   }

   INT32 _daoCB::deactive()
   {
      cout << "deactive dao" << endl ;
      return SDB_OK ;
   }

   INT32 _daoCB::fini()
   {
      cout << "fini dao" << endl ;
      return SDB_OK ;
   }

}

