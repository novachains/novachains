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

   Source File Name = pmdMainBaseCallback.cpp

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

#include "pmdMainBaseCallback.hpp"

namespace engine
{

   #define PMD_MAIN_STOP_TIMEOUT          ( 600000 )  /// 10mins

   /*
      _pmdMainBaseCallback implement
   */
   _pmdMainBaseCallback::_pmdMainBaseCallback()
   {
      _pResource = NULL ;
   }

   _pmdMainBaseCallback::~_pmdMainBaseCallback()
   {
   }

   INT64 _pmdMainBaseCallback::getStopTimeout() const
   {
      return PMD_MAIN_STOP_TIMEOUT ;
   }

   void _pmdMainBaseCallback::attach( IPmdResource *pResource )
   {
      _pResource = pResource ;
   }

   void _pmdMainBaseCallback::detach()
   {
      _pResource = NULL ;
   }

   INT32 _pmdMainBaseCallback::onPreStart()
   {
      return SDB_OK ;
   }

   INT32 _pmdMainBaseCallback::onPostStart()
   {
      return SDB_OK ;
   }

   void _pmdMainBaseCallback::onRunTimer( UINT32 interval )
   {
   }

   void _pmdMainBaseCallback::onPreStop()
   {
   }

   void _pmdMainBaseCallback::onPreEDUStop()
   {
   }

   void _pmdMainBaseCallback::onPostEDUStop()
   {
   }

   void _pmdMainBaseCallback::onPostStop()
   {
   }

}

