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

   Source File Name = netService.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of Net component. This file contains declare of Net functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          14/12/2018  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef NET_SERVICE_HPP__
#define NET_SERVICE_HPP__

#include "core.hpp"
#include "oss.hpp"
#include "netDef.hpp"
#include "netRouteAgent.hpp"

#include <map>
#include <vector>

using namespace std ;
namespace engine
{

   class _netMsgStream : SDBObject
   {
      public:
	 _netMsgStream () {}
	 virtual ~_netMsgStream () {}

      public:
	 virtual UINT32 getSeq() = 0 ;
	 virtual UINT32 getSenderID() = 0 ;
	 virtual UINT32 getRouteID() = 0 ;
	 virtual UINT32 getReceiver() = 0 ;
	 virtual UINT32 getLen() = 0 ;
	 virtual char * serialize() = 0 ;
   } ;
   typedef _netMsgStream MsgStream ;

   class _netMsgParser : SDBObject
   {
      public:
      	 _netMsgParser() {}
      	 virtual ~_netMsgParser() {}

      public:
         virtual INT32 push(const char * pByte) = 0 ;

	 virtual INT32 get(MsgStream ** ppMsg) = 0 ;
   } ;
   typedef _netMsgParser MsgParser

   class _netMsgRouter : SDBObject
   {
      friend INT32 _netServiceItem::closeAll() ;
//      public:
//	 _netMsgRouter() ;
//	 ~_netMsgRouter() ;

      public:
	 void createRouterItem( _netRouteNode &routeNode ) ;

	 void deleteRouterItem( const _MsgRouteID &id ) ;

	 _netRouteNode * getRouterItem( const _MsgRouteID &id ) ;

	 void updateRouterItem ( const _MsgRouterID &id) ;

      private:
	 map<_MsgRouteID, _netRouteNode>  _routerMap ;
   } ;
   typedef _netMsgRouter MsgRouter ;

   class _netServiceItem : SDBObject
   {
      public:
	 _netServiceItem( _netMsgHandler * pHandler, MsgRouter * pRouter, MsgParser * pParser ) ;
	 ~_netServiceItem() ;

      public:
	 INT32 listen(const _MsgRouteID &id) ;

	 INT32 send(const MsgStream &msg) ;

	 INT32 close(const _MsgRouteID &id) ;

	 INT32 closeAll() ;

      private:

	 EDUID _eduID ;
	 _netMsgHandler * _handler ;
	 MsgRouter * _router ;
	 MsgParser * _parser ;
	 _netRouteAgent * _routeAgent ;
   } ;
   typedef _netServiceItem netServiceItem ;

   class _netSvcManager : SDBObject
   {

      public:
	 netServiceItem * createSvcItem( _netMsgHandler * pHandler, MsgRouter * pRouter, MsgParser * pParser) ;

	 void freeSvcItem( netServiceItem ** ppItem) ;

      private:
	vector<netServiceItem *> itemList ;
   }

}

#endif 
