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
          11/28/2012  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef MSG_H__
#define MSG_H__
#pragma warning( disable: 4200 )
#include "core.h"


#define MAKE_REPLY_TYPE(type)       (INT32)((UINT32)type | 0x80000000)
#define IS_REPLY_TYPE(type)         (INT32)((UINT32)type >> 31 )
#define GET_REQUEST_TYPE(type)      (INT32)((UINT32)type & 0x7FFFFFFF)

/// Reserved for cata delay event
#define CAT_DELAY_EVENT_TYPE        ( MAKE_REPLY_TYPE( 0 ) )

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
   MSG_BS_INSERT_REQ                   = 2002,
   MSG_BS_INSERT_RES                   = MAKE_REPLY_TYPE(MSG_BS_INSERT_REQ),
   MSG_BS_UPDATE_REQ                   = 2001,
   MSG_BS_UPDATE_RES                   = MAKE_REPLY_TYPE(MSG_BS_UPDATE_REQ),
   MSG_BS_SQL_REQ                      = 2003,
   MSG_BS_SQL_RES                      = MAKE_REPLY_TYPE(MSG_BS_SQL_REQ),
   MSG_BS_QUERY_REQ                    = 2004,
   MSG_BS_QUERY_RES                    = MAKE_REPLY_TYPE(MSG_BS_QUERY_REQ),
   MSG_BS_GETMORE_REQ                  = 2005,
   MSG_BS_GETMORE_RES                  = MAKE_REPLY_TYPE(MSG_BS_GETMORE_REQ),
   MSG_BS_DELETE_REQ                   = 2006,
   MSG_BS_DELETE_RES                   = MAKE_REPLY_TYPE(MSG_BS_DELETE_REQ),
   MSG_BS_KILL_CONTEXT_REQ             = 2007,
   MSG_BS_KILL_CONTEXT_RES             = MAKE_REPLY_TYPE(MSG_BS_KILL_CONTEXT_REQ),
   MSG_BS_DISCONNECT                   = 2008,
   MSG_BS_INTERRUPTE                   = 2009,
   MSG_BS_TRANS_BEGIN_REQ              = 2010,
   MSG_BS_TRANS_BEGIN_RSP              = MAKE_REPLY_TYPE(MSG_BS_TRANS_BEGIN_REQ),
   MSG_BS_TRANS_COMMIT_REQ             = 2011,
   MSG_BS_TRANS_COMMIT_RSP             = MAKE_REPLY_TYPE(MSG_BS_TRANS_COMMIT_REQ),
   MSG_BS_TRANS_ROLLBACK_REQ           = 2012,
   MSG_BS_TRANS_ROLLBACK_RSP           = MAKE_REPLY_TYPE(MSG_BS_TRANS_ROLLBACK_REQ),
   MSG_BS_TRANS_COMMITPRE_REQ          = 2015,
   MSG_BS_TRANS_COMMITPRE_RSP          = MAKE_REPLY_TYPE(MSG_BS_TRANS_COMMITPRE_REQ),
   MSG_BS_TRANS_INSERT_REQ             = 2016,
   MSG_BS_TRANS_INSERT_RSP             = MAKE_REPLY_TYPE(MSG_BS_TRANS_INSERT_REQ),
   MSG_BS_TRANS_UPDATE_REQ             = 2017,
   MSG_BS_TRANS_UPDATE_RSP             = MAKE_REPLY_TYPE(MSG_BS_TRANS_UPDATE_REQ),
   MSG_BS_TRANS_DELETE_REQ             = 2018,
   MSG_BS_TRANS_DELETE_RSP             = MAKE_REPLY_TYPE(MSG_BS_TRANS_DELETE_REQ),
   MSG_BS_AGGREGATE_REQ                = 2019,
   MSG_BS_AGGREGATE_RSP                = MAKE_REPLY_TYPE(MSG_BS_AGGREGATE_REQ),
   MSG_BS_INTERRUPTE_SELF              = 2020,
   MSG_BS_TRANS_QUERY_REQ              = 2021,
   MSG_BS_TRANS_QUERY_RES              = MAKE_REPLY_TYPE(MSG_BS_TRANS_QUERY_REQ),
   MSG_BS_TRANS_GETMORE_REQ            = 2022,
   MSG_BS_TRANS_GETMORE_RES            = MAKE_REPLY_TYPE(MSG_BS_TRANS_GETMORE_REQ),

   //catalogue msg
   //(MainController:3001~3099, catalogueManager:3100~3199, nodeManager:3200~3299)
   MSG_CAT_BEGIN                       = 3000,
   MSG_CAT_CATALOGUE_BEGIN             = 3100,

   // catalog query is searching for CAT_COLLECTION_INFO_COLLECTION collection
   // using whatever matcher/selector/orderBy and hint provided by the caller
   MSG_CAT_QUERY_CATALOG_REQ           = 3101,
   MSG_CAT_QUERY_CATALOG_RSP           = MAKE_REPLY_TYPE(MSG_CAT_QUERY_CATALOG_REQ),

   // create collectionspace need 1 argument
   // 1) CAT_COLLECTION_SPACE_NAME for collection space name
   MSG_CAT_CREATE_COLLECTION_SPACE_REQ = 3102,
   MSG_CAT_CREATE_COLLECTION_SPACE_RSP = MAKE_REPLY_TYPE(MSG_CAT_CREATE_COLLECTION_SPACE_REQ),

   // create collection need 1 or 2 arguments
   // 1) CAT_COLLECTION_NAME for full collection name
   // 2) (optional)CAT_SHARDINGKEY_NAME for sharding key def
   // format: {Field1:1, Field2:-1...}, 1 for asc, -1 for desc
   MSG_CAT_CREATE_COLLECTION_REQ       = 3103,
   MSG_CAT_CREATE_COLLECTION_RSP       = MAKE_REPLY_TYPE(MSG_CAT_CREATE_COLLECTION_REQ),

   // drop collection need 1 argument
   // 1) CAT_COLLECTION_NAME for full collection name
   MSG_CAT_DROP_COLLECTION_REQ         = 3104,
   MSG_CAT_DROP_COLLECTION_RSP         = MAKE_REPLY_TYPE(MSG_CAT_DROP_COLLECTION_REQ),

   // drop space need 1 argument
   // 1) CAT_COLLECTION_SPACE_NAME for collection name
   MSG_CAT_DROP_SPACE_REQ              = 3105,
   MSG_CAT_DROP_SPACE_RSP              = MAKE_REPLY_TYPE(MSG_CAT_DROP_SPACE_REQ),

   // query space info need 1 argument
   // 1) CAT_COLLECTION_SPACE_NAME for collection name
   MSG_CAT_QUERY_SPACEINFO_REQ         = 3106,
   MSG_CAT_QUERY_SPACEINFO_RSP         = MAKE_REPLY_TYPE(MSG_CAT_QUERY_SPACEINFO_REQ),

   // split prepare request need 5 or 4 arguments
   // 1) CAT_COLLECTION_NAME for collection string
   // 2) CAT_SOURCE_NAME for source group name string
   // 3) CAT_TARGET_NAME for target group name string
   // by range
   // 4) CAT_SPLITQUERY_NAME for begin search condition Object
   // 5) CAT_SPLITENDQUERY_NAME for end search condition Object
   // by percent
   // 4) CAT_SPLITPERCENT_NAME for search percent
   MSG_CAT_SPLIT_PREPARE_REQ           = 3107,
   MSG_CAT_SPLIT_PREPARE_RSP           = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_PREPARE_REQ),

   // split ready request need 4 or 5 arguments
   // 1) CAT_COLLECTION_NAME for collection string
   // 2) CAT_SOURCE_NAME for source group name string
   // 3) CAT_TARGET_NAME for target group name string
   // hash sharding and by percent :
   // 4) CAT_SPLITPERCENT_NAME for being split percent
   // else :
   // 4) CAT_SPLITVALUE_NAME for the key of split
   // 5) CAT_SPLITENDVALUE_NAME for the end key of split
   MSG_CAT_SPLIT_READY_REQ             = 3108,
   MSG_CAT_SPLIT_READY_RSP             = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_READY_REQ),

   // split cancel request need 1 or more arguments
   // by task id :
   // 1) CAT_TASKID_NAME for task id
   // else :
   // the same param with MSG_CAT_SPLIT_READY_REQ
   MSG_CAT_SPLIT_CANCEL_REQ            = 3109,
   MSG_CAT_SPLIT_CANCEL_RSP            = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_CANCEL_REQ),

   // split start/chgmeta/cleanup/finish request need 2 arguments
   // 1) CAT_COLLECTION_NAME for collection string
   // 2) CAT_TASKID_NAME for target group name string
   MSG_CAT_SPLIT_START_REQ             = 3110,
   MSG_CAT_SPLIT_START_RSP             = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_START_REQ),

   MSG_CAT_SPLIT_CHGMETA_REQ           = 3111,
   MSG_CAT_SPLIT_CHGMETA_RSP           = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_CHGMETA_REQ),

   MSG_CAT_SPLIT_CLEANUP_REQ           = 3112,
   MSG_CAT_SPLIT_CLEANUP_RSP           = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_CLEANUP_REQ),

   MSG_CAT_SPLIT_FINISH_REQ            = 3113,
   MSG_CAT_SPLIT_FINISH_RSP            = MAKE_REPLY_TYPE(MSG_CAT_SPLIT_FINISH_REQ),

   // task request search for SYSCAT.SYSTASKS collection using whatever search
   // condition provided by caller
   MSG_CAT_QUERY_TASK_REQ              = 3120,
   MSG_CAT_QUERY_TASK_RSP              = MAKE_REPLY_TYPE(MSG_CAT_QUERY_TASK_REQ),

   // alter collection request takes 2 parameters
   // 1) CAT_COLLECTION_NAME for full collection name
   // 2) CAT_OPTIONS         for options
   // In options there could be FIELD_NAME_W field exist
   MSG_CAT_ALTER_COLLECTION_REQ        = 3130,
   MSG_CAT_ALTER_COLLECTION_RSP        = MAKE_REPLY_TYPE(MSG_CAT_ALTER_COLLECTION_REQ),

   MSG_CAT_CRT_PROCEDURES_REQ          = 3131,
   MSG_CAT_CRT_PROCEDURES_RES          = MAKE_REPLY_TYPE(MSG_CAT_CRT_PROCEDURES_REQ),
   MSG_CAT_RM_PROCEDURES_REQ           = 3132,
   MSG_CAT_RM_PRODEDURES_RES           = MAKE_REPLY_TYPE(MSG_CAT_RM_PROCEDURES_REQ),

   MSG_CAT_LINK_CL_REQ                 = 3133,
   MSG_CAT_LINK_CL_RSP                 = MAKE_REPLY_TYPE(MSG_CAT_LINK_CL_REQ),
   MSG_CAT_UNLINK_CL_REQ               = 3134,
   MSG_CAT_UNLINK_CL_RSP               = MAKE_REPLY_TYPE(MSG_CAT_UNLINK_CL_REQ),

   // catalog domain related

   // create domain
   // CAT_DOMAIN_NAME for domain name
   // FIELD_NAME_OPTIONS for options
   //    FIELD_NAME_GROUP for group list
   MSG_CAT_CREATE_DOMAIN_REQ           = 3136,
   MSG_CAT_CREATE_DOMAIN_RSP           = MAKE_REPLY_TYPE(MSG_CAT_CREATE_DOMAIN_REQ),
   // drop domain
   // CAT_DOMAIN_NAME for domain name
   MSG_CAT_DROP_DOMAIN_REQ             = 3137,
   MSG_CAT_DROP_DOMAIN_RSP             = MAKE_REPLY_TYPE(MSG_CAT_DROP_DOMAIN_REQ),
   MSG_CAT_ALTER_DOMAIN_REQ            = 3138,
   MSG_CAT_ALTER_DOMAIN_RSP            = MAKE_REPLY_TYPE(MSG_CAT_ALTER_DOMAIN_REQ),

   // create index
   MSG_CAT_CREATE_IDX_REQ             = 3139,
   MSG_CAT_CREATE_IDX_RSP             = MAKE_REPLY_TYPE(MSG_CAT_CREATE_IDX_REQ),
   MSG_CAT_DROP_IDX_REQ               = 3140,
   MSG_CAT_DROP_IDX_RSP               = MAKE_REPLY_TYPE(MSG_CAT_DROP_IDX_REQ),

   // alter collection space
   MSG_CAT_ALTER_CS_REQ               = 3141,
   MSG_CAT_ALTER_CS_RSP               = MAKE_REPLY_TYPE( MSG_CAT_ALTER_CS_REQ ),

   // all messages for CATALOGUE must be smaller than MSG_CAT_CATALOGUE_END
   MSG_CAT_CATALOGUE_END               = 3199,

   MSG_CAT_NODE_BEGIN                  = 3200,

   // group message request need 1 argument that's in MsgCatGroupReq header
   // MsgCatGroupReq::id.columns for groupid, nodeid and serviceid
   // There's no additional BSON arguments attached
   MSG_CAT_GRP_REQ                     = 3201,
   MSG_CAT_GRP_RES                     = MAKE_REPLY_TYPE(MSG_CAT_GRP_REQ),

   // same for MSG_CAT_GRP_REQ
   MSG_CAT_CATGRP_REQ                  = 3202,
   MSG_CAT_CATGRP_RES                  = MAKE_REPLY_TYPE(MSG_CAT_CATGRP_REQ),

   // change primary node id for a group, 1 argument in MsgCatPrimaryChange
   // header
   // MsgCatPrimaryChange::newPrimary.columns for groupid and nodeid
   MSG_CAT_PAIMARY_CHANGE              = 3203,
   MSG_CAT_PAIMARY_CHANGE_RES          = MAKE_REPLY_TYPE(MSG_CAT_PAIMARY_CHANGE),

   // create a group, requires 1 argument
   // 1) CAT_GROUPNAME_NAME for group name
   MSG_CAT_CREATE_GROUP_REQ            = 3205,
   MSG_CAT_CREATE_GROUP_RSP            = MAKE_REPLY_TYPE(MSG_CAT_CREATE_GROUP_REQ),
   MSG_CAT_REG_REQ                     = 3206,
   MSG_CAT_REG_RES                     = MAKE_REPLY_TYPE(MSG_CAT_REG_REQ),
   MSG_CAT_CREATE_NODE_REQ             = 3207,
   MSG_CAT_CREATE_NODE_RSP             = MAKE_REPLY_TYPE(MSG_CAT_CREATE_NODE_REQ),
   MSG_CAT_ACTIVE_GROUP_REQ            = 3208,
   MSG_CAT_ACTIVE_GROUP_RSP            = MAKE_REPLY_TYPE(MSG_CAT_ACTIVE_GROUP_REQ),
   MSG_CAT_UPDATE_NODE_REQ             = 3209,
   MSG_CAT_UPDATE_NODE_RSP             = MAKE_REPLY_TYPE(MSG_CAT_ACTIVE_GROUP_REQ),

   // same for MSG_CAT_GRP_REQ
   MSG_CAT_NODEGRP_REQ                 = 3210,
   MSG_CAT_NODEGRP_RES                 = MAKE_REPLY_TYPE(MSG_CAT_NODEGRP_REQ),
   MSG_CAT_DEL_NODE_REQ                = 3211,
   MSG_CAT_DEL_NODE_RSP                = MAKE_REPLY_TYPE(MSG_CAT_DEL_NODE_REQ),
   MSG_CAT_RM_GROUP_REQ                = 3212,
   MSG_CAT_RM_GROUP_RES                = MAKE_REPLY_TYPE(MSG_CAT_RM_GROUP_REQ),
   MSG_CAT_GRP_CHANGE_NTY              = 3213,
   MSG_CAT_SHUTDOWN_GROUP_REQ          = 3214,
   MSG_CAT_SHUTDOWN_GROUP_RES          = MAKE_REPLY_TYPE(MSG_CAT_SHUTDOWN_GROUP_REQ),
   MSG_CAT_NODE_END                    = 3299,

   MSG_CAT_DC_BEGIN                    = 3300,
   MSG_CAT_ALTER_IMAGE_REQ             = 3301,
   MSG_CAT_ALTER_IMAGE_RSP             = MAKE_REPLY_TYPE(MSG_CAT_ALTER_IMAGE_REQ),
   MSG_CAT_DC_END                      = 3399,

   MSG_CAT_END                         = 3999,

   MSG_CLS_BEGIN                       = 4000,
   MSG_CLS_BEAT                        = 4001,
   MSG_CLS_BEAT_RES                    = MAKE_REPLY_TYPE(MSG_CLS_BEAT),
   MSG_CLS_BALLOT                      = 4002,
   MSG_CLS_BALLOT_RES                  = MAKE_REPLY_TYPE(MSG_CLS_BALLOT),
   MSG_CLS_SYNC_REQ                    = 4003,
   MSG_CLS_SYNC_RES                    = MAKE_REPLY_TYPE(MSG_CLS_SYNC_REQ),
   MSG_CLS_SYNC_NOTIFY                 = 4004,
   MSG_CLS_SYNC_VIR_REQ                = 4005,
   MSG_CLS_CONSULTATION_REQ            = 4006,
   MSG_CLS_CONSULTATION_RES            = MAKE_REPLY_TYPE(MSG_CLS_CONSULTATION_REQ),
   MSG_CLS_FULL_SYNC_BEGIN             = 4007,
   MSG_CLS_FULL_SYNC_BEGIN_RES         = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_BEGIN),
   MSG_CLS_FULL_SYNC_END               = 4008,
   MSG_CLS_FULL_SYNC_END_RES           = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_END),
   MSG_CLS_FULL_SYNC_META_REQ          = 4009,
   MSG_CLS_FULL_SYNC_META_RES          = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_META_REQ),
   MSG_CLS_FULL_SYNC_INDEX_REQ         = 4010,
   MSG_CLS_FULL_SYNC_INDEX_RES         = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_INDEX_REQ),
   MSG_CLS_FULL_SYNC_NOTIFY            = 4011,
   MSG_CLS_FULL_SYNC_NOTIFY_RES        = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_NOTIFY),
   MSG_CLS_FULL_SYNC_LEND_REQ          = 4012,
   MSG_CLS_FULL_SYNC_LEND_RES          = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_LEND_REQ),
   MSG_CLS_FULL_SYNC_TRANS_REQ         = 4013,
   MSG_CLS_FULL_SYNC_TRANS_RES         = MAKE_REPLY_TYPE(MSG_CLS_FULL_SYNC_TRANS_REQ),
   MSG_CLS_GINFO_UPDATED               = 4014,
   MSG_CLS_NODE_STATUS_NOTIFY          = 4015,
   MSG_CLS_END                         = 4999,

   /// common msg
   MSG_COM_BEGIN                       = 5000,
   MSG_COM_REMOTE_DISC                 = 5001,
   MSG_COM_SESSION_INIT_REQ            = 5002,
   MSG_COM_SESSION_INIT_RSP            = MAKE_REPLY_TYPE(MSG_COM_SESSION_INIT_REQ),
   MSG_COM_END                         = 5999,

   MSG_CM_REMOTE                       = 6000,

   MSG_AUTH_VERIFY_REQ                 = 7000,
   MSG_AUTH_VERIFY_RES                 = MAKE_REPLY_TYPE(MSG_AUTH_VERIFY_REQ),
   MSG_AUTH_CRTUSR_REQ                 = 7001,
   MSG_AUTH_CRTUSR_RES                 = MAKE_REPLY_TYPE(MSG_AUTH_CRTUSR_REQ),
   MSG_AUTH_DELUSR_REQ                 = 7002,
   MSG_AUTH_DELUSR_RES                 = MAKE_REPLY_TYPE(MSG_AUTH_DELUSR_REQ),

   MSG_LOB_BEGIN                       = 8000,
   MSG_BS_LOB_OPEN_REQ                 = 8001,
   MSG_BS_LOB_OPEN_RES                 = MAKE_REPLY_TYPE( MSG_BS_LOB_OPEN_REQ ),
   MSG_BS_LOB_WRITE_REQ                = 8002,
   MSG_BS_LOB_WRITE_RES                = MAKE_REPLY_TYPE( MSG_BS_LOB_WRITE_REQ ),
   MSG_BS_LOB_READ_REQ                 = 8003,
   MSG_BS_LOB_READ_RES                 = MAKE_REPLY_TYPE( MSG_BS_LOB_READ_REQ ),
   MSG_BS_LOB_REMOVE_REQ               = 8004,
   MSG_BS_LOB_REMOVE_RES               = MAKE_REPLY_TYPE( MSG_BS_LOB_REMOVE_REQ ),
   MSG_BS_LOB_UPDATE_REQ               = 8005,
   MSG_BS_LOB_UPDATE_RES               = MAKE_REPLY_TYPE( MSG_BS_LOB_UPDATE_REQ ),
   MSG_BS_LOB_CLOSE_REQ                = 8006,
   MSG_BS_LOB_CLOSE_RES                = MAKE_REPLY_TYPE( MSG_BS_LOB_CLOSE_REQ ),
   MSG_BS_LOB_LOCK_REQ                 = 8007,
   MSG_BS_LOB_LOCK_RES                 = MAKE_REPLY_TYPE( MSG_BS_LOB_LOCK_REQ ),
   MSG_BS_LOB_TRUNCATE_REQ             = 8008,
   MSG_BS_LOB_TRUNCATE_RES             = MAKE_REPLY_TYPE( MSG_BS_LOB_TRUNCATE_REQ ),
   MSG_LOB_END                         = 8999,

   MSG_OM_BEGIN                        = 9000,
   MSG_OM_UPDATE_TASK_REQ              = 9001,
   MSG_OM_UPDATE_TASK_RES              = MAKE_REPLY_TYPE( MSG_OM_UPDATE_TASK_REQ ),
   MSG_OM_END                          = 9999,

   MSG_SEADPT_BEGIN                    = 10000,
   MSG_SEADPT_UPDATE_IDXINFO_REQ       = 10001,
   MSG_SEADPT_UPDATE_IDXINFO_RES       = MAKE_REPLY_TYPE( MSG_SEADPT_UPDATE_IDXINFO_REQ ),

   MSG_SEADPT_END                      = 10999,

   MSG_NULL                            = 999999        //reserved
};

