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

   Source File Name = sptConvertorHelper.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of Script component. This file contains structures for javascript
   engine wrapper

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/13/2013  YW Initial Draft

   Last Changed =

*******************************************************************************/

#include "sptConvertorHelper.hpp"
#include "sptConvertor.hpp"
#include "ossMem.hpp"
#include "ossUtil.hpp"

namespace engine
{
   INT32 JSVal2String( JSContext *cx, const jsval &val, std::string &str )
   {
      return sptConvertor::toString( cx, val, str ) ;
   }

   // caller should free the return pointer using SAFE_JS_FREE
   CHAR *convertJsvalToString ( JSContext *cx , jsval val )
   {
      JSString *  str   = NULL ;
      CHAR *      cstr  = NULL ;

      str = JS_ValueToString ( cx , val ) ;
      if ( ! str )
         goto error ;

      // cstr is freed by caller
      cstr = JS_EncodeString ( cx , str ) ;
      if ( ! cstr )
         goto error ;

   done :
      return cstr ;
   error :
      goto done ;
   }

}