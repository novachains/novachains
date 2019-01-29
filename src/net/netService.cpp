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

   Source File Name = netFrame.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of PD component. This file contains declare of PD functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          17/12/2018  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/

#include "netService.hpp"
#include "pdTrace.hpp"
#include "netTrace.hpp"
#include "netMsgHandler.hpp"

namespace engine
{

   _netServiceItem::_netServiceItem( _netMsgHandler* handler, MsgRouter& router, MsgParser* parser, NET_START_THREAD_FUNC pFunc )
   :
   _router(router)
//   _parser(parser)
   {
      if ( handler )
      {
	 _pHandler = handler ;
         hdAllocated = FALSE ;
      }
      else
      {
	 _pHandler = SDB_OSS_NEW _netCommHandler() ;
	 hdAllocated = TRUE ;
      }

      _pRouteAgent = SDB_OSS_NEW _netRouteAgent( _pHandler, _router.getRoute(), parser ) ;

//      _pThread = new boost::thread (boost::bind( &_netRouteAgent::run , _pRouteAgent)) ;
      _pRouteAgent->startThread(&_pThread, pFunc) ;

   }

   _netServiceItem::~_netServiceItem()
   {
      if ( hdAllocated )
      {
	 delete _pHandler ;
      }
      _pHandler = NULL  ;
      _pRouteAgent->stop() ;
      _pThread->join() ;
      delete _pRouteAgent ;
      _pRouteAgent = NULL ;
   }

   INT32 _netServiceItem::listen(const _MsgRouteID &id)
   {
      INT32 rc = _pRouteAgent->listen( id ) ;
      return rc ;
   }

   INT32 _netServiceItem::send(const MsgStream &msg)
   {
      INT32 rc = SDB_OK ;
      _MsgRouteID id = msg.getRouteID() ;
      char* pBuffer  = msg.serialize() ;
      INT32 bufLen = msg.getLen() ;
      rc = _pRouteAgent->sendBuf(id, pBuffer, bufLen) ;
      if (pBuffer)
      {
	 SDB_OSS_FREE(pBuffer) ;
      }
      return rc ;

   }

   void _netServiceItem::close(const _MsgRouteID &id)
   {
      _pRouteAgent->close(id) ;
   }

/*   INT32 _netServiceItem::closeAll()
   {
      INT32 rc = SDB_OK ;
      set<_MsgRouterID> idList ;

      _router.getIDList( idList ) ;
      set<_MsgRouterID>::const_iterator itr = idList.begin() ;
      while ( itr != idList.end() )
      {
	 rc = close( *itr ) ;
	 if ( rc )
	 {
	    break ;
	 }
	 itr++ ;
      } 

      return rc ;
   } */

   void _netServiceItem::closeAll()
   {
      _pRouteAgent->disconnectAll() ;
   }

   netServiceItem * _netSvcManager::createSvcItem( _netMsgHandler * pHandler, MsgRouter& router, MsgParser * parser, NET_START_THREAD_FUNC pFunc) 
   {
      netServiceItem * pItem = SDB_OSS_NEW netServiceItem( pHandler, router, parser, pFunc ) ;
      if ( pItem )
      {
	  _mtx.get() ;
          itemList.push_back(pItem) ;
	  _mtx.release() ;
      }
      return pItem ;
   }

   void _netSvcManager::freeSvcItem( netServiceItem ** ppItem)
   {
      SDB_ASSERT( *ppItem, " NULL net service item") ;
      _mtx.get() ;
      for ( int i = 0; i< itemList.size(); i++)
      {
          if (*ppItem == itemList[i])
          {
             itemList.erase(itemList.begin() + i) ;
             break ;
          }
          SDB_ASSERT( i != (itemList.size() - 1) , " item not in the active list " ) ;
      }
      _mtx.release() ;
      delete *ppItem ;
      *ppItem = NULL ;
   }

}