// 4 bytes aligned, otherwise incompatible with java client
#pragma pack(4)
#define OP_ERRNOFIELD     "errno"
#define OP_ERRDESP_FIELD  "description"
#define OP_ERR_DETAIL     "detail"

// maximized collection name length
#define OP_MAXNAMELENGTH   255

/*
messageLength : This is the total size of the message in bytes. This total
includes the 4 bytes that holds the message length.

requestID : This is a client or database-generated identifier that uniquely
identifies this message. For the case of client-generated messages (e.g.
CONTRIB:MSG_BS_QUERY_REQ and CONTRIB:OP_GET_MORE), it will be returned in the
responseTo field of the highest bit(1) message. Along with the reponseTo field
in responses, clients can use this to associate query responses with the
originating query.

responseTo : In the case of a message from the database, this will be the
requestID taken from the CONTRIB:MSG_BS_QUERY_REQ or CONTRIB:OP_GET_MORE messages from
the client. Along with the requestID field in queries, clients can use this to
associate query responses with the originating query.

opCode : Type of message. See the table below in the next section.
*/
/*
Clients can send all messages except for reply. This is reserved for
use by the database.
Note that only the CONTRIB:MSG_BS_QUERY_REQ and CONTRIB:OP_GET_MORE messages result in a
response from the database. There will be no response sent for any other
message.
You can determine if a message was successful with a getLastError command.
*/

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
   MSG_ROUTE_CAT_SERVICE,
   MSG_ROUTE_REST_SERVICE,
   MSG_ROUTE_OM_SERVICE,

   MSG_ROUTE_SERVICE_TYPE_MAX
}MSG_ROUTE_SERVICE_TYPE;
// 28 bytes
struct _MsgHeader
{
   SINT32 messageLength ; // total message size, including this
   SINT32 opCode ;        // operation code
   UINT32 TID ;           // client thead id
   MsgRouteID routeID ;   // route id 8 bytes
   UINT64 requestID ;     // identifier for this message
} ;
typedef struct _MsgHeader MsgHeader ;

