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

   Source File Name = msgMessageFormat.cpp

   Descriptive Name = Messaging Client

   When/how to use: this program may be used on binary and text-formatted
   versions of Messaging component. This file contains functions for building
   and extracting data for client-server communication.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#include "core.hpp"
#include "msgMessageFormat.hpp"
#include "pd.hpp"
#include "utilStr.hpp"
#include "pdTrace.hpp"
#include "msgTrace.hpp"
#include <sstream>
#include <stddef.h>

using namespace engine;

const CHAR* serviceID2String( UINT32 serviceID )
{
   switch ( serviceID )
   {
      case MSG_ROUTE_LOCAL_SERVICE :
         return "LOCAL" ;
      case MSG_ROUTE_REPL_SERVICE :
         return "REPL" ;
      case MSG_ROUTE_SHARD_SERVCIE :
         return "SHARD" ;
      case MSG_ROUTE_REST_SERVICE :
         return "REST" ;
      default :
         break ;
   }
   return "UNKNOW" ;
}

string routeID2String( MsgRouteID routeID )
{
   stringstream ss ;
   ss << "{ GroupID:" << routeID.columns.groupID
      << ", NodeID:" << routeID.columns.nodeID
      << ", ServiceID:" << routeID.columns.serviceID
      << "(" << serviceID2String( routeID.columns.serviceID ) << ") }" ;
   return ss.str() ;
}

string routeID2String( UINT64 nodeID )
{
   return routeID2String( *(MsgRouteID*)&nodeID ) ;
}

const CHAR* msgType2String( MSG_TYPE msgType, BOOLEAN isCommand )
{
   return "UNKNOW" ;
}

/*
   Message to string functions
*/
typedef struct _msgMsgFuncItem
{
   INT32                      _opCode ;
   const CHAR                 *_opDesp ;
   EXPAND_MSG_2_STRING_FUNC   _pFunc ;
} msgMsgFuncItem ;

#define MSG_MAP_2_STRING_FUNC(opCode,pFunc) \
   { (INT32)opCode, #opCode, (EXPAND_MSG_2_STRING_FUNC)pFunc }

#define MSG_MAP_2_STRING_FUNC_WITH_DESP(opCode,opDesp,pFunc) \
   { (INT32)opCode, opDesp, (EXPAND_MSG_2_STRING_FUNC)pFunc }

msgMsgFuncItem* msgGetExpand2StringFunc( const MsgHeader *pMsg )
{
   static msgMsgFuncItem s_Entry[] = {
      /// Begin to map expand msg to string functions
      /// End map
      { MSG_NULL, NULL }
   } ;
   static UINT32 s_EntrySize = sizeof( s_Entry ) / sizeof( msgMsgFuncItem ) ;

   /// find the function
   for ( UINT32 i = 0 ; i < s_EntrySize ; ++i )
   {
      if ( s_Entry[ i ]._opCode == pMsg->opCode )
      {
         return &s_Entry[ i ] ;
      }
   }
   return NULL ;   
}

string msg2String( const MsgHeader *pMsg,
                   UINT32 headMask,
                   UINT32 expandMask )
{
   stringstream ss ;
   msgMsgFuncItem *pItem = msgGetExpand2StringFunc( pMsg ) ;

   /// First format header to string
   if ( MSG_HEADER_MASK_LEN & headMask )
   {
      ss << "Length: " << pMsg->messageLength << ", " ;
   }
   if ( MSG_HEADER_MASK_OPCODE & headMask )
   {
      ss << "OpCode: (" << IS_REPLY_TYPE( pMsg->opCode ) << ")"
         << GET_REQUEST_TYPE( pMsg->opCode ) ;
      if ( pItem && pItem->_opDesp )
      {
         ss << "(" << pItem->_opDesp << ")" ;
      }
      ss << "," ;
   }
   if ( MSG_HEADER_MASK_TID & headMask )
   {
      ss << "TID: " << pMsg->TID << ", " ;
   }
   if ( MSG_HEADER_MASK_ROUTEID & headMask )
   {
      ss << "RouteID: { GroupID:" << pMsg->routeID.columns.groupID
         << ", NodeID:" << pMsg->routeID.columns.nodeID
         << ", ServiceID:" << pMsg->routeID.columns.serviceID
         << "(" << serviceID2String( pMsg->routeID.columns.serviceID )
         << ") }," ;
   }
   if ( MSG_HEADER_MASK_REQID & headMask )
   {
      ss << "RequestID: " << pMsg->requestID << ", " ;
   }

   /// sencond format expand msg to string
   if ( pItem && pItem->_pFunc && 0 != expandMask )
   {
      pItem->_pFunc( ss, pMsg, expandMask ) ;
   }

   /// adjust
   string str = ss.str() ;
   utilStrRtrim( str ) ;
   UINT32 len = str.length() ;
   if ( len > 1 && str.at( len - 1 ) == ',' )
   {
      str.erase( len - 1 ) ;
   }
   return str ;
}


