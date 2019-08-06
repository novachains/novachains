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

   Source File Name = dmsBlockData.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DMS component. This file contains declare of DMS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          17/05/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef DMSBLOCKDATA_HPP__
#define DMSBLOCKDATA_HPP__

#include "dmsExtent.hpp"
#include "ossMemPool.hpp"
#include "core.hpp"
#include "oss.hpp"
#include "ossMmap.hpp"
#include "dms.hpp"
#include "ossUtil.hpp"
#include "ossMem.hpp"
#include "../bson/bson.h"
#include "../bson/bsonobj.h"
#include "../bson/oid.h"
#include "dmsSMEMgr.hpp"
#include "pmdEnv.hpp"
#include "pmdEDU.hpp"
#include "sdbInterface.hpp"

using namespace bson ;

namespace engine
{

#pragma pack(1)
   /*
      _IDToInsert define
   */
   class _IDToInsert : public SDBObject
   {
   public :
      CHAR _type ;
      CHAR _id[4] ; // _id + '\0'
      OID _oid ;
      _IDToInsert ()
      {
         _type = (CHAR)jstOID ;
         _id[0] = '_' ;
         _id[1] = 'i' ;
         _id[2] = 'd' ;
         _id[3] = 0 ;
         SDB_ASSERT ( sizeof ( _IDToInsert) == 17,
                      "IDToInsert should be 17 bytes" ) ;
      }
   } ;
   typedef class _IDToInsert IDToInsert ;

   /*
      _idToInsert define
   */
   class _idToInsertEle : public BSONElement
   {
   public :
      _idToInsertEle( CHAR* x ) : BSONElement((CHAR*) ( x )){}
   } ;
   typedef class _idToInsertEle idToInsertEle ;

#pragma pack()

#pragma pack(4)
   /*
      _dmsMetadataBlock defined
   */
   struct _dmsMetadataBlock
   {
      // every records <= 32 bytes go to slot 0
      // every records >32 and <= 64 go to slot 1...
      // every records
      enum deleteListType
      {
         _32 = 0,
         _64,
         _128,
         _256,
         _512,
         _1k,
         _2k,
         _4k,
         _8k,
         _16k,
         _32k,
         _64k,
         _128k,
         _256k,
         _512k,
         _1m,
         _2m,
         _4m,
         _8m,
         _16m,
         _max
      } ;

      CHAR           _collectionName [ DMS_COLLECTION_NAME_SZ+1 ] ;
      UINT16         _flag ;
      UINT16         _blockID ;
      dmsExtentID    _firstExtentID ;
      dmsExtentID    _lastExtentID ;
      dmsRecordID    _deleteList [_max] ;
      UINT32         _logicalID ;
      UINT32         _attributes ;
      dmsExtentID    _loadFirstExtentID ;
      dmsExtentID    _loadLastExtentID ;
      dmsExtentID    _mbExExtentID ;
      // for stat
      UINT64         _totalRecords ;
      UINT32         _totalDataPages ;
      UINT64         _totalDataFreeSpace ;
      // end

      // This extent is used to store dictionary of the collection. If the
      // dictionary has not been created, the value should be DMS_INVALID_EXTENT.
      dmsExtentID    _dictExtentID ;
      dmsExtentID    _newDictExtentID ;
      SINT32         _dictStatPageID ;
      UINT8          _dictVersion ;
      // for stat
      UINT64         _totalOrgDataLen ;
      UINT64         _totalDataLen ;
      CHAR           _pad2[ 303 ] ;  // reserved
      // end stat

      // Extend option extent id for collection.
      // If one storage type has its own special options, allocate one seperate
      // page to store them, instead of putting them in this common structure.
      dmsExtentID    _mbOptExtentID ;

      utilCLUniqueID _clUniqueID ;

      CHAR           _pad [ 336 ] ;

      void reset ( const CHAR *clName = NULL,
                   utilCLUniqueID clUniqueID = UTIL_UNIQUEID_NULL,
                   UINT16 mbID = DMS_INVALID_MBID,
                   UINT32 clLID = DMS_INVALID_CLID,
                   UINT32 attr = 0 )
      {
         INT32 i = 0 ;
         ossMemset( _collectionName, 0, sizeof( _collectionName ) ) ;
         if ( clName )
         {
            ossStrncpy( _collectionName, clName, DMS_COLLECTION_NAME_SZ ) ;
         }
         _clUniqueID = clUniqueID ;
         if ( DMS_INVALID_MBID != mbID )
         {
            DMS_SET_MB_INUSE( _flag ) ;
         }
         else
         {
            DMS_SET_MB_FREE( _flag ) ;
         }
         _blockID = mbID ;
         _firstExtentID = DMS_INVALID_EXTENT ;
         _lastExtentID  = DMS_INVALID_EXTENT ;
         for ( i = 0 ; i < _max ; ++i )
         {
            _deleteList[i].reset() ;
         }

         _logicalID = clLID ;
         _attributes   = attr ;
         _loadFirstExtentID = DMS_INVALID_EXTENT ;
         _loadLastExtentID  = DMS_INVALID_EXTENT ;
         _mbExExtentID      = DMS_INVALID_EXTENT ;

         _totalRecords           = 0 ;
         _totalDataPages         = 0 ;
         _totalDataFreeSpace     = 0 ;
         _dictVersion            = 0 ;
         _dictExtentID           = DMS_INVALID_EXTENT ;
         _newDictExtentID        = DMS_INVALID_EXTENT ;
         _dictStatPageID         = DMS_INVALID_EXTENT ;

         _totalOrgDataLen        = 0 ;
         _totalDataLen           = 0 ;

         _mbOptExtentID          = DMS_INVALID_EXTENT ;

         // pad
         ossMemset( _pad2, 0, sizeof( _pad2 ) ) ;
         ossMemset( _pad, 0, sizeof( _pad ) ) ;
      }
   } ;
   typedef _dmsMetadataBlock  dmsMetadataBlock ;
   typedef dmsMetadataBlock   dmsMB ;
   #define DMS_MB_SIZE                 (1024)

   
   #define DMS_SU_NAME_SZ                    DMS_COLLECTION_SPACE_NAME_SZ
   /*
      _dmsStorageInfo defined
   */
   struct _dmsStorageInfo
   {
      UINT32      _pageSize ;
      CHAR        _suName [ DMS_SU_NAME_SZ + 1 ] ; // storage unit file name is
                                                   // foo.0 / foo.1, where foo
                                                   // is suName, and 0/1 are
                                                   // _sequence
      UINT32      _sequence ;

