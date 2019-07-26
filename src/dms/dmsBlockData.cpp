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

   Source File Name = dmsBlockData.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DMS component. This file contains implement of DMS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          17/05/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/

#include "dmsBlockData.hpp"
#include "utilStr.hpp"

namespace engine
{
   /*
      _dmsMBContext implement
   */
   _dmsMBContext::_dmsMBContext ()
   {
      _reset () ;
   }

   _dmsMBContext::~_dmsMBContext ()
   {
      _reset () ;
   }

   void _dmsMBContext::_reset ()
   {
      _mb            = NULL ;
      _mbStat        = NULL ;
      _latch         = NULL ;
      _clLID         = DMS_INVALID_CLID ;
      _startLID      = DMS_INVALID_CLID ;
      _mbID          = DMS_INVALID_MBID ;
      _mbLockType    = -1 ;
      _resumeType    = -1 ;
   }

   string _dmsMBContext::toString() const
   {
      stringstream ss ;
      ss << "dms-mb-context[" ;
      if ( _mb )
      {
         ss << "Name: " ;
         ss << _mb->_collectionName ;
         ss << ", " ;
      }
      ss << "ID: " << _mbID ;
      ss << ", LID: " << _clLID ;
      ss << ", StartLID: " << _startLID ; 
      ss << ", LockType: " << _mbLockType ; 
      ss << ", ResumeType: " << _resumeType ;

      ss << " ]" ;
      
      return ss.str() ;
   }

   INT32 _dmsMBContext::pause()
   {
      INT32 rc = SDB_OK ;

      _resumeType = _mbLockType ;
      if ( SHARED == _mbLockType || EXCLUSIVE == _mbLockType )
      {
         rc = mbUnlock() ;
      }

      return rc ;
   }

   INT32 _dmsMBContext::resume()
   {
      INT32 rc = SDB_OK ;
      if ( SHARED == _resumeType || EXCLUSIVE == _resumeType )
      {
         INT32 lockType = _resumeType ;
         _resumeType = -1 ;
         rc = mbLock( lockType ) ;
      }
      return rc ;
   }


   /*
      _dmsDirtyList implement
   */
   _dmsDirtyList::_dmsDirtyList()
   :_dirtyBegin( 0x7FFFFFFF ), _dirtyEnd( 0 )
   {
      _pData = NULL ;
      _capacity = 0 ;
      _size  = 0 ;
      _fullDirty = FALSE ;
   }

   _dmsDirtyList::~_dmsDirtyList()
   {
      destory() ;
   }

   INT32 _dmsDirtyList::init( UINT32 capacity )
   {
      INT32 rc = SDB_OK ;
      UINT32 arrayNum = 0 ;

      SDB_ASSERT( capacity > 0 , "Capacity must > 0" ) ;

      if ( capacity <= _capacity )
      {
         cleanAll() ;
         goto done ;
      }
      /// first destory
      destory() ;

      arrayNum = ( capacity + 7 ) >> 3 ;
      _pData = ( CHAR* )SDB_OSS_MALLOC( arrayNum ) ;
      if ( !_pData )
      {
         PD_LOG( PDERROR, "Alloc dirty list failed" ) ;
         rc = SDB_OOM ;
         goto error ;
      }
      ossMemset( _pData, 0, arrayNum ) ;
      _capacity = arrayNum << 3 ;

   done:
      return rc ;
   error:
      goto done ;
   }

   void _dmsDirtyList::destory()
   {
      if ( _pData )
      {
         SDB_OSS_FREE( _pData ) ;
         _pData = NULL ;
      }
      _capacity = 0 ;
      _size = 0 ;
      _fullDirty = FALSE ;

      _dirtyBegin.init( 0x7FFFFFFF ) ;
      _dirtyEnd.init( 0 ) ;
   }

   void _dmsDirtyList::setSize( UINT32 size )
   {
      SDB_ASSERT( size <= _capacity, "Size must <= Capacity" ) ;
      _size = size <= _capacity ? size : _capacity ;
   }

   void _dmsDirtyList::cleanAll()
   {
      _dirtyBegin.init( 0x7FFFFFFF ) ;
      _dirtyEnd.init( 0 ) ;
      _fullDirty = FALSE ;

      UINT32 arrayNum = ( _size + 7 ) >> 3 ;
      for ( UINT32 i = 0 ; i < arrayNum ; ++i )
      {
         if ( _pData[ i ] != 0 )
         {
            _pData[ i ] = 0 ;
         }
      }
   }

   INT32 _dmsDirtyList::nextDirtyPos( UINT32 &fromPos ) const
   {
      INT32 pos = -1 ;

      while ( fromPos < _size )
      {
         if ( !_fullDirty && 0 == ( fromPos & 7 ) &&
              0 == _pData[ fromPos >> 3 ] )
         {
            fromPos += 8 ;
         }
         else if ( !isDirty( fromPos ) )
         {
            ++fromPos ;
         }
         else
         {
            pos = fromPos++ ;
            break ;
         }
      }
      return pos ;
   }

   UINT32 _dmsDirtyList::dirtyNumber() const
   {
      UINT32 dirtyNum = 0 ;

      if ( _fullDirty )
      {
         dirtyNum = _size ;
      }
      else
      {
         UINT32 beginPos = _dirtyBegin.peek() ;
         UINT32 endPos = _dirtyEnd.peek() + 1 ;

         while ( beginPos < endPos )
         {
            if ( 0 == ( beginPos & 7 ) &&
                 0 == _pData[ beginPos >> 3 ] )
            {
               beginPos += 8 ;
            }
            else
            {
               if ( isDirty( beginPos ) )
               {
                  ++dirtyNum ;
               }
               ++beginPos ;
            }
         }
      }
      return dirtyNum ;
   }

   UINT32 _dmsDirtyList::dirtyGap() const
   {
      if ( _fullDirty )
      {
         return _size ;
      }
      else
      {
         UINT32 beginPos = _dirtyBegin.peek() ;
         UINT32 endPos = _dirtyEnd.peek() ;
         if ( beginPos > endPos )
         {
            return 0 ;
         }
         return endPos - beginPos + 1 ;
      }
   }

   /*
      _dmsExtRW implement
   */
   _dmsExtRW::_dmsExtRW()
   {
      _extentID = -1 ;
      _collectionID = -1 ;
      _attr = 0 ;
      _pBase = NULL ;
      _ptr   = ( ossValuePtr ) 0 ;
   }

   _dmsExtRW::~_dmsExtRW()
   {
      if ( _pBase && isDirty() )
      {
         _pBase->markDirty( _collectionID, _extentID, DMS_CHG_AFTER ) ;
      }
   }

   BOOLEAN _dmsExtRW::isEmpty() const
   {
      return _pBase ? FALSE : TRUE ;
   }

   void _dmsExtRW::setNothrow( BOOLEAN nothrow )
   {
      if ( nothrow )
      {
         _attr |= DMS_RW_ATTR_NOTHROW ;
      }
      else
      {
         OSS_BIT_CLEAR( _attr, DMS_RW_ATTR_NOTHROW ) ;
      }
   }

   BOOLEAN _dmsExtRW::isNothrow() const
   {
      return ( _attr & DMS_RW_ATTR_NOTHROW ) ? TRUE : FALSE ;
   }

   const CHAR* _dmsExtRW::readPtr( UINT32 offset, UINT32 len ) const
   {
      if ( (ossValuePtr)0 == _ptr )
      {
         std::string text = "readPtr is NULL: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_SYS ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_SYS, text ) ;
      }
      return ( const CHAR* )_ptr + offset ;
   }

   CHAR* _dmsExtRW::writePtr( UINT32 offset, UINT32 len )
   {
      if ( (ossValuePtr)0 == _ptr )
      {
         std::string text = "writePtr is NULL: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_SYS ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_SYS, text ) ;
      }
      _markDirty() ;
      _pBase->markDirty( _collectionID, _extentID, DMS_CHG_BEFORE ) ;
      return ( CHAR* )_ptr + offset ;
   }

   _dmsExtRW _dmsExtRW::derive( INT32 extentID )
   {
      if ( _pBase )
      {
         return _pBase->extent2RW( extentID, _collectionID ) ;
      }
      return _dmsExtRW() ;
   }

   BOOLEAN _dmsExtRW::isDirty() const
   {
      return ( _attr & DMS_RW_ATTR_DIRTY ) ? TRUE : FALSE ;
   }

   void _dmsExtRW::_markDirty()
   {
      _attr |= DMS_RW_ATTR_DIRTY ;
   }

   std::string _dmsExtRW::toString() const
   {
      std::stringstream ss ;
      ss << "ExtentRW(" << _collectionID << "," << _extentID << ")" ;
      return ss.str() ;
   }

   #define DMS_EXTEND_THRESHOLD_SIZE      ( 33554432 )   // 32MB

   /*
      Sync Config Default Value
   */
   #define DMS_SYNC_RECORDNUM_DFT         ( 0 )
   #define DMS_SYNC_DIRTYRATIO_DFT        ( 50 )
   #define DMS_SYNC_INTERVAL_DFT          ( 10000 )
   #define DMS_SYNC_NOWRITE_DFT           ( 5000 )

   /*
      _dmsRecordRW implement
   */
   _dmsRecordRW::_dmsRecordRW()
   :_ptr( NULL )
   {
      _pData = NULL ;
      _isDirectMem = FALSE ;
   }

   _dmsRecordRW::~_dmsRecordRW()
   {
   }

   BOOLEAN _dmsRecordRW::isEmpty() const
   {
      return _pData ? FALSE : TRUE ;
   }

   _dmsRecordRW _dmsRecordRW::derive( const dmsRecordID &rid ) const
   {
      if ( _pData )
      {
         return _pData->record2RW( rid, _rw.getCollectionID() ) ;
      }
      return _dmsRecordRW() ;
   }

