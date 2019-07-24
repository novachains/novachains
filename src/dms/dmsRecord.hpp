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

   Source File Name = dmsRecord.hpp

   Descriptive Name = Data Management Service Record Header

   When/how to use: this program may be used on binary and text-formatted
   versions of data management component. This file contains structure for
   data record, including normal record and deleted record.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef DMSRECORD_HPP_
#define DMSRECORD_HPP_

#include "dms.hpp"
#include "../bson/bson.h"
#include "oss.hpp"
#include "ossUtil.hpp"
#include "utilCompression.hpp"

namespace engine
{
   // 2^24 = 16MB, shouldn't be changed without completely redesign the
   // dmsRecord structure this size includes metadata header

   #define DMS_RECORD_MAX_SZ           0x01000000

   // since DPS may need extra space for header, let's make sure the max size of
   // user record ( the ones need to log ) cannot exceed 16M-4K

   #define DMS_RECORD_USER_MAX_SZ      (DMS_RECORD_MAX_SZ-4096)

   /*
      _dmsRecordData define
   */
   class _dmsRecordData : public SDBObject
   {
      public:
         _dmsRecordData()
         {
            reset() ;
         }
         _dmsRecordData( const CHAR *data, UINT32 len,
                         UINT8 compressType = UTIL_COMPRESSOR_INVALID,
                         BOOLEAN isOrgData = TRUE )
         {
            _data = data ;
            _len = len ;

            _compressType = compressType ;

            if ( isOrgData )
            {
               _orgData = data ;
               _orgLen = len ;
            }
            else
            {
               _orgData = NULL ;
               _orgLen = 0 ;
            }
         }
         ~_dmsRecordData()
         {
         }

         BOOLEAN isEmpty() const { return !_data || 0 == _len ; }

         const CHAR* data() const { return _data ; }
         UINT32 len() const { return _len ; }

         const CHAR* orgData() const { return _orgData ; }
         UINT32 orgLen() const { return _orgLen ; }

         BOOLEAN isCompressed() const { return UTIL_COMPRESSOR_INVALID != _compressType ; }
         UINT8 getCompressType () const { return _compressType ; }
         FLOAT32 getCompressRatio() const
         {
            if ( isCompressed() && _len > 0 && _orgLen > 0 )
            {
               return ( (FLOAT32)_len ) / (FLOAT32)_orgLen ;
            }
            return 1.0 ;
         }

         void setData( const CHAR *data, UINT32 len,
                       UINT8 compressType = UTIL_COMPRESSOR_INVALID,
                       BOOLEAN isOrgData = TRUE )
         {
            _data = data ;
            _len = len ;

            _compressType = compressType ;

            if ( isOrgData )
            {
               _orgData = data ;
               _orgLen = len ;
            }
         }
         void setOrgData( const CHAR *orgData, UINT32 orgLen )
         {
            _orgData = orgData ;
            _orgLen = orgLen ;
         }

         void reset()
         {
            _data = NULL ;
            _len = 0 ;
            _compressType = UTIL_COMPRESSOR_INVALID ;
            _orgData = NULL ;
            _orgLen = 0 ;
         }
         void resetOrgData()
         {
            _orgData = NULL ;
            _orgLen = 0 ;
         }

      private:
         const CHAR     *_data ;
         UINT32         _len ;

         UINT8          _compressType ;

         const CHAR     *_orgData ;
         UINT32         _orgLen ;

   } ;
   typedef _dmsRecordData dmsRecordData ;

   /*
      Record Flag define:
   */
   // 0~3 bit for STATE
   #define DMS_RECORD_FLAG_NORMAL            0x00
   #define DMS_RECORD_FLAG_OVERFLOWF         0x01 // Overflow from record
   #define DMS_RECORD_FLAG_OVERFLOWT         0x02 // Overflow to record
   #define DMS_RECORD_FLAG_DELETED           0x04
   // 4~7 bit for ATTR
   #define DMS_RECORD_FLAG_COMPRESSED        0x10
   // some one wait X-lock, the last one who get X-lock will delete the record
   #define DMS_RECORD_FLAG_DELETING          0x80

   #define DMS_RECORD_METADATA_SZ   sizeof(_dmsRecord)

