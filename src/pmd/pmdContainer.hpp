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

   Source File Name = pmdContainer.hpp

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
#ifndef PMD_CONTAINER_HPP__
#define PMD_CONTAINER_HPP__

#include "pmdInterface.hpp"

namespace engine
{

   /*
      _pmdContainer
   */
   class _pmdContainer : public SDBObject
   {
      /*
         PMD_CB_OPR_STAT define
      */
      enum PMD_CB_OPR_STAT
      {
         CB_OPR_NULL = 0 ,
         CB_OPR_INIT,
         CB_OPR_ACTIVE
      } ;

      /*
         _pmdCBOprItem define
      */
      struct _pmdCBOprItem
      {
         IPmdCB            *_cb ;
         PMD_CB_OPR_STAT   _stat ;

         _pmdCBOprItem()
         {
            _cb = NULL ;
            _stat = CB_OPR_NULL ;
         }
         _pmdCBOprItem( IPmdCB *cb )
         {
            _cb = cb ;
            _stat = CB_OPR_NULL ;
         }
      } ;
      typedef _pmdCBOprItem pmdCBOprItem ;

      typedef vector<pmdCBOprItem>        VEC_CB ;

      public:
         _pmdContainer() ;
         ~_pmdContainer() ;

         IPmdCB*           getCB( PMD_CB_TYPE type ) ;

         INT32             initCB( IPmdResource *pResource ) ;
         INT32             activeCB() ;

         INT32             deactiveCB() ;
         INT32             finiCB() ;

      public:

         INT32             registerCB( IPmdCB *cb ) ;

      private:
         INT32             _checkDependency() ;
         INT32             _addDependencyCB( IPmdCB *cb, INT32 *pFlag ) ;

         BOOLEAN           _isInOrder( IPmdCB *cb ) const ;

      private:
         IPmdCB*                    _arrayCB[ PMD_CB_MAX ] ;
         VEC_CB                     _vecCB ;
         BOOLEAN                    _hasChecked ;

   } ;
   typedef _pmdContainer pmdContainer ;

}

#endif //PMD_CONTAINER_HPP__