   _dmsRecordRW _dmsRecordRW::deriveNext() const
   {
      if ( _ptr && DMS_INVALID_OFFSET != _ptr->_nextOffset )
      {
         return _pData->record2RW( dmsRecordID( _rid._extent,
                                                _ptr->_nextOffset ),
                                   _rw.getCollectionID() ) ;
      }
      return _dmsRecordRW() ;
   }

   _dmsRecordRW _dmsRecordRW::derivePre() const
   {
      if ( _ptr && DMS_INVALID_OFFSET != _ptr->_previousOffset )
      {
         return _pData->record2RW( dmsRecordID( _rid._extent,
                                                _ptr->_previousOffset ),
                                   _rw.getCollectionID() ) ;
      }
      return _dmsRecordRW() ;
   }

   _dmsRecordRW _dmsRecordRW::deriveOverflow() const
   {
      if ( _ptr && _ptr->isOvf() )
      {
         return _pData->record2RW( _ptr->getOvfRID(), _rw.getCollectionID() ) ;
      }
      return _dmsRecordRW() ;
   }

   void _dmsRecordRW::setNothrow( BOOLEAN nothrow )
   {
      _rw.setNothrow( nothrow ) ;
   }

   BOOLEAN _dmsRecordRW::isNothrow() const
   {
      return _rw.isNothrow() ;
   }

   const dmsRecord* _dmsRecordRW::readPtr( UINT32 len ) const
   {
      if ( !_ptr )
      {
         std::string text = "Point is NULL: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_SYS ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_SYS, text ) ;
      }

      // special case when we don't read through extent but use in memory
      // old version directly
      // record2RW already set the _prt to record directly based on rid
      // here we would use 0 offset directly
      if( _isDirectMem )
      {
         return (const dmsRecord*)_ptr ;
      }

      if ( 0 == len )
      {
         len = _ptr->getSize() ;
      }
      if ( len > DMS_RECORD_MAX_SZ )
      {
         std::string text = "Record size is grater than max record size: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_DMS_CORRUPTED_EXTENT ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_DMS_CORRUPTED_EXTENT, text ) ;
      }
      return (const dmsRecord*)_rw.readPtr( _rid._offset, len ) ;
   }

   dmsRecord* _dmsRecordRW::writePtr( UINT32 len )
   {
      if ( !_ptr )
      {
         std::string text = "Point is NULL: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_SYS ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_SYS, text ) ;
      }

      if ( _isDirectMem )
      {
         return ( dmsRecord* )_ptr ;
      }

      if ( 0 == len )
      {
         len = _ptr->getSize() ;
      }
      if ( len > DMS_RECORD_MAX_SZ )
      {
         std::string text = "Record size is grater than max record size: " ;
         text += toString() ;

         if ( isNothrow() )
         {
            PD_LOG( PDERROR, "Exception: %s", text.c_str() ) ;
            pdSetLastError( SDB_DMS_CORRUPTED_EXTENT ) ;
            return NULL ;
         }
         throw pdGeneralException( SDB_DMS_CORRUPTED_EXTENT, text ) ;
      }
      return (dmsRecord*)_rw.writePtr( _rid._offset, len ) ;
   }

   std::string _dmsRecordRW::toString() const
   {
      std::stringstream ss ;
      ss << "RecordRW(" << _rw.getCollectionID()
         << "," << _rid._extent << "," << _rid._offset << ")" ;
      return ss.str() ;
   }

   void _dmsRecordRW::_doneAddr()
   {
      BOOLEAN oldThrow = _rw.isNothrow() ;
      _rw.setNothrow( TRUE ) ;
      /// Need to read the overflow rid
      UINT32 len = DMS_RECORD_METADATA_SZ + sizeof( dmsRecordID ) ;
      _ptr = (const dmsRecord*)_rw.readPtr( _rid._offset, len ) ;
      /// Restore nothrow
      _rw.setNothrow( oldThrow ) ;
   }


   _dmsBlockData::_dmsBlockData( const CHAR *pSuFileName,
                                 dmsStorageInfo *pInfo )
   {
      _logicalCSID        = 0 ;
      _mmeSegID           = 0 ;
      _CSID               = DMS_INVALID_SUID ;
      _isCrash            = FALSE ;
      _dmsMME             = NULL ;
      _pageSizeSquare     = 0 ;
      _dataSegID          = 0 ;
      _pStorageInfo       = pInfo ;
      _maxSegID           = -1 ;
      _pageNum            = 0 ;
      _segmentPages       = 0 ;
      _segmentPagesSquare = 0 ;
      _pageSize           = 0 ;
      _dmsHeader          = NULL ;
      _dmsSME             = NULL ;
      ossMemset( _fullPathName, 0, sizeof(_fullPathName) ) ;
      ossStrncpy( _suFileName, pSuFileName, DMS_SU_FILENAME_SZ ) ;
      _suFileName[ DMS_SU_FILENAME_SZ ] = 0 ;
   }



   void _dmsBlockData::_finalRecordSize( UINT32 &size,
                                         const dmsRecordData &recordData )
   {
      size = size * DMS_RECORD_OVERFLOW_RATIO ;
      size += DMS_RECORD_METADATA_SZ ;
      // record is ALWAYS 4 bytes aligned
      size = OSS_MIN( DMS_RECORD_MAX_SZ, ossAlignX ( size, 4 ) ) ;
   }

   INT32 _dmsBlockData::_allocRecordSpaceByPos( dmsMBContext *context,
                                                  UINT32 size,
                                                  INT64 position,
                                                  dmsRecordID &foundRID,
                                                  pmdEDUCB *cb )
   {
      return SDB_OPERATION_INCOMPATIBLE ;
   }

   INT32 _dmsBlockData::_extendSegments( UINT32 numSeg )
   {
      INT32 rc = SDB_OK ;
      INT64 fileSize = 0 ;

      // now other normal applications still able to access metadata in
      // read-only mode
      // Then we'll check if adding new segments will exceed the limit
      UINT32 beginExtentID = _dmsHeader->_pageNum ;
      UINT32 endExtentID   = beginExtentID + _segmentPages * numSeg ;

      if ( endExtentID > DMS_MAX_PG )
      {
         PD_LOG( PDERROR, "Extent page[%u] exceed max pages[%u] in su[%s]",
                 endExtentID, DMS_MAX_PG, _suFileName ) ;
         rc = SDB_DMS_NOSPC ;
         goto error ;
      }

      // We'll also verify the SME shows DMS_SME_FREE for all needed pages
      for ( UINT32 i = beginExtentID; i < endExtentID; i++ )
      {
         if ( DMS_SME_FREE != _dmsSME->getBitMask( i ) )
         {
            rc = SDB_DMS_CORRUPTED_SME ;
            goto error ;
         }
      }

      // get file size for map or rollback
      rc = ossGetFileSize ( &_file, &fileSize ) ;
      PD_RC_CHECK ( rc, PDERROR, "Failed to get file size, rc = %d", rc ) ;

      // check wether the file length is match storage unit pages
      if ( fileSize > (INT64)_dmsHeader->_storageUnitSize * pageSize() )
      {
         PD_LOG( PDWARNING, "File[%s] size[%llu] is not match with storage "
                 "unit pages[%u]", _suFileName, fileSize,
                 _dmsHeader->_storageUnitSize ) ;

         fileSize = (UINT64)_dmsHeader->_storageUnitSize * pageSize() ;
         rc = ossTruncateFile( &_file, fileSize ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Truncate file[%s] to size[%lld] failed, rc: %d",
                    _suFileName, fileSize, rc ) ;
            goto error ;
         }
         PD_LOG( PDEVENT, "Truncate file[%s] to size[%lld]", _suFileName,
                 fileSize ) ;
      }

      // now we only hold extendsegment latch, no other sessions can extend
      // but other sessions can freely create new extents in existing segments
      // This should be safe because no one knows we are increasing the size of
      // file, so other sessions will not attempt to access the new space
      // then we need to increase the size of file first
      // MAKE SURE NOT HOLD ANY METADATA LATCH DURING SUCH EXPENSIVE DISK
      // OPERATION extendSeg latch is held here so that it's not possible //
      // two sessions doing same extend
   retry:
      if ( _pStorageInfo->_enableSparse )
      {
         rc = ossExtentBySparse( &_file, (UINT64)_getSegmentSize() * numSeg ) ;
      }
      else
      {
         rc = ossExtendFile( &_file, _getSegmentSize() * numSeg ) ;
      }

      if ( rc )
      {
         INT32 rc1 = SDB_OK ;
         PD_LOG ( PDERROR, "Failed to extend storage unit for %lld "
                  "bytes, sparse:%s, rc: %d",
                  _getSegmentSize() * (UINT64)numSeg,
                  _pStorageInfo->_enableSparse ? "TRUE" : "FALSE", rc ) ;

         // truncate the file when it's failed to extend file
         rc1 = ossTruncateFile ( &_file, fileSize ) ;
         if ( rc1 )
         {
            PD_LOG ( PDSEVERE, "Failed to revert the increase of segment, "
                     "rc = %d", rc1 ) ;
            // if we increased the file size but got error, and we are not able
            // to decrease it, something BIG wrong, let's panic
            ossPanic () ;
         }

         if ( SDB_INVALIDARG == rc && _pStorageInfo->_enableSparse )
         {
            _pStorageInfo->_enableSparse = FALSE ;
            goto retry ;
         }
         // we need to manage how to truncate the file to original size here
         goto error ;
      }

      // map all new segments into memory
      for ( UINT32 i = 0; i < numSeg ; i++ )
      {
         rc = map ( fileSize, _getSegmentSize(), NULL ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to map storage unit from offset %lld",
                     _getSegmentSize() * i + _dmsHeader->_storageUnitSize ) ;
            goto error ;
         }
         _maxSegID += 1 ;
         _dirtyList.setSize( segmentSize() - _dataSegID ) ;

