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

   Source File Name = dmsScanner.cpp

   Descriptive Name = Data Management Service Scanner

   When/how to use: this program may be used on binary and text-formatted
   versions of data management component. This file contains structure for
   DMS storage unit and its methods.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          22/07/2018  jiaqi Initial Draft

   Last Changed =

*******************************************************************************/

#include "dmsScanner.hpp"
#include "dmsBlockData.hpp"
#include "pmdEDU.hpp"
#include "pdTrace.hpp"

using namespace bson ;

namespace engine
{

   #define DMS_IS_WRITE_OPR(accessType)   \
      ( DMS_ACCESS_TYPE_UPDATE == accessType || \
        DMS_ACCESS_TYPE_DELETE == accessType ||\
        DMS_ACCESS_TYPE_INSERT == accessType )

   #define DMS_IS_READ_OPR(accessType) \
      ( DMS_ACCESS_TYPE_QUERY == accessType || \
        DMS_ACCESS_TYPE_FETCH == accessType )



   /*
      _dmsRecordGenerator implement
   */
   _dmsRecordGenerator::_dmsRecordGenerator()
   {
      _index         = 0 ;
      _dataPtr       = 0 ;
      _totalNum      = 0 ;
      _validNum      = 0 ;
      _src           = BSONObj() ;
      _srcType       = DMS_SRC_TYPE_ORIGINAL ;
   }

   _dmsRecordGenerator::~_dmsRecordGenerator()
   {
      _index         = 0 ;
      _dataPtr       = 0 ;
      _totalNum      = 0 ;
      _validNum      = 0 ;
      _src           = BSONObj() ;
      _srcType       = DMS_SRC_TYPE_ORIGINAL ;
   }

   INT32 _dmsRecordGenerator::resetValue( const BSONObj &src )
   {
      INT32 rc    = SDB_OK ;
      _index      = 0 ;
      _src        = src ;

      _srcType  = DMS_SRC_TYPE_ORIGINAL ;
      _totalNum = 1 ;
      _validNum = _totalNum ;

      return rc ;
   }

   INT32 _dmsRecordGenerator::getNext( BSONObj &record )
   {
      SDB_ASSERT( _validNum > 0, "must hasNext" ) ;

      BSONObj tmpObj ;
      INT32 rc = SDB_OK ;
      record = _src ;
      _index++ ;
      _validNum-- ;

      return rc ;
   }

   /*
      _dmsScanner implement
   */
   _dmsScanner::_dmsScanner( dmsBlockData *su, dmsMBContext *context,
                             DMS_ACCESS_TYPE accessType )
   {
      SDB_ASSERT( su, "storage data can't be NULL" ) ;
      SDB_ASSERT( context, "context can't be NULL" ) ;
      _pSu = su ;
      _context = context ;
      _accessType = accessType ;
      _mbLockType = SHARED ;

      if ( DMS_IS_WRITE_OPR( _accessType ) )
      {
         _mbLockType = EXCLUSIVE ;
      }
   }

   _dmsScanner::~_dmsScanner()
   {
      _context    = NULL ;
      _pSu        = NULL ;
   }

   /*
      _dmsExtScannerBase implement
   */
   _dmsExtScannerBase::_dmsExtScannerBase( dmsBlockData *su,
                                           dmsMBContext *context,
                                           dmsExtentID curExtentID,
                                           DMS_ACCESS_TYPE accessType,
                                           INT64 maxRecords,
                                           INT64 skipNum,
                                           INT32 flag )
   :_dmsScanner( su, context, accessType ),
    _curRecordPtr( NULL )
   {
      _maxRecords          = maxRecords ;
      _skipNum             = skipNum ;
      _next                = DMS_INVALID_OFFSET ;
      _firstRun            = TRUE ;
      _extent              = NULL ;
      _curRID._extent      = curExtentID ;
      _cb                  = NULL ;
   }

   _dmsExtScannerBase::~_dmsExtScannerBase ()
   {
      _extent     = NULL ;
   }

   dmsExtentID _dmsExtScannerBase::nextExtentID() const
   {
      if ( _extent )
      {
         return _extent->_nextExtent ;
      }
      return DMS_INVALID_EXTENT ;
   }

   INT32 _dmsExtScannerBase::stepToNextExtent()
   {
      if ( 0 != _maxRecords &&
           DMS_INVALID_EXTENT != nextExtentID() )
      {
         _curRID._extent = nextExtentID() ;
         _firstRun = TRUE ;
         return SDB_OK ;
      }
      return SDB_DMS_EOC ;
   }

