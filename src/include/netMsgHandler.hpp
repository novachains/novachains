/******************************************************************************


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

   Source File Name = pd.hpp

   Descriptive Name = Problem Determination Header

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

#ifndef NETMSGHANDLER_HPP_
#define NETMSGHANDLER_HPP_

#include "core.hpp"
#include "oss.hpp"
#include "netDef.hpp"

namespace engine
{
   /*
      _netMsgHandler define
   */
   class _netMsgHandler : public SDBObject
   {
      public:
        _netMsgHandler(){}
        virtual ~_netMsgHandler(){}
      public:
        virtual INT32   handleMsg( const NET_HANDLE &handle,
                                   const _MsgHeader *header,
                                   const CHAR *msg )
	{
	   return 0 ;
	}

	virtual INT32   handleStream( const _MsgRouteID &id,
				      MsgStream *pMsg ) = 0 ;

        virtual void    handleClose( const NET_HANDLE &handle,
                                     _MsgRouteID id )
        {
        }

        virtual void    handleConnect( const NET_HANDLE &handle,
                                       _MsgRouteID id,
                                       BOOLEAN isPositive )
        {
        }

        virtual void    onStop() {}

   } ;

   typedef _netMsgHandler INetMsgHandler ;

   class _netCommHandler : public _netMsgHandler
   {
      public:
        _netCommHandler(){}
        virtual ~_netCommHandler(){}
      public:
        virtual INT32   handleStream( const _MsgRouteID &id,
                                      MsgStream *pMsg )
        {
	   return 0 ;
        }

   } ; 

}

#endif // NETMSGHANDLER_HPP_