   /*
      _dmsRecord defined
   */
   class _dmsRecord : public SDBObject
   {
   public:
      union
      {
         CHAR     _recordHead[4] ;     // 1 byte flag, 3 bytes length - 1
         UINT32   _flag_and_size ;
      }                 _head ;
      dmsOffset         _myOffset ;
      dmsOffset         _previousOffset ;
      dmsOffset         _nextOffset ;
      /*
         Follow _nextOffset is:
            if overflow, is overflow rid(8bytes)
            else, is the data length(4 bytes).
                In compressed: 4bytes + data
                uncompressed:  data( the first 4bytes is bson size )
      */

      /*
         Get Functions
      */
      dmsOffset getMyOffset() const
      {
         return _myOffset ;
      }
      dmsOffset getPrevOffset() const
      {
         return _previousOffset ;
      }
      dmsOffset getNextOffset() const
      {
         return _nextOffset ;
      }
      BYTE getFlag() const
      {
         return (BYTE)_head._recordHead[ 0 ] ;
      }
      BYTE getState() const
      {
         return (BYTE)(getFlag() & 0x0F) ;
      }
      BYTE getAttr() const
      {
         return (BYTE)(getFlag() & 0xF0) ;
      }
      BOOLEAN isOvf() const
      {
         return  DMS_RECORD_FLAG_OVERFLOWF == getState() ;
      }
      BOOLEAN isOvt() const
      {
         return DMS_RECORD_FLAG_OVERFLOWT == getState() ;
      }
      BOOLEAN isDeleted() const
      {
         return DMS_RECORD_FLAG_DELETED == getState() ;
      }
      BOOLEAN isNormal() const
      {
         return DMS_RECORD_FLAG_NORMAL == getState() ;
      }
      BOOLEAN isCompressed() const
      {
         return getAttr() & DMS_RECORD_FLAG_COMPRESSED ;
      }
      BOOLEAN isDeleting() const
      {
         return getAttr() & DMS_RECORD_FLAG_DELETING ;
      }
      dmsRecordID getOvfRID() const
      {
         if ( isOvf() )
         {
            return *( const dmsRecordID* )( (const CHAR*)this +
                                            sizeof( _dmsRecord ) ) ;
         }
         return dmsRecordID() ;
      }

      UINT32 getSize() const
      {
#if defined (SDB_BIG_ENDIAN)
         return (((*((const UINT32*)this))&0x00FFFFFF)+1) ;
#else
         return (((*((const UINT32*)this))>>8)+1) ;
#endif // SDB_BIG_ENDIAN
      }

      UINT8 getCompressType () const
      {
#if defined (SDB_BIG_ENDIAN)
         return ( (const UINT8 *)this + DMS_RECORD_METADATA_SZ )[ 0 ] ;
#else
         return ( (const UINT8 *)this + DMS_RECORD_METADATA_SZ )[ 3 ] ;
#endif // SDB_BIG_ENDIAN
      }

      UINT32 getDataLength() const
      {
         return ( *(const UINT32 *)( (const CHAR*)this + DMS_RECORD_METADATA_SZ ) ) & 0x00FFFFFF ;
      }
      /*
         Get disk data only, if compressed, not uncompressed
      */
      const CHAR* getData() const
      {
         return isCompressed() ?
            ((const CHAR*)this+sizeof(UINT32)+DMS_RECORD_METADATA_SZ) :
            ((const CHAR*)this+DMS_RECORD_METADATA_SZ) ;
      }

