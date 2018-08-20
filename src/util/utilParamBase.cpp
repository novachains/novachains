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

   Source File Name = utilParamBase.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used for managing sequoiadb
   configuration.It can be initialized from cmd and configure file.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          17/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "utilParamBase.hpp"
#include "utilStr.hpp"
#include "pmdDef.hpp"
#include "ossIO.hpp"
#include "pd.hpp"

#include <boost/algorithm/string.hpp>

using namespace bson ;

namespace engine
{

   #define ISALLOWRUNCHANGE( level )   ( ( level == UTIL_CFG_CHANGE_REBOOT || \
                                       level == UTIL_CFG_CHANGE_FORBIDDEN ) ? \
                                       FALSE : TRUE )


   /*
      _utilCfgExchange implement
   */
   _utilCfgExchange::_utilCfgExchange( MAP_K2V *pMapField,
                                       const BSONObj &dataObj,
                                       BOOLEAN load,
                                       UTIL_CFG_STEP step,
                                       UINT32 mask )
   :_pMapKeyField( pMapField ), _cfgStep( step ),
    _isLoad( load ), _dataObj( dataObj ), _mask( mask )
   {
      _dataType   = PMD_CFG_DATA_BSON ;
      _pVMFile    = NULL ;
      _pVMCmd     = NULL ;
      _isWhole    = FALSE ;
      _pMapColdKeyField = NULL ;

      SDB_ASSERT( _pMapKeyField, "Map key field can't be NULL" ) ;
   }

   _utilCfgExchange::_utilCfgExchange ( MAP_K2V *pMapField,
                                        MAP_K2V *pMapColdField,
                                        const BSONObj &dataObj,
                                        BOOLEAN load,
                                        UTIL_CFG_STEP step,
                                        UINT32 mask )
      :_pMapKeyField( pMapField ), _pMapColdKeyField(pMapColdField),
      _cfgStep( step ),  _isLoad( load ), _dataObj( dataObj ), _mask( mask )
      {
         _dataType   = PMD_CFG_DATA_BSON ;
         _pVMFile    = NULL ;
         _pVMCmd     = NULL ;
         _isWhole    = FALSE ;

         SDB_ASSERT( _pMapKeyField, "Map key field can't be NULL" ) ;
         SDB_ASSERT( _pMapColdKeyField, "Map cold key field can't be NULL" ) ;
      }

   _utilCfgExchange::_utilCfgExchange( MAP_K2V *pMapField,
                                       po::variables_map *pVMCmd,
                                       po::variables_map * pVMFile,
                                       BOOLEAN load,
                                       UTIL_CFG_STEP step,
                                       UINT32 mask )
   :_pMapKeyField( pMapField ), _cfgStep( step ), _isLoad( load ),
    _pVMFile( pVMFile ), _pVMCmd( pVMCmd ), _mask( mask )
   {
      _dataType   = PMD_CFG_DATA_CMD ;
      _isWhole    = FALSE ;
      _pMapColdKeyField = NULL ;

      SDB_ASSERT( _pMapKeyField, "Map key field can't be NULL" ) ;
   }

   _utilCfgExchange::~_utilCfgExchange()
   {
      _pVMFile    = NULL ;
      _pVMCmd     = NULL ;
   }

   INT32 _utilCfgExchange::readInt( const CHAR * pFieldName,
                                    INT32 &value,
                                    UTIL_CFG_CHANGE changeLevel )
   {
      INT32 rc = SDB_OK ;
      INT32 readValue ;

      if ( PMD_CFG_DATA_BSON == _dataType )
      {
         BSONElement ele = _dataObj.getField( pFieldName ) ;
         if ( ele.eoo() )
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
         else if ( !ele.isNumber() )
         {
            PD_LOG( PDERROR, "Field[%s] type[%d] is not number", pFieldName,
                    ele.type() ) ;
            rc = SDB_INVALIDARG ;
         }
         else
         {
            readValue = (INT32)ele.numberInt() ;
         }
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         if ( _pVMCmd && _pVMCmd->count( pFieldName ) )
         {
            readValue = (*_pVMCmd)[pFieldName].as<int>() ;
         }
         else if ( _pVMFile && _pVMFile->count( pFieldName ) )
         {
            readValue = (*_pVMFile)[pFieldName].as<int>() ;
         }
         else
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
      }
      else
      {
         rc = SDB_SYS ;
      }

      if ( SDB_OK == rc )
      {
         _saveToMapInt( pFieldName, value, readValue, changeLevel, FALSE ) ;
      }

      return rc ;
   }

   INT32 _utilCfgExchange::readInt( const CHAR * pFieldName,
                                    INT32 &value,
                                    INT32 defaultValue,
                                    UTIL_CFG_CHANGE changeLevel )
   {
      INT32 rc = SDB_OK ;

      rc = readInt( pFieldName, value, changeLevel ) ;
      if ( SDB_FIELD_NOT_EXIST == rc )
      {
         _saveToMapInt( pFieldName, value, defaultValue, changeLevel, TRUE ) ;
         rc = SDB_OK ;
      }

      return rc ;
   }

   INT32 _utilCfgExchange::readString( const CHAR *pFieldName,
                                       CHAR *pValue,
                                       UINT32 len,
                                       UTIL_CFG_CHANGE changeLevel )
   {
      INT32 rc = SDB_OK ;
      string readValue ;

      if ( PMD_CFG_DATA_BSON == _dataType )
      {
         BSONElement ele = _dataObj.getField( pFieldName ) ;
         if ( ele.eoo() )
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
         else if ( String != ele.type() )
         {
            PD_LOG( PDERROR, "Field[%s] type[%d] is not string", pFieldName,
                    ele.type() ) ;
            rc = SDB_INVALIDARG ;
         }
         else
         {
            readValue = ele.String() ;
         }
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         if ( _pVMCmd && _pVMCmd->count( pFieldName ) )
         {
            readValue = (*_pVMCmd)[pFieldName].as<string>() ;
         }
         else if ( _pVMFile && _pVMFile->count( pFieldName ) )
         {
            readValue = (*_pVMFile)[pFieldName].as<string>() ;
         }
         else
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
      }
      else
      {
         rc = SDB_SYS ;
      }

      if ( SDB_OK == rc )
      {
         _saveToMapString( pFieldName, pValue, len, readValue,
                           changeLevel, FALSE );
      }

      return rc ;
   }

   INT32 _utilCfgExchange::readString( const CHAR *pFieldName,
                                       CHAR *pValue,
                                       UINT32 len,
                                       const CHAR *pDefault,
                                       UTIL_CFG_CHANGE changeLevel )
   {
      INT32 rc = SDB_OK ;

      rc = readString( pFieldName, pValue, len, changeLevel ) ;
      if ( SDB_FIELD_NOT_EXIST == rc && pDefault )
      {
         _saveToMapString( pFieldName, pValue, len, pDefault,
                           changeLevel, TRUE ) ;
         rc = SDB_OK ;
      }

      return rc ;
   }

   INT32 _utilCfgExchange::writeInt( const CHAR * pFieldName, INT32 value )
   {
      INT32 rc = SDB_OK ;

      if ( PMD_CFG_DATA_BSON == _dataType )
      {
         _dataBuilder.append( pFieldName, value ) ;
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         _strStream << pFieldName << "=" << value << OSS_NEWLINE ;
      }
      else
      {
         rc = SDB_SYS ;
      }

      return rc ;
   }

   INT32 _utilCfgExchange::writeString( const CHAR *pFieldName,
                                        const CHAR *pValue )
   {
      INT32 rc = SDB_OK ;

      if ( PMD_CFG_DATA_BSON == _dataType )
      {
         _dataBuilder.append( pFieldName, pValue ) ;
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         _strStream << pFieldName << "=" << pValue << OSS_NEWLINE ;
      }
      else
      {
         rc = SDB_SYS ;
      }

      return rc ;
   }

   BOOLEAN _utilCfgExchange::hasField( const CHAR * pFieldName )
   {
      if ( PMD_CFG_DATA_BSON == _dataType &&
           !_dataObj.getField( pFieldName ).eoo() )
      {
         return TRUE ;
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         if ( ( _pVMCmd && _pVMCmd->count( pFieldName ) ) ||
              ( _pVMFile && _pVMFile->count( pFieldName ) ) )
         {
            return TRUE ;
         }
      }

      return FALSE ;
   }

   const CHAR* _utilCfgExchange::getData( UINT32 & dataLen,
                                          MAP_K2V &mapKeyValue )
   {
      MAP_K2V::iterator it ;
      if ( PMD_CFG_DATA_BSON == _dataType )
      {
         it = mapKeyValue.begin() ;
         while ( it != mapKeyValue.end() )
         {
            if ( FALSE == it->second._hasMapped )
            {
               _dataBuilder.append( it->first, it->second._value ) ;
            }
            ++it ;
         }
         _dataObj = _dataBuilder.obj() ;
         dataLen = _dataObj.objsize() ;
         return _dataObj.objdata() ;
      }
      else if ( PMD_CFG_DATA_CMD == _dataType )
      {
         it = mapKeyValue.begin() ;
         while ( it != mapKeyValue.end() )
         {
            if ( FALSE == it->second._hasMapped )
            {
               _strStream << it->first << "=" << it->second._value
                          << OSS_NEWLINE ;
            }
            ++it ;
         }
         _dataStr = _strStream.str() ;
         dataLen = _dataStr.size() ;
         return _dataStr.c_str() ;
      }
      return NULL ;
   }

   void _utilCfgExchange::_makeKeyValueMap( po::variables_map *pVM )
   {
      po::variables_map::iterator it = pVM->begin() ;
      MAP_K2V::iterator itKV ;
      MAP_K2V::iterator itKVCold ;
      while ( it != pVM->end() )
      {
         itKV = _pMapKeyField->find( it->first ) ;
         if ( itKV != _pMapKeyField->end() &&
              TRUE == itKV->second._hasMapped )
         {
            ++it ;
            continue ;
         }
         if ( _pMapColdKeyField )
         {
            itKVCold = _pMapColdKeyField->find( it->first ) ;
            if ( itKVCold != _pMapColdKeyField->end() &&
                 TRUE == itKVCold->second._hasMapped )
            {
               continue ;
            }
         }
         try
         {
            (*_pMapKeyField)[ it->first ] = utilParamValue( it->second.as<string>(),
                                                            FALSE, TRUE ) ;
            ++it ;
            continue ;
         }
         catch( std::exception )
         {
         }

         try
         {
            (*_pMapKeyField)[ it->first ] = utilParamValue( it->second.as<int>(),
                                                            FALSE, TRUE ) ;
            ++it ;
            continue ;
         }
         catch( std::exception )
         {
         }

         ++it ;
      }
   }

   void _utilCfgExchange::_saveToMapInt( const CHAR * pFieldName,
                                         INT32 &value,
                                         const INT32 &newValue,
                                         UTIL_CFG_CHANGE changeLevel,
                                         BOOLEAN useDefault )
   {
      BOOLEAN hasFieldValue = useDefault ? FALSE : TRUE ;

      if ( UTIL_CFG_STEP_CHG == _cfgStep &&
           !ISALLOWRUNCHANGE( changeLevel ) )
      {
         if ( NULL != _pMapColdKeyField )
         {
            (*_pMapColdKeyField)[ pFieldName ] = utilParamValue( newValue,
                                                                 TRUE,
                                                                 hasFieldValue,
                                                                 changeLevel ) ;
         }
      }
      else
      {

         value = newValue ;
         (*_pMapKeyField)[ pFieldName ] = utilParamValue( newValue,
                                                          TRUE,
                                                          hasFieldValue,
                                                          changeLevel ) ;
      }
   }

   void _utilCfgExchange::_saveToMapString( const CHAR *pFieldName, 
                                            CHAR *pValue,
                                            UINT32 len,
                                            const string &newValue,
                                            UTIL_CFG_CHANGE changeLevel,
                                            BOOLEAN useDefault )
   {
      BOOLEAN hasFieldValue = useDefault ? FALSE : TRUE ;

      if ( UTIL_CFG_STEP_CHG == _cfgStep &&
           !ISALLOWRUNCHANGE( changeLevel ) )
      {
         if ( NULL != _pMapColdKeyField )
         {
            (*_pMapColdKeyField)[ pFieldName ] = utilParamValue( newValue,
                                                                 TRUE,
                                                                 hasFieldValue,
                                                                 changeLevel ) ;
         }
      }
      else
      {

         ossStrncpy( pValue, newValue.c_str(), len ) ;
         pValue[ len - 1 ] = 0 ;
         (*_pMapKeyField)[ pFieldName ] = utilParamValue( newValue,
                                                          TRUE,
                                                          hasFieldValue,
                                                          changeLevel ) ;
      }
   }

   MAP_K2V* _utilCfgExchange::getKVMap()
   {
      if ( PMD_CFG_DATA_CMD == _dataType )
      {
         if ( _pVMFile )
         {
            _makeKeyValueMap( _pVMFile ) ;
         }
         if ( _pVMCmd )
         {
            _makeKeyValueMap( _pVMCmd ) ;
         }
      }
      else
      {
         MAP_K2V::iterator itKV ;
         MAP_K2V::iterator itKVCold ;
         BSONObjIterator it( _dataObj ) ;
         while ( it.more() )
         {
            BSONElement e = it.next() ;

            itKV = _pMapKeyField->find( e.fieldName() ) ;
            if ( itKV != _pMapKeyField->end() &&
                 TRUE == itKV->second._hasMapped )
            {
               continue ;
            }

            if ( _pMapColdKeyField )
            {
               itKVCold = _pMapColdKeyField->find( e.fieldName() ) ;
               if ( itKVCold != _pMapColdKeyField->end() &&
                    TRUE == itKVCold->second._hasMapped )
               {
                  continue ;
               }
            }

            if ( String == e.type() )
            {
               (*_pMapKeyField)[ e.fieldName() ] = utilParamValue( e.valuestrsafe(),
                                                                   FALSE,
                                                                   TRUE ) ;
            }
            else if ( e.isNumber() )
            {
               (*_pMapKeyField)[ e.fieldName() ] = utilParamValue( e.numberInt(),
                                                                   FALSE,
                                                                   TRUE ) ;
            }
         }
      }

      return _pMapKeyField ;
   }

   /*
      _utilCfgRecord implement
   */
   _utilCfgRecord::_utilCfgRecord ()
   {
      _result = SDB_OK ;
      _changeID = 0 ;
      _pConfigHander = NULL ;
   }

   _utilCfgRecord::~_utilCfgRecord ()
   {
   }

   void _utilCfgRecord::setConfigHandler( IConfigHandle * pConfigHandler )
   {
      _pConfigHander = pConfigHandler ;
   }

   IConfigHandle* _utilCfgRecord::getConfigHandler() const
   {
      return _pConfigHander ;
   }

   INT32 _utilCfgRecord::restore( const BSONObj &objData,
                                  po::variables_map *pVMCMD )
   {
      INT32 rc = SDB_OK ;

      pmdCfgExchange ex( &_mapKeyValue, objData, TRUE, UTIL_CFG_STEP_INIT ) ;

      ossScopedLock lock( &_mutex ) ;

      /// clear info
      _mapKeyValue.clear() ;

      resetResult() ;
      doDataExchange( &ex ) ;
      rc = getResult() ;
      if ( rc )
      {
         goto error ;
      }
      /// make kv map
      ex.getKVMap() ;

      if ( pVMCMD )
      {
         pmdCfgExchange ex1( &_mapKeyValue, pVMCMD, NULL, TRUE,
                             UTIL_CFG_STEP_REINIT ) ;
         resetResult() ;
         doDataExchange( &ex1 ) ;
         rc = getResult() ;
         if ( rc )
         {
            goto error ;
         }
         /// make kv map
         ex1.getKVMap() ;
      }
      rc = postLoaded( UTIL_CFG_STEP_REINIT ) ;
      if ( rc )
      {
         goto error ;
      }

      if ( getConfigHandler() )
      {
         rc = getConfigHandler()->onConfigInit() ;
         if ( rc )
         {
            goto error ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::change( const BSONObj &objData,
                                 BOOLEAN isWhole )
   {
      INT32 rc = SDB_OK ;
      BSONObj oldCfg ;
      MAP_K2V mapKeyField ;
      MAP_K2V::iterator it ;
      MAP_K2V::iterator itSelf ;
      BOOLEAN locked = FALSE ;
      pmdCfgExchange ex( &mapKeyField, objData, TRUE, UTIL_CFG_STEP_CHG ) ;
      ex.setWhole( isWhole ) ;

      // save old cfg
      rc = toBSON( oldCfg, UTIL_CFG_MASK_SKIP_UNFIELD ) ;
      PD_RC_CHECK( rc, PDERROR, "Save old config failed, rc: %d", rc ) ;

      _mutex.get() ;
      locked = TRUE ;

      // update new cfg
      resetResult() ;
      doDataExchange( &ex ) ;
      rc = getResult() ;
      if ( rc )
      {
         goto restore ;
      }
      rc = postLoaded( UTIL_CFG_STEP_CHG ) ;
      if ( rc )
      {
         goto restore ;
      }
      ++_changeID ;

      /// make kv map
      ex.getKVMap() ;
      it = mapKeyField.begin() ;
      while ( it != mapKeyField.end() )
      {
         if ( it->second._hasMapped )
         {
            _mapKeyValue[ it->first ] = it->second ;
            ++it ;
            continue ;
         }

         itSelf = _mapKeyValue.find( it->first ) ;
         if ( itSelf != _mapKeyValue.end() )
         {
            if ( itSelf->second._hasMapped &&
                 itSelf->second._value != it->second._value )
            {
               PD_LOG( PDWARNING, "Field[%s]'s value[%s] can't be modified "
                       "to [%s] in running", it->first.c_str(),
                       itSelf->second._value.c_str(),
                       it->second._value.c_str() ) ;
            }
            else if ( !itSelf->second._hasMapped )
            {
               _mapKeyValue[ it->first ] = it->second ;
            }
         }
         else
         {
            _mapKeyValue[ it->first ] = it->second ;
         }
         ++it ;
      }

      if ( isWhole )
      {
         itSelf = _mapKeyValue.begin() ;
         while( itSelf != _mapKeyValue.end() )
         {
            if ( !itSelf->second._hasMapped &&
                 mapKeyField.find( itSelf->first ) == mapKeyField.end() )
            {
               _mapKeyValue.erase( itSelf++ ) ;
            }
            else
            {
               ++itSelf ;
            }
         }
      }

      // change notify
      if ( getConfigHandler() )
      {
         getConfigHandler()->onConfigChange( getChangeID() ) ;
      }

   done:
      if ( locked )
      {
         _mutex.release() ;
      }
      return rc ;
   error:
      goto done ;
   restore:
      if ( locked )
      {
         _mutex.release() ;
         locked = FALSE ;
      }
      restore( oldCfg, NULL ) ;
      goto done ;
   }

   INT32 _utilCfgRecord::update( const BSONObj &userConfig,
                                 BOOLEAN isWhole,
                                 BSONObj &errorObj )
   {
      return _update( userConfig, isWhole, errorObj, FALSE ) ;
   }

   INT32 _utilCfgRecord::delete( const BSONObj &objData,
                                 BSONObj &errorObj )
   {
      INT32 rc = SDB_OK ;
      BSONObj curConf ;
      BSONObjBuilder builder ;

      rc = toBSON( curConf, UTIL_CFG_MASK_SKIP_UNFIELD |
                            UTIL_CFG_MASK_MODE_LOCAL ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Dump current config obj failed, rc: %d", rc ) ;
         goto error ;
      }

      /// erase delete items
      {
         curConf.filterFieldsUndotted(const BSONObj & filter,bool inFilter)
         BSONObjIterator it( objData ) ;
         while( it.more() )
         {
            BSONElement e = it.next() ;
            if ( !curConf.hasField( e.fieldName() ) )
            {
               builder.append( e ) ;
            }
         }
      }
      curConf = builder.obj() ;

      /// update
      rc = _update( curConf, TRUE, errorObj,  TRUE ) ;
      if ( rc )
      {
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::_update( const BSONObj &userConfig,
                                  BOOLEAN isWhole,
                                  BSONObj &errorObj,
                                  BOOLEAN enableColdCompre )
   {
      INT32 rc = SDB_OK ;
      BSONObj oldCfg ;
      MAP_K2V mapKeyField ;
      MAP_K2V mapColdKeyField ;
      BOOLEAN locked = FALSE ;
      pmdCfgExchange ex( &mapKeyField, &mapColdKeyField, 
                         userConfig, TRUE, UTIL_CFG_STEP_CHG ) ;
      ex.setWhole( isWhole ) ;

      // save old cfg
      rc = toBSON( oldCfg, 0 ) ;
      PD_RC_CHECK( rc, PDERROR, "Save old config failed, rc: %d", rc ) ;

      _mutex.get() ;
      locked = TRUE ;

      resetResult() ;
      doDataExchange( &ex ) ;
      rc = getResult() ;
      if ( rc )
      {
         goto restore ;
      }

      rc = postLoaded( UTIL_CFG_STEP_CHG ) ;
      if ( rc )
      {
         goto restore ;
      }

      /// make kv map
      ex.getKVMap() ;

      rc = _saveUpdateChange( mapKeyField, mapColdKeyField,
                              errorObj, enableColdCompre ) ;
      if ( rc )
      {
         goto restore ;
      }
      ++_changeID ;

      if ( isWhole )
      {
         _purgeFieldMap( mapKeyField ) ;
      }

      // change notify
      if ( getConfigHandler() )
      {
         getConfigHandler()->onConfigChange( getChangeID() ) ;
      }

   done:
      if ( locked )
      {
         _mutex.release() ;
      }
      return rc ;
   error:
      goto done ;
   restore:
      if ( locked )
      {
         _mutex.release() ;
         locked = FALSE ;
      }
      restore( oldCfg, NULL ) ;
      goto done ;
   }

   INT32 _utilCfgRecord::init( po::variables_map *pVMFile,
                               po::variables_map *pVMCMD )
   {
      INT32 rc = SDB_OK ;

      pmdCfgExchange ex( &_mapKeyValue, pVMCMD, pVMFile, TRUE,
                         UTIL_CFG_STEP_INIT ) ;

      ossScopedLock lock( &_mutex ) ;

      resetResult() ;
      doDataExchange( &ex ) ;
      rc = getResult() ;
      if ( rc )
      {
         goto error ;
      }
      rc = postLoaded( UTIL_CFG_STEP_INIT ) ;
      if ( rc )
      {
         goto error ;
      }
      ++_changeID ;

      /// make kv map
      ex.getKVMap() ;

      if ( getConfigHandler() )
      {
         rc = getConfigHandler()->onConfigInit() ;
         if ( rc )
         {
            goto error ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::toBSON( BSONObj &objData, UINT32 mask )
   {
      INT32 rc = SDB_OK ;

      ossScopedLock lock( &_mutex ) ;

      rc = preSaving() ;
      if ( SDB_OK == rc )
      {
         pmdCfgExchange ex( &_mapKeyValue, BSONObj(), FALSE,
                            UTIL_CFG_STEP_INIT, mask ) ;
         resetResult() ;
         doDataExchange( &ex ) ;
         rc = getResult() ;
         if ( SDB_OK == rc )
         {
            UINT32 dataLen = 0 ;
            try
            {
               objData = BSONObj( ex.getData( dataLen,
                                              _mapKeyValue ) ).getOwned() ;
            }
            catch( std::exception &e )
            {
               PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
               rc = SDB_SYS ;
            }
         }
      }
      return rc ;
   }

   INT32 _utilCfgRecord::toString( string &str, UINT32 mask )
   {
      INT32 rc = SDB_OK ;

      ossScopedLock lock( &_mutex ) ;

      rc = preSaving() ;
      if ( SDB_OK == rc )
      {
         pmdCfgExchange ex( &_mapKeyValue, NULL, NULL, FALSE,
                            UTIL_CFG_STEP_INIT, mask ) ;
         resetResult() ;
         doDataExchange( &ex ) ;
         rc = getResult() ;
         if ( SDB_OK == rc )
         {
            UINT32 dataLen = 0 ;
            str = ex.getData( dataLen, _mapKeyValue ) ;
         }
      }
      return rc ;
   }

   INT32 _utilCfgRecord::postLoaded( UTIL_CFG_STEP step )
   {
      return SDB_OK ;
   }

   INT32 _utilCfgRecord::preSaving()
   {
      return SDB_OK ;
   }

   INT32 _utilCfgRecord::parseAddressLine( const CHAR * pAddressLine,
                                           vector < utilAddrPair > & vecAddr,
                                           const CHAR * pItemSep,
                                           const CHAR * pInnerSep,
                                           UINT32 maxSize ) const
   {
      INT32 rc = SDB_OK ;
      vector<string> addrs ;
      utilAddrPair addrItem ;

      if ( !pAddressLine || !pItemSep || !pInnerSep ||
           pItemSep[ 0 ] == 0 || pInnerSep[ 0 ] == 0 )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else if ( pAddressLine[ 0 ] == 0 )
      {
         goto done ;
      }

      boost::algorithm::split( addrs, pAddressLine,
                               boost::algorithm::is_any_of(
                               pItemSep ) ) ;
      if ( maxSize > 0 && maxSize < addrs.size() )
      {
         std::cerr << "addr more than max member size" << endl ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      for ( vector<string>::iterator itr = addrs.begin() ;
            itr != addrs.end() ;
            ++itr )
      {
         vector<string> pair ;
         string tmp = *itr ;
         boost::algorithm::trim( tmp ) ;
         boost::algorithm::split( pair, tmp,
                                  boost::algorithm::is_any_of(
                                  pInnerSep ) ) ;
         if ( pair.size() != 2 )
         {
            continue ;
         }
         addrItem._host = pair.at( 0 ) ;
         addrItem._service = pair.at( 1 ) ;
         vecAddr.push_back( addrItem ) ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   string _utilCfgRecord::makeAddressLine( const vector < utilAddrPair > & vecAddr,
                                           CHAR chItemSep,
                                           CHAR chInnerSep ) const
   {
      UINT32 count = 0 ;
      stringstream ss ;
      for ( UINT32 i = 0; i < vecAddr.size() ; ++i )
      {
         const utilAddrPair &item = vecAddr[ i ] ;
         if ( '\0' != item._host[ 0 ] )
         {
            if ( 0 != count )
            {
               ss << chItemSep ;
            }
            ss << item._host << chInnerSep << item._service ;
            ++count ;
         }
         else
         {
            break ;
         }
      }
      return ss.str() ;
   }

   INT32 _utilCfgRecord::_addToFieldMap( const string &key,
                                         const string &value,
                                         BOOLEAN hasMapped,
                                         BOOLEAN hasField )
   {
       _mapKeyValue[ key ] = utilParamValue( value, hasMapped, hasField ) ;
       return SDB_OK ;
   }

   INT32 _utilCfgRecord::_addToFieldMap( const string &key,
                                         INT32 value,
                                         BOOLEAN hasMapped,
                                         BOOLEAN hasField )
   {
      _mapKeyValue[ key ] = utilParamValue( value, hasMapped, hasField ) ;
      return SDB_OK ;
   }

   void _utilCfgRecord::_updateFieldMap( pmdCfgExchange *pEX,
                                         const CHAR *pFieldName,
                                         const CHAR *pValue,
                                         UTIL_CFG_CHANGE changeLevel )
   {
      if ( UTIL_CFG_STEP_CHG == pEX->getCfgStep() &&
           !ISALLOWRUNCHANGE( changeLevel ) )
      {
         MAP_K2V::iterator it = _mapColdKeyValue.find( pFieldName ) ;
         if ( it != _mapColdKeyValue.end() )
         {
            it->second._value = pValue ;
         }
      }
      else
      {
         MAP_K2V::iterator it = _mapKeyValue.find( pFieldName ) ;
         if ( it != _mapKeyValue.end() )
         {
            it->second._value = pValue ;
         }
      }
   }

   void _utilCfgRecord::_purgeFieldMap( MAP_K2V &mapKeyField )
   {
      MAP_K2V::iterator itSelf ;
      itSelf = _mapKeyValue.begin() ;
      while( itSelf != _mapKeyValue.end() )
      {
         if ( !itSelf->second._hasMapped &&
              mapKeyField.find( itSelf->first ) == mapKeyField.end() )
         {
            _mapKeyValue.erase( itSelf++ ) ;
         }
         else
         {
            ++itSelf ;
         }
      }
   }

   INT32 _utilCfgRecord::_saveUpdateChange( MAP_K2V &mapKeyField,
                                            MAP_K2V &mapColdKeyField,
                                            BSONObj &errorObj,
                                            BOOLEAN enableColdCompre )
   {
      INT32 rc = SDB_OK ;
      MAP_K2V::iterator iter ;
      MAP_K2V::iterator iterCold ;
      MAP_K2V::iterator itSelf ;
      MAP_K2V::iterator iterColdSelf ;
      BSONObjBuilder errorObjBuilder ;
      BSONArrayBuilder rebootArrBuilder ;
      BSONArrayBuilder forbidArrBuilder ;

      iter = mapKeyField.begin() ;
      while ( iter != mapKeyField.end() )
      {
         _mapKeyValue[ iter->first ] = iter->second ;
         ++iter ;
      }

      try
      {
         iterCold = mapColdKeyField.begin() ;
         while ( iterCold != mapColdKeyField.end() )
         {
            if ( enableColdCompre )
            {
               itSelf = _mapColdKeyValue.find( iterCold->first ) ;
               if ( itSelf != _mapColdKeyValue.end() &&
                    itSelf->second._value == iterCold->second._value )
               {
                  /// The same with org code value
                  continue ;
               }
            }

            if ( UTIL_CFG_CHANGE_REBOOT == iterCold->second._level )
            {
               _mapColdKeyValue[ iterCold->first ] = iterCold->second ;
            }

            itSelf = _mapKeyValue.find( iterCold->first ) ;
            if ( itSelf != _mapKeyValue.end() &&
                 itSelf->second._value != iterCold->second._value )
            {
               if ( UTIL_CFG_CHANGE_REBOOT == iterCold->second._level )
               {
                  rebootArrBuilder.append( iterCold->first ) ;
               }
               else if ( UTIL_CFG_CHANGE_FORBIDDEN == iterCold->second._level )
               {
                  forbidArrBuilder.append( iterCold->first ) ;
               }
            }
            ++iterCold ;
         }

         errorObjBuilder.append( "Reboot", rebootArrBuilder.arr() ) ;
         errorObjBuilder.append( "Forbidden", forbidArrBuilder.arr() ) ;
         errorObj = errorObjBuilder.obj() ;
      }
      catch( std::exception &e )
      {
         PD_LOG( PDWARNING, "Exception during config update: %s",
                 e.what() ) ;
         rc = SDB_SYS ;
      }
      return rc ;
   }

   BOOLEAN _utilCfgRecord::hasField( const CHAR * pFieldName ) const
   {
      if ( _mapKeyValue.find( pFieldName ) == _mapKeyValue.end() )
      {
         return FALSE ;
      }
      return TRUE ;
   }

   INT32 _utilCfgRecord::getFieldInt( const CHAR * pFieldName,
                                      INT32 &value,
                                      INT32 *pDefault ) const
   {
      INT32 rc = SDB_OK ;

      if ( !hasField( pFieldName ) )
      {
         if ( pDefault )
         {
            value = *pDefault ;
         }
         else
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
      }
      else
      {
         value = ossAtoi( _mapKeyValue[ pFieldName ]._value.c_str() ) ;
      }

      return rc ;
   }

   INT32 _utilCfgRecord::getFieldUInt( const CHAR *pFieldName,
                                       UINT32 &value,
                                       UINT32 *pDefault ) const
   {
      INT32 tmpValue = 0 ;
      INT32 rc = SDB_OK ;

      rc = getFieldInt( pFieldName, tmpValue, (INT32*)pDefault ) ;
      if ( SDB_OK == rc )
      {
         value = tmpValue ;
      }
      return rc ;
   }

   INT32 _utilCfgRecord::getFieldStr( const CHAR * pFieldName,
                                      CHAR *pValue,
                                      UINT32 len,
                                      const CHAR * pDefault ) const
   {
      INT32 rc = SDB_OK ;
      string tmpValue ;

      if ( !hasField( pFieldName ) )
      {
         if ( pDefault )
         {
            tmpValue.assign( pDefault ) ;
         }
         else
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
      }
      else
      {
         tmpValue = _mapKeyValue[ pFieldName ]._value ;
      }

      if ( SDB_OK == rc )
      {
         ossStrncpy( pValue, tmpValue.c_str(), len ) ;
         pValue[ len - 1 ] = 0 ;
      }

      return rc ;
   }

   INT32 _utilCfgRecord::getFieldStr( const CHAR *pFieldName,
                                      string &strValue,
                                      const CHAR *pDefault ) const
   {
      INT32 rc = SDB_OK ;

      if ( !hasField( pFieldName ) )
      {
         if ( pDefault )
         {
            strValue.assign( pDefault ) ;
         }
         else
         {
            rc = SDB_FIELD_NOT_EXIST ;
         }
      }
      else
      {
         strValue = _mapKeyValue[ pFieldName ]._value ;
      }
      return rc ;
   }

   INT32 _utilCfgRecord::rdxString( pmdCfgExchange *pEX,
                                    const CHAR *pFieldName,
                                    CHAR *pValue,
                                    UINT32 len,
                                    BOOLEAN required,
                                    UTIL_CFG_CHANGE changeLevel,
                                    const CHAR *pDefaultValue,
                                    BOOLEAN hideParam )
   {
      if ( _result )
      {
         goto error ;
      }
      _curFieldName = pFieldName ;

      if ( pEX->isLoad() )
      {
         if ( UTIL_CFG_STEP_REINIT == pEX->getCfgStep() &&
              !pEX->hasField( pFieldName ) )
         {
            goto done ;
         }
         else if ( UTIL_CFG_STEP_CHG == pEX->getCfgStep() )
         {
            if ( !pEX->isWhole() && !pEX->hasField( pFieldName ) )
            {
               goto done ;
            }
         }

         if ( required )
         {
            _result = pEX->readString( pFieldName, pValue, len, changeLevel ) ;
         }
         else
         {
            _result = pEX->readString( pFieldName, pValue, len,
                                       pDefaultValue, changeLevel ) ;
         }
         if ( SDB_FIELD_NOT_EXIST == _result )
         {
            ossPrintf( "Error: Field[%s] not config\n", pFieldName ) ;
         }
         else if ( _result )
         {
            ossPrintf( "Error: Read field[%s] config failed, rc: %d\n",
                       pFieldName, _result ) ;
         }
      }
      else
      {
          MAP_K2V::iterator it ;
          MAP_K2V::iterator itCold ;

          /// 1. when is default
          if ( 0 == ossStrcmp( pValue, pDefaultValue ) )
          {
             if ( hideParam && pEX->isSkipHideDefault() )
             {
                goto done ;
             }
             else if ( !hideParam && pEX->isSkipNormalDefault() )
             {
                goto done ;
             }
          }

          /// 2. skip unfield
          if ( pEX->isSkipUnField() )
          {
             it = _mapKeyValue.find( pFieldName ) ;
             itCold = _mapColdKeyValue.end() ;

             if ( UTIL_CFG_CHANGE_REBOOT == changeLevel )
             {
                if ( _mapColdKeyValue.end() != ( itCold = _mapColdKeyValue.find( pFieldName ) ) )
                {
                   it = _mapKeyValue.end() ;
                }
             }

             if ( ( it == _mapKeyValue.end() || !it->second._hasField ) &&
                  ( itCold == _mapColdKeyValue.end() || !itCold->second._hasField ) )
             {
                goto done ;
             }
          }

         if ( UTIL_CFG_CHANGE_REBOOT != changeLevel ||
              !pEX->isLocalMode() ||
              ( _mapKeyValue.end() != ( it = _mapKeyValue.find( pFieldName ) ) &&
                0 != ossStrcmp( pValue, it->second._value.c_str() ) ) ||
              ( _mapColdKeyValue.end() == ( itCold = _mapColdKeyValue.find( pFieldName ) ) ) )
         {
             _result = pEX->writeString( pFieldName, pValue ) ;
         }
         else
         {
             _result = pEX->writeString( pFieldName, 
                                         itCold->second._value.c_str() ) ;
         }
         if ( _result )
         {
            PD_LOG( PDWARNING, "Write field[%s] failed, rc: %d",
                    pFieldName, _result ) ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::_rdxPath( pmdCfgExchange *pEX,
                                   const CHAR *pFieldName,
                                   CHAR *pValue,
                                   UINT32 len,
                                   BOOLEAN required,
                                   UTIL_CFG_CHANGE changeLevel,
                                   const CHAR *pDefaultValue,
                                   BOOLEAN hideParam,
                                   BOOLEAN addSep )
   {
      _result = rdxString( pEX, pFieldName, pValue, len, required, changeLevel,
                           pDefaultValue, hideParam ) ;
      if ( SDB_OK == _result && pEX->isLoad() && 0 != pValue[0] )
      {
         string strTmp = pValue ;
         if ( NULL == ossGetRealPath( strTmp.c_str(), pValue, len ) )
         {
            ossPrintf( "Error: Failed to get real path for %s:%s\n",
                       pFieldName, strTmp.c_str() ) ;
            _result = SDB_INVALIDARG ;
         }
         else
         {
            pValue[ len - 1 ] = 0 ;

            if ( addSep )
            {
               utilCatPath( pValue, len, "" ) ;
            }
         }

         /// update map's value
         _updateFieldMap( pEX, pFieldName, pValue, changeLevel ) ;

      }
      return _result ;
   }

   INT32 _utilCfgRecord::rdxPath( pmdCfgExchange *pEX,
                                  const CHAR *pFieldName,
                                  CHAR *pValue,
                                  UINT32 len,
                                  BOOLEAN required,
                                  UTIL_CFG_CHANGE changeLevel,
                                  const CHAR *pDefaultValue,
                                  BOOLEAN hideParam )
   {
      return _rdxPath( pEX, pFieldName, pValue, len,
                       required, changeLevel, pDefaultValue,
                       hideParam, TRUE ) ;
   }

   INT32 _utilCfgRecord::rdxPathRaw( pmdCfgExchange *pEX,
                                     const CHAR *pFieldName,
                                     CHAR *pValue,
                                     UINT32 len,
                                     BOOLEAN required,
                                     UTIL_CFG_CHANGE changeLevel,
                                     const CHAR *pDefaultValue,
                                     BOOLEAN hideParam )
   {
      return _rdxPath( pEX, pFieldName, pValue, len,
                       required, changeLevel, pDefaultValue,
                       hideParam, FALSE ) ;
   }

   INT32 _utilCfgRecord::rdxBooleanS( pmdCfgExchange *pEX,
                                      const CHAR *pFieldName,
                                      BOOLEAN &value,
                                      BOOLEAN required,
                                      UTIL_CFG_CHANGE changeLevel,
                                      BOOLEAN defaultValue,
                                      BOOLEAN hideParam )
   {
      CHAR szTmp[ UTIL_MAX_ENUM_STR_LEN + 1 ] = {0} ;
      string boolValue ;
      ossStrcpy( szTmp, value ? "TRUE" : "FALSE" ) ;
      _result = rdxString( pEX, pFieldName, szTmp, sizeof(szTmp), required,
                           changeLevel, defaultValue ? "TRUE" : "FALSE",
                           hideParam ) ;
      if ( SDB_OK == _result && pEX->isLoad() )
      {
         if ( SDB_INVALIDARG == ossStrToBoolean( szTmp, &value ) )
         {
            value = defaultValue ;
         }

         // update map's value
         boolValue = value ? "TRUE" : "FALSE" ;
         _updateFieldMap( pEX, pFieldName, boolValue.c_str(), changeLevel ) ;

      }
      return _result ;
   }

   INT32 _utilCfgRecord::rdxInt( pmdCfgExchange *pEX,
                                 const CHAR *pFieldName,
                                 INT32 &value,
                                 BOOLEAN required,
                                 UTIL_CFG_CHANGE changeLevel,
                                 INT32 defaultValue,
                                 BOOLEAN hideParam )
   {
      if ( _result )
      {
         goto error ;
      }
      _curFieldName = pFieldName ;

      if ( pEX->isLoad() )
      {
         if ( UTIL_CFG_STEP_REINIT == pEX->getCfgStep() &&
              !pEX->hasField( pFieldName ) )
         {
            goto done ;
         }
         else if ( UTIL_CFG_STEP_CHG == pEX->getCfgStep() )
         {
            if ( !pEX->isWhole() && !pEX->hasField( pFieldName ) )
            {
               goto done ;
            }
         }

         if ( required )
         {
            _result = pEX->readInt( pFieldName, value, changeLevel ) ;
         }
         else
         {
            _result = pEX->readInt( pFieldName, value, defaultValue,
                                    changeLevel ) ;
         }
         if ( SDB_FIELD_NOT_EXIST == _result )
         {
            ossPrintf( "Error: Field[%s] not config\n", pFieldName ) ;
         }
         else if ( _result )
         {
            ossPrintf( "Error: Read field[%s] config failed, rc: %d\n",
                       pFieldName, _result ) ;
         }
      }
      else
      {
          MAP_K2V::iterator it ;
          MAP_K2V::iterator itCold ;

          /// 1. when is default
          if ( value == defaultValue )
          {
             if ( hideParam && pEX->isSkipHideDefault() )
             {
                goto done ;
             }
             else if ( !hideParam && pEX->isSkipNormalDefault() )
             {
                goto done ;
             }
          }

          /// 2. skip unfield
          if ( pEX->isSkipUnField() )
          {
             it = _mapKeyValue.find( pFieldName ) ;
             itCold = _mapColdKeyValue.end() ;

             if ( UTIL_CFG_CHANGE_REBOOT == changeLevel )
             {
                if ( _mapColdKeyValue.end() != ( itCold = _mapColdKeyValue.find( pFieldName ) ) )
                {
                   it = _mapKeyValue.end() ;
                }
             }

             if ( ( it == _mapKeyValue.end() || !it->second._hasField ) &&
                  ( itCold == _mapColdKeyValue.end() || !itCold->second._hasField ) )
             {
                goto done ;
             }
          }

         if ( UTIL_CFG_CHANGE_REBOOT != changeLevel ||
              !pEX->isLocalMode() ||
              ( _mapKeyValue.end() != ( it = _mapKeyValue.find( pFieldName ) ) &&
                value != ossAtoi( it->second._value.c_str() ) ) ||
              ( _mapColdKeyValue.end() == ( itCold = _mapColdKeyValue.find( pFieldName ) ) ) )
         {
             _result = pEX->writeInt( pFieldName, value ) ;
         }
         else
         {
             _result = pEX->writeInt( pFieldName, 
                                      ossAtoi( itCold->second._value.c_str() ) ) ;
         }
         if ( _result )
         {
            PD_LOG( PDWARNING, "Write field[%s] failed, rc: %d",
                    pFieldName, _result ) ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdxUInt( pmdCfgExchange *pEX,
                                  const CHAR *pFieldName,
                                  UINT32 &value,
                                  BOOLEAN required,
                                  UTIL_CFG_CHANGE changeLevel,
                                  UINT32 defaultValue,
                                  BOOLEAN hideParam )
   {
      INT32 tmpValue = (INT32)value ;
      _result = rdxInt( pEX, pFieldName, tmpValue, required, changeLevel,
                        (INT32)defaultValue, hideParam ) ;
      if ( SDB_OK == _result && pEX->isLoad() )
      {
         if ( tmpValue < 0 )
         {
            _result = SDB_INVALIDARG ;
            ossPrintf( "Waring: Field[%s] value[%d] is less than min value 0\n",
                       pFieldName, tmpValue ) ;
            goto error ;
         }
         value = (UINT32)tmpValue ;
      }
   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdxShort( pmdCfgExchange *pEX,
                                   const CHAR *pFieldName,
                                   INT16 &value,
                                   BOOLEAN required,
                                   UTIL_CFG_CHANGE changeLevel,
                                   INT16 defaultValue,
                                   BOOLEAN hideParam )
   {
      INT32 tmpValue = (INT32)value ;
      _result = rdxInt( pEX, pFieldName, tmpValue, required, changeLevel,
                        (INT32)defaultValue, hideParam ) ;
      if ( SDB_OK == _result && pEX->isLoad() )
      {
         value = (INT16)tmpValue ;
      }
      return _result ;
   }

   INT32 _utilCfgRecord::rdxUShort( pmdCfgExchange *pEX,
                                    const CHAR *pFieldName,
                                    UINT16 &value,
                                    BOOLEAN required,
                                    UTIL_CFG_CHANGE changeLevel,
                                    UINT16 defaultValue,
                                    BOOLEAN hideParam )
   {
      INT32 tmpValue = (INT32)value ;
      _result = rdxInt( pEX, pFieldName, tmpValue, required, changeLevel,
                        (INT32)defaultValue, hideParam ) ;
      if ( SDB_OK == _result && pEX->isLoad() )
      {
         if ( tmpValue < 0 )
         {
            _result = SDB_INVALIDARG ;
            ossPrintf( "Waring: Field[%s] value[%d] is less than min value 0\n",
                       pFieldName, tmpValue ) ;
            goto error ;
         }
         value = (UINT16)tmpValue ;
      }
   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdvMinMax( pmdCfgExchange *pEX,
                                    UINT32 &value,
                                    UINT32 minV,
                                    UINT32 maxV,
                                    BOOLEAN autoAdjust )
   {
      if ( _result )
      {
         goto error ;
      }

      if ( !pEX->isLoad() )
      {
         goto done ;
      }

      if ( value < minV )
      {
         ossPrintf( "Waring: Field[%s] value[%u] is less than min value[%u]\n",
                    _curFieldName.c_str(), value, minV ) ;
         if ( autoAdjust )
         {
            value = minV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }
      else if ( value > maxV )
      {
         ossPrintf( "Waring: Field[%s] value[%u] is more than max value[%u]\n",
                    _curFieldName.c_str(), value, maxV ) ;
         if ( autoAdjust )
         {
            value = maxV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdvMinMax( pmdCfgExchange *pEX,
                                    INT32 &value,
                                    INT32 minV,
                                    INT32 maxV,
                                    BOOLEAN autoAdjust )
   {
      if ( _result )
      {
         goto error ;
      }

      if ( !pEX->isLoad() )
      {
         goto done ;
      }

      if ( value < minV )
      {
         ossPrintf( "Waring: Field[%s] value[%d] is less than min value[%d]\n",
                    _curFieldName.c_str(), value, minV ) ;
         if ( autoAdjust )
         {
            value = minV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }
      else if ( value > maxV )
      {
         ossPrintf( "Waring: Field[%s] value[%d] is more than max value[%d]\n",
                 _curFieldName.c_str(), value, maxV ) ;
         if ( autoAdjust )
         {
            value = maxV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdvMinMax( pmdCfgExchange *pEX,
                                    UINT16 &value,
                                    UINT16 minV,
                                    UINT16 maxV,
                                    BOOLEAN autoAdjust )
   {
      if ( _result )
      {
         goto error ;
      }

      if ( !pEX->isLoad() )
      {
         goto done ;
      }

      if ( value < minV )
      {
         ossPrintf( "Waring: Field[%s] value[%u] is less than min value[%u]\n",
                    _curFieldName.c_str(), value, minV ) ;
         if ( autoAdjust )
         {
            value = minV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }
      else if ( value > maxV )
      {
         ossPrintf( "Waring: Field[%s] value[%u] is more than max value[%u]\n",
                    _curFieldName.c_str(), value, maxV ) ;
         if ( autoAdjust )
         {
            value = maxV ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdvMaxChar( pmdCfgExchange *pEX,
                                     CHAR *pValue,
                                     UINT32 maxChar,
                                     BOOLEAN autoAdjust )
   {
      UINT32 len = 0 ;

      if ( _result )
      {
         goto error ;
      }
      if ( !pEX->isLoad() )
      {
         goto done ;
      }

      len = ossStrlen( pValue ) ;
      if ( len > maxChar )
      {
         ossPrintf( "Waring: Field[%s] value[%s] length more than [%u]\n",
                    _curFieldName.c_str(), pValue, maxChar ) ;
         if ( autoAdjust )
         {
            pValue[ maxChar ] = 0 ;
         }
         else
         {
            _result = SDB_INVALIDARG ;
            goto error ;
         }
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   INT32 _utilCfgRecord::rdvNotEmpty( pmdCfgExchange * pEX,
                                      CHAR *pValue )
   {
      if ( _result )
      {
         goto error ;
      }
      if ( !pEX->isLoad() )
      {
         goto done ;
      }

      if ( ossStrlen( pValue ) == 0 )
      {
         ossPrintf( "Waring: Field[%s] is empty\n", _curFieldName.c_str() ) ;
         _result = SDB_INVALIDARG ;
         goto error ;
      }

   done:
      return _result ;
   error:
      goto done ;
   }

   /*
      _utilCfgBase implement
   */
   _utilCfgBase::_utilCfgBase()
   {
   }

   _utilCfgBase::~_utilCfgBase()
   {
   }

   void _utilCfgBase::setConfigFile( const CHAR *pFileOrPath )
   {
      if ( pFileOrPath && pFileOrPath[0] )
      {
         BOOLEAN isDir = FALSE ;
         SDB_OSS_FILETYPE type = SDB_OSS_UNK ;
         UINT32 len = ossStrlen( pFileOrPath ) ;

         if ( pFileOrPath[ len - 1 ] == OSS_FILE_SEP_CHAR )
         {
            isDir = TRUE ;
            _confPath = pFileOrPath ;
         }
         else if ( SDB_OK == ossAccess( pFileOrPath, OSS_MODE_ACCESS ) &&
                   SDB_OK == ossGetPathType( pFileOrPath, &type ) &&
                   SDB_OSS_ISDIR( type ) )
         {
            isDir = TRUE ;
            _confPath = pFileOrPath ;
            _confPath += OSS_FILE_SEP ;
         }

         if ( isDir )
         {
            _confFile = _confPath + getConfShortName() ;
         }
         else
         {
            _confFile = pFileOrPath ;

            /// find the last seperator
            size_t pos =  _confFile.find_last_of( OSS_FILE_SEP_CHAR ) ;
            if ( pos != string::npos )
            {
               _confPath = _confFile.substr( 0, pos ) ;
            }
            else
            {
               _confPath = PMD_CURRENT_PATH ;
            }

            _confPath += OSS_FILE_SEP ;
         }
      }
   }

   const CHAR* _utilCfgBase::getConfigFile() const
   {
      if ( !_confFile.empty() )
      {
         return _confFile.c_str() ;
      }
      return NULL ;
   }

   const CHAR* _utilCfgBase::getConfigPath() const
   {
      if ( !_confPath.empty() )
      {
         return _confPath.c_str() ;
      }
      return NULL ;
   }

   INT32 _utilCfgBase::initFromFile( const CHAR *pFileOrPath,
                                     BOOLEAN allowFileNotExist,
                                     BOOLEAN allowUnReg )
   {
      INT32 rc = SDB_OK ;
      const po::options_description &desc = getOptDesc() ;
      po::variables_map vm ;

      if ( pFileOrPath && pFileOrPath[0] )
      {
         CHAR cfgTempPath[ OSS_MAX_PATHSIZE + 1 ] = { 0 } ;
         CHAR *p = NULL ;

         p = ossGetRealPath( pFileOrPath, cfgTempPath, OSS_MAX_PATHSIZE ) ;
         if ( NULL == p )
         {
            PD_LOG( PDERROR, "Failed to get real path for %s", pFileOrPath ) ;
            ossPrintf( "*ERROR: Failed to get real path for %s"OSS_NEWLINE,
                       pFileOrPath ) ;
            rc = SDB_INVALIDPATH ;
            goto error;
         }

         setConfigFile( p ) ;
      }

      if ( !getConfigFile() )
      {
         PD_LOG( PDERROR, "Config file is not set" ) ;
         ossPrintf( "*Error: Config file is not set"OSS_NEWLINE ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      rc = utilReadConfigureFile( getConfigFile(), desc, vm, allowUnReg ) ;
      if ( rc )
      {
         if ( SDB_FNE != rc || !allowFileNotExist )
         {
            PD_LOG( PDERROR, "Failed to read config from file[%s], rc: %d",
                    getConfigFile(), rc ) ;
            ossPrintf( "*Error: Failed to read config from file[%s], "
                       "rc: %d"OSS_NEWLINE, getConfigFile(), rc ) ;
            goto error ;
         }
      }

      rc = utilCfgRecord::init( &vm, NULL ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Init config record failed, rc: %d", rc ) ;
         ossPrintf( "*Error: Init config record failed, rc: %d"OSS_NEWLINE,
                    rc ) ;
         goto error ;
      }

      if ( getConfPathKey() )
      {
         /// update conf path
         _addToFieldMap( getConfPathKey(), getConfigPath(), TRUE, FALSE ) ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCfgBase::initFromArgs( INT32 argc,
                                     const CHAR **argv,
                                     BOOLEAN allowUnReg )
   {
      INT32 rc = SDB_OK ;
      const po::options_description &desc = getOptDesc() ;
      po::variables_map vm ; /// cmd
      po::variables_map vm2 ; /// file

      rc = utilReadCommandLine( argc, argv, desc, vm, allowUnReg );
      if ( rc )
      {
         goto error ;
      }

      rc = onPostParseArgs( vm ) ;
      if ( rc )
      {
         goto error ;
      }

      if ( getConfPathKey() )
      {
         CHAR cfgTempPath[ OSS_MAX_PATHSIZE + 1 ] = { 0 } ;
         string orgPath ;
         CHAR *p = NULL ;
         BOOLEAN specificConf = FALSE ;

         if ( vm.count( getConfPathKey() ) )
         {
            orgPath = vm[getConfPathKey()].as<string>() ;
            specificConf = TRUE ;
         }
         else if ( getDefaultPath() )
         {
            orgPath = getDefaultPath() ;
         }

         if ( !orgPath.empty() )
         {
            p = ossGetRealPath( orgPath.c_str(),
                                cfgTempPath,
                                OSS_MAX_PATHSIZE ) ;
            if ( NULL == p )
            {
               PD_LOG( PDERROR, "Failed to get real path for %s",
                       orgPath.c_str() ) ;
               ossPrintf( "*ERROR: Failed to get real path for %s"OSS_NEWLINE,
                          orgPath.c_str() ) ;
               rc = SDB_INVALIDPATH ;
               goto error;
            }
            setConfigFile( p ) ;

            if ( getConfigFile() )
            {
               rc = utilReadConfigureFile( getConfigFile(), desc,
                                           vm2, allowUnReg ) ;
               if ( SDB_OK != rc )
               {
                  //if user set  configure file,  but cann't read the
                  // configure file. then we should exit.
                  if ( specificConf )
                  {
                     ossPrintf( "*Error: Read config file[%s] failed, "
                                "rc: %d"OSS_NEWLINE, getConfigFile(), rc ) ;
                     PD_LOG( PDERROR, "Read config file[%s] failed, rc: %d",
                             getConfigFile(), rc ) ;
                     goto error ;
                  }
                  // we should avoid exit when config file is not found or 
                  // I/O error. We should continue run but use other 
                  // arguments that uses input from command line
                  else if ( SDB_FNE != rc )
                  {
                     ossPrintf( "*Error: Read default config file[%s] failed, "
                                "rc: %d", getConfigFile(), rc ) ;
                     PD_LOG( PDERROR, "Read default config file[%s] failed, "
                             "rc: %d", getConfigFile(), rc ) ;
                     goto error ;
                  }
                  else
                  {
                     ossPrintf( "Info: Using default config" ) ;
                     PD_LOG( PDINFO, "Using default config" ) ;
                     rc = SDB_OK ;
                  }
               } // if ( SDB_OK != rc )
            } // if ( getConfigFile() )
         } // if ( !orgPath.empty() )
      } // if ( getConfPathKey() )

      rc = utilCfgRecord::init( &vm2, &vm ) ;
      if ( rc )
      {
         goto error ;
      }

      if ( getConfPathKey() )
      {
         /// update conf path
         _addToFieldMap( getConfPathKey(), getConfigPath(), TRUE, FALSE ) ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCfgBase::saveToFile( UINT32 mask, const CHAR *pPathFile )
   {
      INT32 rc = SDB_OK ;
      string line ;

      if ( !pPathFile || !pPathFile[0] )
      {
         pPathFile = getConfigFile() ;
      }

      if ( !pPathFile )
      {
         PD_LOG( PDERROR, "Config file is not configured" ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      rc = utilCfgRecord::toString( line, mask ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to get the line str, rc: %d", rc ) ;
         goto error ;
      }

      {
         ossScopedLock lock( &_mutex ) ;

         rc = utilWriteConfigFile( pPathFile, line.c_str(), FALSE ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to write config[%s], rc: %d",
                    pPathFile, rc ) ;
            goto error ;
         }

         // save notify
         if ( getConfigHandler() )
         {
            getConfigHandler()->onConfigSave() ;
         }
      }

   done:
      return rc ;
   error:
      goto done ;
   }

}


