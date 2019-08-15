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

   Source File Name = dpsLogRecord.cpp

   Descriptive Name = Data Protection Services Log Record

   When/how to use: this program may be used on binary and text-formatted
   versions of DPS component. This file contains implementation for log record.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/05/2019  YW  Initial Draft

   Last Changed =

*******************************************************************************/
#include "core.hpp"
#include "dpsLogRecord.hpp"
#include "dms.hpp"
#include "../bson/bson.h"
#include "ossUtil.hpp"
#include "ossMem.h"
#include "dpsLogRecordDef.hpp"
#include "ossMemPool.hpp"
#include "pdTrace.hpp"

using namespace bson ;
namespace engine
{
#define DPS_RECORD_ELE_HEADER_LEN 5

#define DPS_GET_RECORD_TAG(a) \
        (*((DPS_TAG *)((CHAR *)(a))))

#define DPS_GET_RECORD_LENGTH(a) \
        (*(UINT32 *)((CHAR *)(a) + sizeof(DPS_TAG)))

#define DPS_GET_RECORD_VALUE( a ) \
        ((CHAR *)(a) + DPS_RECORD_ELE_HEADER_LEN )

   _dpsLogRecord::_dpsLogRecord ()
   :_write(0)
   {
      ossMemset ( _data, 0, sizeof(_data) ) ;
      _result = SDB_OK ;
   }

   _dpsLogRecord::_dpsLogRecord( const _dpsLogRecord &record )
   :_head(record._head),
    _write( record._write )
   {
      ossMemcpy( _data, record._data, sizeof(_data) ) ;
      ossMemcpy( _dataHeader, record._dataHeader, sizeof(_dataHeader) ) ;
      _result = record._result ;
   }

   _dpsLogRecord::~_dpsLogRecord ()
   {
//      clear() ;
   }

   _dpsLogRecord &_dpsLogRecord::operator=( const _dpsLogRecord &record )
   {
      _head = record._head ;
      ossMemcpy( _data, record._data, sizeof(_data) ) ;
      ossMemcpy( _dataHeader, record._dataHeader, sizeof(_dataHeader) ) ;
      _write = record._write ;
      _result = record._result ;
      return *this ;
   }

   UINT32 _dpsLogRecord::alignedLen() const
   {
      UINT32 len = 0 ;
      for ( UINT32 i = 0; i < DPS_MERGE_BLOCK_MAX_DATA; i++ )
      {
         if ( DPS_INVALID_TAG == _dataHeader[i].tag )
         {
            break ;
         }
         else
         {
            len += _dataHeader[i].len ;
            len += sizeof(_dpsRecordEle) ;
         }
      }

      return ossRoundUpToMultipleX(len + sizeof(dpsLogRecordHeader),
                                   sizeof(UINT32)) ;
   }

   void _dpsLogRecord::clear()
   {
      _clearTags() ;

      if ( DPS_INVALID_LSN_OFFSET != _head._lsn )
      {
         _head.clear() ;
      }

      _result = SDB_OK ;

      return ;
   }

   void _dpsLogRecord::_clearTags ()
   {
      if ( 0 != _write )
      {
         for ( UINT32 i = 0; i < _write; i++ )
         {
            _data[i] = NULL ;
            _dataHeader[i].tag = DPS_INVALID_TAG ;
            _dataHeader[i].len = 0 ;
         }

         _write = 0 ;
      }
   }

