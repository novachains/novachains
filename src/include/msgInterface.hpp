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

   Source File Name = msgInterface.hpp

   Descriptive Name =

   When/how to use:

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          31/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef MSG_INTERFACE_HPP__
#define MSG_INTERFACE_HPP__

#include "ossTypes.hpp"
#include "msg.h"
#include <string>

using namespace std ;

namespace engine
{

   /*
      _IMsgRoot define
   */
   class _IMsgRoot
   {
      public:
         _IMsgRoot() {}
         virtual ~_IMsgRoot() {}

      public:
         virtual UINT32       getLength() const = 0 ;

   } ;
   typedef _IMsgRoot IMsgRoot ;

}

#endif // MSG_INTERFACE_HPP__
