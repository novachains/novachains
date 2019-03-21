/*******************************************************************************
   Copyright (C) 2011-2014 SequoiaDB Ltd.
   This program is free software: you can redistribute it and/or modify
   it under the term of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warrenty of
   MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.
   Source File Name = dms.hpp
   Descriptive Name = Data Management Service Header
   When/how to use: this program may be used on binary and text-formatted
   versions of data management component. This file contains structure for
   dms Reccord ID (RID).
   Dependencies: N/A
   Restrictions: N/A
   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2012  TW  Initial Draft
   Last Changed =
*******************************************************************************/
#ifndef DMS_HPP_
#define DMS_HPP_
#include "core.hpp"
#include "oss.hpp"

#include <string>

namespace engine
{
#define DMS_COLLECTION_SPACE_NAME_SZ      127

#define DMS_PAGE_SIZE4K        4096    // 4K
#define DMS_PAGE_SIZE8K        8192    // 8K
#define DMS_PAGE_SIZE16K       16384   // 16K
#define DMS_PAGE_SIZE32K       32768   // 32K
#define DMS_PAGE_SIZE64K       65536   // 64K

#define DMS_PAGE_SIZE64B       64
#define DMS_PAGE_SIZE256B      256

#define DMS_PAGE_SIZE128K      131072  // 128K
#define DMS_PAGE_SIZE256K      262144  // 256K
#define DMS_PAGE_SIZE512K      524288  // 512K

#define DMS_PAGE_SIZE_DFT      DMS_PAGE_SIZE64K
#define DMS_PAGE_SIZE_MAX      DMS_PAGE_SIZE64K
#define DMS_PAGE_SIZE_BASE     DMS_PAGE_SIZE4K

#define DMS_DEFAULT_LOB_PAGE_SZ  DMS_PAGE_SIZE256K
#define DMS_DO_NOT_CREATE_LOB    0

#define DMS_MAX_PG             (128*1024*1024)
#define DMS_MAX_SZ(x)          (((UINT64)DMS_MAX_PG)*(x))

#define DMS_SEGMENT_SZ         (128*1024*1024)
#define DMS_SEGMENT_PG(x)      (DMS_SEGMENT_SZ/(x))

#define DMS_MAX_EXTENT_SZ      DMS_SEGMENT_SZ
#define DMS_MIN_EXTENT_SZ(x)   (x)
#define DMS_BEST_UP_EXTENT_SZ  (16*1024*1024)

#define DMS_COMPRESS_RATIO_THRESHOLD      ( 95 )

#define DMS_ID_KEY_NAME        "_id"

#define DMS_COLLECTION_NAME_SZ      127
#define DMS_COLLECTION_MAX_INDEX    64
#define DMS_MME_SLOTS               4096
#define DMS_COLLECTION_FULL_NAME_SZ \
   ( DMS_COLLECTION_SPACE_NAME_SZ + DMS_COLLECTION_NAME_SZ + 1 )

#define DMS_INVALID_SUID            -1
#define DMS_INVALID_CLID            ~0
#define DMS_INVALID_OFFSET          -1
#define DMS_INVALID_EXTENT          -1
#define DMS_INVALID_MBID            65535
#define DMS_INVALID_PAGESIZE        0
#define DMS_INVALID_LOGICCSID       0xffffffff
#define DMS_INVALID_LOGICCLID       0xffffffff

#define DMS_DATA_SU_EXT_NAME        "data"
#define DMS_INDEX_SU_EXT_NAME       "idx"
#define DMS_LOB_DATA_SU_EXT_NAME    "lobd"
#define DMS_LOB_META_SU_EXT_NAME    "lobm"

#define SDB_DMSTEMP_NAME            "SYSTEMP"
#define DMS_TEMP_NAME_PATTERN       "%s%04d"

#define DMS_INDEX_SORT_BUFFER_MIN_SIZE     32

#define DMS_CAP_EXTENT_SZ           (32 * 1024 * 1024)
#define DMS_CAP_EXTENT_BODY_SZ      ( DMS_CAP_EXTENT_SZ - DMS_EXTENT_METADATA_SZ )

}

#endif /* DMS_HPP_ */
