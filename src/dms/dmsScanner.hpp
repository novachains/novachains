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

   Source File Name = dmsScanner.hpp

   Descriptive Name = Data Management Service Storage Unit Header

   When/how to use: this program may be used on binary and text-formatted
   versions of data management component. This file contains structure for
   DMS storage unit and its methods.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          22/07/2019  jiaqi Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef DMSSCANNER_HPP__
#define DMSSCANNER_HPP__

#include "core.hpp"
#include "oss.hpp"
#include "dms.hpp"
#include "dmsExtent.hpp"
#include "ossUtil.hpp"
#include "ossMem.hpp"
#include "dmsBlockData.hpp"
#include "../bson/bson.h"
#include "../bson/bsonobj.h"
#include "ossMemPool.hpp"

using namespace bson ;

namespace engine
{

   class _dmsMBContext ;
   class _dmsBlockData ;
   class _pmdEDUCB ;

   /*
      _dmsRecordGenerator define
   */
   class _dmsRecordGenerator
   {
      enum EN_MATCH_RECORD_SRC_TYPE
      {
         // just return original src obj
         DMS_SRC_TYPE_ORIGINAL       = 1,
      } ;

   public:
      _dmsRecordGenerator() ;
      ~_dmsRecordGenerator() ;

   public:
      INT32 resetValue( const BSONObj &src ) ;
      INT32 getNext( BSONObj &record ) ;

      OSS_INLINE INT32 getRecordNum()
      {
         return _validNum ;
      }

      OSS_INLINE BOOLEAN hasNext()
      {
         if ( _validNum > 0 )
         {
            return TRUE ;
         }

         return FALSE ;
      }

      OSS_INLINE void setDataPtr( ossValuePtr &dataPtr )
      {
         _dataPtr = dataPtr ;
      }

      OSS_INLINE void getDataPtr( ossValuePtr &dataPtr )
      {
         dataPtr = _dataPtr ;
      }

   private:
      ossValuePtr _dataPtr ;
      BSONObj _src ;
      UINT32 _index ;
      UINT32 _totalNum ;
      UINT32 _validNum ;
      EN_MATCH_RECORD_SRC_TYPE _srcType ;
   } ;
   typedef _dmsRecordGenerator dmsRecordGenerator ;

   /*
      _dmsScanner define
   */
   class _dmsScanner : public SDBObject
   {
      public:
         _dmsScanner ( _dmsBlockData *su, _dmsMBContext *context,
                       DMS_ACCESS_TYPE accessType = DMS_ACCESS_TYPE_FETCH ) ;
         virtual ~_dmsScanner () ;

         BOOLEAN  isReadOnly() const
         {
            return SHARED == _mbLockType ? TRUE : FALSE ;
         }

      public:
         virtual INT32 advance ( dmsRecordID &recordID,
                                 dmsRecordGenerator &generator,
                                 _pmdEDUCB *cb ) = 0 ;

         virtual void  stop () = 0 ;

      protected:
         _dmsBlockData          *_pSu ;
         _dmsMBContext          *_context ;
         DMS_ACCESS_TYPE         _accessType ;
         INT32                   _mbLockType ;
   } ;
   typedef _dmsScanner dmsScanner ;

   /*
      _dmsExtScanner define
   */
   class _dmsExtScannerBase : public _dmsScanner
   {
      public:
         _dmsExtScannerBase ( _dmsBlockData *su, _dmsMBContext *context,
                              dmsExtentID curExtentID,
                              DMS_ACCESS_TYPE accessType = DMS_ACCESS_TYPE_FETCH,
                              INT64 maxRecords = -1,
                              INT64 skipNum = 0,
                              INT32 flag = 0 ) ;
         virtual ~_dmsExtScannerBase () ;

         const dmsExtent* curExtent () const { return _extent ; }
         dmsExtentID nextExtentID () const ;
         INT32 stepToNextExtent() ;
         INT64 getMaxRecords() const { return _maxRecords ; }
         INT64 getSkipNum () const { return _skipNum ; }

      public:
         virtual INT32 advance ( dmsRecordID &recordID,
                                 dmsRecordGenerator &generator,
                                 _pmdEDUCB *cb ) ;
         virtual void  stop () ;

      protected:
         virtual INT32 _firstInit( _pmdEDUCB *cb ) = 0 ;
         virtual INT32 _fetchNext( dmsRecordID &recordID,
                                   dmsRecordGenerator &generator,
                                   _pmdEDUCB *cb ) = 0 ;
         void _checkMaxRecordsNum( dmsRecordGenerator &generator ) ;

      protected:
         INT64                _maxRecords ;
         INT64                _skipNum ;
         dmsExtRW             _extRW ;
         const dmsExtent      *_extent ;
         dmsRecordID          _curRID ;
         dmsRecordRW          _recordRW ;
         const dmsRecord      *_curRecordPtr ;
         dmsOffset            _next ;
         BOOLEAN              _firstRun ;
         _pmdEDUCB            *_cb ;

   };
   typedef _dmsExtScannerBase dmsExtScannerBase ;

   class _dmsExtScanner : public _dmsExtScannerBase
   {
      public:
         _dmsExtScanner( dmsBlockData *su, _dmsMBContext *context,
                         dmsExtentID curExtentID,
                         DMS_ACCESS_TYPE accessType = DMS_ACCESS_TYPE_FETCH,
                         INT64 maxRecords = -1,
                         INT64 skipNum = 0,
                         INT32 flag = 0 ) ;
         virtual ~_dmsExtScanner() ;

      private:
         virtual INT32 _firstInit( _pmdEDUCB *cb ) ;
         virtual INT32 _fetchNext( dmsRecordID &recordID,
                                   dmsRecordGenerator &generator,
                                   _pmdEDUCB *cb ) ;
   } ;
   typedef _dmsExtScanner dmsExtScanner ;

}

#endif //DMSSCANNER_HPP__

