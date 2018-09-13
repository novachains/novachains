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

   Source File Name = pmdMainBaseCallback.hpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          25/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_MAIN_BASE_CALLBACK_HPP_
#define PMD_MAIN_BASE_CALLBACK_HPP_

#include "pmdInterface.hpp"

namespace engine
{

   /*
      _pmdMainBaseCallback define
   */
   class _pmdMainBaseCallback : public IPmdMainCallback
   {
      public:
         _pmdMainBaseCallback() ;
         virtual ~_pmdMainBaseCallback() ;

      public:
      
         virtual INT64  getStopTimeout() const ;
      
         virtual void   attach( IPmdResource *pResource ) ;
         virtual void   detach() ;
      
         virtual INT32  onPreStart() ;
         virtual INT32  onPostStart() ;
      
         virtual void   onRunTimer( UINT32 interval ) ;
      
         virtual void   onPreStop() ;
         virtual void   onPreEDUStop() ;
         virtual void   onPostEDUStop() ;
         virtual void   onPostStop() ;

      private:
         IPmdResource            *_pResource ;

   } ;
   typedef _pmdMainBaseCallback pmdMainBaseCallback ;

}

#endif //PMD_MAIN_BASE_CALLBACK_HPP_

