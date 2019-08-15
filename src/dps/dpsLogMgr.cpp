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

   Source File Name = dpsLogMgr.cpp

   Descriptive Name = Data Protection Service Replica Log Manager

   When/how to use: this program may be used on binary and text-formatted
   versions of DPS component. This file contains code logic for log manager,
   which is the driving logic to insert log records into log buffer

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          08/02/2019  jiaqi Initial Draft

   Last Changed =

*******************************************************************************/
#include "dpsLogMgr.hpp"
#include "dpsMergeBlock.hpp"
#include "dpsLogDef.hpp"
#include "dpsLogRecordDef.hpp"
#include "pmdEDU.hpp"
#include "pdTrace.hpp"

namespace engine
{
   #define WORK_PAGE ( &( _pages[_work] ) )

   #define PAGE( n ) (&( _pages [ ( DPS_SUB_BIT ? (n)&DPS_SUB_BIT : (n)%_pageNum ) ] ))

   #define SHARED_LOCK_NODES( meta )\
            {for ( UINT32 i = 0;\
                   i < (meta).pageNum; \
                   i++ ) \
            {(PAGE((meta).beginSub + i))->lockShared();}}

   #define SHARED_UNLOCK_NODES( meta ) \
           {for ( UINT32 i = 0;\
                   i < (meta).pageNum; \
                   i++ ) \
           {(PAGE((meta).beginSub + i))->unlockShared();}}

   static UINT32 DPS_SUB_BIT = 0 ;

   _dpsLogMgr::_dpsLogMgr()
   :_logger(this), _pages(NULL), _idleSize(0), _totalSize(0),
    _work(0), _pageNum(0), _queSize(0)
   {
      _begin = 0 ;
      _restoreFlag = FALSE ;
      _rollFlag = FALSE ;

      _incVersion = FALSE ;
      _pageFlushCount = 0 ;
   }

   _dpsLogMgr::~_dpsLogMgr()
   {
      if ( NULL != _pages )
      {
         SDB_OSS_DEL []_pages;
         _pages = NULL;
      }
   }

   // initialize log manager
   // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSRPCMGR_INIT, "_dpsLogMgr::init" )
   INT32 _dpsLogMgr::init ( const CHAR *path, UINT32 pageNum )
   {
      INT32 rc = SDB_OK;
      PD_TRACE_ENTRY ( SDB__DPSRPCMGR_INIT );
      SDB_ASSERT ( path, "path can't be NULL" ) ;

      // free in destructor
      _pages = SDB_OSS_NEW _dpsLogPage[pageNum];
      if ( NULL == _pages )
      {
         rc = SDB_OOM;
         PD_LOG (PDERROR, "new _dpsLogPage[pageNum] failed!" );
         goto error;
      }
      // set ID for each page
      for ( UINT32 i = 0; i < pageNum; i++ )
      {
         _pages[i].setNumber( i );
      }
      _offset = 0 ;
      _totalSize = DPS_DEFAULT_PAGE_SIZE * pageNum;
      _idleSize.init( _totalSize );
      _pageNum = pageNum ;
      if ( ossIsPowerOf2( pageNum ) )
      {
         DPS_SUB_BIT = ~pageNum ;
      }

      // initialize log files
      rc = _logger.init( path );
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to initial log files, rc = %d", rc ) ;
         goto error;
      }

