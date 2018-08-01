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

   Source File Name = pmdIFAdapter.cpp

   Descriptive Name = Process MoDel

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains kernel control block object.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdIFAdapter.hpp"
#include "pd.hpp"

namespace engine
{

   /*
      _pmdIFAdapter implement
   */
   _pmdIFAdapter::_pmdIFAdapter()
   {
   }

   _pmdIFAdapter::~_pmdIFAdapter()
   {
   }

   BOOLEAN _pmdIFAdapter::hasIF( PMD_IF_TYPE type ) const
   {
      if ( _mapType2IF.find( type ) != _mapType2IF.end() )
      {
         return TRUE ;
      }
      return FALSE ;
   }

   void* _pmdIFAdapter::queryIF( PMD_IF_TYPE type )
   {
      MAP_TYPE_2_IF_IT it = _mapType2IF.find( type ) ;
      if ( it != _mapType2IF.end() )
      {
         return it->second ;
      }
      return NULL ;
   }

   BOOLEAN _pmdIFAdapter::registerRoot( IPmdRoot *pRoot )
   {
      void *pIF = NULL ;
      BOOLEAN result = TRUE ;
      IPmdRoot::IF_PUB_MODE mode = IPmdRoot::IF_NULL ;

      mode = pRoot->publishMode() ;
      if ( IPmdRoot::IF_ALL == mode )
      {
         /// query all
         for ( INT32 type = PMD_IF_MIN + 1 ; type < PMD_IF_MAX ; ++type )
         {
            pIF = pRoot->queryIF( (PMD_IF_TYPE)type ) ;
            if ( pIF )
            {
               if ( FALSE == ( result = _addIF( (PMD_IF_TYPE)type, pIF ) ) )
               {
                  break ;
               }
            }
         }
      }
      else if ( IPmdRoot::IF_LIST == mode )
      {
         vector<PMD_IF_TYPE> vecIF ;
         pRoot->publishList( vecIF ) ;

         for ( UINT32 i = 0 ; i < vecIF.size() ; ++i )
         {
            pIF = pRoot->queryIF( vecIF[i] ) ;
            if ( pIF )
            {
               if ( FALSE == ( result = _addIF( vecIF[i], pIF ) ) )
               {
                  break ;
               }
            }
         }
      }

      return result ;
   }

   BOOLEAN _pmdIFAdapter::_addIF( PMD_IF_TYPE type, void *pIF )
   {
      BOOLEAN result = FALSE ;

      /// check exist
      if ( hasIF( type ) )
      {
         PD_LOG( PDSEVERE, "Iterface is already exist: %d", type ) ;
      }
      else
      {
         _mapType2IF[ type ] = pIF ;
         result = TRUE ;
      }
      return result ;
   }

}

