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

   Source File Name = netMsgStream.hpp

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
#ifndef NET_MSGSTREAM_HPP__
#define NET_MSGSTREAM_HPP__

#include "core.hpp"
#include "oss.hpp"
#include "msg.hpp"


namespace engine
{

   class _netMsgStream : public SDBObject
   {
      public:
         _netMsgStream () {}
         virtual ~_netMsgStream () {}

      public:
         virtual UINT32 getSeq() const = 0 ;
         virtual _MsgRouteID getSenderID() const = 0 ;
         virtual _MsgRouteID getRouteID() const = 0 ;
         virtual UINT32 getReceiver() const = 0 ;
         virtual UINT32 getLen() const = 0 ;
         virtual char* serialize() const = 0 ;
         virtual char* getData() const = 0 ;
   } ;
   typedef _netMsgStream MsgStream ;

   class _netMsgParser :public SDBObject
   {
      public:
         _netMsgParser() {}
         virtual ~_netMsgParser() {}

      public:
	 // copy the actual object using a point of base type
	 virtual _netMsgParser* clone() = 0 ;

	 // get maximum length of the message
	 virtual UINT32 getLen() = 0 ;

	 // parse the received msg and return a boolean to indicate whether the msg ends
         virtual BOOLEAN push(const char* pBuf, const std::size_t& dataRecvd) = 0 ;

         virtual _netMsgStream* get(const char* pBuf) = 0 ;
   } ;
   typedef _netMsgParser MsgParser ;

}
#endif
