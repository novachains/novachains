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

   Source File Name = msgMessageFormat.hpp

   Descriptive Name = Message Client Header

   When/how to use: this program may be used on binary and text-formatted
   versions of Messaging component. This file contains message structure for
   client-server communication.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/10/2016  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef MSGMESSAGE_FORMAT_HPP_
#define MSGMESSAGE_FORMAT_HPP_

#include "msg.hpp"
#include <string>

using namespace std;

const CHAR* serviceID2String( UINT32 serviceID ) ;

string routeID2String( MsgRouteID routeID ) ;
string routeID2String( UINT64 nodeID ) ;

///  get the msg type string desp
const CHAR* msgType2String( MSG_TYPE msgType, BOOLEAN isCommand = FALSE ) ;

/*
   Message to string functions
*/
#define MSG_HEADER_MASK_LEN            0x00000001
#define MSG_HEADER_MASK_OPCODE         0x00000002
#define MSG_HEADER_MASK_TID            0x00000004
#define MSG_HEADER_MASK_ROUTEID        0x00000008
#define MSG_HEADER_MASK_REQID          0x00000010

#define MSG_MASK_ALL                   0xFFFFFFFF

string  msg2String( const MsgHeader *pMsg,
                    UINT32 headMask = MSG_MASK_ALL,
                    UINT32 expandMask = MSG_MASK_ALL ) ;

/*
   Message expand to string functions
*/
typedef void (*EXPAND_MSG_2_STRING_FUNC)( stringstream &ss,
                                          const MsgHeader *pMsg,
                                          UINT32 expandMask ) ;

#endif // MSGMESSAGE_FORMAT_HPP_