      UINT32      _extentThreshold ;

      BOOLEAN     _enableSparse ;
      BOOLEAN     _directIO ;
      UINT32      _logWriteMod ;
      UINT32      _cacheMergeSize ;
      UINT32      _pageAllocTimeout ;

      /// Data is OK
      BOOLEAN     _dataIsOK ;
      UINT64      _curLSNOnStart ;

      DMS_STORAGE_TYPE _type ;

      utilCSUniqueID _csUniqueID ;

      _dmsStorageInfo ()
      {
         _pageSize      = DMS_PAGE_SIZE_DFT ;
         ossMemset( _suName, 0, sizeof( _suName ) ) ;
         _sequence      = 0 ;

         _extentThreshold = 0 ;
         _enableSparse = FALSE ;
         _directIO = FALSE ;
         _logWriteMod = DMS_LOG_WRITE_MOD_INCREMENT ;
         _cacheMergeSize = 0 ;
         _pageAllocTimeout = 0 ;

         _dataIsOK       = FALSE ;
         _curLSNOnStart  = ~0 ;
         _type = DMS_STORAGE_NORMAL ;

         _csUniqueID     = UTIL_UNIQUEID_NULL ;
      }
   };
   typedef _dmsStorageInfo dmsStorageInfo ;

   #define DMS_HEADER_EYECATCHER_LEN         (8)

   /*
      Storage Unit Header : 65536(64K)
   */
   struct _dmsStorageUnitHeader : public SDBObject
   {
      CHAR   _eyeCatcher[DMS_HEADER_EYECATCHER_LEN] ;
      UINT32 _version ;
      UINT32 _pageSize ;                                 // size of byte
      UINT32 _storageUnitSize ;                          // all file pages
      CHAR   _name [ DMS_SU_NAME_SZ+1 ] ;                // storage unit name
      UINT32 _sequence ;                                 // storage unit seq
      UINT32 _numMB ;                                    // Number of MB
      UINT32 _MBHWM ; 
      UINT32 _pageNum ;                                  // current page number
      utilCSUniqueID _csUniqueID ;                       // cs unique id
      CHAR   _pad [ 65368 ] ;

      _dmsStorageUnitHeader()
      {
         reset() ;
      }

      void reset()
      {
         SDB_ASSERT( DMS_PAGE_SIZE_MAX == sizeof( _dmsStorageUnitHeader ),
                     "_dmsStorageUnitHeader size must be 64K" ) ;
         ossMemset( this, 0, DMS_PAGE_SIZE_MAX ) ;
      }

   } ;
   typedef _dmsStorageUnitHeader dmsStorageUnitHeader ;
   #define DMS_HEADER_SZ   sizeof(dmsStorageUnitHeader)

   #define DMS_SME_LEN                 (DMS_MAX_PG/8)
   #define DMS_SME_FREE                 0
   #define DMS_SME_ALLOCATED            1


   /* Space Management Extent, 1 bit for 1 page */
   struct _dmsSpaceManagementExtent : public SDBObject
   {
      CHAR _smeMask [ DMS_SME_LEN ] ;

      _dmsSpaceManagementExtent()
      {
         SDB_ASSERT( DMS_SME_LEN == sizeof( _dmsSpaceManagementExtent ),
                     "SME size error" ) ;
         ossMemset( _smeMask, DMS_SME_FREE, sizeof( _smeMask ) ) ;
      }
      CHAR getBitMask( UINT32 bitNum ) const
      {
         SDB_ASSERT( bitNum < DMS_MAX_PG, "Invalid bitNum" ) ;
         return (_smeMask[bitNum >> 3] >> (7 - (bitNum & 7))) & 1 ;
      }
      void freeBitMask( UINT32 bitNum )
      {
         SDB_ASSERT( bitNum < DMS_MAX_PG, "Invalid bitNum" ) ;
         _smeMask[bitNum >> 3] &= ~( 1 << (7 - (bitNum & 7))) ;
      }
      void setBitMask( UINT32 bitNum )
      {
         SDB_ASSERT( bitNum < DMS_MAX_PG, "Invalid bitNum" ) ;
         _smeMask[bitNum >> 3] |= ( 1 << (7 - (bitNum & 7))) ;
      }
   } ;
   typedef _dmsSpaceManagementExtent dmsSpaceManagementExtent ;
   #define DMS_SME_SZ  sizeof(dmsSpaceManagementExtent)


#pragma pack()

   /*
      _metadataBlockEx define
   */
   struct _metadataBlockEx
   {
      dmsMetaExtent        _header ;
      dmsExtentID          _array[1] ;

