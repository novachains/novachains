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

   Source File Name = msg.h

   Descriptive Name =

   When/how to use:

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef MSG_H__
#define MSG_H__
#pragma warning( disable: 4200 )
#include "core.h"

#define MAKE_REPLY_TYPE(type)       (INT32)((UINT32)type | 0x80000000)
#define IS_REPLY_TYPE(type)         (INT32)((UINT32)type >> 31 )
#define GET_REQUEST_TYPE(type)      (INT32)((UINT32)type & 0x7FFFFFFF)

/*
   define message type
*/
enum MSG_TYPE
{
   // system msg
   MSG_HEARTBEAT                       = 1,
   MSG_HEARTBEAT_RES                   = MAKE_REPLY_TYPE(MSG_HEARTBEAT),

   MSG_PACKET                          = 2,
   MSG_PACKET_RES                      = MAKE_REPLY_TYPE(MSG_PACKET),

   //shard msg
   MSG_BS_MSG_REQ                      = 1000,
   MSG_BS_MSG_RES                      = MAKE_REPLY_TYPE(MSG_BS_MSG_REQ),

   /// common msg
   MSG_COM_BEGIN                       = 5000,
   MSG_COM_REMOTE_DISC                 = 5001,
   MSG_COM_SESSION_INIT_REQ            = 5002,
   MSG_COM_SESSION_INIT_RSP            = MAKE_REPLY_TYPE(MSG_COM_SESSION_INIT_REQ),
   MSG_COM_END                         = 5999,

   MSG_NULL                            = 999999        //reserved
} ;

// 4 bytes aligned, otherwise incompatible with java client
#pragma pack(4)

/// 8 bytes
union _MsgRouteID
{
   struct
   {
      UINT32 groupID ;
      UINT16 nodeID ;
      UINT16 serviceID ;
   } columns;

   UINT64 value ;
} ;

typedef union _MsgRouteID MsgRouteID ;
#define MSG_INVALID_ROUTEID  0

// system info request packet is very special since we do NOT know the endianess
// of server and client
// Therefore we are using a special length "-1" to indicate the request, and we
// are going to reply the real system info
#define MSG_SYSTEM_INFO_LEN                  0xFFFFFFFF
#define MSG_SYSTEM_INFO_EYECATCHER           0xFFFEFDFC
#define MSG_SYSTEM_INFO_EYECATCHER_REVERT    0xFCFDFEFF

struct _MsgSysInfoHeader
{
   SINT32 specialSysInfoLen ;
   UINT32 eyeCatcher ;
   SINT32 realMessageLength ;
} ;
typedef struct _MsgSysInfoHeader MsgSysInfoHeader ;

struct _MsgSysInfoRequest
{
   MsgSysInfoHeader header ;
} ;
typedef struct _MsgSysInfoRequest MsgSysInfoRequest ;

struct _MsgSysInfoReply
{
   MsgSysInfoHeader header ;
   INT32            osType ;
   CHAR             pad[112] ; // total 128 bytes for reply
} ;
typedef struct _MsgSysInfoReply MsgSysInfoReply ;
// end system info requests

typedef enum _MSG_ROUTE_SERVICE_TYPE
{
   MSG_ROUTE_LOCAL_SERVICE = 0,
   MSG_ROUTE_REPL_SERVICE,
   MSG_ROUTE_SHARD_SERVCIE,
   MSG_ROUTE_REST_SERVICE,
   MSG_ROUTE_TEST,

   MSG_ROUTE_SERVICE_TYPE_MAX
}MSG_ROUTE_SERVICE_TYPE;

/*
   Current message version
*/
#define MSG_CURRENT_VERSION            ( 1 )

/*
   Message header define
   32 bytes
*/
struct _MsgHeader
{
   UINT32      messageLength ;   // total message size, including this
   SINT32      opCode ;          // operation code
   UINT32      TID ;             // client thead id
   MsgRouteID  routeID ;         // route id 8 bytes
   UINT64      requestID ;       // identifier for this message
   UINT8       version ;         // version
	BYTE        padding[3] ;      // reserved
} ;
typedef struct _MsgHeader MsgHeader ;

struct _MsgOpMsg
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   // whatever the size of the package indicated in header
   // minus the sizeof(MsgHeader) bytes
   CHAR      msg[1] ;     // message body
} ;
typedef struct _MsgOpMsg MsgOpMsg ;

// Followed by numReturned BSON objects
struct _MsgOpReply
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   // 28-31 bytes
   SINT64    contextID ;   // context id if client need to get more
   // 32-35 bytes
   SINT32    flags ;      // reply flags
   // 36-39 bytes
   // 1. In most cases, startFrom (>=0) is where "this" reply is starting
   // in the context given by contextID
   // 2. If flags is SDB_CLS_NOT_PRIMARY, startFrom might be the new primary
   //    node to re-send the request
   // 3. If startFrom (<0) is between RTN_CTX_PROCESSOR_BEGIN and
   //    RTN_CTX_PROCESSOR_END, it is the type of data dispatcher which is
   //    used to process "this" reply
   SINT32    startFrom ;
   // 40-43 bytes
   SINT32    numReturned ;// number of records returned in the reply
} ;
typedef struct _MsgOpReply MsgOpReply ;

typedef struct _MsgComSessionInitReq
{
   MsgHeader   header ;
   MsgRouteID  dstRouteID ;
   /// The new infomation( with the version 2.0 and more )
   MsgRouteID  srcRouteID ;
   UINT32      localIP ;         /// local ip of the hostname
   UINT32      peerIP ;          /// 0, not used
   UINT16      localPort ;       /// the port of svcname
   UINT16      peerPort ;        /// 0, not used
   UINT32      localTID ;        /// local TID
   UINT64      localSessionID ;  /// local eduid
   CHAR        reserved[8] ;
   CHAR        data[0] ;         /// BSON DATA( usename, passwd and so on...)
} MsgComSessionInitReq ;


#pragma pack()

#endif // MSG_H__

