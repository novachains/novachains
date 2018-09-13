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

   Source File Name = pmdDef.hpp

   Descriptive Name = Process MoDel Engine Dispatchable Unit Event Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure for events that
   used as inter-EDU communications.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_DEF_HPP__
#define PMD_DEF_HPP__

#include "core.hpp"
#include "oss.hpp"

namespace engine
{
   /*
      pmdEDUEventTypes define
   */
   enum pmdEDUEventTypes
   {
      PMD_EDU_EVENT_NONE = 0 ,
      PMD_EDU_EVENT_TERM ,        // terminate EDU
      PMD_EDU_EVENT_RESUME,       // resume a EDU, the data is startEDU's argv
      PMD_EDU_EVENT_MSG,          // pmd msg event
      PMD_EDU_EVENT_TIMEOUT,      // pmd edu timeout,

      PMD_EDU_EVENT_MAX
   } ;

   /*
      pmdEDUMemTypes define
   */
   enum pmdEDUMemTypes
   {
      PMD_EDU_MEM_NONE     = 0,   // Memory is not unknow
      PMD_EDU_MEM_ALLOC    = 1,   // Memory is by SDB_OSS_MALLOC
      PMD_EDU_MEM_SELF     = 2    // Memory is by pmdEDU::allocBuff
   } ;

   /*
      PMD_EVENT_MESSAGES define, for different event
   */
   union PMD_EVENT_MESSAGES
   {
      // for PMD_EDU_EVENT_TIMEOUT
      struct timeoutMsg
      {
         UINT64   timerID ;
         UINT32   interval ;
         UINT64   occurTime ;
      } timeoutMsg ;
   } ;

   typedef union PMD_EVENT_MESSAGES PMD_EVENT_MESSAGES ;

   /*
      _pmdEDUEvent define
   */
   class _pmdEDUEvent : public SDBObject
   {
   public :
      pmdEDUEventTypes  _eventType ;
      UINT64            _userData ;
      pmdEDUMemTypes    _dataMemType ;
      void              *_Data ;

      _pmdEDUEvent ( pmdEDUEventTypes type = PMD_EDU_EVENT_NONE,
                     pmdEDUMemTypes dataMemType = PMD_EDU_MEM_NONE,
                     void *data = NULL,
                     UINT64 userData = 0 )
      {
         _reset ( type, dataMemType, data, userData ) ;
      }
      _pmdEDUEvent( const _pmdEDUEvent &rhs )
      {
         _reset ( rhs._eventType, rhs._dataMemType, rhs._Data, rhs._userData ) ;
      }
      _pmdEDUEvent& operator=( const _pmdEDUEvent &rhs )
      {
         _reset( rhs._eventType, rhs._dataMemType, rhs._Data, rhs._userData ) ;
         return *this ;
      }
      void reset ()
      {
         _reset () ;
      }

   protected:
      void _reset ( pmdEDUEventTypes type = PMD_EDU_EVENT_NONE,
                    pmdEDUMemTypes dataMemType = PMD_EDU_MEM_NONE,
                    void *data = NULL,
                    UINT64 userData = 0 )
      {
         _eventType  = type ;
         _dataMemType= dataMemType ;
         _Data       = data ;
         _userData   = userData ;
      }

   } ;

   typedef _pmdEDUEvent pmdEDUEvent ;


   #define PMD_INVALID_EDUID              ( 0 )

   /*
      PMD_EDU_TYPES define
   */
   enum PMD_EDU_TYPES
   {
      PMD_EDU_MIN                         = 0,
      /// {{ Begin user define

      PMD_EDU_TYPE_MAIN,
      PMD_EDU_IOSVC,
      PMD_EDU_CBMAIN,

      /// End user define }}
      PMD_EDU_MAX
   } ;

   /*
      PMD_EDU_STATUS define
   */
   enum PMD_EDU_STATUS
   {
      PMD_EDU_CREATING = 0,
      PMD_EDU_RUNNING,
      PMD_EDU_WAITING,
      PMD_EDU_IDLE,