   INT32 _dpsLogRecord::loadRowBody()
   {
      INT32 rc = SDB_OK ;

      const CHAR * rowData = NULL ;
      UINT32 dataLen = 0 ;

      PD_CHECK( _head._length >= sizeof( dpsLogRecordHeader) &&
                _head._length <= DPS_RECORD_MAX_LEN,
                SDB_DPS_CORRUPTED_LOG, error, PDERROR,
                "the length of record is out of range: %d",
                _head._length) ;
      if ( LOG_TYPE_DUMMY == _head._type )
      {
         goto done ;
      }

      {
      _dpsLogRecord::iterator iter = find( DPS_LOG_ROW_ROWDATA ) ;
      if ( !iter.valid() )
      {
         goto done ;
      }

      // Complete element must contain 5 bytes at least: TAG(1) + Valuesize(4).
      // TLV must end with 0(1 byte).
      // So totalSize - 5 to avoid read the invalid bytes
      rowData = iter.value() ;
      dataLen = iter.len() - DPS_RECORD_ELE_HEADER_LEN ;

      // Tags were based on DPS_LOG_ROW_ROWDATA
      // Let's clear them and reload from beginning
      _clearTags() ;

      rc = loadBody( rowData, dataLen ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse row-record(rc=%d)!", rc ) ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dpsLogRecord::loadBody( const CHAR * pData, INT32 totalSize )
   {
      SDB_ASSERT( pData, "pData can't be null!" ) ;

      INT32 rc = SDB_OK ;
      INT32 loadSize = 0 ;
      const CHAR *location = pData ;
      while ( loadSize < totalSize )
      {
         DPS_TAG tag = DPS_GET_RECORD_TAG(location) ;
         UINT32 valueSize = DPS_GET_RECORD_LENGTH( location ) ;

         if ( DPS_MERGE_BLOCK_MAX_DATA == _write )
         {
            PD_LOG( PDERROR, "data num is larger than %d",
                    DPS_MERGE_BLOCK_MAX_DATA ) ;
            SDB_ASSERT( FALSE, "impossible" ) ;
            rc = SDB_DPS_CORRUPTED_LOG ;
            goto error ;
         }
         else if ( DPS_INVALID_TAG == tag )
         {
            /// the length might be changed. DPS_INVALID_TAG is a stop flag.
            break ;
         }
         else if ( (UINT32)( totalSize - loadSize ) < valueSize )
         {
            PD_LOG( PDERROR, "get a invalid value size:%d, total size: %d, "
                    "load size: %d", valueSize, totalSize, loadSize ) ;
            rc = SDB_DPS_CORRUPTED_LOG ;
            goto error ;
         }

         _dataHeader[_write].tag = tag ;
         _dataHeader[_write].len = valueSize ;
         _data[_write++] = DPS_GET_RECORD_VALUE(location)  ;
         loadSize += ( valueSize + DPS_RECORD_ELE_HEADER_LEN ) ;
         location += ( valueSize + DPS_RECORD_ELE_HEADER_LEN ) ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dpsLogRecord::load( const CHAR *pData )
   {
      SDB_ASSERT( NULL != pData, "impossible" ) ;
      INT32 rc = SDB_OK ;
      INT32 totalSize = 0 ;
      const CHAR *location = NULL ;
      _head = *(( dpsLogRecordHeader * )pData) ;

      if ( _head._length < sizeof( dpsLogRecordHeader ) ||
           DPS_RECORD_MAX_LEN < _head._length )
      {
         PD_LOG ( PDERROR, "the length of record is out of range: %d",
                  _head._length ) ;
         rc = SDB_DPS_CORRUPTED_LOG ;
         goto error ;
      }

      if ( LOG_TYPE_DUMMY == _head._type )
      {
         goto done ;
      }

      location = pData + sizeof( dpsLogRecordHeader ) ;

      /// may be byte-aligned.the length of each field is
      /// at least greater than 5.
      totalSize = _head._length
                  - sizeof( dpsLogRecordHeader )
                  - DPS_RECORD_ELE_HEADER_LEN ;
      rc = loadBody( location, totalSize ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse row-record(rc=%d)!", rc ) ;
   done:
      return rc ;
   error:
      _result = rc ;
      goto done ;
   }

   _dpsLogRecord::iterator _dpsLogRecord::find( DPS_TAG tag ) const
   {
      _dpsLogRecord::iterator itr( this ) ;

      /// rewrite this func when we have a big number of data.
      /// now it is only 10 at most.
      for ( UINT32 i = 0; i < DPS_MERGE_BLOCK_MAX_DATA; i++ )
      {
         if ( DPS_INVALID_TAG == _dataHeader[i].tag)
         {
            break ;
         }
         else if ( _dataHeader[i].tag  == tag )
         {
            itr._current = i ;
            break ;
         }
         else
         {
            /// continue ;
         }
      }
      return itr ;
   }

   INT32 _dpsLogRecord::push( DPS_TAG tag, UINT32 len, const CHAR *value )
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT( DPS_INVALID_TAG != tag , "impossible" ) ;
      if ( DPS_MERGE_BLOCK_MAX_DATA == _write )
      {
         PD_LOG( PDERROR, "data num is larger than %d",
                 DPS_MERGE_BLOCK_MAX_DATA ) ;
         rc = SDB_DPS_CORRUPTED_LOG ;
         goto error ;
      }
      else
      {
         _dataHeader[_write].tag = tag ;
         _dataHeader[_write].len = len ;
         _data[_write++] = value ;
      }
   done:
       return rc ;
   error:
       goto done ;
   }

   void _dpsLogRecord::sampleTime()
   {
      _timeMicroSeconds = ossGetCurrentMicroseconds() ;
   }

   // used to log dps, so must return the pointer and keep the lifecycle until
   // data is copied
   UINT64* _dpsLogRecord::getTime()
   {
      return &_timeMicroSeconds ;
   }
}
