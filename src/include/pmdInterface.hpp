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

   Source File Name = pmdInterface.hpp

   Descriptive Name = Process MoDel Engine Dispatchable Unit Event Header

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains structure for events that
   used as inter-EDU communications.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          30/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_INTERFACE_HPP__
#define PMD_INTERFACE_HPP__

#include "core.hpp"
#include "oss.hpp"
#include <string>
#include <vector>

using namespace std ;

namespace engine
{

   /*
      PMD_CB_TYPE define
   */
   enum PMD_CB_TYPE
   {
      PMD_CB_MIN                 = 0,
      /// {{ Begin user control block define

      PMD_CB_PARAM,

      /// End user control block define }}
      PMD_CB_MAX
   } ;

   /*
      PMD_IF_TYPE DEFINE
   */
   enum PMD_IF_TYPE
   {
      PMD_IF_MIN                 = 0,
      /// {{ Begin user interface define

      PMD_IF_PARAM,

      // End user interface define }}
      PMD_IF_MAX
   } ;

   /*
      PMD_NODE_STATUS define
   */
   enum PMD_NODE_STATUS
   {
      PMD_NODE_NORMAL           = 0,
      PMD_NODE_SHUTDOWN,
      PMD_NODE_REBUILDING,
      PMD_NODE_PULLINGUP,

      PMD_NODE_STATUS_MAX
   } ;

   /*
      PMD_NODE_ROLE define
   */
   enum PMD_NODE_ROLE
   {
      PMD_NODE_WITNESS        = 1,  /// Witness node
      PMD_NODE_FULL,                /// Full node
      PMD_NODE_USER,                /// User node
      PMD_NODE_SPV,                 /// SPV node

      PMD_NODE_CM,                  /// cm node

      PMD_NODE_ROLE_MAX
   } ;

   /*
      Define PMD_NODE_MODE value
   */
   #define PMD_NODE_MODE_READONLY         0x00000001
   #define PMD_NODE_MODE_DEACTIVATED      0x00000002

   /*
      _IPmdRoot define
   */
   class _IPmdRoot
   {
      public:
         enum IF_PUB_MODE
         {
            IF_ALL,
            IF_LIST,
            IF_NULL
         } ;
      public:
         _IPmdRoot () {}
         virtual ~_IPmdRoot () {}

         virtual void*        queryIF( PMD_IF_TYPE type )
         {
            return NULL ;
         }
         virtual IF_PUB_MODE  publishMode() const
         {
            return IF_ALL ;
         }
         virtual void         publishList( vector<PMD_IF_TYPE> &vecIF ) const
         {
         }
   } ;
   typedef _IPmdRoot IPmdRoot ;

   /*
      _IPmdParam define
   */
   class _IPmdParam : public SDBObject
   {
      public:
         _IPmdParam() {}
         virtual ~_IPmdParam() {}

      public:
         virtual  BOOLEAN  hasField( const CHAR *pFieldName ) = 0 ;
         virtual  INT32    getFieldInt( const CHAR *pFieldName,
                                        INT32 &value,
                                        INT32 *pDefault = NULL ) = 0 ;
         virtual  INT32    getFieldUInt( const CHAR *pFieldName,
                                         UINT32 &value,
                                         UINT32 *pDefault = NULL ) = 0 ;
         virtual  INT32    getFieldStr( const CHAR *pFieldName,
                                        CHAR *pValue, UINT32 len,
                                        const CHAR *pDefault = NULL ) = 0 ;
         virtual  INT32    getFieldStr( const CHAR *pFieldName,
                                        string &strValue,
                                        const CHAR *pDefault = NULL ) = 0 ;

   } ;
   typedef _IPmdParam IPmdParam ;

   /*
      _IPmdIOService define
   */
   class _IPmdIOService : public SDBObject
   {
      public:
         _IPmdIOService() {}
         virtual ~_IPmdIOService() {}

      public:
         virtual INT32     run() = 0 ;
         virtual void      stop() = 0 ;
         virtual void      resetMon() = 0 ;
   } ;
   typedef _IPmdIOService IPmdIOService ;

