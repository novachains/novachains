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

   Source File Name = pmdCBFactory.cpp

   Descriptive Name = Process MoDel

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains kernel control block object.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          30/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdCBFactory.hpp"
#include "pd.hpp"

namespace engine
{

   /*
      _pmdCBAssist implement
   */
   _pmdCBAssist::_pmdCBAssist( PMD_CREATE_CB_FUNC pCreate,
                               PMD_RELEASE_CB_FUNC pRelease )
   {
      BOOLEAN hasReg = FALSE ;
      IPmdCB *cb = pCreate() ;
      if ( !cb )
      {
         throw pdGeneralException( SDB_OOM, "Create cb failed" ) ;
      }
      hasReg = pmdGetCBFactory()->registerCB( cb->cbType(),
                                              cb->cbName(),
                                              pCreate,
                                              pRelease ) ;
      /// release the cb
      pRelease( cb ) ;
      /// judge result
      if ( !hasReg )
      {
         throw pdGeneralException( SDB_SYS, "Register cb failed" ) ;
      }
   }

   _pmdCBAssist::~_pmdCBAssist()
   {
   }

   /*
      _pmdCBFactory implement
   */
   _pmdCBFactory::_pmdCBFactory()
   {
   }

   _pmdCBFactory::~_pmdCBFactory()
   {
   }

   BOOLEAN _pmdCBFactory::registerCB( PMD_CB_TYPE type,
                                      const string &name,
                                      PMD_CREATE_CB_FUNC pCreateFunc,
                                      PMD_RELEASE_CB_FUNC pReleaseFunc )
   {
      BOOLEAN hasReg = FALSE ;

      SDB_ASSERT( pCreateFunc && pReleaseFunc, "Funcs can't be NULL" ) ;

      /// Check whether already exists
      if ( _mapCB2Func.find( type ) != _mapCB2Func.end() )
      {
         SDB_ASSERT( FALSE, "CB already exists" ) ;
         PD_LOG( PDSEVERE, "Control Block is already exist: %d", type ) ;
      }
      else if ( _mapName2Func.find( name ) != _mapName2Func.end() )
      {
         SDB_ASSERT( FALSE, "CB already exists" ) ;
         PD_LOG( PDSEVERE, "Control Block is already exist: %s",
                 name.c_str() ) ;
      }
      else
      {
         _mapCB2Func[ type ] = PMD_FUNC_PAIR( pCreateFunc, pReleaseFunc ) ;
         _mapName2Func[ name ] = PMD_FUNC_PAIR( pCreateFunc, pReleaseFunc ) ;
         hasReg = TRUE ;
      }

      return hasReg ;
   }

   BOOLEAN _pmdCBFactory::hasCB( PMD_CB_TYPE type ) const
   {
      if ( _mapCB2Func.find( type ) != _mapCB2Func.end() )
      {
         return TRUE ;
      }
      return FALSE ;
   }

   BOOLEAN _pmdCBFactory::hasCB( const string &name ) const
   {
      if ( _mapName2Func.find( name ) != _mapName2Func.end() )
      {
         return TRUE ;
      }
      return FALSE ;
   }

   IPmdCB* _pmdCBFactory::createCB( PMD_CB_TYPE type )
   {
      IPmdCB *cb = NULL ;
      MAP_TYPE_2_FUNC_IT it = _mapCB2Func.find( type ) ;
      if ( it != _mapCB2Func.end() )
      {
         cb = (it->second.first)() ;
      }
      return cb ;
   }

   IPmdCB* _pmdCBFactory::createCB( const string &name )
   {
      IPmdCB *cb = NULL ;
      MAP_NAME_2_FUNC_IT it = _mapName2Func.find( name ) ;
      if ( it != _mapName2Func.end() )
      {
         cb = (it->second.first)() ;
      }
      return cb ;
   }

   void _pmdCBFactory::releaseCB( IPmdCB *&cb )
   {
      MAP_TYPE_2_FUNC_IT it = _mapCB2Func.find( cb->cbType() ) ;
      if ( it != _mapCB2Func.end() )
      {
         (it->second.second)( cb ) ;
         cb = NULL ;
      }
      else
      {
         SDB_ASSERT( FALSE, "Can't found cb's release function" ) ;
      }
   }

   /*
      Global functions implement
   */
   pmdCBFactory* pmdGetCBFactory()
   {
      static pmdCBFactory __cbFactory ;
      return &__cbFactory ;
   }

}

