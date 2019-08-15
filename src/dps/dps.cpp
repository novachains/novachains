/*******************************************************************************


   Copyright (C) 2011-2019 NOVACHAIN Ltd.

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

   Source File Name = dps.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DPS component. This file contains implement of DPS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          08/05/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/

#include "dps.hpp"
#include "dpsDef.hpp"
#include "dpsLogRecordDef.hpp"

using namespace bson ;

namespace engine
{
   _dpsCB::_dpsCB()
   {
      _pLogMgr = SDB_OSS_NEW dpsLogMgr() ;
   }

   _dpsCB::~_dpsCB()
   {
      SDB_OSS_DEL _pLogMgr ;
   }

   INT32 _dpsCB::init( const CHAR *path, UINT32 pageNum )
   {
      INT32 rc = SDB_OK ;
      rc = _pLogMgr->init( path, pageNum ) ;
      return rc ;
   }

   INT32 _dpsCB::writeLog( dpsMergeInfo &info,
                           pmdEDUCB *cb,
                           UINT32 csLID,
                           dmsMBContext *context,
                           dmsExtentID extLID,
                           BOOLEAN needUnLock,
                           DMS_FILE_TYPE type,
                           UINT32 *clLID )
   {
      INT32 rc = SDB_OK ;
      info.setInfoEx( csLID,
                      NULL == clLID ?
                      context->clLID() : *clLID,
                      extLID, cb ) ;
      rc = _pLogMgr->preparePages( info ) ;
      if ( rc )
      {
         goto error ;
      }

      // release lock
      if ( needUnLock )
      {
         context->mbUnlock() ;
      }

      // write dps
      _pLogMgr->writeData( info ) ;

   done :
      return rc ;
   error :
      goto done ;
   }

   INT32 _dpsCB::writeLog( const CHAR *fullName,
                           const BSONObj &obj)
   {
      INT32 rc = SDB_OK ;
      dpsMergeInfo info ;
      dpsLogRecord &logRecord = info.getMergeBlock().record() ;

      rc = dpsInsert2Record( fullName, obj, logRecord) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to push data into log, rc = %d", rc ) ;

      rc = _pLogMgr->preparePages( info ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to prepare log page, rc = %d", rc ) ;

      _pLogMgr->writeData( info ) ;

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 checkAndAddTimeInfo( dpsLogRecord &record )
   {
      INT32 rc = SDB_OK ;
      UINT64 *timeMicroSeconds = NULL ;

      record.sampleTime() ;
      timeMicroSeconds = record.getTime() ;

      rc = record.push( DPS_LOG_PUBLIC_TIME, sizeof(*timeMicroSeconds),
                        (CHAR *)timeMicroSeconds ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed push time(%lld), rc = %d",
                   *timeMicroSeconds, rc ) ;

   done:
      return rc ;
   error:
      goto done ;
   }


   INT32 dpsInsert2Record( const CHAR *fullName,
                           const BSONObj &obj,
                           dpsLogRecord &record )
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT( NULL != fullName, "Collection name can't be NULL" ) ;
      dpsLogRecordHeader &header = record.head() ;
      header._type = LOG_TYPE_DATA_INSERT ;

      rc = record.push( DPS_LOG_PUBLIC_FULLNAME,
                        ossStrlen(fullName) + 1, // '1 for '\0'
                        fullName ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to push fullname to record, rc: %d", rc ) ;
         goto error ;
      }

      rc = record.push( DPS_LOG_INSERT_OBJ, obj.objsize(), obj.objdata() ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to push obj to record, rc: %d", rc ) ;
         goto error ;
      }

      rc = checkAndAddTimeInfo( record ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to add time info, rc = %d", rc ) ;

      header._length = record.alignedLen() ;
   done:
      return rc ;
   error:
      goto done ;
   }

}