      INT32 getFirstExtentID( UINT32 segID, dmsExtentID &extID ) const
      {
         if ( segID >= _header._segNum )
         {
            return SDB_INVALIDARG ;
         }
         UINT32 index = segID << 1 ;
         extID = _array[index] ;
         return SDB_OK ;
      }
      INT32 getLastExtentID( UINT32 segID, dmsExtentID &extID ) const
      {
         if ( segID >= _header._segNum )
         {
            return SDB_INVALIDARG ;
         }
         UINT32 index = ( segID << 1 ) + 1 ;
         extID = _array[index] ;
         return SDB_OK ;
      }
      INT32 setFirstExtentID( UINT32 segID, dmsExtentID extID )
      {
         if ( segID >= _header._segNum )
         {
            return SDB_INVALIDARG ;
         }
         UINT32 index = segID << 1 ;
         _array[index] = extID ;
         return SDB_OK ;
      }
      INT32 setLastExtentID( UINT32 segID, dmsExtentID extID )
      {
         if ( segID >= _header._segNum )
         {
            return SDB_INVALIDARG ;
         }
         UINT32 index = ( segID << 1 ) + 1 ;
         _array[index] = extID ;
         return SDB_OK ;
      }
   } ;
   typedef _metadataBlockEx   dmsMBEx ;

   #define DMS_MME_SZ               (DMS_MME_SLOTS*DMS_MB_SIZE)
   /*
      _dmsMetadataManagementExtent defined
   */
   struct _dmsMetadataManagementExtent : public SDBObject
   {
      dmsMetadataBlock  _mbList [ DMS_MME_SLOTS ] ;

      _dmsMetadataManagementExtent ()
      {
         SDB_ASSERT( DMS_MME_SZ == sizeof( _dmsMetadataManagementExtent ),
                     "MME size error " ) ;
         ossMemset( this, 0, sizeof( _dmsMetadataManagementExtent ) ) ;
      }
   } ;
   typedef _dmsMetadataManagementExtent dmsMetadataManagementExtent ;

   /*
      _dmsMBStatInfo define
   */
   struct _dmsMBStatInfo
   {
      UINT64      _totalRecords ;
      UINT32      _totalDataPages ;
      UINT64      _totalDataFreeSpace ;
      UINT64      _totalOrgDataLen ;
      UINT64      _totalDataLen ;
      UINT32      _startLID ;
      UINT32      _flag ;

      UINT64      _lastWriteTick ;
      BOOLEAN     _isCrash ;

      void reset()
      {
         _totalRecords           = 0 ;
         _totalDataPages         = 0 ;
         _totalDataFreeSpace     = 0 ;
         _totalOrgDataLen        = 0 ;
         _totalDataLen           = 0 ;
         _startLID               = DMS_INVALID_CLID ;
         _flag                   = 0 ;
         _lastWriteTick          = 0 ;
         _isCrash                = FALSE ;
      }

      _dmsMBStatInfo ()
      {
         reset() ;
      }

      ~_dmsMBStatInfo ()
      {
         reset() ;
      }
   } ;
   typedef _dmsMBStatInfo dmsMBStatInfo ;


   /*
      _dmsContext define
   */
   class _dmsContext : public _IContext
   {
      public:
         _dmsContext () {}
         virtual ~_dmsContext () {}

      public:
         virtual string toString () const = 0 ;
         virtual UINT16 mbID() const = 0 ;

   };
   typedef _dmsContext  dmsContext ;

   class _dmsBlockData ;
   /*
      _dmsMBContext define
   */
   class _dmsMBContext : public _dmsContext
   {
      friend class _dmsBlockData ;
      private:
         _dmsMBContext() ;
         virtual ~_dmsMBContext() ;
         void _reset () ;

      public:
         virtual string toString () const ;
         virtual INT32  pause () ;
         virtual INT32  resume () ;

         OSS_INLINE INT32   mbLock( INT32 lockType ) ;
         OSS_INLINE INT32   mbTryLock( INT32 lockType, BOOLEAN force = FALSE ) ;
         OSS_INLINE INT32   mbUnlock() ;
         OSS_INLINE BOOLEAN isMBLock( INT32 lockType ) const ;
         OSS_INLINE BOOLEAN isMBLock() const ;
         OSS_INLINE BOOLEAN canResume() const ;

         virtual     UINT16 mbID () const { return _mbID ; }
         OSS_INLINE  dmsMB* mb () { return _mb ; }
         OSS_INLINE  UINT32 clLID () const { return _clLID ; }
         OSS_INLINE  UINT32 startLID() const { return _startLID ; }
         OSS_INLINE  INT32  mbLockType() const { return _mbLockType ; }

      private:
         OSS_INLINE INT32   _mbLock( INT32 lockType, BOOLEAN isTry,
                                     BOOLEAN force = FALSE ) ;
      private:
         dmsMB             *_mb ;
         dmsMBStatInfo     *_mbStat ;
         ossSpinSLatch     *_latch ;
         UINT32            _clLID ;
         UINT32            _startLID ;
         UINT16            _mbID ;
         INT32             _mbLockType ;
         INT32             _resumeType ;
   };
   typedef _dmsMBContext   dmsMBContext ;