struct _MsgInternalReplyHeader
{
   MsgHeader header ;
   SINT32     res ;
} ;
typedef struct _MsgInternalReplyHeader MsgInternalReplyHeader ;

// If set, the database will insert the supplied object into the collection if
// no matching document is found.
#define FLG_UPDATE_UPSERT           0x00000001
#define FLG_UPDATE_MULTIUPDATE      0x00000002
#define FLG_UPDATE_RETURNNUM        0x00000004
#define FLG_UPDATE_KEEP_SHARDINGKEY FLG_QUERY_KEEP_SHARDINGKEY_IN_UPDATE
// For Update, 3 BSON objects will be followed
// Selector + Updator + Hint
struct _MsgOpUpdate
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   INT32     version ;    // the collection catalog version
   SINT16    w ;          // need w nodes to operator succeed
   SINT16    padding;     // padding
   SINT32    flags ;      // update flag
   SINT32    nameLength ; // length of collection name, without '\0'
   // whatever number of bytes in nameLength
   CHAR      name[1] ;    // name of the object, ended with '\0'
} ;
typedef struct _MsgOpUpdate MsgOpUpdate ;

// if set, db will not stop bulk insert if one failed with dup key
#define FLG_INSERT_CONTONDUP  0x00000001
#define FLG_INSERT_RETURNNUM  0x00000002
// For Insert, 1 BSON object will be followed
// Inserted Data
struct _MsgOpInsert
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   INT32     version ;    // the collection catalog version
   SINT16    w ;          // need w nodes to operator succeed
   SINT16    padding;     // padding
   SINT32    flags ;      // insert flag
   SINT32    nameLength ; // length of collection name
   // whatever number of bytes in nameLength
   CHAR      name[1] ;    // name of the object
} ;
typedef struct _MsgOpInsert MsgOpInsert ;