   /*
      _IPmdClient define
   */
   class _IPmdClient : public SDBObject
   {
      public:
         _IPmdClient() {}
         virtual ~_IPmdClient() {}

      public:
         virtual const CHAR*  clientName() const = 0 ;

         virtual INT32        authenticate( const CHAR *username,
                                            const CHAR *password ) = 0 ;
         virtual void         logout() = 0 ;
         virtual INT32        disconnect() = 0 ;

         virtual BOOLEAN      isAuthed() const = 0 ;
         virtual BOOLEAN      isConnected() const = 0 ;
         virtual BOOLEAN      isClosed() const = 0 ;

         virtual BOOLEAN      isInterrupted() = 0 ;

         virtual INT32        send( const CHAR *pMsg,
                                    INT32 len,
                                    INT32 &sentLen,
                                    INT32 timeout,
                                    INT32 flags = 0,
                                    BOOLEAN block = TRUE ) = 0 ;

         virtual INT32        recv ( CHAR *pMsg,
                                     INT32 len,
                                     INT32 &receivedLen,
                                     INT32 timeout,
                                     INT32 flags = 0,
                                     BOOLEAN block = TRUE ) = 0 ;

         virtual UINT16       getLocalPort() const = 0 ;
         virtual const CHAR*  getLocalIPAddr() const = 0 ;
         virtual UINT16       getPeerPort() const = 0 ;
         virtual const CHAR*  getPeerIPAddr() const = 0 ;
         virtual const CHAR*  getUsername() const = 0 ;
         virtual const CHAR*  getPassword() const = 0 ;

         virtual const CHAR*  getFromIPAddr() const = 0 ;
         virtual UINT16       getFromPort() const = 0 ;

   } ;
   typedef _IPmdClient IPmdClient ;

   /*
      _IPmdSession define
   */
   class _IPmdSession : public SDBObject
   {
      public:
         _IPmdSession() {}
         virtual ~_IPmdSession() {}

      public:
         virtual const CHAR*        sessionName() const = 0 ;
         virtual IPmdClient*        getClient() = 0 ;

      protected:
         virtual void               _onAttach () {}
         virtual void               _onDetach () {}

   } ;
   typedef _IPmdSession IPmdSession ;

   /*
      _IPmdExecutor define
   */
   class _IPmdExecutor : public SDBObject
   {
      public:
         _IPmdExecutor() {}
         virtual ~_IPmdExecutor() {}

      public:

         /*
            Base Function
         */
         virtual EDUID        getID() const = 0 ;
         virtual UINT32       getTID() const = 0 ;
         virtual const CHAR*  getName() const = 0 ;

         /*
            Session Related
         */
         virtual IPmdSession* getSession() = 0 ;

         /*
            Status and Control
         */
         virtual BOOLEAN   isInterrupted () = 0 ;
         virtual BOOLEAN   isDisconnected () = 0 ;
         virtual BOOLEAN   isForced () = 0 ;

         virtual UINT32    getProcessedNum() const = 0 ;
         virtual void      incEventCount( UINT32 step = 1 ) = 0 ;

         virtual UINT32    getQueSize() = 0 ;

         /*
            Buffer Manager
         */
         virtual INT32     allocBuff( UINT32 len,
                                      CHAR **ppBuff,
                                      UINT32 *pRealSize = NULL ) = 0 ;

         virtual INT32     reallocBuff( UINT32 len,
                                        CHAR **ppBuff,
                                        UINT32 *pRealSize = NULL ) = 0 ;

         virtual void      releaseBuff( CHAR *pBuff ) = 0 ;

         virtual void*     getAlignedBuff( UINT32 size,
                                           UINT32 *pRealSize = NULL,
                                           UINT32 alignment =
                                           OSS_FILE_DIRECT_IO_ALIGNMENT ) = 0 ;
         
         virtual void      releaseAlignedBuff() = 0 ;

         /*
            Mon stat
         */
         virtual void      resetMon() = 0 ;

   } ;
   typedef _IPmdExecutor IPmdExecutor ;

   /*
      _IPmdExecutorMgr define
   */
   class _IPmdExecutorMgr : public SDBObject
   {
      public:
         _IPmdExecutorMgr() {}
         virtual ~_IPmdExecutorMgr() {}

