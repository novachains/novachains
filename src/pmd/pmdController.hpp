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

   Source File Name = pmdController.hpp

   Descriptive Name = Process MoDel Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure kernel control block,
   which is the most critical data structure in the engine process.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_CONTROLLER_HPP__
#define PMD_CONTROLLER_HPP__

#include "pmdInterface.hpp"

namespace engine
{

   /*
      _pmdController define
   */
   class _pmdController : public SDBObject
   {
      public:
         _pmdController() ;
         ~_pmdController() ;

         INT32       init( const IPmdCBConfig &config,
                           INT32 argc,
                           const CHAR *argv[] ) ;

         INT32       run() ;

         void        fini() ;

      protected:

      private:

   } ;
   typedef _pmdController pmdController ;

}

#endif //PMD_CONTROLLER_HPP__