// String output, each fields are separated by |, this is only used by Hadoop
// this flag is taking effect only with selector
#define FLG_QUERY_STRINGOUT                  0x00000001
// Allow query of replica slave
#define FLG_QUERY_SLAVEOK                    0x00000002
// Internal replication use only
#define FLG_QUERY_OPLOGREPLAY                0x00000004
// Context never timeout
#define FLG_QUERY_NOCONTEXTTIMEOUT           0x00000008
// If we are at end of the data, block for a while rather than return no data
// After timeout period, we do return as normal
#define FLG_QUERY_AWAITDATA                  0x00000010
// Allow partial reads if some of the shards are down
#define FLG_QUERY_PARTIALREAD                0x00000020
// Command[Create collection] if cs not exist,will create first
#define FLG_CREATE_WHEN_NOT_EXIST            0x00000040
// If use hint failed, query return failed
#define FLG_QUERY_FORCE_HINT                 0x00000080
// Enable paralled sub querys
#define FLG_QUERY_PARALLED                   0x00000100
// Find with return data in query response, can use for find one
#define FLG_QUERY_WITH_RETURNDATA            0x00000200
// explain query
#define FLG_QUERY_EXPLAIN                    0x00000400
// Query from the primary node( only use in inner )
#define FLG_QUERY_PRIMARY                    0x00000800
// query and modify
#define FLG_QUERY_MODIFY                     0x00001000
// query must use index by sort
#define FLG_QUERY_FORCE_IDX_BY_SORT          0x00002000
// query prepare more data
#define FLG_QUERY_PREPARE_MORE               0x00004000
// The sharding key in update rule is not filtered, when executing findAndUpdate.
#define FLG_QUERY_KEEP_SHARDINGKEY_IN_UPDATE 0x00008000