      PMD_EDU_UNKNOW,
      PMD_EDU_STATUS_MAXIMUM = PMD_EDU_UNKNOW
   } ;

   #define PMD_IS_EDU_CREATING(x)      ( PMD_EDU_CREATING == x )
   #define PMD_IS_EDU_RUNNING(x)       ( PMD_EDU_RUNNING  == x )
   #define PMD_IS_EDU_WAITING(x)       ( PMD_EDU_WAITING  == x )
   #define PMD_IS_EDU_IDLE(x)          ( PMD_EDU_IDLE     == x )

   /*
      PMD_NODE_ROLE STR DEFINE
   */
   #define PMD_NODE_WITNESS_STR           "witness"
   #define PMD_NODE_FULL_STR              "full"
   #define PMD_NODE_USER_STR              "user"
   #define PMD_NODE_SPV_STR               "spv"

   /*
      PMD_NODE_TYPE_STR DEFINE
   */
   #define PMD_TYPE_DN_STR                "novad"
   #define PMD_TYPE_CM_STR                "novacm"

   /*
      PMD_NODE_STATUS_STR DEFINE
   */
   #define PMD_NODE_NORMAL_STR            "Normal"
   #define PMD_NODE_SHUTDOWN_STR          "Shutdown"
   #define PMD_NODE_REBUILDING_STR        "Rebuilding"
   #define PMD_NODE_PULLINGUP_STR         "PullingUp"

   /*
      PMD_DATA_STATUS_STR DEFINE
   */
   #define PMD_DATA_NORMAL_STR            "Normal"
   #define PMD_DATA_REPAIR_STR            "Repairing"
   #define PMD_DATA_FAULT_STR             "Fault"

   /*
      PMD_NODE_MODE_STR DEFINE
   */
   #define PMD_NODE_MODE_READONLY_STR     "Readonly"
   #define PMD_NODE_MODE_DEACTIVATED_STR  "Deactivated"

   /*
      define
   */
   #define PMD_CONF_DIR_NAME           "conf"
   #define PMD_DFT_CONF                "nova.conf"
   #define PMD_CURRENT_PATH            "."

   #define ENGINE_NPIPE_PREFIX         "novad_engine_"
#if defined (_LINUX)
   #define PROC_PATH                   "/proc"
   #define PROC_CMDLINE_PATH_FORMAT    PROC_PATH"/%s/cmdline"
   #define ENGINE_NPIPE_PREFIX_BW      "novad_engine_bw_"
#endif // _LINUX

   #define ENGINE_NPIPE_MSG_PID        "$pid"
   #define ENGINE_NPIPE_MSG_SHUTDOWN   "$shutdown"
   #define ENGINE_NPIPE_MSG_TYPE       "$type"
   #define ENGINE_NPIPE_MSG_ROLE       "$role"
   #define ENGINE_NPIPE_MSG_PATH       "$path"
   #define ENGINE_NPIPE_MSG_ENDPIPE    "$endpipe"
   #define ENGINE_NPIPE_MSG_STARTTIME  "$starttime"

   /*
      PMD Option define
   */
   #define PMD_ADD_PARAM_OPTIONS_BEGIN( desc )  desc.add_options()
   #define PMD_ADD_PARAM_OPTIONS_END ;
   #define PMD_COMMANDS_STRING( a, b )       (string(a) +string( b)).c_str()

   /*
      PMD_NODE_TYPE define
   */
   enum PMD_NODE_TYPE
   {
      PMD_TYPE_DN  = 1,
      PMD_TYPE_CM,

      PMD_NODE_TYPE_MAX
   } ;

   /*
      other define
   */
   #define PMD_IPADDR_LEN              ( 40 )
   #define PMD_CLIENTNAME_LEN          ( 64 )

}

#endif // PMD_DEF_HPP__

