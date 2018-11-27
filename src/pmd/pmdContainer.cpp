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

   Source File Name = pmdContainer.cpp

   Descriptive Name = Process MoDel

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains kernel control block object.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdContainer.hpp"
#include "pmdTrace.hpp"
#include "pd.hpp"

namespace engine
{

   /*
      _pmdContainer implement
   */
   _pmdContainer::_pmdContainer()
   {
      for ( UINT32 i = 0 ; i < PMD_CB_MAX ; ++i )
      {
         _arrayCB[ i ] = NULL ;
      }
      _hasChecked = FALSE ;
      _pResource = NULL ;
   }

   _pmdContainer::~_pmdContainer()
   {
      _pResource = NULL ;
   }

   void _pmdContainer::clear()
   {
      for ( UINT32 i = 0 ; i < PMD_CB_MAX ; ++i )
      {
         _arrayCB[ i ] = NULL ;
      }
      _vecCB.clear() ;
   }

   IPmdCB* _pmdContainer::getCB( PMD_CB_TYPE type )
   {
      if ( type >= 0 && type < PMD_CB_MAX )
      {
         return _arrayCB[ type ] ;
      }
      return NULL ;
   }

   INT32 _pmdContainer::registerCB( IPmdCB *cb )
   {
      INT32 rc = SDB_OK ;

      if ( cb->cbType() >= 0 && cb->cbType() < PMD_CB_MAX )
      {
         _arrayCB[ cb->cbType() ] = cb ;
      }
      else
      {
         rc = SDB_SYS ;
      }
      return rc ;
   }

