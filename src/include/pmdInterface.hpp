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
   class _IPmdParam
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
   class _IPmdIOService
   {
      public:
         _IPmdIOService() {}
         virtual ~_IPmdIOService() {}

      public:
         virtual INT32     run() = 0 ;
         virtual void      stop() = 0 ;
   } ;
   typedef _IPmdIOService IPmdIOService ;

   /*
      _IPmdExecutor define
   */
   class _IPmdExecutor
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
         virtual ISession* getSession() = 0 ;
         virtual IRemoteSite* getRemoteSite() = 0 ;

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

   } ;
   typedef _IPmdExecutor IPmdExecutor ;

   class _IPmdResource ;
   /*
      _IPmdCB define
   */
   class _IPmdCB : public SDBObject, public IPmdRoot
   {
      public:
         _IPmdCB () {}
         virtual ~_IPmdCB () {}

         virtual PMD_CB_TYPE cbType() const = 0 ;
         virtual const CHAR* cbName() const = 0 ;

         virtual INT32  init ( _IPmdResource *pResource ) = 0 ;
         virtual INT32  active () = 0 ;
         virtual INT32  deactive () = 0 ;
         virtual INT32  fini () = 0 ;
         virtual void   onConfigChange() {}
         virtual void   onConfigSave() {}

   } ;
   typedef _IPmdCB IPmdCB ;

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

   } ;
   typedef _IPmdResource IPmdResource ;

}

#endif // PMD_INTERFACE_HPP__