// For query takes 4 document
// Query + returnFieldSelector + orderBy + hint
struct _MsgOpQuery
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   INT32     version ;    // the collection catalog version
   SINT16    w ;          // need w nodes to operator succeed
   SINT16    padding;     // padding
   SINT32    flags ;      // query flag
   SINT32    nameLength ; // length of collection name
   SINT64    numToSkip ;  // number of rows to skip
   SINT64    numToReturn ;// number of rows to return
   // whatever number of bytes in nameLength
   CHAR      name[1] ;    // name of the object
} ;
typedef struct _MsgOpQuery MsgOpQuery ;

// get next few records
struct _MsgOpGetMore
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   SINT64    contextID ;  // context ID from reply
   SINT32    numToReturn ;// number of rows to return
} ;
typedef struct _MsgOpGetMore MsgOpGetMore ;

// remove one row instead of all matching records
#define FLG_DELETE_SINGLEREMOVE     0x00000001
#define FLG_DELETE_RETURNNUM        0x00000004
// take 2 BSON object
// Selector + hint
struct _MsgOpDelete
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   INT32     version ;    // the collection catalog version
   SINT16    w ;          // need w nodes to operator succeed
   SINT16    padding;     // padding
   SINT32    flags ;      // delete flags
   SINT32    nameLength ; // length of collection name
   // whatever number of bytes in nameLength
   CHAR      name[1] ;    // name of the object
} ;
typedef struct _MsgOpDelete MsgOpDelete ;