         // we need to set _storageUnitSize before depositASegment()
         // If not, FreeSize calculated by snapshot cs may be
         // larger than TotalSize.
         _dmsHeader->_storageUnitSize += _segmentPages ;

         // update SME Manager
         rc = _smeMgr.depositASegment( (dmsExtentID)beginExtentID ) ;
         if ( rc )
         {
            _dmsHeader->_storageUnitSize -= _segmentPages ;
            PD_LOG ( PDERROR, "Failed to deposit new segment into SMEMgr, "
                     "rc = %d", rc ) ;
            ossPanic() ;
            goto error ;
         }
         beginExtentID += _segmentPages ;
         fileSize += _getSegmentSize() ;

         // update header
         _dmsHeader->_pageNum += _segmentPages ;
         _pageNum = _dmsHeader->_pageNum ;
      }

   done :
      return rc ;
   error :
      goto done ;
   }

   INT32 _dmsBlockData::_findFreeSpace( UINT16 numPages, SINT32 & foundPage,
                                          dmsContext *context )
   {
      UINT32 segmentSize = 0 ;
      INT32 rc = SDB_OK ;
      INT32 rc1 = SDB_OK ;

      while ( TRUE )
      {
         rc = _smeMgr.reservePages( numPages, foundPage, &segmentSize ) ;
         if ( rc )
         {
            goto error ;
         }

         if ( DMS_INVALID_EXTENT != foundPage )
         {
            break ;
         }

         // if not able to find any, that means all pages are occupied
         // then we should call extendSegments
         ossLatch( &_segmentLatch, EXCLUSIVE ) ;         
         if ( segmentSize != _smeMgr.segmentNum() )
         {
            ossUnlatch( &_segmentLatch, EXCLUSIVE ) ;
            continue ;
         }

         // begin for extent
         rc = context ? context->pause() : SDB_OK ;
         if ( rc )
         {
            ossUnlatch( &_segmentLatch, EXCLUSIVE ) ;
            PD_LOG( PDERROR, "Failed to pause context[%s], rc: %d",
                    context->toString().c_str(), rc ) ;
            goto error ;
         }

         rc = _extendSegments( 1 ) ;

         // end to resume
         rc1 = context ? context->resume() : SDB_OK ;

         ossUnlatch( &_segmentLatch, EXCLUSIVE ) ;

         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to extend storage unit, rc=%d", rc );
            goto error ;
         }
         PD_RC_CHECK( rc1, PDERROR, "Failed to resume context[%s], rc: %d",
                      context->toString().c_str(), rc1 ) ;

         PD_LOG ( PDDEBUG, "Successfully extend storage unit for %d pages",
                  numPages ) ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_saveDeletedRecord( dmsMB *mb,
                                            const dmsRecordID &rid,
                                            INT32 recordSize,
                                            dmsExtent *extAddr,
                                            dmsDeletedRecord *pRecord )
   {
      UINT8 deleteRecordSlot = 0 ;

      SDB_ASSERT( extAddr && pRecord, "NULL Pointer" ) ;
      
      // assign flags to the memory
      pRecord->setDeleted() ;
      if ( recordSize > 0 )
      {
         pRecord->setSize( recordSize ) ;
      }
      else
      {
         recordSize = pRecord->getSize() ;
      }
      pRecord->setMyOffset( rid._offset ) ;

      // change free space
      extAddr->_freeSpace += recordSize ;
      _mbStatInfo[mb->_blockID]._totalDataFreeSpace += recordSize ;

      // let's count which delete slots it fits
      // divide by 32 first since our first slot is for <32 bytes
      recordSize = ( recordSize - 1 ) >> 5 ;
      // while loop, divde by 2 everytime, find the closest delete slot
      // for example, for a given size 3000, we should go _4k (which is
      // _deleteList[7], using 3000>>5=93
      // then in a loop, first round we have 46, type=1
      // then 23, type=2
      // then 11, type=3
      // then 5, type=4
      // then 2, type=5
      // then 1, type=6
      // finally 0, type=7

      while ( (recordSize) != 0 )
      {
         deleteRecordSlot ++ ;
         recordSize = ( recordSize >> 1 ) ;
      }

      // make sure we don't mis calculated it
      SDB_ASSERT ( deleteRecordSlot < dmsMB::_max, "Invalid record size" ) ;

      // set the first matching delete slot to the
      // next rid for the deleted record
      pRecord->setNextRID( mb->_deleteList [ deleteRecordSlot ] ) ;
      // Then assign MB delete slot to the extent and offset
      mb->_deleteList[ deleteRecordSlot ] = rid ;
      return SDB_OK ;
   }

   INT32 _dmsBlockData::_saveDeletedRecord( dmsMB * mb,
                                            const dmsRecordID &recordID,
                                            INT32 recordSize )
   {
      INT32 rc = SDB_OK ;
      dmsExtRW rw ;
      dmsRecordRW recordRW ;

      if ( recordID.isNull() )
      {
         rc = SDB_INVALIDARG ;
         goto done ;
      }

      rw = extent2RW( recordID._extent, mb->_blockID ) ;
      recordRW = record2RW( recordID, mb->_blockID ) ;

      try
      {
         dmsExtent *pExtent = rw.writePtr<dmsExtent>() ;
         dmsDeletedRecord* pRecord = recordRW.writePtr<dmsDeletedRecord>() ;
         rc = _saveDeletedRecord ( mb, recordID, recordSize,
                                   pExtent, pRecord ) ;
      }
      catch( std::exception &e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = pdGetLastError() ? pdGetLastError() : SDB_SYS ;
         goto error ;
      }

   done :
      return rc ;
   error:
      goto done ;
   }

   void _dmsBlockData::_mapExtent2DelList( dmsMB *mb, dmsExtent *extAddr,
                                             SINT32 extentID )
   {
      INT32 extentSize         = 0 ;
      INT32 extentUseableSpace = 0 ;
      INT32 deleteRecordSize   = 0 ;
      dmsOffset recordOffset   = 0 ;
      INT32 curUseableSpace    = 0 ;

      if ( (UINT32)extAddr->_freeSpace < DMS_MIN_RECORD_SZ )
      {
         if ( extAddr->_freeSpace != 0 )
         {
            PD_LOG( PDINFO, "Collection[%s, mbID: %d]'s extent[%d] free "
                    "space[%d] is less than min record size[%d]",
                    mb->_collectionName, mb->_blockID, extentID,
                    extAddr->_freeSpace, DMS_MIN_RECORD_SZ ) ;
         }
         goto done ;
      }

      // calculate the delete record size we need to use
      extentSize          = extAddr->_blockSize << pageSizeSquareRoot() ;
      extentUseableSpace  = extAddr->_freeSpace ;
      extAddr->_freeSpace = 0 ;

      // make sure the delete record is not greater 16MB
      deleteRecordSize    = OSS_MIN ( extentUseableSpace,
                                      DMS_RECORD_MAX_SZ ) ;
      // place first record offset
      recordOffset        = extentSize - extentUseableSpace ;
      curUseableSpace     = extentUseableSpace ;

      /// extentUseableSpace > 16MB
      while ( curUseableSpace - deleteRecordSize >=
              (INT32)DMS_MIN_DELETEDRECORD_SZ )
      {
         dmsRecordID rid( extentID, recordOffset ) ;
         dmsRecordRW rRW = record2RW( rid, mb->_blockID ) ;
         _saveDeletedRecord( mb, rid, deleteRecordSize,
                             extAddr, rRW.writePtr<dmsDeletedRecord>() ) ;
         curUseableSpace -= deleteRecordSize ;
         recordOffset += deleteRecordSize ;
      }
      /// 16MB < curUseableSpace < 16MB + DMS_MIN_DELETEDRECORD_SZ
      if ( curUseableSpace > deleteRecordSize )
      {
         dmsRecordID rid( extentID, recordOffset ) ;
         dmsRecordRW rRW = record2RW( rid, mb->_blockID ) ;
         _saveDeletedRecord( mb, rid, DMS_PAGE_SIZE4K,
                             extAddr, rRW.writePtr<dmsDeletedRecord>() ) ;
         curUseableSpace -= DMS_PAGE_SIZE4K ;
         recordOffset += DMS_PAGE_SIZE4K ;
      }
      /// 0 < curUseableSpace < 16MB
      if ( curUseableSpace > 0 )
      {
         dmsRecordID rid( extentID, recordOffset ) ;
         dmsRecordRW rRW = record2RW( rid, mb->_blockID ) ;
         _saveDeletedRecord( mb, rid, curUseableSpace,
                             extAddr, rRW.writePtr<dmsDeletedRecord>() ) ;
      }

      // correct check
      SDB_ASSERT( extentUseableSpace == extAddr->_freeSpace,
                  "Extent[%d] free space invalid" ) ;
   done :
      return ;
   }

   INT32 _dmsBlockData::_extentInsertRecord( dmsMBContext *context,
                                               dmsExtRW &extRW,
                                               dmsRecordRW &recordRW,
                                               const dmsRecordData &recordData,
                                               UINT32 needRecordSize,
                                               _pmdEDUCB *cb,
                                               BOOLEAN isInsert )
   {
      INT32 rc                         = SDB_OK ;
      dmsRecord* pRecord               = NULL ;
      dmsOffset  myOffset              = DMS_INVALID_OFFSET ;

      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

      pRecord = recordRW.writePtr( needRecordSize ) ;
      myOffset = pRecord->getMyOffset() ;
      // first we need to check if the delete record is large enough
      if ( pRecord->getSize() < needRecordSize )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else if ( !recordData.isCompressed()
                && recordData.len() < DMS_MIN_RECORD_DATA_SZ )
      {
         PD_LOG( PDERROR, "Bson obj size[%d] is invalid",
                 recordData.len() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      // set to normal status
      pRecord->setNormal() ;
      pRecord->resetAttr() ;

      // and then need to check if we need to split deleted record
      if ( pRecord->getSize() - needRecordSize > DMS_MIN_RECORD_SZ )
      {
         // original offset+new size = new delete offset
         dmsOffset newOffset = myOffset + needRecordSize ;
         // original size - new size = new delete size
         INT32 newSize = pRecord->getSize() - needRecordSize ;
         dmsRecordID newRid = recordRW.getRecordID() ;
         newRid._offset = newOffset ;
         rc = _saveDeletedRecord( context->mb(), newRid, newSize ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to save deleted record, rc: %d", rc ) ;
            goto error ;
         }
         // set the original place with new dmsrecordSize
         pRecord->setSize( needRecordSize ) ;
      }
      // if the leftover space is not good enough for a min_record, then we
      // don't change the record size

      // then for the original location we set new record header and copy data
      pRecord->setData( recordData ) ;

      pRecord->setNextOffset( DMS_INVALID_OFFSET ) ;
      pRecord->setPrevOffset( DMS_INVALID_OFFSET ) ;

      // no need to change offset
      if ( isInsert )
      {
         dmsExtent *extent       = extRW.writePtr<dmsExtent>() ;
         dmsOffset   offset      = extent->_lastRecordOffset ;
         // finally add the record into list
         extent->_recCount++ ;
         ++ ( _mbStatInfo[ context->mbID() ]._totalRecords ) ;
         // if there is last record in the extent
         if ( DMS_INVALID_OFFSET != offset )
         {
            // if there is already record in the extent
            dmsRecordRW preRW = record2RW( dmsRecordID( extRW.getExtentID(),
                                                        offset ),
                                           context->mbID() ) ;
            dmsRecord *preRecord = preRW.writePtr() ;
            // set the next of previous point to the new record
            preRecord->setNextOffset( myOffset ) ;
            // set the previous of current points to the original last
            pRecord->setPrevOffset( offset ) ;
         }
         extent->_lastRecordOffset = myOffset ;
         // then check for first record in extent
         if ( DMS_INVALID_OFFSET == extent->_firstRecordOffset )
         {
            // we only change it when it points to nothing
            extent->_firstRecordOffset = myOffset ;
         }
      }

      _mbStatInfo[context->mbID()]._totalOrgDataLen += recordData.orgLen() ;
      _mbStatInfo[context->mbID()]._totalDataLen += recordData.len() ;

   done :
      return rc ;
   error :
      goto done ;
   }


   INT32 _dmsBlockData::_allocateExtent( dmsMBContext *context,
                                                 UINT16 numPages,
                                                 BOOLEAN deepInit,
                                                 BOOLEAN add2LoadList,
                                                 dmsExtentID *allocExtID )
   {
      SDB_ASSERT( context, "dms mb context can't be NULL" ) ;
      INT32 rc                 = SDB_OK ;
      SINT32 firstFreeExtentID = DMS_INVALID_EXTENT ;
      dmsExtRW extRW ;
      dmsExtent *extAddr       = NULL ;
      if ( numPages > _segmentPages || numPages < 1 )
      {
         PD_LOG ( PDERROR, "Invalid number of pages: %d", numPages ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb lock failed, rc: %d", rc ) ;

      rc = _findFreeSpace ( numPages, firstFreeExtentID, context ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Error find free space for %d pages, rc = %d",
                  numPages, rc ) ;
         goto error ;
      }

      if ( DMS_INVALID_EXTENT == firstFreeExtentID )
      {
         // In _findFreeSpace, it decided not to allocate space this time.
         *allocExtID = DMS_INVALID_EXTENT ;
         goto done ;
      }

      extRW = extent2RW( firstFreeExtentID, context->mbID() ) ;
      extRW.setNothrow( TRUE ) ;
      extAddr = extRW.writePtr<dmsExtent>() ;
      if ( !extAddr )
      {
         PD_LOG( PDERROR, "Get extent[%d] address failed",
                 firstFreeExtentID ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      /// Init
      extAddr->init( numPages, context->mbID(),
                     (UINT32)numPages << pageSizeSquareRoot() ) ;

      // and add the new extent into MB's extent chain
      // now let's change the extent pointer into MB's extent list
      // new extent->preextent always assign to _mbList.lastExtentID
      if ( TRUE == add2LoadList )
      {
         extAddr->_prevExtent = context->mb()->_loadLastExtentID ;
         extAddr->_nextExtent = DMS_INVALID_EXTENT ;
         // if this is the load first extent in this MB, we assign
         // firstExtentID to it
         if ( DMS_INVALID_EXTENT == context->mb()->_loadFirstExtentID )
         {
            context->mb()->_loadFirstExtentID = firstFreeExtentID ;
         }

         if ( DMS_INVALID_EXTENT != extAddr->_prevExtent )
         {
            dmsExtRW prevRW = extent2RW( extAddr->_prevExtent,
                                         context->mbID() ) ;
            dmsExtent *prevExt = prevRW.writePtr<dmsExtent>() ;
            prevExt->_nextExtent = firstFreeExtentID ;
         }

         // MB's last extent always assigned to the new extent
         context->mb()->_loadLastExtentID = firstFreeExtentID ;
      }
      else
      {
         rc = addExtent2Meta( firstFreeExtentID, extAddr, context ) ;
         PD_RC_CHECK( rc, PDERROR, "Add extent to meta failed, rc: %d", rc ) ;
      }

      if ( allocExtID )
      {
         *allocExtID = firstFreeExtentID ;
      }

      if ( deepInit )
      {
         _mapExtent2DelList( context->mb(), extAddr, firstFreeExtentID ) ;
      }

   done :
      return rc ;
   error :
      //TODO: error handing
      goto done ;
   }

   INT32 _dmsBlockData::addExtent2Meta( dmsExtentID extID,
                                        dmsExtent *extent,
                                        dmsMBContext *context )
   {
      INT32 rc = SDB_OK ;
      dmsExtRW mbExRW ;
      dmsMBEx *mbEx = NULL ;
      UINT32 segID = extent2Segment( extID ) - dataStartSegID() ;
      dmsExtentID lastExtID = DMS_INVALID_EXTENT ;
      dmsExtRW prevRW ;
      dmsExtRW nextRW ;
      dmsExtent *prevExt = NULL ;
      dmsExtent *nextExt = NULL ;

      if ( !context->isMBLock( EXCLUSIVE ) )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Caller must hold mb exclusive lock[%s]",
                 context->toString().c_str() ) ;
         goto error ;
      }

      mbExRW = extent2RW( context->mb()->_mbExExtentID,
                          context->mbID() ) ;
      mbExRW.setNothrow( TRUE ) ;
      mbEx = mbExRW.writePtr<dmsMBEx>() ;
      if ( NULL == mbEx )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "dms mb expand extent is invalid: %d",
                 context->mb()->_mbExExtentID ) ;
         goto error ;
      }

      if ( segID >= mbEx->_header._segNum )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Invalid segID: %d, max segNum: %d", segID,
                 mbEx->_header._segNum ) ;
         goto error ;
      }

      /// re-write ptr
      mbEx = mbExRW.writePtr<dmsMBEx>( 0,
                                       (UINT32)mbEx->_header._blockSize <<
                                       pageSizeSquareRoot() ) ;  
      mbEx->getLastExtentID( segID, lastExtID ) ;

      if ( DMS_INVALID_EXTENT == lastExtID )
      {
         extent->_logicID = ( segID << _getFactor() ) ;
         mbEx->setFirstExtentID( segID, extID ) ;
         mbEx->setLastExtentID( segID, extID ) ;
         ++(mbEx->_header._usedSegNum) ;

         // find prevExt
         INT32 tmpSegID = segID ;
         dmsExtentID tmpExtID = DMS_INVALID_EXTENT ;
         while ( DMS_INVALID_EXTENT != context->mb()->_firstExtentID &&
                 --tmpSegID >= 0 )
         {
            mbEx->getLastExtentID( tmpSegID, tmpExtID ) ;
            if ( DMS_INVALID_EXTENT != tmpExtID )
            {
               extent->_prevExtent = tmpExtID ;
               prevRW = extent2RW( tmpExtID, context->mbID() ) ;
               prevExt = prevRW.writePtr<dmsExtent>() ;
               break ;
            }
         }
      }
      else
      {
         mbEx->setLastExtentID( segID, extID ) ;
         extent->_prevExtent = lastExtID ;
         prevRW = extent2RW( lastExtID, context->mbID() ) ;
         prevExt = prevRW.writePtr<dmsExtent>() ;
         extent->_logicID = prevExt->_logicID + 1 ;
      }

      if ( prevExt )
      {
         if ( DMS_INVALID_EXTENT != prevExt->_nextExtent )
         {
            extent->_nextExtent = prevExt->_nextExtent ;
            nextRW = extent2RW( extent->_nextExtent, context->mbID() ) ;
            nextExt = nextRW.writePtr<dmsExtent>() ;
            nextExt->_prevExtent = extID ;
         }
         else
         {
            context->mb()->_lastExtentID = extID ;
         }
         prevExt->_nextExtent = extID ;
      }
      else
      {
         if ( DMS_INVALID_EXTENT != context->mb()->_firstExtentID )
         {
            extent->_nextExtent = context->mb()->_firstExtentID ;
            nextRW = extent2RW( extent->_nextExtent, context->mbID() ) ;
            nextExt = nextRW.writePtr<dmsExtent>() ;
            nextExt->_prevExtent = extID ;
         }
         context->mb()->_firstExtentID = extID ;

         if ( DMS_INVALID_EXTENT == context->mb()->_lastExtentID )
         {
            context->mb()->_lastExtentID = extID ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_prepareInsertData( const BSONObj &record,
                                            BOOLEAN mustOID,
                                            pmdEDUCB *cb,
                                            dmsRecordData &recordData,
                                            BOOLEAN &memReallocate,
                                            INT64 position )
   {
      INT32 rc = SDB_OK ;
      IDToInsert oid ;
      idToInsertEle oidEle((CHAR*)(&oid)) ;
      CHAR *pMergedData = NULL ;

      try
      {
         // Step 1: Prepare the data, add OID and compress if necessary.
         recordData.setData( record.objdata(), record.objsize(),
                             UTIL_COMPRESSOR_INVALID, TRUE ) ;
         // verify whether the record got "_id" inside
         BSONElement ele = record.getField( DMS_ID_KEY_NAME ) ;
         const CHAR *pCheckErr = "" ;
         if ( !dmsIsRecordIDValid( ele, TRUE, &pCheckErr ) )
         {
            PD_LOG( PDERROR, "Record[%s] _id is error: %s",
                    record.toString().c_str(), pCheckErr ) ;
            rc = SDB_INVALIDARG ;
            goto error ;
         }
         // judge must oid
         if ( mustOID && ele.eoo() )
         {
            oid._oid.init() ;
            rc = cb->allocBuff( oidEle.size() + record.objsize(),
                                &pMergedData ) ;
            if ( rc )
            {
               PD_LOG( PDERROR, "Alloc memory[size:%u] failed, rc: %d",
                       oidEle.size() + record.objsize(), rc ) ;
               goto error ;
            }
            /// copy to new data
            *(UINT32*)pMergedData = oidEle.size() + record.objsize() ;
            ossMemcpy( pMergedData + sizeof(UINT32), oidEle.rawdata(),
                       oidEle.size() ) ;
            ossMemcpy( pMergedData + sizeof(UINT32) + oidEle.size(),
                       record.objdata() + sizeof(UINT32),
                       record.objsize() - sizeof(UINT32) ) ;
            recordData.setData( pMergedData,
                                oidEle.size() + record.objsize(),
                                UTIL_COMPRESSOR_INVALID, TRUE ) ;
            memReallocate = TRUE ;
         }
      }
      catch ( std::exception &e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = pdGetLastError() ? pdGetLastError() : SDB_SYS ;
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_allocRecordSpace( dmsMBContext *context,
                                           UINT32 requiredSize,
                                           dmsRecordID &foundRID,
                                           pmdEDUCB *cb )
   {
      INT32 rc                      = SDB_OK ;
      UINT32 dmsRecordSizeTemp      = 0 ;
      UINT8  deleteRecordSlot       = 0 ;
      const static INT32 s_maxSearch = 3 ;

      INT32  j                      = 0 ;
      INT32  i                      = 0 ;
      dmsRecordID foundDeletedID  ;
      dmsRecordRW delRecordRW ;
      const dmsDeletedRecord* pRead = NULL ;

      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

   retry:
      // let's count which delete slots it fits
      // divide by 32 first since our first slot is for <32 bytes
      dmsRecordSizeTemp = ( requiredSize-1 ) >> 5 ;
      deleteRecordSlot  = 0 ;
      while ( dmsRecordSizeTemp != 0 )
      {
         deleteRecordSlot ++ ;
         dmsRecordSizeTemp = dmsRecordSizeTemp >> 1 ;
      }

      SDB_ASSERT( deleteRecordSlot < dmsMB::_max, "Invalid record size" ) ;

      if ( deleteRecordSlot >= dmsMB::_max )
      {
         PD_LOG( PDERROR, "Invalid record size: %u", requiredSize ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      rc = SDB_DMS_NOSPC ;
      try
      {
         for ( j = deleteRecordSlot ; j < dmsMB::_max ; ++j )
         {
            dmsRecordRW preRW ;
            // get the first delete record from delete list
            foundDeletedID = _dmsMME->_mbList[context->mbID()]._deleteList[j] ;
            for ( i = 0 ; i < s_maxSearch ; ++i )
            {
               // if we don't get a valid record id, we break to get next slot
               if ( foundDeletedID.isNull() )
               {
                  break ;
               }
               delRecordRW = record2RW( foundDeletedID, context->mbID() ) ;
               pRead = delRecordRW.readPtr<dmsDeletedRecord>() ;

               // once the extent is valid, let's check the record is deleted
               // and got sufficient size for us
               if( pRead->isDeleted() && pRead->getSize() >= requiredSize )
               {
                  if ( preRW.isEmpty() )
                     {
                        // it's just the first one from delete list, let's get it
                        context->mb()->_deleteList[j] = pRead->getNextRID() ;
                     }
                     else
                     {
                        dmsDeletedRecord *preWrite =
                           preRW.writePtr<dmsDeletedRecord>() ;
                        // we need to link the previous delete record to the next
                        preWrite->setNextRID( pRead->getNextRID() ) ;
                     }

                     // change extent free space
                     dmsExtRW rw = extent2RW( foundDeletedID._extent,
                                              context->mbID() ) ;
                     dmsExtent *pExtent = rw.writePtr<dmsExtent>() ;
                     pExtent->_freeSpace -= pRead->getSize() ;

                     foundRID = foundDeletedID ;
                     rc = SDB_OK ;
                     goto done ;
               }
            
               //for some reason this slot can't be reused, let's get to the next
               preRW = delRecordRW ;
               foundDeletedID = pRead->getNextRID() ;
            }
         }
      }
      catch( std::exception &e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = pdGetLastError() ? pdGetLastError() : SDB_SYS ;
         goto error ;
      }

      // no space, need to allocate extent
      {
         UINT32 expandSize = requiredSize << DMS_RECORDS_PER_EXTENT_SQUARE ;
         if ( expandSize > DMS_BEST_UP_EXTENT_SZ )
         {
            expandSize = requiredSize < DMS_BEST_UP_EXTENT_SZ ?
                         DMS_BEST_UP_EXTENT_SZ : requiredSize ;
         }
         UINT32 reqPages = ( expandSize + DMS_EXTENT_METADATA_SZ +
                             pageSize() - 1 ) >> pageSizeSquareRoot() ;
         if ( reqPages > segmentPages() )
         {
            reqPages = segmentPages() ;
         }
         if ( reqPages < 1 )
         {
            reqPages = 1 ;
         }

         rc = _allocateExtent( context, reqPages, TRUE, FALSE, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Unable to allocate %d pages extent to the "
                      "collection, rc: %d", reqPages, rc ) ;
         goto retry ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::insertRecord ( dmsMBContext *context,
                                       const BSONObj &record,
                                       pmdEDUCB *cb,
                                       BOOLEAN mustOID,
                                       BOOLEAN canUnLock,
                                       INT64 position )
   {
      INT32 rc                      = SDB_OK ;
      UINT32 dmsRecordSize          = 0 ;
      BSONObj insertObj             = record ;

      // delete record related
      dmsRecordID foundRID ;
      dmsRecordData recordData ;
      dmsExtRW extRW ;
      dmsRecordRW recordRW ;
      const dmsExtent *pExtent      = NULL ;
      BOOLEAN newMem                = FALSE ;
      CHAR *pMergedData             = NULL ;

      try
      {
         rc = _prepareInsertData( record, mustOID, cb, recordData,
                                  newMem, position ) ;
         PD_RC_CHECK( rc, PDERROR, "Prepare data for insertion failed, rc: %d",rc ) ;
         if ( newMem )
         {
            pMergedData = (CHAR *)recordData.data() ;
         }

         insertObj = BSONObj( recordData.data() ) ;
         dmsRecordSize = recordData.len() ;

         // check
         if ( recordData.len() + DMS_RECORD_METADATA_SZ >
              DMS_RECORD_USER_MAX_SZ )
         {
            rc = SDB_DMS_RECORD_TOO_BIG ;
            goto error ;
         }


         _finalRecordSize( dmsRecordSize, recordData ) ;

         //TODO: calculate necessary log space and reserve it

         if ( position >= 0 )
         {
            rc = _allocRecordSpaceByPos( context, dmsRecordSize, position,
                                            foundRID, cb ) ;
         }
         else
         {
            rc = _allocRecordSpace( context, dmsRecordSize, foundRID, cb ) ;
         }
         PD_RC_CHECK( rc, PDERROR, "Allocate space for record failed, "
                      "rc: %d", rc ) ;

         // Step 3: Insert the record into target extent.
         /// validate the extent page information
         extRW = extent2RW( foundRID._extent, context->mbID() ) ;
         pExtent = extRW.readPtr<dmsExtent>() ;
         if ( !pExtent->validate( context->mbID() ) )
         {
            rc = SDB_SYS ;
            goto error ;
         }
         recordRW = record2RW( foundRID, context->mbID() ) ;

         // insert to extent
         rc = _extentInsertRecord( context, extRW, recordRW, recordData,
                                   dmsRecordSize, cb, TRUE ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to append record, rc: %d", rc ) ;

      }
      catch( std::exception &e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = pdGetLastError() ? pdGetLastError() : SDB_SYS ;
         goto error ;
      }
      //TODO: log the insert operation


   done:
      //TODO: release reserved log space

      if ( pMergedData )
      {
         cb->releaseBuff( pMergedData ) ;
      }

      return rc ;
   error:
      goto done ;

   }

   INT32 _dmsBlockData::openStorage ( const CHAR *pPath,
                                      BOOLEAN createNew )
   {
      INT32 rc               = SDB_OK ;
      UINT64 fileSize        = 0 ;
      UINT64 currentOffset   = 0 ;
      UINT32 mode = OSS_READWRITE|OSS_EXCLUSIVE ;
      UINT64 rightSize = 0 ;
      BOOLEAN reGetSize = FALSE ;

      SDB_ASSERT( pPath, "path can't be NULL" ) ;

      if ( NULL == _pStorageInfo )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      if ( createNew )
      {
         mode |= OSS_CREATEONLY ;
         _isCrash = FALSE ;
      }

      rc = utilBuildFullPath( pPath, _suFileName, OSS_MAX_PATHSIZE,
                              _fullPathName ) ;

      if ( rc )
      {
         PD_LOG ( PDERROR, "Path+filename are too long: %s; %s", pPath,
                  _suFileName ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      PD_LOG ( PDDEBUG, "Open storage unit file %s", _fullPathName ) ;

      // open the file, create one if not exist
      rc = ossMmapFile::open ( _fullPathName, mode, OSS_RU|OSS_WU|OSS_RG ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to open %s, rc=%d", _fullPathName, rc ) ;
         goto error ;
      }
      if ( createNew )
      {
         PD_LOG( PDEVENT, "Create storage unit file[%s] succeed, "
                 "mode: 0x%08x", _fullPathName, mode ) ;
      }

      rc = ossMmapFile::size ( fileSize ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to get file size: %s, rc: %d",
                  _fullPathName, rc ) ;
         goto error ;
      }

      // is it a brand new file
      if ( 0 == fileSize )
      {
         // if it's a brand new file but we don't ask for creating new storage
         // unit, then we exit with invalid su error
         if ( !createNew )
         {
            PD_LOG ( PDERROR, "Storage unit file[%s] is empty", _suFileName ) ;
            rc = SDB_DMS_INVALID_SU ;
            goto error ;
         }
         rc = _initializeStorageUnit () ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to initialize Storage Unit, rc=%d", rc ) ;
            goto error ;
         }
         // then we get the size again to make sure it's what we need
         rc = ossMmapFile::size ( fileSize ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to get file size: %s, rc: %d",
                     _suFileName, rc ) ;
            goto error ;
         }
      }

      if ( fileSize < _dataOffset() )
      {
         PD_LOG ( PDERROR, "Invalid storage unit size: %s", _suFileName ) ;
         PD_LOG ( PDERROR, "Expected more than %d bytes, actually read %lld "
                  "bytes", _dataOffset(), fileSize ) ;
         rc = SDB_DMS_INVALID_SU ;
         goto error ;
      }

      // map metadata
      // header, 64K
      rc = map ( DMS_HEADER_OFFSET, DMS_HEADER_SZ, (void**)&_dmsHeader ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to map header: %s", _suFileName ) ;
         goto error ;
      }

      // after we load SU, let's verify it's expected file
      rc = _validateHeader( _dmsHeader ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Storage Unit Header is invalid: %s, rc: %d",
                  _suFileName, rc ) ;
         goto error ;
      }

      // SME, 16MB
      rc = map ( DMS_SME_OFFSET, DMS_SME_SZ, (void**)&_dmsSME ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to map SME: %s", _suFileName ) ;
         goto error ;
      }

      // initialize SME Manager, which is used to do fast-lookup and release
      // for extents. Note _pageSize is initialized in _validateHeader, so
      // we are safe to use page size here
      rc = _smeMgr.init ( this, _dmsSME ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to initialize SME, rc = %d", rc ) ;
         goto error ;
      }

      rc = _onMapMeta( (UINT64)( DMS_SME_OFFSET + DMS_SME_SZ ) ) ;
      PD_RC_CHECK( rc, PDERROR, "map file[%s] meta failed, rc: %d",
                   _suFileName, rc ) ;

      // make sure the file size is multiple of segments
      if ( 0 != ( fileSize - _dataOffset() ) % _getSegmentSize() )
      {
         PD_LOG ( PDWARNING, "Unexpected length[%llu] of file: %s", fileSize,
                  _suFileName ) ;

         /// need to truncate the file
         rightSize = ( ( fileSize - _dataOffset() ) /
                       _getSegmentSize() ) * _getSegmentSize() +
                       _dataOffset() ;
         rc = ossTruncateFile( &_file, (INT64)rightSize ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Truncate file[%s] to size[%llu] failed, rc: %d",
                    _suFileName, rightSize, rc ) ;
            goto error ;
         }
         PD_LOG( PDEVENT, "Truncate file[%s] to size[%llu] succeed",
                 _suFileName, rightSize ) ;
         // then we get the size again to make sure it's what we need
         rc = ossMmapFile::size ( fileSize ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to get file size: %s, rc: %d",
                     _suFileName, rc ) ;
            goto error ;
         }
      }

      rightSize = (UINT64)_dmsHeader->_storageUnitSize * pageSize() ;
      /// make sure the file is correct with meta data
      if ( fileSize > rightSize )
      {
         PD_LOG( PDWARNING, "File[%s] size[%llu] is grater than storage "
                 "unit pages[%u]", _suFileName, fileSize,
                 _dmsHeader->_storageUnitSize ) ;

         rc = ossTruncateFile( &_file, (INT64)rightSize ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Truncate file[%s] to size[%llu] failed, rc: %d",
                    _suFileName, rightSize, rc ) ;
            goto error ;
         }
         PD_LOG( PDEVENT, "Truncate file[%s] to size[%lld] succeed",
                 _suFileName, rightSize ) ;
         reGetSize = TRUE ;
      }
      else if ( fileSize < rightSize )
      {
         PD_LOG( PDWARNING, "File[%s] size[%llu] is less than storage "
                 "unit pages[%u]", _suFileName, fileSize,
                 _dmsHeader->_storageUnitSize ) ;

         rc = ossExtendFile( &_file, (INT64)( rightSize - fileSize ) ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Extend file[%s] to size[%lld] from size[%lld] "
                    "failed, rc: %d", _suFileName, rightSize,
                    fileSize, rc ) ;
            goto error ;
         }
         PD_LOG( PDEVENT, "Extend file[%s] to size[%lld] from size[%lld] "
                 "succeed", _suFileName, rightSize, fileSize ) ;
         reGetSize = TRUE ;
      }

      if ( reGetSize )
      {
         // then we get the size again to make sure it's what we need
         rc = ossMmapFile::size ( fileSize ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to get file size: %s, rc: %d",
                     _suFileName, rc ) ;
            goto error ;
         }
         reGetSize = FALSE ;
      }

      // loop and map each segment into separate mem range
      _dataSegID = segmentSize() ;
      currentOffset = _dataOffset() ;
      while ( currentOffset < fileSize )
      {
         rc = map( currentOffset, _getSegmentSize(), NULL ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to map data segment at offset %lld",
                     currentOffset ) ;
            goto error ;
         }
         currentOffset += _getSegmentSize() ;
      }
      _maxSegID = (INT32)segmentSize() - 1 ;

      // create dirtyList to record dirty pages. Note dirty list doesn't
      // contain header and metadata segments, only for data segments
      rc = _dirtyList.init( maxSegmentNum() ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Init dirty list failed in file[%s], rc: %d",
                  _suFileName, rc ) ;
         goto error ;
      }
      _dirtyList.setSize( segmentSize() - _dataSegID ) ;

      _isClosed = FALSE ;

   done:
      return rc ;
   error:
      ossMmapFile::close () ;
      _postOpen( rc ) ;
      goto done ;
   }

   INT32 _dmsBlockData::addCollection( const CHAR * pName,
                                       UINT16 * collectionID,
                                       utilCLUniqueID clUniqueID,
                                       UINT32 attributes,
                                       pmdEDUCB * cb,
                                       UINT16 initPages,
                                       BOOLEAN sysCollection,
                                       UINT32 *logicID,
                                       const BSONObj *extOptions )
   {
      INT32 rc                = SDB_OK ;
      UINT16 newCollectionID  = DMS_INVALID_MBID ;
      UINT32 logicalID        = DMS_INVALID_CLID ;
      BOOLEAN metalocked      = FALSE ;
      dmsMB *mb               = NULL ;
      dmsMBContext *context   = NULL ;

      UINT32 segNum           = DMS_MAX_PG >> segmentPagesSquareRoot() ;
      UINT32 mbExSize         = (( segNum << 3 ) >> pageSizeSquareRoot()) + 1 ;
      UINT16 optExtSize       = 0 ;
      dmsExtentID mbExExtent  = DMS_INVALID_EXTENT ;
      dmsExtentID mbOptExtent = DMS_INVALID_EXTENT ;
      dmsMetaExtent *mbExtent = NULL ;
      dmsExtRW rw ;

      SDB_ASSERT( pName, "Collection name cat't be NULL" ) ;

      // check cl name
      rc = dmsCheckCLName ( pName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid collection name %s, rc: %d",
                   pName, rc ) ;

      // allocate the meta extent
      rc = _findFreeSpace( mbExSize, mbExExtent, NULL ) ;
      PD_RC_CHECK( rc, PDERROR, "Allocate metablock expand extent failed, "
                   "pageNum: %d, rc: %d", mbExSize, rc ) ;

      // first exclusive latch metadata, this shouldn't be replaced by SHARED to
      // prevent racing with dropCollection
      ossLatch( &_metadataLatch, EXCLUSIVE ) ;
      metalocked = TRUE ;

      // then let's make sure the collection name does not exist
      if ( DMS_INVALID_MBID != _collectionNameLookup ( pName ) )
      {
         rc = SDB_DMS_EXIST ;
         goto error ;
      }
      if ( DMS_INVALID_MBID != _collectionIdLookup ( clUniqueID ) )
      {
         rc = SDB_DMS_UNIQUEID_CONFLICT ;
         goto error ;
      }

      if ( DMS_MME_SLOTS <= _dmsHeader->_numMB )
      {
         PD_LOG ( PDERROR, "There is no free slot for extra collection" ) ;
         rc = SDB_DMS_NOSPC ;
         goto error ;
      }

      // find a slot
      for ( UINT32 i = 0 ; i < DMS_MME_SLOTS ; ++i )
      {
         if ( DMS_IS_MB_FREE ( _dmsMME->_mbList[i]._flag ) )
         {
            newCollectionID = i ;
            break ;
         }
      }
      // make sure we find free collection id
      if ( DMS_INVALID_MBID == newCollectionID )
      {
         PD_LOG ( PDERROR, "Unable to find free collection id" ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      // set mb meta data and header data
      logicalID = _dmsHeader->_MBHWM++ ;
      mb = &_dmsMME->_mbList[newCollectionID] ;
      mb->reset( pName, clUniqueID, newCollectionID, logicalID,
                 attributes ) ;
      _mbStatInfo[ newCollectionID ].reset() ;
      _mbStatInfo[ newCollectionID ]._startLID = logicalID ;

      _dmsHeader->_numMB++ ;
      _collectionInsert( pName, newCollectionID, clUniqueID ) ;

      rw = extent2RW( mbExExtent, newCollectionID ) ;
      rw.setNothrow( TRUE ) ;
      mbExtent = rw.writePtr<dmsMetaExtent>( 0,
                                             mbExSize <<
                                             pageSizeSquareRoot() ) ;
      PD_CHECK( mbExtent, SDB_SYS, error, PDERROR,
                "Invalid meta extent[%d]", mbExExtent ) ;
      mbExtent->init( mbExSize, newCollectionID, segNum ) ;
      mb->_mbExExtentID = mbExExtent ;
      mbExExtent = DMS_INVALID_EXTENT ;

      mb->_mbOptExtentID = mbOptExtent ;

      // release meta lock
      if ( metalocked )
      {
         ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;
         metalocked = FALSE ;
      }

      if ( collectionID )
      {
         *collectionID = newCollectionID ;
      }

      // create dms cb context
      rc = getMBContext( &context, newCollectionID, logicalID, logicalID, EXCLUSIVE ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Failed to get mb[%u] context, rc: %d",
                 newCollectionID, rc ) ;
         if ( SDB_DMS_NOTEXIST == rc )
         {
            newCollectionID = DMS_INVALID_MBID ;
         }
         goto error ;
      }

      // allocate new extent
      if ( 0 != initPages )
      {
         rc = _allocateExtent( context, initPages, TRUE, FALSE, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Allocate new %u pages of collection[%s] "
                      "failed, rc: %d", initPages, pName, rc ) ;
      }

      if ( logicID )
      {
         *logicID = logicalID ;
      }

   done:
      if ( context )
      {
         releaseMBContext( context ) ;
      }
      if ( SDB_OK == rc )
      {
         flushMeta() ;
      }
      return rc ;

   error:
      if ( metalocked )
      {
         ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;
      }
      if ( DMS_INVALID_EXTENT != mbExExtent )
      {
         _releaseSpace( mbExExtent, mbExSize ) ;
      }
      if ( DMS_INVALID_EXTENT != mbOptExtent )
      {
         _releaseSpace( mbOptExtent, optExtSize ) ;
      }

      goto done ;
   }

   INT32 _dmsBlockData::_initializeStorageUnit ()
   {
      INT32   rc        = SDB_OK ;
      _dmsHeader        = NULL ;
      _dmsSME           = NULL ;

      // move to beginning of the file
      rc = ossSeek ( &_file, 0, OSS_SEEK_SET ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to seek to beginning of the file, rc: %d",
                  rc ) ;
         goto error ;
      }

      // allocate buffer for dmsHeader
      _dmsHeader = SDB_OSS_NEW dmsStorageUnitHeader ;
      if ( !_dmsHeader )
      {
         PD_LOG ( PDSEVERE, "Failed to allocate memory to for dmsHeader" ) ;
         PD_LOG ( PDSEVERE, "Requested memory: %d bytes", DMS_HEADER_SZ ) ;
         rc = SDB_OOM ;
         goto error ;
      }

      // initialize a new header with empty size
      _initHeader ( _dmsHeader ) ;

      // write the buffer into file
      rc = _writeFile ( &_file, (const CHAR *)_dmsHeader, DMS_HEADER_SZ ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to write to file duirng SU init, rc: %d",
                  rc ) ;
         goto error ;
      }
      SDB_OSS_DEL _dmsHeader ;
      _dmsHeader = NULL ;

      // then SME
      _dmsSME = SDB_OSS_NEW dmsSpaceManagementExtent ;
      if ( !_dmsSME )
      {
         PD_LOG ( PDSEVERE, "Failed to allocate memory to for dmsSME" ) ;
         PD_LOG ( PDSEVERE, "Requested memory: %d bytes", DMS_SME_SZ ) ;
         rc = SDB_OOM ;
         goto error ;
      }

      rc = _writeFile ( &_file, (CHAR *)_dmsSME, DMS_SME_SZ ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to write to file duirng SU init, rc: %d",
                  rc ) ;
         goto error ;
      }
      SDB_OSS_DEL _dmsSME ;
      _dmsSME = NULL ;

      rc = _onCreate( &_file, (UINT64)( DMS_HEADER_SZ + DMS_SME_SZ )  ) ;
      PD_RC_CHECK( rc, PDERROR, "create storage unit failed, rc: %d", rc ) ;

   done :
      return rc ;
   error :
      if (_dmsHeader)
      {
         SDB_OSS_DEL _dmsHeader ;
         _dmsHeader = NULL ;
      }
      if (_dmsSME)
      {
         SDB_OSS_DEL _dmsSME ;
         _dmsSME = NULL ;
      }
      goto done ;
   }

   INT32 _dmsBlockData::_writeFile( OSSFILE *file, const CHAR * pData,
                                      INT64 dataLen )
   {
      INT32 rc = SDB_OK;
      SINT64 written = 0;
      SINT64 needWrite = dataLen;
      SINT64 bufOffset = 0;

      while ( 0 < needWrite )
      {
         rc = ossWrite( file, pData + bufOffset, needWrite, &written );
         if ( rc && SDB_INTERRUPT != rc )
         {
            PD_LOG( PDWARNING, "Failed to write data, rc: %d", rc ) ;
            goto error ;
         }
         needWrite -= written ;
         bufOffset += written ;

         rc = SDB_OK ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_onCreate( OSSFILE * file, UINT64 curOffSet )
   {
      INT32 rc          = SDB_OK ;
      _dmsMME           = NULL ;

      SDB_ASSERT( DMS_MME_OFFSET == curOffSet, "Offset is not MME offset" ) ;

      _dmsMME = SDB_OSS_NEW dmsMetadataManagementExtent ;
      if ( !_dmsMME )
      {
         PD_LOG ( PDSEVERE, "Failed to allocate memory to for dmsMME" ) ;
         PD_LOG ( PDSEVERE, "Requested memory: %d bytes", DMS_MME_SZ ) ;
         rc = SDB_OOM ;
         goto error ;
      }
      _initializeMME () ;

      rc = _writeFile ( file, (CHAR *)_dmsMME, DMS_MME_SZ ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to write to file duirng SU init, rc: %d",
                  rc ) ;
         goto error ;
      }
      SDB_OSS_DEL _dmsMME ;
      _dmsMME = NULL ;

   done:
      return rc ;
   error:
      if ( _dmsMME )
      {
         SDB_OSS_DEL _dmsMME ;
         _dmsMME = NULL ;
      }
      goto done ;
   }

   INT32 _dmsBlockData::_validateHeader( dmsStorageUnitHeader * pHeader )
   {
      INT32 rc = SDB_OK ;

      // check eye catcher
      if ( 0 != ossStrncmp ( pHeader->_eyeCatcher, DMS_DATASU_EYECATCHER,
                             DMS_HEADER_EYECATCHER_LEN ) )
      {
         CHAR szTmp[ DMS_HEADER_EYECATCHER_LEN + 1 ] = {0} ;
         ossStrncpy( szTmp, pHeader->_eyeCatcher, DMS_HEADER_EYECATCHER_LEN ) ;
         PD_LOG ( PDERROR, "Invalid eye catcher: %s", szTmp ) ;
         rc = SDB_INVALID_FILE_TYPE ;
         goto error ;
      }

      // check version
      if ( pHeader->_version > _curVersion() )
      {
         PD_LOG( PDERROR, "Incompatible version: %u", pHeader->_version ) ;
         rc = SDB_DMS_INCOMPATIBLE_VERSION ;
         goto error ;
      }

      // check page size
      rc = _checkPageSize( pHeader ) ;
      if ( rc )
      {
         goto error ;
      }
      _pageSize = pHeader->_pageSize ;

      if ( 0 != _dataOffset() % pHeader->_pageSize )
      {
         rc = SDB_SYS ;
         PD_LOG( PDSEVERE, "Dms storage meta size[%llu] is not a mutiple of "
                 "pagesize[%u]", _dataOffset(), pHeader->_pageSize ) ;
      }
      else if ( DMS_MAX_PG < pHeader->_pageNum )
      {
         PD_LOG ( PDERROR, "Invalid storage unit page number: %u",
                  pHeader->_pageNum ) ;
         rc = SDB_SYS ;
      }
      else if ( pHeader->_storageUnitSize - pHeader->_pageNum !=
                _dataOffset() / pHeader->_pageSize )
      {
         PD_LOG( PDERROR, "Invalid storage unit size: %u",
                 pHeader->_storageUnitSize ) ;
         rc = SDB_SYS ;
      }
      else if ( 0 != ossStrncmp ( _pStorageInfo->_suName, pHeader->_name,
                                  DMS_SU_NAME_SZ ) )
      {
         PD_LOG ( PDERROR, "Invalid storage unit name: %s", pHeader->_name ) ;
         rc = SDB_SYS ;
      }

      if ( rc )
      {
         goto error ;
      }

      if ( !ossIsPowerOf2( pHeader->_pageSize, &_pageSizeSquare ) )
      {
         PD_LOG( PDERROR, "Page size must be the power of 2" ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      if ( _pStorageInfo->_sequence != pHeader->_sequence )
      {
         _pStorageInfo->_sequence = pHeader->_sequence ;
      }

      _pageNum = pHeader->_pageNum ;
      _segmentPages = _getSegmentSize() >> _pageSizeSquare ;

      if ( !ossIsPowerOf2( _segmentPages, &_segmentPagesSquare ) )
      {
         PD_LOG( PDERROR, "Segment pages[%u] must be the power of 2",
                 _segmentPages ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      if ( _pStorageInfo->_csUniqueID != pHeader->_csUniqueID )
      {
         _pStorageInfo->_csUniqueID = pHeader->_csUniqueID ;
      }

      PD_LOG ( PDDEBUG, "Validated storage unit file %s\n"
               "page size: %d\ndata size: %d pages\nname: %s\nsequence: %d",
               getSuFileName(), pHeader->_pageSize, pHeader->_pageNum,
               pHeader->_name, pHeader->_sequence ) ;

   done :
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_onMapMeta( UINT64 curOffSet )
   {
      INT32 rc = SDB_OK ;

      // MME, 4MB
      _mmeSegID = ossMmapFile::segmentSize() ;
      rc = map ( DMS_MME_OFFSET, DMS_MME_SZ, (void**)&_dmsMME ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to map MME: %s", getSuFileName() ) ;
         goto error ;
      }

      // load collection names in the SU
      for ( UINT16 i = 0 ; i < DMS_MME_SLOTS ; i++ )
      {
         _mbStatInfo[i]._lastWriteTick = ~0 ;

         if ( DMS_IS_MB_INUSE ( _dmsMME->_mbList[i]._flag ) )
         {
            _collectionInsert ( _dmsMME->_mbList[i]._collectionName, i,
                                _dmsMME->_mbList[i]._clUniqueID ) ;

            _mbStatInfo[i]._totalRecords = _dmsMME->_mbList[i]._totalRecords ;
            _mbStatInfo[i]._totalDataPages =
               _dmsMME->_mbList[i]._totalDataPages ;
            _mbStatInfo[i]._totalDataFreeSpace =
               _dmsMME->_mbList[i]._totalDataFreeSpace ;
            _mbStatInfo[i]._totalDataLen =
               _dmsMME->_mbList[i]._totalDataLen ;
            _mbStatInfo[i]._totalOrgDataLen =
               _dmsMME->_mbList[i]._totalOrgDataLen ;
            _mbStatInfo[i]._startLID =
               _dmsMME->_mbList[i]._logicalID ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsBlockData::_postOpen( INT32 cause )
   {
      INT32 rc = SDB_OK ;

      if ( SDB_DMS_INVALID_SU == cause &&
           _fullPathName[0] != '\0' )
      {
         CHAR tmpFile[ OSS_MAX_PATHSIZE + 1 ] = { 0 } ;
         ossSnprintf( tmpFile, OSS_MAX_PATHSIZE, "%s.err.%u",
                      _fullPathName, ossGetCurrentProcessID() ) ;
         if ( SDB_OK == ossAccess( tmpFile, 0 ) )
         {
            rc = ossDelete( tmpFile ) ;
            if ( rc )
            {
               PD_LOG( PDERROR, "Remove file[%s] failed, rc: %d",
                       tmpFile, rc ) ;
               goto error ;
            }
         }
         /// rename the file
         rc = ossRenamePath( _fullPathName, tmpFile ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Rename file[%s] to [%s] failed, rc: %d",
                    _fullPathName, tmpFile, rc ) ;
         }
         else
         {
            PD_LOG( PDEVENT, "Rename file[%s] to [%s] succeed",
                    _fullPathName, tmpFile ) ;
         }
      }

   done:
      return cause ;
   error:
      goto done ;
   }

   void _dmsBlockData::_initializeMME ()
   {
      SDB_ASSERT ( _dmsMME, "MME is NULL" ) ;

      for ( INT32 i = 0; i < DMS_MME_SLOTS ; i++ )
      {
         _dmsMME->_mbList[i].reset () ;
      }
   }

   INT32 _dmsBlockData::flushMeta( BOOLEAN sync, UINT32 *pExceptID,
                                   UINT32 num )
   {
      INT32 rc = SDB_OK ;
      INT32 rcTmp = SDB_OK ;

      syncMemToMmap() ;

      for ( UINT32 i = 0 ; i < _dataSegID ; ++i )
      {
         if ( pExceptID )
         {
            BOOLEAN bExcept = FALSE ;
            for ( UINT32 j = 0 ; j < num ; ++j )
            {
               if ( pExceptID[ j ] == i )
               {
                  bExcept = TRUE ;
                  break ;
               }
            }
            if ( bExcept )
            {
               continue ;
            }
         }
         rcTmp = _ossMmapFile::flush( i, sync ) ;
         if ( rcTmp )
         {
            PD_LOG( PDERROR, "Flush segment %u to disk failed, rc: %d",
                    i, rcTmp ) ;
            if ( SDB_OK == rc )
            {
               rc = rcTmp ;
            }
         }
      }
      return rc ;
   }

   void _dmsBlockData::syncMemToMmap ()
   {
      // write total count to disk
      for ( UINT32 i = 0 ; i < DMS_MME_SLOTS ; ++i )
      {
         if ( DMS_IS_MB_INUSE ( _dmsMME->_mbList[i]._flag ) )
         {
            if ( _dmsMME->_mbList[i]._totalRecords !=
                 _mbStatInfo[i]._totalRecords )
            {
               _dmsMME->_mbList[i]._totalRecords =
                  _mbStatInfo[i]._totalRecords ;
            }
            if ( _dmsMME->_mbList[i]._totalDataPages !=
                 _mbStatInfo[i]._totalDataPages )
            {
               _dmsMME->_mbList[i]._totalDataPages =
                  _mbStatInfo[i]._totalDataPages ;
            }
            if ( _dmsMME->_mbList[i]._totalDataFreeSpace !=
                 _mbStatInfo[i]._totalDataFreeSpace )
            {
               _dmsMME->_mbList[i]._totalDataFreeSpace =
                  _mbStatInfo[i]._totalDataFreeSpace ;
            }
            if ( _dmsMME->_mbList[i]._totalDataLen !=
                 _mbStatInfo[i]._totalDataLen )
            {
               _dmsMME->_mbList[i]._totalDataLen =
                  _mbStatInfo[i]._totalDataLen ;
            }
            if ( _dmsMME->_mbList[i]._totalOrgDataLen !=
                 _mbStatInfo[i]._totalOrgDataLen )
            {
               _dmsMME->_mbList[i]._totalOrgDataLen =
                  _mbStatInfo[i]._totalOrgDataLen ;
            }
         }
      }
   }

   void _dmsBlockData::_markHeaderInvalid( INT32 collectionID,
                                           BOOLEAN isAll )
   {
      if ( _dmsHeader )
      {
         if ( isAll )
         {
            _onMarkHeaderInvalid( -1 ) ;
         }
         else if ( collectionID >= 0 )
         {
            _onMarkHeaderInvalid( collectionID ) ;
         }
      }
   }

   INT32 _dmsBlockData::_onMarkHeaderInvalid( INT32 collectionID )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN needSync = FALSE ;

      if ( collectionID >= 0 && collectionID < DMS_MME_SLOTS )
      {
         if ( !_mbStatInfo[ collectionID ]._isCrash )
         {
            needSync = TRUE ;
         }
      }
      else if ( -1 == collectionID )
      {
         for ( UINT16 i = 0 ; i < DMS_MME_SLOTS ; ++i )
         {
            if ( DMS_IS_MB_INUSE ( _dmsMME->_mbList[i]._flag ) &&
                 !_mbStatInfo[ i ]._isCrash )
            {
               needSync = TRUE ;
            }
         }
      }

      if ( needSync )
      {
         rc = flushMME() ;
      }
      return rc ;
   }

   INT32 _dmsBlockData::flushMME( BOOLEAN sync )
   {
      syncMemToMmap() ;
      return flushSegment( _mmeSegID, sync ) ;
   }

   INT32 _dmsBlockData::flushSegment( UINT32 segmentID, BOOLEAN sync )
   {
      /// When in openStorage, the _dataSegID is not init and the
      /// _dirtyList is also not init
      if ( _dataSegID > 0 && segmentID >= _dataSegID )
      {
         _dirtyList.cleanDirty( segmentID - _dataSegID ) ;
      }
      return _ossMmapFile::flush( segmentID, sync ) ;
   }

   /*
      Tool Fuctions
   */
   BOOLEAN dmsIsKeyNameValid( const BSONObj &obj,
                              const CHAR **pErrStr )
   {
      const CHAR *pTmpStr = NULL ;
      BOOLEAN valid = TRUE ;

      BSONObjIterator itr( obj ) ;
      while ( itr.more() )
      {
         BSONElement e = itr.next() ;

         if ( '$' == e.fieldName()[ 0 ] )
         {
            pTmpStr = "field name can't start with \'$\'" ;
            valid = FALSE ;
            break ;
         }
         else if ( ossStrchr( e.fieldName(), '.' ) )
         {
            pTmpStr = "field name can't include \'.\'" ;
            valid = FALSE ;
            break ;
         }
         else if ( e.isABSONObj() &&
                   !dmsIsKeyNameValid( e.embeddedObject(), pErrStr ) )
         {
            valid = FALSE ;
            break ;
         }
      }

      if ( !valid && pErrStr && pTmpStr )
      {
         *pErrStr = pTmpStr ;
      }

      return valid ;
   }

   BOOLEAN dmsIsRecordIDValid( const BSONElement &oidEle,
                               BOOLEAN allowEOO,
                               const CHAR **pErrStr )
   {
      const CHAR *pTmpStr = NULL ;
      BOOLEAN valid = TRUE ;

      switch ( oidEle.type() )
      {
         case EOO :
            if ( !allowEOO )
            {
               pTmpStr = "is not exist" ;
               valid = FALSE ;
            }
            break ;
         case Array :
            pTmpStr = "can't be Array" ;
            valid = FALSE ;
            break ;
         case RegEx:
            pTmpStr = "can't be RegEx" ;
            valid = FALSE ;
            break ;
         case Object :
            valid = dmsIsKeyNameValid( oidEle.embeddedObject(), pErrStr ) ;
            break ;
         default :
            break ;
      }

      
      if ( !valid && pErrStr && pTmpStr )
      {
         *pErrStr = pTmpStr ;
      }

      return valid ;
   }

}