      /*
         Set Functions
      */
      void setMyOffset( dmsOffset offset )
      {
         _myOffset = offset ;
      }
      void setPrevOffset( dmsOffset offset )
      {
         _previousOffset = offset ;
      }
      void setNextOffset( dmsOffset offset )
      {
         _nextOffset = offset ;
      }
      void  setFlag( BYTE flag )
      {
         _head._recordHead[ 0 ] = (CHAR)flag ;
      }
      void  setState( BYTE state )
      {
         _head._recordHead[ 0 ] = (CHAR)((state&0x0F)|getAttr()) ;
      }
      void setAttr( BYTE attr )
      {
         _head._recordHead[ 0 ] |= (BYTE)(attr&0xF0) ;
      }
      void unsetAttr( BYTE attr )
      {
         _head._recordHead[ 0 ] &= (BYTE)(~(attr&0xF0)) ;
      }
      void resetAttr()
      {
         unsetAttr( 0xF0 ) ;
      }
      void setOvf()
      {
         setState( DMS_RECORD_FLAG_OVERFLOWF ) ;
      }
      void setOvt()
      {
         setState( DMS_RECORD_FLAG_OVERFLOWT ) ;
      }
      void setNormal()
      {
         setState( DMS_RECORD_FLAG_NORMAL ) ;
      }
      void setDeleted()
      {
         setState( DMS_RECORD_FLAG_DELETED ) ;
      }
      void setCompressed()
      {
         setAttr( DMS_RECORD_FLAG_COMPRESSED ) ;
      }
      void unsetCompressed()
      {
         unsetAttr( DMS_RECORD_FLAG_COMPRESSED ) ;
      }
      void setDeleting()
      {
         setAttr( DMS_RECORD_FLAG_DELETING ) ;
      }
      void unsetDeleting()
      {
         unsetAttr( DMS_RECORD_FLAG_DELETING ) ;
      }
      void setOvfRID( const dmsRecordID &rid )
      {
         *((dmsRecordID*)((CHAR*)this+DMS_RECORD_METADATA_SZ)) = rid ;
      }
      void  setSize( UINT32 size )
      {
#if defined (SDB_BIG_ENDIAN)
      (*((UINT32*)this) = ((UINT32)getFlag()<<24) |
                          ((UINT32)((size)-1)&0x00FFFFFF)) ;
#else
      (*((UINT32*)this) = (UINT32)getFlag() | ((UINT32)((size)-1)<<8)) ;
#endif
      }

      void setCompressType ( UINT8 type )
      {
         *(UINT32 *)( (CHAR *)this + DMS_RECORD_METADATA_SZ ) |=
            ( ( (UINT32)type << 24 ) & 0xFF000000 ) ;
//#if defined (SDB_BIG_ENDIAN)
//         ( (UINT8 *)this + DMS_RECORD_METADATA_SZ )[ 0 ] = type ;
//#else
//         ( (UINT8 *)this + DMS_RECORD_METADATA_SZ )[ 3 ] = type ;
//#endif // SDB_BIG_ENDIAN
      }

      /*
         Copy the data to disk directly
      */
      void  setData( const dmsRecordData &data )
      {
         if ( data.isEmpty() )
         {
            return ;
         }
         if ( data.isCompressed() )
         {
            setCompressed() ;
            UINT32 * temp = (UINT32 *)( (CHAR *)this + DMS_RECORD_METADATA_SZ ) ;
            (*temp) = data.len() ;
            (*temp) |= ( ( (UINT32)data.getCompressType() << 24 ) &
                           0xFF000000 ) ;
            ossMemcpy( (CHAR*)this+DMS_RECORD_METADATA_SZ+sizeof(UINT32),
                       data.data(), data.len() ) ;
         }
         else
         {
            unsetCompressed() ;
            ossMemcpy( (CHAR*)this+DMS_RECORD_METADATA_SZ,
                       data.data(), data.len() ) ;
         }
      }
   } ;
   typedef _dmsRecord dmsRecord ;

   // Extract Data
   #define DMS_RECORD_EXTRACTDATA( pRecord, retPtr, compressorEntry )   \
   do {                                                                 \
         if ( !pRecord->isCompressed() )                                \
         {                                                              \
            (retPtr) = (ossValuePtr)pRecord->getData() ;                \
         }                                                              \
         else                                                           \
         {                                                              \
            INT32 uncompLen = 0 ;                                       \
            UINT8 compressType = pRecord->getCompressType() ;           \
            rc = dmsUncompress( cb, compressorEntry, compressType,      \
                                pRecord->getData(),                     \
                                pRecord->getDataLength(),               \
                                (const CHAR**)&(retPtr), &uncompLen ) ; \
            PD_RC_CHECK ( rc, PDERROR,                                  \
                          "Failed to uncompress record, rc = %d", rc ); \
            PD_CHECK ( uncompLen == *(INT32*)(retPtr),                  \
                       SDB_CORRUPTED_RECORD, error, PDERROR,            \
                       "uncompressed length %d does not match real "    \
                       "len %d", uncompLen, *(INT32*)(retPtr) ) ;       \
         }                                                              \
      } while ( FALSE )