struct _MsgOpKillContexts
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   SINT32    ZERO ;       // reserved
   SINT32    numContexts ; // Number of contexts to be killed
   // whatever number of contexts * 4
   SINT64    contextIDs[1] ;
} ;
typedef struct _MsgOpKillContexts MsgOpKillContexts ;

struct _MsgOpKillAllContexts
{
   // 0-27 bytes
   MsgHeader header ;    // message header
} ;
typedef struct _MsgOpKillAllContexts MsgOpKillAllContexts ;

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

// indicates disconnection between client and server
// when this message is received by data nodes from coord, that means the
// current unit of work is finished
struct _MsgOpDisconnect
{
   // 0-27 bytes
   MsgHeader header ;
} ;
typedef struct _MsgOpDisconnect MsgOpDisconnect ;


struct _MsgCMRequest
{
   // 0-27 bytes
   MsgHeader header ;
   SINT32 remoCode ;     // remote operation code
   CHAR arguments[0] ;    // operation arguments
} ;
typedef struct _MsgCMRequest MsgCMRequest ;

// | -- msg header -- | -- sql -- |
// header.length = sizeof(header) + len(sql)
struct _MsgOpSql
{
   // 0-27 bytes
   MsgHeader header ;
} ;
typedef struct _MsgOpSql MsgOpSql ;

