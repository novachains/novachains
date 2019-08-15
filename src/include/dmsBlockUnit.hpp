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

   Source File Name = dmsBlockUnit.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DMS component. This file contains declare of DMS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          24/06/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef DMSBLOCKUNIT_HPP__
#define DMSBLOCKUNIT_HPP__

#include "dmsBlockData.hpp"

namespace engine
{
   class _dmsBlockUnit : public SDBObject
   {

      public:
         _dmsBlockUnit ( const CHAR *pSUName,
                           UINT32 csUniqueID,
                           UINT32 sequence,
                           INT32 pageSize = DMS_PAGE_SIZE_DFT,
                           DMS_STORAGE_TYPE type = DMS_STORAGE_NORMAL ) ;
         ~_dmsBlockUnit() { SDB_OSS_DEL _pDataSu ; }

         INT32    open( const CHAR *pDataPath, BOOLEAN createNew = TRUE ) ;

         _dmsBlockData *data() { return _pDataSu ; }

         INT32    insertRecord ( const CHAR *pName,
                                 BSONObj &record,
                                 pmdEDUCB *cb,
                                 BOOLEAN mustOID = TRUE,
                                 BOOLEAN canUnLock = TRUE,
                                 dmsMBContext *context = NULL,
                                 INT64 position = -1 ) ;

      private :
         _dmsBlockData                     *_pDataSu ;
         dmsStorageInfo                   _storageInfo ;
   };
   typedef _dmsBlockUnit dmsBlockUnit ;

}

#endif //DMSBLOCKUNIT_HPP