   // Capped collectionr record header.
   class _dmsCappedRecord : public SDBObject
   {
      // Caution: This structure will use dmsRecord structure for data and
      // attribute setting. DO NOT modify the members!
   public:
      union
      {
         CHAR     _recordHead[4] ;
         UINT32   _flag_and_size ;
      }           _head ;
      // Record number which have been inserted into this extent. It's used to
      // calculate record number being popped in pop operation, avoid scanning
      // the whole extent.
      // Note: It includes records which have been popped out forward, but
      // excludes those which have been popped out backward. In a valid record,
      // it should always be greater than 0.
      UINT32      _recNo ;
      INT64       _logicalID ;

   public:
      CHAR getFlag() const
      {
         return _head._recordHead[ 0 ] ;
      }

      void setSize( UINT32 size )
      {
         return ((dmsRecord*)this)->setSize( size ) ;
      }

      UINT32 getSize() const
      {
         return ((const dmsRecord*)this)->getSize() ;
      }

      void setLogicalID( INT64 logicalID )
      {
         _logicalID = logicalID ;
      }

      INT64 getLogicalID() const
      {
         return _logicalID ;
      }

      void setRecordNo( UINT32 recNo )
      {
         _recNo = recNo ;
      }

      UINT32 getRecordNo() const
      {
         return _recNo ;
      }

      void setNormal()
      {
         return ((dmsRecord*)this)->setNormal() ;
      }

      BOOLEAN isNormal() const
      {
         return ((const dmsRecord*)this)->isNormal() ;
      }

      void resetAttr()
      {
         return ((dmsRecord*)this)->resetAttr() ;
      }

      void setData( const dmsRecordData &data )
      {
         return ((dmsRecord*)this)->setData(data) ;
      }

      const CHAR* getData() const
      {
         return ((const dmsRecord*)this)->getData() ;
      }

      UINT32 getDataLength() const
      {
         return ((const dmsRecord*)this)->getDataLength() ;
      }

      BOOLEAN isCompressed() const
      {
         return ((const dmsRecord*)this)->isCompressed() ;
      }

      UINT8 getCompressType () const
      {
         return ((const dmsRecord *)this)->getCompressType() ;
      }

      BYTE getState() const
      {
         return ((const dmsRecord*)this)->getState() ;
      }
   } ;
   typedef _dmsCappedRecord dmsCappedRecord ;

   /*
      _dmsDeletedRecord defined
   */
   class _dmsDeletedRecord : public SDBObject
   {
   public :
      union
      {
         CHAR     _recordHead[4] ;     // 1 byte flag, 3 bytes length-1
         UINT32   _flag_and_size ;
      }                 _head ;
      dmsOffset         _myOffset ;
      dmsRecordID       _next ;

      /*
         Get Functions
      */
      CHAR getFlag() const
      {
         return _head._recordHead[ 0 ] ;
      }
      BOOLEAN isDeleted() const
      {
         return DMS_RECORD_FLAG_DELETED == getFlag() ;
      }
      UINT32 getSize() const
      {
         return ((const dmsRecord*)this)->getSize() ;
      }
      dmsOffset getMyOffset() const
      {
         return _myOffset ;
      }
      dmsRecordID getNextRID() const
      {
         return _next ;
      }

      /*
         Set Functions
      */
      void setSize( UINT32 size )
      {
         return ((dmsRecord*)this)->setSize( size ) ;
      }
      void setMyOffset( dmsOffset myOffset )
      {
         _myOffset = myOffset ;
      }
      void setNextRID( const dmsRecordID &rid )
      {
         _next = rid ;
      }
      void setFlag( CHAR flag )
      {
         _head._recordHead[ 0 ] = flag ;
      }
      void setDeleted()
      {
         setFlag( DMS_RECORD_FLAG_DELETED ) ;
      }
   } ;
   typedef _dmsDeletedRecord dmsDeletedRecord ;
   #define DMS_DELETEDRECORD_METADATA_SZ  sizeof(dmsDeletedRecord)

   // oid + one field = 12 + 5 = 17, Algned:20
   #define DMS_MIN_DELETEDRECORD_SZ    (DMS_DELETEDRECORD_METADATA_SZ+20)
   #define DMS_MIN_RECORD_SZ           DMS_MIN_DELETEDRECORD_SZ
   // The min size of BSONObj data.
   #define DMS_MIN_RECORD_DATA_SZ      5
}

#endif //DMSRECORD_HPP_