/// |-- header--|--{usrname:xx, passwd:xx}--|
struct _MsgAuthentication
{
   MsgHeader header ;
   CHAR      data[0] ;
} ;
typedef struct _MsgAuthentication MsgAuthentication ;

/// |-- header--|--{usrname:xx, passwd:xx}--|
struct _MsgAuthCrtUsr
{
   MsgHeader header ;
   CHAR      data[0] ;
} ;
typedef struct _MsgAuthCrtUsr MsgAuthCrtUsr ;

/// |-- header--|--{usrname:xx, passwd:xx}--|
struct _MsgAuthDelUsr
{
   MsgHeader header ;
   CHAR      data[0] ;
} ;
typedef struct _MsgAuthDelUsr MsgAuthDelUsr ;

typedef struct _MsgOpTransBegin
{
   MsgHeader header;
} MsgOpTransBegin;

typedef struct _MsgOpTransCommit
{
   MsgHeader header;
} MsgOpTransCommit;

typedef struct _MsgOpTransCommitPre
{
   MsgHeader header;
} MsgOpTransCommitPre;

typedef struct _MsgOpTransRollback
{
   MsgHeader header;
} MsgOpTransRollback;

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

typedef struct _MsgOpAggregate
{
   // 0-27 bytes
   MsgHeader header ;     // message header
   INT32     version ;    // the collection catalog version
   SINT16    w ;          // need w nodes to operator succeed
   SINT16    padding;     // padding
   SINT32    flags ;      // aggregate flag
   SINT32    nameLength ; // length of collection name
   // whatever number of bytes in nameLength
   CHAR      name[1] ;    // name of the object
}MsgOpAggregate;


