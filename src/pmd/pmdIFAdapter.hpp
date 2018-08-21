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

   Source File Name = pmdIFAdapter.hpp

   Descriptive Name = Process MoDel Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure kernel control block,
   which is the most critical data structure in the engine process.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          30/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_IF_ADAPTER_HPP__
#define PMD_IF_ADAPTER_HPP__

#include "core.hpp"
#include "pmdInterface.hpp"
#include <map>

using namespace std ;

namespace engine
{

   typedef map<PMD_IF_TYPE, void*>                 MAP_TYPE_2_IF ;
   typedef MAP_TYPE_2_IF::iterator                 MAP_TYPE_2_IF_IT ;

   /*
      _pmdIFAdapter define
   */
   class _pmdIFAdapter : public SDBObject
   {
      public:
         _pmdIFAdapter() ;
         ~_pmdIFAdapter() ;

         BOOLEAN           hasIF( PMD_IF_TYPE type ) const ;
         void*             queryIF( PMD_IF_TYPE type ) ;

      public:

         BOOLEAN           registerRoot( IPmdRoot *pRoot ) ;

      protected:
         BOOLEAN           _addIF( PMD_IF_TYPE type, void *pIF ) ;

      private:
         MAP_TYPE_2_IF              _mapType2IF ;

   } ;
   typedef _pmdIFAdapter pmdIFAdapter ;

}

#endif //PMD_IF_ADAPTER_HPP__