   /*
      _dmsMBContext OSS_INLINE functions
   */
   OSS_INLINE INT32 _dmsMBContext::_mbLock( INT32 lockType,
                                            BOOLEAN isTry,
                                            BOOLEAN force )
   {
      INT32 rc = SDB_OK ;
      if ( SHARED != lockType && EXCLUSIVE != lockType )
      {
         return SDB_INVALIDARG ;
      }
      if ( _mbLockType == lockType )
      {
         return SDB_OK ;
      }
      // already lock(type not same), need to unlock
      if ( -1 != _mbLockType && SDB_OK != ( rc = pause() ) )
      {
         return rc ;
      }

      if ( !force )
      {
         // check before lock
         if ( !DMS_IS_MB_INUSE(_mb->_flag) )
         {
            return SDB_DMS_NOTEXIST ;
         }
         if ( _clLID != _mb->_logicalID )
         {
            return SDB_DMS_NOTEXIST ;
         }
      }

      if ( isTry )
      {
         BOOLEAN hasLock = FALSE ;
         hasLock = ( SHARED == lockType ) ?
                   _latch->try_get_shared() : _latch->try_get() ;
         if ( !hasLock )
         {
            return SDB_TIMEOUT ;
         }
      }
      else
      {
         ossLatch( _latch, (OSS_LATCH_MODE)lockType ) ;
      }

      // check after lock
      if ( !force )
      {
         if ( !DMS_IS_MB_INUSE(_mb->_flag) )
         {
            ossUnlatch( _latch, (OSS_LATCH_MODE)lockType ) ;
            return SDB_DMS_NOTEXIST ;
         }
         if ( _clLID != _mb->_logicalID )
         {
            ossUnlatch( _latch, (OSS_LATCH_MODE)lockType ) ;
            return SDB_DMS_NOTEXIST ;
         }
      }
      _mbLockType = lockType ;
      _resumeType = -1 ;
      return SDB_OK ;
   }
   OSS_INLINE INT32 _dmsMBContext::mbLock( INT32 lockType )
   {
      return _mbLock( lockType, FALSE, FALSE ) ;
   }
   OSS_INLINE INT32 _dmsMBContext::mbTryLock( INT32 lockType, BOOLEAN force )
   {
      return _mbLock( lockType, TRUE, force ) ;
   }
   OSS_INLINE INT32 _dmsMBContext::mbUnlock()
   {
      if ( SHARED == _mbLockType || EXCLUSIVE == _mbLockType )
      {
         ossUnlatch( _latch, (OSS_LATCH_MODE)_mbLockType ) ;
         _resumeType = _mbLockType ;
         _mbLockType = -1 ;
      }
      return SDB_OK ;
   }
   OSS_INLINE BOOLEAN _dmsMBContext::isMBLock( INT32 lockType ) const
   {
      return lockType == _mbLockType ? TRUE : FALSE ;
   }
   OSS_INLINE BOOLEAN _dmsMBContext::isMBLock() const
   {
      if ( SHARED == _mbLockType || EXCLUSIVE == _mbLockType )
      {
         return TRUE ;
      }
      return FALSE ;
   }
   OSS_INLINE BOOLEAN _dmsMBContext::canResume() const
   {
      if ( SHARED == _resumeType || EXCLUSIVE == _resumeType )
      {
         return TRUE ;
      }
      return FALSE ;
   }

   /*
      DMS_CHG_STEP define
   */
   enum DMS_CHG_STEP
   {
      DMS_CHG_BEFORE    = 1,
      DMS_CHG_AFTER
   } ;


   /*
      _dmsDirtyList define
   */
   class _dmsDirtyList : public SDBObject
   {
      public:
         _dmsDirtyList() ;
         ~_dmsDirtyList() ;

         INT32    init( UINT32 capacity ) ;
         void     destory() ;
         void     setSize( UINT32 size ) ;

         void     setDirty( UINT32 pos )
         {
            SDB_ASSERT( pos < _size, "Invalid pos" ) ;
            _pData[pos >> 3] |= ( 1 << (7 - (pos & 7))) ;
            _dirtyBegin.swapLesserThan( pos ) ;
            _dirtyEnd.swapGreaterThan( pos ) ;
         }

         void     cleanDirty( UINT32 pos )
         {
            SDB_ASSERT( pos < _size, "Invalid pos" ) ;
            _pData[pos >> 3] &= ~( 1 << (7 - (pos & 7))) ;
         }

         BOOLEAN  isDirty( UINT32 pos ) const
         {
            SDB_ASSERT( pos < _size, "Invalid pos" ) ;
            if ( _fullDirty )
            {
               return TRUE ;
            }
            return ( (_pData[pos >> 3] >> (7 - (pos & 7))) & 1 ) ? TRUE : FALSE ;
         }

         void     setFullDirty() { _fullDirty = TRUE ; }
         BOOLEAN  isFullDirty() const { return _fullDirty ; }

         /// return -1 for no find the pos
         INT32    nextDirtyPos( UINT32 &fromPos ) const ;
         void     cleanAll() ;
         UINT32   dirtyNumber() const ;
         UINT32   dirtyGap() const ;

      private:
         CHAR     *_pData ;
         UINT32   _capacity ;
         UINT32   _size ;
         BOOLEAN  _fullDirty ;

         ossAtomic32 _dirtyBegin ;
         ossAtomic32 _dirtyEnd ;
   } ;
   typedef _dmsDirtyList dmsDirtyList ;

   #define DMS_RW_ATTR_DIRTY                 0x00000001
   #define DMS_RW_ATTR_NOTHROW               0x00000002
 
   /*
      _dmsExtRW define
   */
   class _dmsExtRW
   {
      friend class _dmsBlockData ;

      public:
         _dmsExtRW() ;
         ~_dmsExtRW() ;

         BOOLEAN        isEmpty() const ;
         _dmsExtRW      derive( INT32 extentID ) ;

         void           setNothrow( BOOLEAN nothrow ) ;
         BOOLEAN        isNothrow() const ;

         void           setCollectionID( INT32 id ) { _collectionID = id ; }

         INT32          getExtentID() const { return _extentID ; }
         INT32          getCollectionID() const { return _collectionID ; }

         /*
            readPtr and writePtr will throw pdGeneralException
            with error. Use pdGetLastError() can get the error number

            If you set nothrow attribute, the both functions will return
            NULL instead of throw pdGeneralException with error.
         */
         const CHAR*    readPtr( UINT32 offset, UINT32 len ) const ;
         CHAR*          writePtr( UINT32 offset, UINT32 len ) ;

         template< typename T >
         const T*       readPtr( UINT32 offset = 0,
                                 UINT32 len = sizeof(T) ) const
         {
            return ( const T* )readPtr( offset, len ) ;
         }

         template< typename T >
         T*             writePtr( UINT32 offset = 0,
                                  UINT32 len = sizeof(T) )
         {
            return ( T* )writePtr( offset, len ) ;
         }