   INT32 _dmsExtScannerBase::advance( dmsRecordID &recordID,
                                      dmsRecordGenerator &generator,
                                      pmdEDUCB *cb )
   {
      INT32 rc = SDB_OK ;

      if ( _firstRun )
      {
         rc = _firstInit( cb ) ;
         PD_RC_CHECK( rc, PDWARNING, "first init failed, rc: %d", rc ) ;
      }

      rc = _fetchNext( recordID, generator, cb ) ;
      if ( rc )
      {
         // Do not write error log when EOC.
         if ( SDB_DMS_EOC != rc )
         {
            PD_LOG( PDERROR, "Get next record failed, rc: %d", rc ) ;
         }
         goto error ;
      }

   done:
      return rc ;
   error:
      recordID.reset() ;
      _curRID._offset = DMS_INVALID_OFFSET ;
      goto done ; 
   }

   void _dmsExtScannerBase::stop()
   {
      _next = DMS_INVALID_OFFSET ;
      _curRID._offset = DMS_INVALID_OFFSET ;
   }

   _dmsExtScanner::_dmsExtScanner( dmsBlockData *su,
                                   _dmsMBContext *context,
                                   dmsExtentID curExtentID,
                                   DMS_ACCESS_TYPE accessType,
                                   INT64 maxRecords,
                                   INT64 skipNum,
                                   INT32 flag )
   : _dmsExtScannerBase( su, context, curExtentID, accessType,
                         maxRecords, skipNum, flag )
   {
   }

   _dmsExtScanner::~_dmsExtScanner()
   {
   }

   INT32 _dmsExtScanner::_firstInit( pmdEDUCB *cb )
   {
      INT32 rc          = SDB_OK ;

      _extRW = _pSu->extent2RW( _curRID._extent, _context->mbID() ) ;
      _extRW.setNothrow( TRUE ) ;
      _extent = _extRW.readPtr<dmsExtent>() ;
      if ( NULL == _extent )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
/*      if ( cb->isInterrupted() )
      {
         rc = SDB_APP_INTERRUPT ;
         goto error ;
      }
*/
      if ( !_context->isMBLock( _mbLockType ) )
      {
         rc = _context->mbLock( _mbLockType ) ;
         PD_RC_CHECK( rc, PDERROR, "dms mb lock failed, rc: %d", rc ) ;
      }
      if ( !_extent->validate( _context->mbID() ) )
      {
         rc = SDB_SYS ;
         goto error ;
      }

      _cb   = cb ;
      _next = _extent->_firstRecordOffset ;

      // unset first run
      _firstRun = FALSE ;

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _dmsExtScanner::_fetchNext( dmsRecordID &recordID,
                                     dmsRecordGenerator &generator,
                                     pmdEDUCB *cb )
   {
      INT32 rc                = SDB_OK ;
      ossValuePtr recordDataPtr ;
      dmsRecordData recordData ;

      while ( DMS_INVALID_OFFSET != _next && 0 != _maxRecords )
      {
         _curRID._offset = _next ;
         _recordRW = _pSu->record2RW( _curRID, _context->mbID() ) ;
         _curRecordPtr = _recordRW.readPtr( 0 ) ;
         _next = _curRecordPtr->getNextOffset() ; 

         SDB_ASSERT( !_curRecordPtr->isDeleted(), "record can't be deleted" ) ;

         recordID = _curRID ;
         rc = _pSu->extractData( _context, _recordRW, recordData ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Extract record data failed, rc: %d", rc ) ;
            goto error ;
         }
         recordDataPtr = ( ossValuePtr )recordData.data() ;
         generator.setDataPtr( recordDataPtr ) ;

         try
         {
            BSONObj obj( recordData.data() ) ;
            rc = generator.resetValue( obj ) ;
            PD_RC_CHECK( rc, PDERROR, "resetValue failed:rc=%d", rc ) ;
         }
         catch( std::exception &e )
         {
            rc = SDB_SYS ;
            PD_RC_CHECK( rc, PDERROR, "Failed to create BSON object: %s",
                         e.what() ) ;
            goto error ;
         }

         if ( _skipNum > 0 )
         {
            --_skipNum ;
         }
         else
         {
            if ( _maxRecords > 0 )
            {
               --_maxRecords ;
            }
            goto done ; // find ok
         }
      } // while

      rc = SDB_DMS_EOC ;
      goto error ;

   done:
      return rc ;
   error:
      recordID.reset() ;
      recordDataPtr = 0 ;
      generator.setDataPtr( recordDataPtr ) ;
      _curRID._offset = DMS_INVALID_OFFSET ;
      goto done ;
   }

}