      public:
         virtual INT32     startEDU( INT32 type,
                                     void *args,
                                     EDUID *pEDUID = NULL,
                                     const CHAR *pInitName = "" ) = 0 ;

         virtual void      addIOService( IPmdIOService *pIOService ) = 0 ;
         virtual void      delIOSerivce( IPmdIOService *pIOService ) = 0 ;

   } ;
   typedef _IPmdExecutorMgr IPmdExecutorMgr ;

   class _IPmdResource ;
   /*
      _IPmdCB define
   */
   class _IPmdCB : public SDBObject, public IPmdRoot
   {
      public:
         _IPmdCB () {}
         virtual ~_IPmdCB () {}

         virtual PMD_CB_TYPE  cbType() const = 0 ;
         virtual const CHAR*  cbName() const = 0 ;

         virtual void   dependentcy( vector<PMD_CB_TYPE> &cbs ) const {}

         virtual INT32  init ( _IPmdResource *pResource ) = 0 ;
         virtual INT32  active () = 0 ;
         virtual INT32  deactive () = 0 ;
         virtual INT32  fini () = 0 ;
         virtual void   onConfigChange() {}
         virtual void   onConfigSave() {}

   } ;
   typedef _IPmdCB IPmdCB ;

   /*
      _IPmdCBConfig define
   */
   class _IPmdCBConfig : public SDBObject
   {
      public:
         enum INSTALL_MODE
         {
            INSTALL_ALL,
            INSTALL_LIST
         } ;
      public:
         _IPmdCBConfig() {}
         virtual ~_IPmdCBConfig() {}

         virtual INSTALL_MODE getInstallMode() const
         {
            return INSTALL_LIST ;
         }
         virtual void getInstallList( vector<PMD_CB_TYPE> &vecCBs ) const
         {
         }
   } ;
   typedef _IPmdCBConfig IPmdCBConfig ;

   /*
      _IPmdCmdArg define
   */
   class _IPmdCmdArg : public SDBObject
   {
      public:
         _IPmdCmdArg() {}
         virtual ~_IPmdCmdArg() {}

         virtual INT32        getArgc() const = 0 ;
         virtual const CHAR** getArgv() const = 0 ;
         virtual const CHAR*  getCmdLine() const = 0 ;
   } ;
   typedef _IPmdCmdArg IPmdCmdArg ;

   /*
      _IPmdEnv define
   */
   class _IPmdEnv : public SDBObject
   {
      public:
         _IPmdEnv() {}
         virtual ~_IPmdEnv() {}

         virtual PMD_NODE_STATUS    getNodeStatus() const = 0 ;
         virtual BOOLEAN            isShutdown() const = 0 ;
         virtual BOOLEAN            isNormal() const = 0 ;
         virtual INT32              getShutdownCode() const = 0 ;

         virtual BOOLEAN            isActive() const = 0 ;

         virtual PMD_NODE_ROLE      getNodeRole() const = 0 ;

         virtual const CHAR*        getHostName() const = 0 ;

         virtual UINT64             getStartTime() const = 0 ;

         virtual void               getVersion( INT32 &ver,
                                                INT32 &subVer,
                                                INT32 &fixVer,
                                                INT32 &release,
                                                const CHAR **build ) const = 0 ;

         virtual const CHAR*        getDataPath() const = 0 ;

         virtual void               shutdownNode( INT32 shutdownCode ) = 0 ;
         virtual void               restartNode( INT32 code ) = 0 ;
   } ;
   typedef _IPmdEnv IPmdEnv ;

   /*
      _IPmdResource define
   */
   class _IPmdResource : public SDBObject, public IPmdRoot
   {
      public:
         _IPmdResource() {}
         virtual ~_IPmdResource() {}

      public:
         virtual IPmdParam*         getParam() = 0 ;
         virtual IPmdCmdArg*        getCmdArg() = 0 ;
         virtual IPmdEnv*           getEnv() = 0 ;

         virtual IPmdCB*            getCB( PMD_CB_TYPE type ) = 0 ;

   } ;
   typedef _IPmdResource IPmdResource ;

}

#endif // PMD_INTERFACE_HPP__

