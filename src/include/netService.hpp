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
#include "netMsgStream.hpp"
#include "boost/thread.hpp"

#include <map>
#include <vector>

using namespace std ;
namespace engine
{
   class _netMsgHandler ;

   class _netServiceItem : public SDBObject
   {
      public:
	 _netServiceItem( _netMsgHandler* pHandler, MsgRouter& router, MsgParser* parser, NET_START_THREAD_FUNC pFunc = NULL ) ;
	 ~_netServiceItem() ;

      public:
	 INT32 listen(const _MsgRouteID& id) ;

	 INT32 send(const MsgStream& msg) ;

	 void close(const _MsgRouteID& id) ;

	 void closeAll() ;

      private:
	 boost::thread* _pThread ;
	 _netMsgHandler* _pHandler ;
	 _netRouteAgent* _pRouteAgent ;
	 BOOLEAN hdAllocated ;
//	 MsgParser  *_parser ;
	 MsgRouter  _router ;
   } ;
   typedef _netServiceItem netServiceItem ;

   class _netSvcManager : public SDBObject
   {

      public:
         ~_netSvcManager() ;
	 netServiceItem* createSvcItem( _netMsgHandler* pHandler, MsgRouter& router, MsgParser*  parser, NET_START_THREAD_FUNC pFunc = NULL) ;

	 void freeSvcItem( netServiceItem** ppItem) ;

      private:
	 vector<netServiceItem*> itemList ;
	 _ossSpinSLatch _mtx ;
   } ;

}

#endif 