   done:
      return rc;
   error:
      goto done;
   }

   INT32 _dpsLogMgr::preparePages ( dpsMergeInfo &info )
   {
      INT32 rc = SDB_OK ;
      info.getDummyBlock().clear() ;
      dpsMergeBlock &block = info.getMergeBlock () ;
      block.pageMeta().clear() ;
      dpsLogRecordHeader &head = block.record().head() ;
      UINT32 logFileSz = _logger.getLogFileSz() ;
      BOOLEAN locked = FALSE ;

      // all pages memory less than data size
      if ( _totalSize < head._length )
      {
         PD_LOG ( PDERROR, "dps total memory size[%d] less than block size[%d]",
                  _totalSize, head._length ) ;
         rc = SDB_SYS ;
         SDB_ASSERT ( 0, "system error" ) ;
         goto error ;
      }

      if ( FALSE == _restoreFlag )
      {
         /// first to lock writeMutex, then make sure idle space is enough,
         /// at last lock mtx. So, this don't block read operations
         _writeMutex.get() ;
         while ( _idleSize.peek() < head._length )
         {
            PD_LOG ( PDWARNING, "No space in log buffer for %d bytes, "
                     "currently left %d bytes", head._length,
                     _idleSize.peek() ) ;
            _allocateEvent.wait ( OSS_ONE_SEC ) ;
         }
         _mtx.get();
         locked = TRUE ;
      }

      if ( !block.isRow() )
      {
         // is the full record able to sit in the same log file?
         if ( ( _offset / logFileSz ) !=
               ( _offset + head._length - 1 ) / logFileSz )
         {
            // if the log is replicated and hit this logic, something really
            // goes wrong, because the dummy record should already be inserted
            // in primary node
            SDB_ASSERT ( !block.isRow(), "replicated log record should never"
                         " hit this part" ) ;
            // we are going to insert a dummy log record
            UINT32 dummyLogSize = logFileSz - ( _offset % logFileSz ) ;
            SDB_ASSERT ( dummyLogSize >= sizeof ( dpsLogRecordHeader ),
                         "dummy log size is smaller than log head" ) ;
            SDB_ASSERT ( dummyLogSize % sizeof(SINT32) == 0,
                         "dummy log size is not 4 bytes aligned" ) ;

            // set dummyhead as a reference to log head
            dpsLogRecordHeader &dummyhead =
                              info.getDummyBlock().record().head() ;
            // initialize dummyhead
            dummyhead._length = dummyLogSize ;
            dummyhead._type   = LOG_TYPE_DUMMY ;
            // allocate the space in metadata and page, receive dummynodes
            // for the nodes contains dummy record, and offset for where
            // to start
            _allocate ( dummyhead._length, info.getDummyBlock().pageMeta() ) ;

            // share lock the pages we are going to write
            SHARED_LOCK_NODES ( info.getDummyBlock().pageMeta()) ;
            // send the pages into queue
            _push2SendQueue ( info.getDummyBlock().pageMeta() ) ;
         }

         // after we push dummy record, we have to check if the rest of space
         // able to put a log head in log file
         if ( ( (_offset+head._length) / logFileSz ) !=
              ( (_offset+head._length+
                 sizeof(dpsLogRecordHeader)) / logFileSz ) )
         {
            SDB_ASSERT ( !block.isRow(), "replicated log record should never"
                         " hit this part" ) ;
            head._length = logFileSz - _offset % logFileSz ;
            //head._length += logFileSz - ((_lsn.offset+head._length)%logFileSz) ;
         }
      }
      // now let's continue allocate the real data
      _allocate( head._length, block.pageMeta() );

      // and share lock the nodes with real data
      SHARED_LOCK_NODES( block.pageMeta() ) ;
      // push them into queue together with dummy record
      _push2SendQueue( block.pageMeta() ) ;

   done:
      // unlock metadata
      if ( locked )
      {
         _mtx.release() ;
         _writeMutex.release() ;
         locked = FALSE ;
      }
      return rc;
   error:
      goto done;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSRPCMGR_WRITEDATA, "_dpsLogMgr::writeData" )
   void _dpsLogMgr::writeData ( dpsMergeInfo & info )
   {
      SDB_ASSERT ( info.getMergeBlock().pageMeta().valid(),
                   "block not prepared" ) ;

      // if has dummy block
      if ( info.hasDummy() )
      {
         _mergeLogs( info.getDummyBlock(), info.getDummyBlock().pageMeta());
         SHARED_UNLOCK_NODES( info.getDummyBlock().pageMeta() );
      }

      _mergeLogs( info.getMergeBlock(), info.getMergeBlock().pageMeta() );
      SHARED_UNLOCK_NODES( info.getMergeBlock().pageMeta() );

   }

   // allocate len bytes, returns nodes for which nodes contains data, and
   // offset for the starting point in the first page
   void _dpsLogMgr::_allocate( UINT32 len,
                                      dpsPageMeta &allocated )
   {
      UINT32 needAlloc = len;
      UINT32 pageNum = 0;

      SDB_ASSERT( 0 != len, "can not allocate zero length" ) ;
      SDB_ASSERT ( _totalSize > len,
                  "total memory size must grater than record size" ) ;

      // make sure there's enough space
      while ( _idleSize.peek() < needAlloc )
      {
         PD_LOG ( PDWARNING, "No space in log buffer for %d bytes, currently "
                  "left %d bytes", needAlloc, _idleSize.peek() ) ;
         _allocateEvent.wait ( OSS_ONE_SEC ) ;
      }
      // get the current working offset in the first page
      allocated.offset = WORK_PAGE->getLength();
      allocated.beginSub = _work ;

      do
      {
         pageNum++ ;
         // how much space left in the page?
         UINT32 pageIdle = WORK_PAGE->getLastSize();
         // is it large enough to allocate all? otherwise allocate rest
         INT32 allocLen = pageIdle < needAlloc ? pageIdle : needAlloc;
         // reserve space in the page
         WORK_PAGE->allocate( allocLen );
         // how much data still left?
         needAlloc -= allocLen;

         // if we don't have any extra space in the current page, let's move to
         // next
         if ( 0 == WORK_PAGE->getLastSize() )
         {
            _work = _incPageID ( _work ) ;

            if ( !_rollFlag && _begin != _work )
            {
               _rollFlag = TRUE ;
            }
            else if ( _begin == _work && _rollFlag )
            {
               _begin = _incPageID ( _begin ) ;
            }
         }
      } while ( needAlloc > 0 ) ;

      // reduce the total free space in metadata
      _idleSize.sub( len ) ;
      _offset+= len ;
      allocated.pageNum = pageNum ;
      allocated.totalLen = len ;
   }

   void _dpsLogMgr::_push2SendQueue( const dpsPageMeta &allocated )
   {
      // push pages into flush queue. Note all nodes should be shared locked at
      // the moment

      SDB_ASSERT( allocated.valid(), "impossible" ) ;
      for ( UINT32 i = 0; i < allocated.pageNum; ++i )
      {
         _dpsLogPage *page = PAGE(allocated.beginSub + i) ;
         if ( 0 == page->getLastSize() )
         {
            if ( !_restoreFlag )
            {
               _queue.push ( page ) ;
               _queSize.inc() ;
            }
            else
            {
               _idleSize.add( page->getLength() );
               page->clear() ;
            }
         }
      }

      return ;
   }

   // copy logs into buffer
   void _dpsLogMgr::_mergeLogs( _dpsMergeBlock &block,
                                       const dpsPageMeta &meta )
   {
      PD_TRACE_ENTRY ( SDB__DPSRPCMGR__MRGLOGS );
      UINT32 offset = meta.offset ;
      UINT32 work = meta.beginSub ;
      dpsLogRecordHeader &head = block.record().head() ;
      // first we copy head
      _mergePage( (CHAR *)&head,
                  sizeof( dpsLogRecordHeader ),
                  work,
                  offset);

      // and then copy body
      dpsLogRecord::iterator itr( &(block.record()) ) ;
      if ( block.isRow() )
      {
         /// row data's size should always be one.
         /// and dataheader should not be merged.
         BOOLEAN res = itr.next() ;
         SDB_ASSERT( res, "impossible" ) ;
         const _dpsRecordEle &dataMeta = itr.dataMeta() ;
         _mergePage( itr.value(), dataMeta.len,
                     work, offset ) ;
      }
      else
      {
         UINT32 mergeSize = 0 ;

         /// dummy log has no body.
         if ( LOG_TYPE_DUMMY == head._type )
         {
            goto done ;
         }

         while ( itr.next() )
         {
            const _dpsRecordEle &dataMeta = itr.dataMeta() ;
            SDB_ASSERT( DPS_INVALID_TAG != dataMeta.tag, "impossible" ) ;

            _mergePage( ( CHAR *)(&dataMeta), sizeof( dataMeta ),
                         work, offset ) ;
            mergeSize += sizeof( dataMeta ) ;
            _mergePage( itr.value(), dataMeta.len,
                        work, offset ) ;
            mergeSize += dataMeta.len ;
         }

         /// the len might be changed in preparePages().
         /// we add a stop flag to record for loading.
         if (  mergeSize <= head._length -
                           sizeof(dpsRecordEle) -
                           sizeof(dpsLogRecordHeader))
         {
            CHAR stop[sizeof(dpsRecordEle)] = {0} ;
            _mergePage( stop, sizeof(stop), work, offset ) ;
         }
      }

   done:
      return;
   }

   // copy data into buffer, return workSub = current working node, and offset
   // for the offset in the current working node
   void _dpsLogMgr::_mergePage( const CHAR *src,
                                       UINT32 len,
                                       UINT32 &workSub,
                                       UINT32 &offset )
   {
      UINT32 needcpy = len ;
      UINT32 srcOffset = 0 ;
      UINT32 pageIdle = 0 ;
      UINT32 cpylen = 0 ;

      while ( needcpy > 0 )
      {
         dpsLogPage *page = PAGE(workSub) ;
         pageIdle = page->getBufSize() - offset ;
         cpylen = pageIdle < needcpy ? pageIdle : needcpy ;
         page->fill( offset, src + srcOffset, cpylen ) ;

         needcpy -= cpylen ;
         srcOffset += cpylen ;
         offset += cpylen ;

         if ( offset == page->getBufSize() )
         {
            workSub++ ;
            offset = 0 ;
         }
      }

      return;
   }

   // entry function to flush buffer to disk
   // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSRPCMGR_RUN, "_dpsLogMgr::run" )
   INT32 _dpsLogMgr::run( _pmdEDUCB *cb )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DPSRPCMGR_RUN );
      _dpsLogPage *page = NULL;
      // wait for queue for 1 second
      if ( _queue.timed_wait_and_pop( page, OSS_ONE_SEC  ) )
      {
         if ( cb )
         {
            cb->incEventCount() ;
         }
         rc = _flushPage ( page ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to flush page, rc = %d", rc ) ;
            goto error ;
         }

         _pageFlushCount++ ;

      }

   done :
      return rc ;
   error :
      goto done ;
   }

   // flush all is flushing all pages to disk except the current working one
   // note this function SHOULD NOT BE CALLED ANYWHERE IN ENGINE
   // this function is ONLY USED IN TESTCASE
   INT32 _dpsLogMgr::flushAll()
   {
      INT32 rc = SDB_OK ;
      rc = _flushAll() ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSRPCMGR__FLUSHALL, "_dpsLogMgr::_flushAll" )
   INT32 _dpsLogMgr::_flushAll()
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DPSRPCMGR__FLUSHALL );
      _dpsLogPage *page = NULL ;
      // clean up queue
      while ( TRUE )
      {
         if ( !_queue.try_pop( page ) )
         {
            break;
         }
         else
         {
            rc = _flushPage( page );
            if ( rc )
            {
               PD_LOG ( PDERROR, "Failed to flush page, rc = %d", rc ) ;
               goto error ;
            }
            page = NULL;
         }
      }

   done :
      return rc ;
   error :
      goto done ;
   }

   // this function is called during database shutdown
   // this function flush all dirty pages AND the working copy to disk, so this
   // function should ONLY be when database is shutdown. Otherwise please call
   // flushAll()
   // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSRPCMGR_TEARDOWN, "_dpsLogMgr::tearDown" )
   INT32 _dpsLogMgr::tearDown()
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DPSRPCMGR_TEARDOWN );
      _dpsLogPage *page = NULL;
      rc = _flushAll() ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to flush all, rc = %d", rc ) ;
         goto error ;
      }

      if ( 0 != WORK_PAGE->getLength() )
      {
         page = WORK_PAGE;
         _work = _incPageID ( _work ) ;
         // fill the rest to '0'
         ossMemset( page->mb()->writePtr(), 0, page->getLastSize() ) ;
         _queSize.inc() ;
         rc = _flushPage( page, TRUE );
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to flush page, rc = %d", rc ) ;
            goto error ;
         }
      }

   done :
      return rc ;
   error :
      goto done ;
   }

   INT32 _dpsLogMgr::_flushPage( _dpsLogPage *page, BOOLEAN shutdown )
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT ( page, "page can't be NULL" ) ;
      UINT32 length = 0 ;
      // note in tearDown it may call _flushPage with non-full page
      page->lock();
      page->unlock();
      rc = _logger.flush( page->mb(), shutdown );
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to flush page, rc = %d", rc ) ;
         goto error ;
      }
      SDB_ASSERT ( shutdown || page->getLength() == DPS_DEFAULT_PAGE_SIZE,
                   "page can't be partial during flush except shutdown" ) ;
      length = page->getLength() ;
      page->clear();
      _idleSize.add( length );
      _queSize.dec() ;
      _allocateEvent.signalAll() ;

   done :
      return rc ;
   error :
      goto done ;
   }

}

