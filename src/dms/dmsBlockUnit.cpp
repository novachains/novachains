/*******************************************************************************


   Copyright (C) 2011-2019 SequoiaDB Ltd.

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

   Source File Name = dmsBlockUnit.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DMS component. This file contains implement of DMS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          24/06/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/

#include "dmsBlockUnit.hpp"

namespace engine
{

   /*
     dmsBlockUnit implementation
   */
   _dmsBlockUnit::_dmsBlockUnit ( const CHAR *pSUName,
                                  UINT32 csUniqueID,
                                  UINT32 sequence,
                                  INT32 pageSize,
                                  DMS_STORAGE_TYPE type )
   
   {
      SDB_ASSERT ( pSUName, "name can't be null" ) ;

      if ( 0 == pageSize )
      {
         pageSize = DMS_PAGE_SIZE_DFT ;
      }

      _storageInfo._pageSize = pageSize ;
      ossStrncpy( _storageInfo._suName, pSUName, DMS_SU_NAME_SZ ) ;
      _storageInfo._suName[DMS_SU_NAME_SZ] = 0 ;
      _storageInfo._csUniqueID = csUniqueID ;
      _storageInfo._sequence = sequence ;
      // make secret value
      _storageInfo._secretValue = ossPack32To64( (UINT32)time(NULL),
                                                 (UINT32)(ossRand()*239641) ) ;
      _storageInfo._type = type ;

   }


   INT32 _dmsBlockUnit::open( const CHAR *pDataPath )
   {
      INT32 rc = SDB_OK ;
      CHAR dataFileName[DMS_SU_FILENAME_SZ + 1] = {0} ;

      ossSnprintf( dataFileName, DMS_SU_FILENAME_SZ, "%s.%d.%s",
                   _storageInfo._suName, _storageInfo._sequence,
                   DMS_DATA_SU_EXT_NAME ) ;

      _pDataSu = SDB_OSS_NEW _dmsBlockData( dataFileName, &_storageInfo) ;

      rc = _pDataSu->openStorage( pDataPath ) ;

      if ( rc )
      {
         PD_LOG( PDERROR, "Open storage data su failed, rc: %d", rc ) ;
         goto error ;
      }

   done:
      return rc ;

   error:
      INT32 rcTmp = _pDataSu->removeStorage() ;
      if ( rcTmp )
      {
         PD_LOG( PDWARNING, "Failed to remove cs data file[%s] "
                    "rc: %d", dataFileName, rc ) ;
      }
      goto done ;
   }

   INT32 _dmsBlockUnit::insertRecord(const CHAR *pName,
                                     BSONObj &record,
                                     pmdEDUCB *cb,
                                     BOOLEAN mustOID,
                                     BOOLEAN canUnLock,
                                     dmsMBContext *context,
                                     INT64 position )
   {
      INT32 rc                     = SDB_OK ;
      BOOLEAN getContext           = FALSE ;

      if ( NULL == context )
      {
         SDB_ASSERT( pName, "Collection name can't be NULL" ) ;

         rc = _pDataSu->getMBContext( &context, pName, -1 ) ;
         PD_RC_CHECK( rc, PDERROR, "Get collection[%s] mb context failed, "
                      "rc: %d", pName, rc ) ;
         getContext = TRUE ;
      }

      rc = _pDataSu->insertRecord( context, record, cb, mustOID,
                                   canUnLock, position ) ;
      if ( rc )
      {
         goto error ;
      }

   done :
      if ( getContext && context )
      {
         _pDataSu->releaseMBContext( context ) ;
      }
      return rc ;
   error:
      goto done ;
   }
}
