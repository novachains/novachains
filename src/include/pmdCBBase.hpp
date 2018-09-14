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

   Source File Name = pmdCBBase.hpp

   Descriptive Name = Data Management Service Header

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/08/2018  Xu Jianhui  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef PMD_CB_BASE_HPP_
#define PMD_CB_BASE_HPP_

#include "pmdInterface.hpp"
#include "pmdObjBase.hpp"
#include "pmdEPFactory.hpp"
#include "pmdTool.hpp"

namespace engine
{

   /*
      _pmdCBBase define
   */
   class _pmdCBBase : public _pmdObjBase, public _IPmdCB
   {
      public:
         _pmdCBBase() {}
         virtual ~_pmdCBBase() {}

      public:
         virtual INT32   runCBMain( _IPmdResource *pResource,
                                    IPmdExecutor *cb )
         {
            INT32 rc = SDB_OK ;
            pmdEDUEvent eventData;
            INT64 timeSpan = 0 ; /// usec
            INT64 maxMsgTime = getMaxProcMsgTime() * 1000000 ;
            INT64 maxEventTime = getMaxProcEventTime() * 1000000 ;
            INT64 *pMsgTimeSpan = maxMsgTime >= 0 ? &timeSpan : NULL ;
            INT64 *pEventTimeSpan = maxEventTime >= 0 ? &timeSpan : NULL ;

            attachCB( cb ) ;

            //Wait event msg and dispatch msg
            while ( !cb->isDisconnected() )
            {
               if ( cb->waitEvent( eventData, OSS_ONE_SEC, TRUE ) )
               {
                  cb->resetForLoopOnce() ;

                  if ( PMD_EDU_EVENT_TERM == eventData._eventType )
                  {
                     PD_LOG ( PDDEBUG, "EDU[%s(%lld), Name:%s] is terminated",
                              getEDUName( cb->getType() ), cb->getID(),
                              cb->getName() ) ;
                  }
                  //Dispatch event msg to cb manager
                  else if ( PMD_EDU_EVENT_MSG == eventData._eventType )
                  {
                     //restore handle
                     dispatchMsg( (NET_HANDLE)eventData._userData,
                                  (MsgHeader*)(eventData._Data),
                                  pMsgTimeSpan ) ;
                     if ( pMsgTimeSpan && timeSpan > maxMsgTime )
                     {
                        MsgHeader *pMsg = (MsgHeader*)(eventData._Data) ;
                        PD_LOG( PDWARNING, "[%s] Process msg[opCode:[%d]%d, "
                                "requestID: %lld, TID: %d, Len: %d] "
                                "over %d us",
                                name(), IS_REPLY_TYPE(pMsg->opCode),
                                GET_REQUEST_TYPE(pMsg->opCode),
                                pMsg->requestID, pMsg->TID,
                                pMsg->messageLength, timeSpan ) ;
                     }
                  }
                  else
                  {
                     dispatchEvent( &eventData, pEventTimeSpan ) ;
                     if ( pEventTimeSpan && timeSpan > maxEventTime )
                     {
                        PD_LOG( PDWARNING, "[%s] Process event[type:%d] "
                                "over %d us", name(),
                                eventData._eventType, timeSpan ) ;
                     }
                  }
         
                  //Relase memory
                  pmdEduEventRelase( eventData, cb ) ;
                  eventData.reset () ;
               }
            }

            detachCB( cb ) ;
            return rc ;
         }

   } ;
   typedef _pmdCBBase pmdCBBase ;

} ;

#endif //PMD_CB_BASE_HPP_