         BOOLEAN        isDirty() const ;

         std::string    toString() const ;

      protected:
         void           _markDirty() ;

      private:
         INT32                _extentID ;
         INT32                _collectionID ;
         UINT32               _attr ;
         ossValuePtr          _ptr ;
         _dmsBlockData        *_pBase ;
   } ;
   typedef _dmsExtRW dmsExtRW ;

   /*
      _dmsRecordRW define
   */
   class _dmsRecordRW
   {
      friend class _dmsBlockData ;

      public:
         _dmsRecordRW() ;
         virtual ~_dmsRecordRW() ;

         BOOLEAN           isEmpty() const ;
         _dmsRecordRW      derivePre() const ;
         _dmsRecordRW      deriveNext() const ;
         _dmsRecordRW      deriveOverflow() const ;
         _dmsRecordRW      derive( const dmsRecordID &rid ) const ;

         void              setNothrow( BOOLEAN nothrow ) ;
         BOOLEAN           isNothrow() const ;

         dmsRecordID       getRecordID() const { return _rid ; }

         /*
            When len == 0, Use the record's size
         */
         const dmsRecord*  readPtr( UINT32 len = sizeof( dmsRecord ) ) const ;
         dmsRecord*        writePtr( UINT32 len = sizeof( dmsRecord ) ) ;


         template< typename T >
         const T*          readPtr( UINT32 len = sizeof(T) ) const
         {
            return ( const T* )readPtr( len ) ;
         }

         template< typename T >
         T*                writePtr( UINT32 len = sizeof(T)  )
         {
            return ( T* )writePtr( len ) ;
         }

         std::string       toString() const ;

      private:
         void              _doneAddr() ;

      protected:
         BOOLEAN           _isDirectMem ;
         const dmsRecord   *_ptr ;

      private:
         dmsRecordID       _rid ;
         dmsExtRW          _rw ;
         _dmsBlockData   *_pData ;
   } ;
   typedef class _dmsRecordRW dmsRecordRW ;

   #define DMS_MME_OFFSET                 ( DMS_SME_OFFSET + DMS_SME_SZ )
   #define DMS_DATASU_EYECATCHER          "NOVADATA"

   #define DMS_DATASU_CUR_VERSION         1
   #define DMS_CONTEXT_MAX_SIZE           (2000)
   #define DMS_RECORDS_PER_EXTENT_SQUARE  4     // value is 2^4=16
   #define DMS_RECORD_OVERFLOW_RATIO      1.2f

   #define DMS_SU_FILENAME_SZ       ( DMS_SU_NAME_SZ + 15 )
   #define DMS_HEADER_OFFSET        ( 0 )
   #define DMS_SME_OFFSET           ( DMS_HEADER_OFFSET + DMS_HEADER_SZ )


   class _dmsBlockData : public _ossMmapFile
   {
      struct cmp_str
      {
         bool operator() (const char *a, const char *b)
         {
            return ossStrcmp( a, b ) < 0 ;
         }
      } ;

      typedef ossPoolMap<const CHAR*, UINT16, cmp_str> COLNAME_MAP ;
      typedef ossPoolMap<utilCLUniqueID, UINT16>       COLID_MAP ;
#if defined (_WINDOWS)
      typedef COLNAME_MAP::iterator                         COLNAME_MAP_IT ;
      typedef COLNAME_MAP::const_iterator                   COLNAME_MAP_CIT ;
#else
      typedef ossPoolMap<const CHAR*, UINT16>::iterator       COLNAME_MAP_IT ;
      typedef ossPoolMap<const CHAR*, UINT16>::const_iterator COLNAME_MAP_CIT ;
#endif
      typedef COLID_MAP::iterator    COLID_MAP_IT ;
      typedef COLID_MAP::const_iterator COLID_MAP_CIT ;

   public:
      _dmsBlockData( const CHAR *pSuFileName,
                     dmsStorageInfo *pInfo ) ;

      virtual ~_dmsBlockData()
      {
      }

   private:
      void _finalRecordSize( UINT32 &size, const dmsRecordData &recordData ) ;

      INT32 _prepareInsertData( const BSONObj &record,
                                BOOLEAN mustOID,
                                pmdEDUCB *cb,
                                dmsRecordData &recordData,
                                BOOLEAN &memReallocate,
                                INT64 position ) ;

      OSS_INLINE const CHAR* _clFullName( const CHAR *clName,
                                          CHAR *clFullName,
                                          UINT32 fullNameLen ) ;

      INT32 _allocRecordSpaceByPos( dmsMBContext *context,
                                    UINT32 size,
                                    INT64 position,
                                    dmsRecordID &foundRID,
                                    pmdEDUCB *cb ) ;

      INT32 _allocRecordSpace( dmsMBContext *context,
                               UINT32 requiredSize,
                               dmsRecordID &foundRID,
                               pmdEDUCB *cb ) ;

      INT32 _allocateExtent ( dmsMBContext *context,
                              UINT16 numPages,
                              BOOLEAN deepInit = TRUE,
                              BOOLEAN add2LoadList = FALSE,
                              dmsExtentID *allocExtID = NULL ) ;

      INT32 _findFreeSpace ( UINT16 numPages, SINT32 &foundPage,
                             dmsContext *context ) ;

      void  _mapExtent2DelList ( dmsMB *mb, dmsExtent *extAddr,
                                 SINT32 extentID ) ;

      INT32 _extendSegments( UINT32 numSeg ) ;

      INT32 _saveDeletedRecord ( dmsMB *mb,
                                 const dmsRecordID &recordID,
                                 INT32 recordSize = 0 ) ;


      INT32 _saveDeletedRecord( dmsMB *mb,
                                const dmsRecordID &rid,
                                INT32 recordSize,
                                dmsExtent *extAddr,
                                dmsDeletedRecord *pRecord ) ;