/// read on primary node( use only in inner)
#define FLG_LOBREAD_PRIMARY               0x00000001
/// open with return data( when mode = read )
#define FLG_LOBOPEN_WITH_RETURNDATA       0X00000002
/// write or update in write operation( when mode = write )
#define FLG_LOBWRITE_OR_UPDATE            0x00000004

/// when it is open reg |MsgOpLob|bsonobj|
/// when it is open res |MsgOpReply|bsonobj|
/// when it is put req |MsgOpLob|_MsgLobTuple|data|
/// when it is put res |MsgOpReply|
/// when it is read req |MsgOpLob|_MsgLobTuple|
/// when it is read res |MsgOpReply|_MsgLobTuple|data|
typedef struct _MsgOpLob
{
   MsgHeader header ;
   INT32 version ;
   SINT16 w ;
   SINT16 padding ;
   SINT32 flags ;
   SINT64 contextID ;
   UINT32 bsonLen ;
} MsgOpLob ;

union _MsgLobTuple
{
   struct
   {
      UINT32 len ;
      UINT32 sequence ;
      SINT64 offset ;
   } columns ;

   CHAR data[16] ;
} ;
typedef union _MsgLobTuple MsgLobTuple ;

#define NODE_INSTANCE_ID_MIN        ( 0 )
#define NODE_INSTANCE_ID_UNKNOWN    ( NODE_INSTANCE_ID_MIN )
#define NODE_INSTANCE_ID_MAX        ( 256 )

#define PREFER_INSTANCE_MASTER_STR  "M"
#define PREFER_INSTANCE_SLAVE_STR   "S"
#define PREFER_INSTANCE_ANY_STR     "A"

#define PREFER_INSTANCE_MASTER_LOWSTR  "m"
#define PREFER_INSTANCE_SLAVE_LOWSTR   "s"
#define PREFER_INSTANCE_ANY_LOWSTR     "a"

#define PREFER_INSTANCE_RANDOM_STR  "random"
#define PREFER_INSTANCE_ORDERED_STR "ordered"

// Note: deprecated options, for old version of setSesstionAttr
typedef enum _PREFER_REPLICA_TYPE
{
   PREFER_REPL_TYPE_MIN = 0,
   PREFER_REPL_NODE_1 = 1,   // first node
   PREFER_REPL_NODE_2,
   PREFER_REPL_NODE_3,
   PREFER_REPL_NODE_4,
   PREFER_REPL_NODE_5,
   PREFER_REPL_NODE_6,
   PREFER_REPL_NODE_7,
   PREFER_REPL_MASTER,        // master node
   PREFER_REPL_SLAVE,         // any slave node
   PREFER_REPL_ANYONE,        // any node( include master )

   PREFER_REPL_TYPE_MAX
} PREFER_REPLICA_TYPE ;

#define CLS_RG_NODE_POS_INVALID        0xffffffff

#pragma pack()

#endif // MSG_H__

