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

   Source File Name = daoCB.hpp

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
#ifndef PMD_DAO_CB_HPP__
#define PMD_DAO_CB_HPP__

#include "pmdCBBase.hpp"
#include "pmdCBFactory.hpp"

namespace engine
{

   /*
      _daoCB define
   */
   class _daoCB : public pmdCBBase
   {
      DECLARE_OBJ_MSG_MAP() ;
      PMD_DECLARE_CB_AUTO_REGISTER() ;

      public:
         _daoCB() ;
         virtual ~_daoCB() ;

      public:
         virtual PMD_CB_TYPE  cbType() const ;
         virtual const CHAR*  cbName() const ;

         virtual BOOLEAN      enableCBMain() const { return TRUE ; }

         virtual INT32  init () ;
         virtual INT32  active () ;
         virtual INT32  deactive () ;
         virtual INT32  fini () ;

      private:

   } ;
   typedef _daoCB daoCB ;

}

#endif //PMD_DAO_CB_HPP__