      INT32 _extentInsertRecord( dmsMBContext *context,
                                 dmsExtRW &extRW,
                                 dmsRecordRW &recordRW,
                                 const dmsRecordData &recordData,
                                 UINT32 needRecordSize,
                                 _pmdEDUCB *cb,
                                 BOOLEAN isInsert ) ;

      INT32 _initializeStorageUnit () ;

      UINT16 _collectionNameLookup( const CHAR * pName ) ;

      INT32 _writeFile( OSSFILE *file, const CHAR * pData, INT64 dataLen ) ;

      void _initHeader( dmsStorageUnitHeader * pHeader ) ;

      INT32 _validateHeader( dmsStorageUnitHeader * pHeader ) ;

      INT32 _onCreate( OSSFILE * file, UINT64 curOffSet ) ;

      void _initializeMME () ;

      INT32 _checkPageSize( dmsStorageUnitHeader * pHeader ) ;

      INT32 _onMapMeta( UINT64 curOffSet ) ;

      INT32 _postOpen( INT32 cause ) ;

      void  _collectionInsert( const CHAR * pName,
                               UINT16 mbID,
                               utilCLUniqueID clUniqueID ) ;

      void  _markHeaderInvalid( INT32 collectionID,
                                BOOLEAN isAll ) ;

      INT32 _onMarkHeaderInvalid( INT32 collectionID ) ;

   public:
      INT32 getMBContext( dmsMBContext ** pContext,
                          UINT16 mbID,
                          UINT32 clLID,
                          UINT32 startLID,
                          INT32 lockType ) ;

      INT32 getMBContext( dmsMBContext **pContext,
                          const CHAR* pName,
                          INT32 lockType ) ;

      void releaseMBContext( dmsMBContext *&pContext ) ;

      INT32 addCollection( const CHAR * pName,
                           UINT16 * collectionID,
                           utilCLUniqueID clUniqueID,
                           UINT32 attributes = 0,
                           pmdEDUCB * cb = NULL,
                           UINT16 initPages = 0,
                           BOOLEAN sysCollection = FALSE,
                           UINT32 *logicID = NULL,
                           const BSONObj *extOptions = NULL ) ;

      INT32 insertRecord ( dmsMBContext *context,
                           const BSONObj &record,
                           _pmdEDUCB *cb,
                           BOOLEAN mustOID = FALSE,
                           BOOLEAN canUnLock = TRUE,
                           INT64 position = -1 ) ;

      INT32 openStorage ( const CHAR *pPath,
                          BOOLEAN createNew = TRUE ) ;

      INT32 extractData( const dmsMBContext *mbContext,
                         const dmsRecordRW &recordRW,
                         dmsRecordData &recordData ) ;

      //TODO need to be implemented
      INT32 removeStorage()
      {
         return SDB_OK ;
      }

      INT32 addExtent2Meta( dmsExtentID extID,
                            dmsExtent *extent,
                            dmsMBContext *context ) ;
      /// flush mme
      INT32 flushMME( BOOLEAN sync = FALSE ) ;

      INT32 flushSegment( UINT32 segmentID, BOOLEAN sync = FALSE ) ;

      INT32 flushMeta( BOOLEAN sync = FALSE,
                       UINT32 *pExceptID = NULL,
                       UINT32 num = 0 ) ; ;

      void syncMemToMmap () ;

      // INLINE functions
   public:
      OSS_INLINE ossValuePtr extentAddr( INT32 extentID ) const ;

      OSS_INLINE UINT32  extent2Segment( dmsExtentID extentID,
                                        UINT32 *pSegOffset = NULL ) const ;

      OSS_INLINE const CHAR* getSuName () const
      {
         if ( _pStorageInfo )
         {
            return _pStorageInfo->_suName ;
         }
         return "" ;
      }

      OSS_INLINE const CHAR* getSuFileName () const
      {
         return _suFileName ;
      }

      OSS_INLINE UINT32 pageNum () const
      {
         return _pageNum ;
      }

      OSS_INLINE UINT32 segmentPages () const
      {
         return _segmentPages ;
      }

      OSS_INLINE UINT32 segmentPagesSquareRoot () const
      {
         return _segmentPagesSquare ;
      }

      OSS_INLINE UINT32 dataStartSegID () const
      {
         return _dataSegID ;
      }

      OSS_INLINE UINT32 pageSize () const
      {
         return _pageSize ;
      }

      OSS_INLINE UINT32 pageSizeSquareRoot () const
      {
         return _pageSizeSquare ;
      }

      OSS_INLINE UINT32 maxSegmentNum() const
      {
         return DMS_MAX_PG >> _segmentPagesSquare ;
      }

      OSS_INLINE void markDirty( INT32 collectionID,
                                 INT32 extentID,
                                 DMS_CHG_STEP step )
      {
         UINT32 segID = extent2Segment( extentID, NULL ) ;
         if ( (INT32)segID <= _maxSegID )
         {
            _markHeaderInvalid( collectionID, FALSE ) ;
            if ( DMS_CHG_BEFORE == step )
            {
               return ;
            }
            _dirtyList.setDirty( segID - _dataSegID ) ;
         }
      }

      OSS_INLINE dmsExtRW extent2RW( INT32 extentID,
                                     INT32 collectionID ) const
      {
         dmsExtRW rw ;
         rw._pBase = const_cast<_dmsBlockData*>( this ) ;
         rw._extentID = extentID ;
         rw._collectionID = collectionID ;
         rw._ptr = extentAddr( extentID ) ;
         return rw ;
      }

      OSS_INLINE dmsRecordRW record2RW( const dmsRecordID &record,
                                        UINT16 collectionID ) const
      {
         dmsRecordRW rRW ;
         rRW._pData = const_cast<_dmsBlockData*>( this ) ;
         rRW._rid = record ;
         rRW._rw = extent2RW( record._extent, collectionID ) ;
         rRW._doneAddr() ;
         return rRW ;
      }

