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


namespace engine
{

   _netServiceItem::_netServiceItem( _netMsgHandler * pHandler, MsgRouter * pRouter, MsgParser * pParser )
   :
   _router(pRouter),
   _parser(pParser)
   {
      SDB_ASSERT(pRouter, "router cannot be NULL") ;
      SDB_ASSERT(pParser, "Parser cannot be NULL") ;
      if ( pHandler )
      {
	 _handler = pHandler ;
      }
      else
      {
	 _handler = new _netCommHandler() ;
      }

      _routeAgent = new _netRouteAgent( _handler ) ;

      _routeAgent.run() ;
   }

   _netServiceItem::~_netServiceItem()
   {
      _handler = NULL ;
      _router = NULL ;
      _parser = NULL ;
      _routeAgent.stop() ;
      delete _routeAgent ;
      _routeAgent = NULL ;
   }

   INT32 _netServiceItem::listen(const _MsgRouteID &id)
   {
      _routeAgent.listen( id ) ;
   }

   // TODO
   INT32 _netServiceItem::send(const MsgStream &msg)
   {
      // SUDO
      // get route id
      // syncConnect
      // call frame.syncconnect and get a net handle
      // get serialized data and length
      // send data through syncsendraw

   }

   // TODO
   INT32 _netServiceItem::close(const _MsgRouteID &id)
   {
      // _routeAgent.close(id) ; should be enough here, still thinking if we should have a interface
      // in this manner, and probably make me think to link routeID with NET_handle
   }

   INT32 _netServiceItem::closeAll()
   {
         INT32 rc = SDB_OK ;
         map<_MsgRouteID, _netRouteNode>::iterator it = _router->_routerMap.begin() ;

         while(it != _routerPtr->_routerMap.end())
         {
            if ( rc = close(it->first) != SDB_OK )
            {
               break ;
            }

            it++ ;
         }

         return rc ;
   }

   netServiceItem * _netSvcManager::createSvcItem( _netMsgHandler * pHandler, MsgRouter * pRouter, MsgParser * pParser) ;
   {
      netServiceItem * pItem = new netServiceItem( _netMsgHandler * pHandler, MsgRouter * pRouter, MsgParser * pParser ) ;
      if ( pItem )
      {
          itemList.push_back(pItem) ;
      }
      return pItem ;
   }

   void _netSvcManager::freeSvcItem( netServiceItem ** ppItem)
   {
      SDB_ASSERT( *ppItem, " NULL net service item") ;
      for ( int i = 0; i< itemList.size(); i++)
      {
          if (*ppItem == itemList[i])
          {
             itemList.erase(i) ;
             break ;
          }
          SDB_ASSERT( i != (itemList.size() - 1) , " item not in the active list " ) ;
      }
      delete *ppItem ;
      *ppItem = NULL ;
   }

}