   INT32 _pmdContainer::_addDependencyCB( IPmdCB *cb, INT32 *pFlag )
   {
      INT32 rc = SDB_OK ;
      vector<PMD_CB_TYPE> cbs ;

      if ( pFlag[ cb->cbType() ] )
      {
         /// The cb is dead dependency
         PD_LOG( PDERROR, "The dependency cb[%d,%s] is endless loop",
                 cb->cbType(), cb->cbName() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      /// set flag
      pFlag[ cb->cbType() ] = 1 ;

      cb->dependentcy( cbs ) ;
      for ( UINT32 i = 0 ; i < cbs.size() ; ++i )
      {
         /// Invalid type
         if ( cbs[ i ] < 0 || cbs[ i ] >= PMD_CB_MAX )
         {
            PD_LOG( PDERROR, "The cb[%d] is invalid", cb->cbType() ) ;
            rc = SDB_SYS ;
            goto error ;
         }
         /// Can't found the dependency cb
         else if ( !_arrayCB[ cbs[ i ] ] )
         {
            PD_LOG( PDERROR, "Can't found the dependency cb[%d,%s]",
                    cb->cbType(), cb->cbName() ) ;
            rc = SDB_SYS ;
            goto error ;
         }

         /// The dependency cb is not added
         if ( !_isInOrder( _arrayCB[ cbs[ i ] ] ) )
         {
            if ( SDB_OK != ( rc = _addDependencyCB( _arrayCB[ cbs[ i ] ],
                                                    pFlag ) ) )
            {
               goto error ;
            }
         }
      }

      _vecCB.push_back( pmdCBOprItem( cb ) ) ;

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _pmdContainer::_checkDependency()
   {
      INT32 rc = SDB_OK ;
      IPmdCB *cb = NULL ;
      INT32 arrayFlag[ PMD_CB_MAX ] = { 0 } ;

      if ( !_hasChecked )
      {
         for ( UINT32 i = 0 ; i < PMD_CB_MAX ; ++i )
         {
            cb = _arrayCB[ i ] ;

            if ( !cb || arrayFlag[i] )
            {
               continue ;
            }

            rc = _addDependencyCB( cb, arrayFlag ) ;
            if ( rc )
            {
               goto error ;
            }
         }

         _hasChecked = TRUE ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   BOOLEAN _pmdContainer::_isInOrder( IPmdCB *cb ) const
   {
      VEC_CB::const_iterator it = _vecCB.begin() ;
      while( it != _vecCB.end() )
      {
         if ( (*it)._cb == cb )
         {
            return TRUE ;
         }
         ++it ;
      }
      return FALSE ;
   }

   INT32 _pmdContainer::initCB( IPmdResource *pResource )
   {
      INT32 rc = SDB_OK ;
      VEC_CB::iterator it ;

      _pResource = pResource ;

      /// check dependency
      rc = _checkDependency() ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Check dependency failed, rc: %d", rc ) ;
         goto error ;
      }

      it = _vecCB.begin() ;
      while( it != _vecCB.end() )
      {
         pmdCBOprItem &item = *it ;
         ++it ;

         if ( item._stat >= CB_OPR_INIT )
         {
            continue ;
         }

         item._cb->setResource( pResource ) ;
         rc = item._cb->init() ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Init cb[%d,%s] failed, rc: %d", 
                    item._cb->cbType(), item._cb->cbName(), rc ) ;
            goto error ;
         }
         item._stat = CB_OPR_INIT ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _pmdContainer::activeCB()
   {
      INT32 rc = SDB_OK ;
      VEC_CB::iterator it ;

      it = _vecCB.begin() ;
      while( it != _vecCB.end() )
      {
         pmdCBOprItem &item = *it ;
         ++it ;

         if ( item._stat >= CB_OPR_ACTIVE )
         {
            continue ;
         }

         rc = item._cb->active() ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Acitve cb[%d,%s] failed, rc: %d",
                    item._cb->cbType(), item._cb->cbName(), rc ) ;
            goto error ;
         }
         item._stat = CB_OPR_ACTIVE ;
      }

      rc = _postActive() ;
      if ( rc )
      {
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _pmdContainer::_postActive()
   {
      INT32 rc = SDB_OK ;
      VEC_CB::iterator it ;
      IPmdExecutorMgr *pMgr = _pResource->getExecutorMgr() ;
      IPmdIOService *pIOSvc = NULL ;

      vector<IPmdIOService*>  vecIOSvc ;

      it = _vecCB.begin() ;
      while( it != _vecCB.end() )
      {
         pmdCBOprItem &item = *it ;
         ++it ;

         /// start io service edu
         vecIOSvc.clear() ;
         item._cb->getIOSvc( vecIOSvc ) ;
         for ( UINT32 i = 0 ; i < vecIOSvc.size() ; ++i )
         {
            pIOSvc = vecIOSvc[ i ] ;
            rc = pMgr->startEDU( PMD_EDU_IOSVC, (void*)pIOSvc,
                                 NULL, pIOSvc->getName() ) ;
            if ( rc )
            {
               PD_LOG( PDERROR, "Start EDU failed, rc: %d", rc ) ;
               goto error ;
            }
         }

         /// start cb edu
         if ( item._cb->enableCBMain() )
         {
            rc = pMgr->startEDU( PMD_EDU_CBMAIN, (void*)item._cb,
                                 NULL, item._cb->cbName() ) ;
            if ( rc )
            {
               PD_LOG( PDERROR, "Start EDU failed, rc: %d", rc ) ;
               goto error ;
            }
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _pmdContainer::deactiveCB()
   {
      VEC_CB::reverse_iterator it ;
      INT32 rc = SDB_OK ;

      it = _vecCB.rbegin() ;
      while( it != _vecCB.rend() )
      {
         pmdCBOprItem &item = *it ;
         ++it ;

         if ( item._stat < CB_OPR_ACTIVE )
         {
            continue ;
         }
         SDB_ASSERT( item._stat == CB_OPR_ACTIVE, "Must be active" ) ;

         rc = item._cb->deactive() ;
         if ( rc )
         {
            PD_LOG( PDWARNING, "Deactive cb[%d,%s] failed, rc: %d",
                    item._cb->cbType(), item._cb->cbName(), rc ) ;
            /// Not goto error, continue do the other cbs
         }

         /// set to init
         item._stat = CB_OPR_INIT ;
      }

      return SDB_OK ;
   }

   INT32 _pmdContainer::finiCB()
   {
      VEC_CB::reverse_iterator it ;
      INT32 rc = SDB_OK ;

      it = _vecCB.rbegin() ;
      while( it != _vecCB.rend() )
      {
         pmdCBOprItem &item = *it ;
         ++it ;

         if ( item._stat < CB_OPR_INIT )
         {
            continue ;
         }
         SDB_ASSERT( item._stat == CB_OPR_INIT, "Must be init" ) ;

         rc = item._cb->fini() ;
         if ( rc )
         {
            PD_LOG( PDWARNING, "Fini cb[%d,%s] failed, rc: %d",
                    item._cb->cbType(), item._cb->cbName(), rc ) ;
            /// Not goto error, continue do the other cbs
         }

         /// set to null
         item._stat = CB_OPR_NULL ;
      }

      return SDB_OK ;
   }

   void _pmdContainer::onConfigChange( UINT32 changeID )
   {
      VEC_CB::iterator it ;
      for ( it = _vecCB.begin() ; it != _vecCB.end() ; ++it )
      {
         (*it)._cb->onConfigChange() ;
      }
   }

   void _pmdContainer::onConfigSave()
   {
      VEC_CB::iterator it ;
      for ( it = _vecCB.begin() ; it != _vecCB.end() ; ++it )
      {
         (*it)._cb->onConfigSave() ;
      }
   }

}