   private:
      OSS_INLINE UINT32 _getSegmentSize() const
      {
         return DMS_SEGMENT_SZ ;
      }

      OSS_INLINE UINT64 _dataOffset()
      {
         return ( DMS_MME_OFFSET + DMS_MME_SZ ) ;
      }

      OSS_INLINE UINT32 _getFactor() const
      {
         return 16 + 14 - pageSizeSquareRoot() ;
      }

      OSS_INLINE UINT32 _curVersion() const
      {
         return DMS_DATASU_CUR_VERSION ;
      }

      OSS_INLINE INT32 _releaseSpace( SINT32 pageStart, UINT16 numPages )
      {
         return _smeMgr.releasePages( pageStart, numPages ) ;
      }

      OSS_INLINE UINT16 _collectionIdLookup( utilCLUniqueID clUniqueID )
      {
         UINT16 mbID = DMS_INVALID_MBID ;
         if ( UTIL_IS_VALID_CLUNIQUEID( clUniqueID ) )
         {
            COLID_MAP_CIT it = _collectionIDMap.find( clUniqueID ) ;
            if ( it != _collectionIDMap.end() )
            {
               mbID = it->second ;
            }
         }
         return mbID ;
      }

   private:
      // latch for each MB. For normal record SIUD, shared latches are
      // requested exclusive latch on mblock is only when changing
      // metadata (say add an extent into the MB, or create/drop the MB)
      ossSpinSLatch                       _mblock [ DMS_MME_SLOTS ] ;
      dmsMBStatInfo                       _mbStatInfo [ DMS_MME_SLOTS ] ;
      ossSpinSLatch                       _metadataLatch ;
      COLNAME_MAP                         _collectionNameMap ;
      COLID_MAP                           _collectionIDMap ;
      UINT32                              _logicalCSID ;
      UINT32                              _mmeSegID ;
      dmsStorageUnitID                    _CSID ;

      vector<dmsMBContext*>               _vecContext ;
      ossSpinXLatch                       _latchContext ;
      BOOLEAN                             _isCrash ;
      BOOLEAN                             _isClosed ;

      dmsMetadataManagementExtent         *_dmsMME ;
      ossSpinSLatch                       _segmentLatch ;
      UINT32                              _pageSizeSquare ;
      UINT32                              _dataSegID ;
      dmsStorageInfo                      *_pStorageInfo ;
      INT32                               _maxSegID ;
      UINT32                              _pageNum ;
      UINT32                              _segmentPages ;
      UINT32                              _segmentPagesSquare ;
      UINT32                              _pageSize ;
      dmsStorageUnitHeader                *_dmsHeader ;     // 64KB
      dmsSMEMgr                           _smeMgr ;
      dmsSpaceManagementExtent            *_dmsSME ;        // 16MB
      dmsDirtyList                        _dirtyList ;
      CHAR                                _fullPathName[ OSS_MAX_PATHSIZE + 1 ] ;
      CHAR                                _suFileName[ DMS_SU_FILENAME_SZ + 1 ] ;

   } ;
   typedef _dmsBlockData dmsBlockData ;

   /*
      OSS_INLINE functions :
   */
   OSS_INLINE UINT32 _dmsBlockData::extent2Segment( dmsExtentID extentID,
                                                      UINT32 * pSegOffset ) const
   {
      if ( pSegOffset )
      {
         // the same with : extentID % _segmentPages
         *pSegOffset = extentID & (( 1 << _segmentPagesSquare ) - 1 ) ;
      }
      // the same with: extentID / _segmentPages + _dataSegID
      return ( extentID >> _segmentPagesSquare ) + _dataSegID ;
   }

   OSS_INLINE ossValuePtr _dmsBlockData::extentAddr( INT32 extentID ) const
   {
      if ( DMS_INVALID_EXTENT == extentID )
      {
         return 0 ;
      }
      UINT32 segOffset = 0 ;
      UINT32 segID = extent2Segment( extentID, &segOffset ) ;
      if ( segID > (UINT32)_maxSegID )
      {
         return 0 ;
      }
      return getSegmentInfo( segID ) +
             (ossValuePtr)( segOffset << _pageSizeSquare ) ;
      // the same with: segOffset * _segmentPages
   }

   OSS_INLINE const CHAR* _dmsBlockData::_clFullName( const CHAR *clName,
                                                      CHAR * clFullName,
                                                      UINT32 fullNameLen )
   {
      SDB_ASSERT( fullNameLen > DMS_COLLECTION_FULL_NAME_SZ,
                  "Collection full name len error" ) ;
      ossStrncat( clFullName, getSuName(), DMS_COLLECTION_SPACE_NAME_SZ ) ;
      ossStrncat( clFullName, ".", 1 ) ;
      ossStrncat( clFullName, clName, DMS_COLLECTION_NAME_SZ ) ;
      clFullName[ DMS_COLLECTION_FULL_NAME_SZ ] = 0 ;

      return clFullName ;
   }

   OSS_INLINE void _dmsBlockData::_collectionInsert( const CHAR * pName,
                                                     UINT16 mbID,
                                                     utilCLUniqueID clUniqueID )
   {
      _collectionNameMap[ ossStrdup( pName ) ] = mbID ;

      if ( UTIL_IS_VALID_CLUNIQUEID( clUniqueID ) )
      {
         _collectionIDMap[ clUniqueID ] = mbID ;
      }
   }

   OSS_INLINE UINT16 _dmsBlockData::_collectionNameLookup( const CHAR * pName )
   {
      UINT16 mbID = DMS_INVALID_MBID ;
      if ( pName )
      {
         COLNAME_MAP_CIT it = _collectionNameMap.find( pName ) ;
         if ( it != _collectionNameMap.end() )
         {
            mbID = it->second ;
         }
      }
      return mbID ;
   }

