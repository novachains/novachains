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

   Source File Name = netRoute.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of PD component. This file contains declare of PD functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef NETROUTE_HPP_
#define NETROUTE_HPP_

#include "core.hpp"
#include "oss.hpp"
#include "ossLatch.hpp"
#include "netDef.hpp"
#include <map>
#include <set>

using namespace std ;

namespace engine
{
   class _netRoute : public SDBObject
   {

      friend class  _netMsgRouter ;
      public:
         _netRoute(){}
         ~_netRoute() ;
         INT32 route( const _MsgRouteID &id,
                      CHAR *host, UINT32 hostLen,
                      CHAR *service, UINT32 svcLen ) ;

         INT32 route( const _MsgRouteID &id,
                      _netRouteNode &node ) ;

         INT32 route( const CHAR* host,
                      const CHAR* service,
                      MSG_ROUTE_SERVICE_TYPE type,
                      _MsgRouteID &id ) ;

       private:
         /// return err when update an existing node.
         INT32 update( const _MsgRouteID &id,
                       const CHAR *host,
                       const CHAR *service,
                       BOOLEAN *newAdd = NULL ) ;
         INT32 update( const _MsgRouteID &id,
                       const _netRouteNode &node,
                       BOOLEAN *newAdd = NULL ) ;
         INT32 update( const _MsgRouteID &oldID,
                       const _MsgRouteID &newID ) ;

         void  del( const _MsgRouteID &id, BOOLEAN &hasDel ) ;

         void  clear() ;

      public:
         OSS_INLINE void setLocal( const _MsgRouteID &id )
         {
            _local = id ;
         }

         OSS_INLINE const _MsgRouteID &local()
         {
            return _local ;
         }

      private:
         map<UINT64, _netRouteNode> _route ;
         _MsgRouteID _local ;
         _ossSpinSLatch _mtx ;
   };

   class _netMsgRouter : public SDBObject
   {
      public:
         _netMsgRouter(const _MsgRouteID &id) ; 

      public:
         INT32 createRouterItem( const _MsgRouteID &id, const CHAR *host, const CHAR *service) ;

         INT32 deleteRouterItem( const _MsgRouteID &id ) ;

         INT32 updateRouterItem ( const _MsgRouteID &id, const CHAR *host, const CHAR *service) ;

//         void getIDList( set<_MsgRouteID> &idList  ) ;

	 OSS_INLINE _netRoute* getRoute()
	 {
	    return &_routerMap ;
	 }

      private:
         _netRoute _routerMap ;
   } ;
   typedef _netMsgRouter MsgRouter ;

}


#endif