   OSS_INLINE INT32 _dmsBlockData::getMBContext( dmsMBContext ** pContext,
                                                         UINT16 mbID,
                                                         UINT32 clLID,
                                                         UINT32 startLID,
                                                         INT32 lockType )
   {
      if ( mbID >= DMS_MME_SLOTS )
      {
         return SDB_INVALIDARG ;
      }

      // metadata shared lock
      if ( (UINT32)DMS_INVALID_CLID == clLID ||
           (UINT32)DMS_INVALID_CLID == startLID )
      {
         _metadataLatch.get_shared() ;
         if ( (UINT32)DMS_INVALID_CLID == clLID )
         {
            clLID = _dmsMME->_mbList[mbID]._logicalID ;
         }
         if ( (UINT32)DMS_INVALID_CLID == startLID )
         {
            startLID = _mbStatInfo[mbID]._startLID ;
         }
         _metadataLatch.release_shared() ;
      }

      // context lock
      _latchContext.get() ;
      if ( _vecContext.size () > 0 )
      {
         *pContext = _vecContext.back () ;
         _vecContext.pop_back () ;
      }
      else
      {
         *pContext = SDB_OSS_NEW dmsMBContext ;
      }
      _latchContext.release() ;

      if ( !(*pContext) )
      {
         return SDB_OOM ;
      }
      (*pContext)->_clLID = clLID ;
      (*pContext)->_startLID = startLID ;
      (*pContext)->_mbID = mbID ;
      (*pContext)->_mb = &_dmsMME->_mbList[mbID] ;
      (*pContext)->_mbStat = &_mbStatInfo[mbID] ;
      (*pContext)->_latch = &_mblock[mbID] ;
      if ( SHARED == lockType || EXCLUSIVE == lockType )
      {
         INT32 rc = (*pContext)->mbLock( lockType ) ;
         if ( rc )
         {
            releaseMBContext( *pContext ) ;
            return rc ;
         }
      }
      return SDB_OK ;
   }

   OSS_INLINE INT32 _dmsBlockData::getMBContext( dmsMBContext **pContext,
                                                 const CHAR* pName,
                                                 INT32 lockType )
   {
      UINT16 mbID = DMS_INVALID_MBID ;
      UINT32 clLID = DMS_INVALID_CLID ;
      UINT32 startLID = DMS_INVALID_CLID ;

      // metadata shared lock
      _metadataLatch.get_shared() ;
      mbID = _collectionNameLookup( pName ) ;
      if ( DMS_INVALID_MBID != mbID )
      {
         clLID = _dmsMME->_mbList[mbID]._logicalID ;
         startLID = _mbStatInfo[mbID]._startLID ;
      }
      _metadataLatch.release_shared() ;

      if ( DMS_INVALID_MBID == mbID )
      {
         return SDB_DMS_NOTEXIST ;
      }
      return getMBContext( pContext, mbID, clLID, startLID, lockType ) ;
   }

   OSS_INLINE void _dmsBlockData::releaseMBContext( dmsMBContext *&pContext )
   {
      if ( !pContext )
      {
         return ;
      }
      pContext->mbUnlock() ;

      _latchContext.get() ;
      if ( _vecContext.size() < DMS_CONTEXT_MAX_SIZE )
      {
         pContext->_reset() ;
         _vecContext.push_back( pContext ) ;
      }
      else
      {
         SDB_OSS_DEL pContext ;
      }
      _latchContext.release() ;
      pContext = NULL ;
   }

   OSS_INLINE void _dmsBlockData::_initHeader( dmsStorageUnitHeader * pHeader )
   {
      ossStrncpy( pHeader->_eyeCatcher, DMS_DATASU_EYECATCHER,
                  DMS_HEADER_EYECATCHER_LEN ) ;
      pHeader->_version = _curVersion() ;
      pHeader->_pageSize = _pStorageInfo->_pageSize ;
      pHeader->_storageUnitSize = _dataOffset() / pHeader->_pageSize ;
      ossStrncpy ( pHeader->_name, _pStorageInfo->_suName, DMS_SU_NAME_SZ ) ;
      pHeader->_sequence = _pStorageInfo->_sequence ;
      pHeader->_numMB    = 0 ;
      pHeader->_MBHWM    = 0 ;
      pHeader->_pageNum  = 0 ;
      pHeader->_csUniqueID = _pStorageInfo->_csUniqueID ;
   }

   OSS_INLINE INT32 _dmsBlockData::_checkPageSize( dmsStorageUnitHeader * pHeader )
   {
      INT32 rc = SDB_OK ;

      // check page size
      if ( DMS_PAGE_SIZE4K  != pHeader->_pageSize &&
           DMS_PAGE_SIZE8K  != pHeader->_pageSize &&
           DMS_PAGE_SIZE16K != pHeader->_pageSize &&
           DMS_PAGE_SIZE32K != pHeader->_pageSize &&
           DMS_PAGE_SIZE64K != pHeader->_pageSize )
      {
         PD_LOG ( PDERROR, "Invalid page size: %u, page size must be one of "
                  "4K/8K/16K/32K/64K", pHeader->_pageSize ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      // must be set storage info page size here, because lob meta page size
      // is 256B, so can't be assign to storage page size in later code
      if ( (UINT32)_pStorageInfo->_pageSize != pHeader->_pageSize )
      {
         _pStorageInfo->_pageSize = pHeader->_pageSize ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }


   /*
      Tool Functions
   */
   BOOLEAN  dmsIsRecordIDValid( const BSONElement &oidEle,
                                BOOLEAN allowEOO,
                                const CHAR **pErrStr = NULL ) ;


}

#endif   
