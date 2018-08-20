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

   Source File Name = utilParamBase.hpp

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
#ifndef UTIL_PARAM_BASE_HPP__
#define UTIL_PARAM_BASE_HPP__

#include "pmdInterface.hpp"
#include "utilParam.hpp"
#include "ossUtil.hpp"
#include "ossLatch.hpp"
#include "../bson/bson.h"

using namespace bson ;

namespace engine
{
   /*
      COMMAND DEFINE
   */
   #define UTIL_MAX_ENUM_STR_LEN                ( 32 )

   /*
      UTIL_CFG_STEP define
   */
   enum UTIL_CFG_STEP
   {
      UTIL_CFG_STEP_INIT         = 0,           // initialize
      UTIL_CFG_STEP_REINIT,                     // re-init
      UTIL_CFG_STEP_CHG                         // change in runtime
   } ;

   /*
      UTIL_CFG_CHANGE define
   */
   enum UTIL_CFG_CHANGE
   {
      UTIL_CFG_CHANGE_INVALID    = 0,
      UTIL_CFG_CHANGE_FORBIDDEN,                // DO NOT allow change
      UTIL_CFG_CHANGE_RUN,                      // allow change in runtime
      UTIL_CFG_CHANGE_REBOOT                    // allow change after reboot
   } ;

   /*
      UTIL_CFG_DATA_TYPE define
   */
   enum UTIL_CFG_DATA_TYPE
   {
      PMD_CFG_DATA_CMD        = 0,              // command
      PMD_CFG_DATA_BSON                         // BSON
   } ;

   /*
      _utilParamValue define
   */
   struct _utilParamValue
   {
      string            _value ;
      BOOLEAN           _hasMapped ;
      BOOLEAN           _hasField ;
      UTIL_CFG_CHANGE   _level;

      _utilParamValue()
      {
         _hasMapped = FALSE ;
         _hasField = FALSE ;
         _level = UTIL_CFG_CHANGE_INVALID ;
      }
      _utilParamValue( INT32 value,
                       BOOLEAN hasMappe,
                       BOOLEAN hasField,
                       UTIL_CFG_CHANGE changeLevel = UTIL_CFG_CHANGE_INVALID )
      {
         CHAR tmp[ 15 ] = { 0 } ;
         ossItoa( value, tmp, sizeof( tmp ) - 1 ) ;
         _value = tmp ;
         _hasMapped = hasMappe ;
         _hasField = hasField ;
         _level  = changeLevel ;
      }
      _utilParamValue( const string &value,
                       BOOLEAN hasMappe,
                       BOOLEAN hasField,
                       UTIL_CFG_CHANGE changeLevel = UTIL_CFG_CHANGE_INVALID )
      {
         _value = value ;
         _hasMapped = hasMappe ;
         _hasField = hasField ;
         _level  = changeLevel ;
      }
   } ;
   typedef _utilParamValue utilParamValue ;

   typedef map< string, utilParamValue >     MAP_K2V ;

   /*
      Mask define
   */
   #define UTIL_CFG_MASK_SKIP_UNFIELD        0x00000001
   #define UTIL_CFG_MASK_SKIP_NORMALDFT      0x00000002
   #define UTIL_CFG_MASK_SKIP_HIDEDFT        0x00000004
   #define UTIL_CFG_MASK_MODE_LOCAL          0x00000008

   /*
      _utilCfgExchange define
   */
   class _utilCfgExchange : public SDBObject
   {
      friend class _pmdCfgRecord ;

      public:
         _utilCfgExchange ( MAP_K2V *pMapField,
                            const BSONObj &dataObj,
                            BOOLEAN load = TRUE,
                            UTIL_CFG_STEP step = UTIL_CFG_STEP_INIT,
                            UINT32 mask = 0 ) ;

         _utilCfgExchange ( MAP_K2V *pMapField,
                            MAP_K2V *pMapColdField,
                            const BSONObj &dataObj,
                            BOOLEAN load = TRUE,
                            UTIL_CFG_STEP step = UTIL_CFG_STEP_INIT,
                            UINT32 mask = 0 ) ;

         _utilCfgExchange ( MAP_K2V *pMapField,
                            po::variables_map *pVMCmd,
                            po::variables_map *pVMFile = NULL,
                            BOOLEAN load = TRUE,
                            UTIL_CFG_STEP step = UTIL_CFG_STEP_INIT,
                            UINT32 mask = 0 ) ;

         ~_utilCfgExchange () ;

         UTIL_CFG_STEP  getCfgStep() const { return _cfgStep ; }
         BOOLEAN        isLoad() const { return _isLoad ; }
         BOOLEAN        isSave() const { return !_isLoad ; }

         void           setLoad() { _isLoad = TRUE ; }
         void           setSave() { _isLoad = FALSE ; }
         void           setCfgStep( UTIL_CFG_STEP step ) { _cfgStep = step ; }
         void           setWhole( BOOLEAN isWhole ) { _isWhole = isWhole ; }

      public:
         INT32 readInt( const CHAR *pFieldName,
                        INT32 &value,
                        INT32 defaultValue,
                        UTIL_CFG_CHANGE changeLevel ) ;

         INT32 readInt( const CHAR *pFieldName,
                        INT32 &value,
                        UTIL_CFG_CHANGE changeLevel ) ;

         INT32 readString( const CHAR *pFieldName,
                           CHAR *pValue,
                           UINT32 len,
                           const CHAR *pDefault,
                           UTIL_CFG_CHANGE changeLevel ) ;

         INT32 readString( const CHAR *pFieldName,
                           CHAR *pValue,
                           UINT32 len,
                           UTIL_CFG_CHANGE changeLevel ) ;

         INT32 writeInt( const CHAR *pFieldName, INT32 value ) ;
         INT32 writeString( const CHAR *pFieldName, const CHAR *pValue ) ;

         BOOLEAN hasField( const CHAR *pFieldName ) ;
         BOOLEAN isSkipUnField() const
         {
            return ( _mask & UTIL_CFG_MASK_SKIP_UNFIELD ) ? TRUE : FALSE ;
         }
         BOOLEAN isSkipHideDefault() const
         {
            return ( _mask & UTIL_CFG_MASK_SKIP_HIDEDFT ) ? TRUE : FALSE ;
         }
         BOOLEAN isSkipNormalDefault() const
         {
            return ( _mask & UTIL_CFG_MASK_SKIP_NORMALDFT ) ? TRUE : FALSE ;
         }
         BOOLEAN isLocalMode() const
         {
            return ( _mask & UTIL_CFG_MASK_MODE_LOCAL ) ? TRUE : FALSE ;
         }
         BOOLEAN isWhole() const { return _isWhole ; }

      private:
         const CHAR *getData( UINT32 &dataLen, MAP_K2V &mapKeyValue ) ;
         MAP_K2V*    getKVMap() ;

         void        _makeKeyValueMap( po::variables_map *pVM ) ;

         void        _saveToMapInt( const CHAR * pFieldName,
                                    INT32 &value,
                                    const INT32 &newValue,
                                    UTIL_CFG_CHANGE changeLevel,
                                    BOOLEAN useDefault ) ;

         void        _saveToMapString( const CHAR *pFieldName, 
                                       CHAR *pValue,
                                       UINT32 len,
                                       const string &newValue,
                                       UTIL_CFG_CHANGE changeLevel,
                                       BOOLEAN useDefault ) ;

      private:
         MAP_K2V*                _pMapKeyField ;
         MAP_K2V*                _pMapColdKeyField ;
         UTIL_CFG_STEP           _cfgStep ;
         BOOLEAN                 _isLoad ;

         UTIL_CFG_DATA_TYPE      _dataType ;
         //
         BSONObj                 _dataObj ;
         BSONObjBuilder          _dataBuilder ;
         po::variables_map       *_pVMFile ;
         po::variables_map       *_pVMCmd ;
         stringstream            _strStream ;
         string                  _dataStr ;

         UINT32                  _mask ;
         BOOLEAN                 _isWhole ;

   } ;
   typedef _utilCfgExchange pmdCfgExchange ;

   /*
      _utilAddrPair define
   */
   typedef class _utilAddrPair
   {
      public :
         string _host ;
         string _service ;

      _utilAddrPair()
      {
      }
      _utilAddrPair( const string &hostname, const string &svcname )
      {
         _host = hostname ;
         _service = svcname ;
      }
   } utilAddrPair ;

   /*
      _utilCfgRecord define
   */
   class _utilCfgRecord : public SDBObject, public IPmdParam
   {
      public:
         virtual  BOOLEAN  hasField( const CHAR *pFieldName ) const ;

         virtual  INT32    getFieldInt( const CHAR *pFieldName,
                                        INT32 &value,
                                        INT32 *pDefault = NULL ) const ;

         virtual  INT32    getFieldUInt( const CHAR *pFieldName,
                                         UINT32 &value,
                                         UINT32 *pDefault = NULL ) const ;

         virtual  INT32    getFieldStr( const CHAR *pFieldName,
                                        CHAR *pValue,
                                        UINT32 len,
                                        const CHAR *pDefault = NULL ) const ;

         virtual  INT32    getFieldStr( const CHAR *pFieldName,
                                        string &strValue,
                                        const CHAR *pDefault = NULL ) const ;

      public:
         _utilCfgRecord () ;
         virtual ~_utilCfgRecord () ;

         void  setConfigHandler( IPmdConfigHandle *pConfigHandler ) ;
         IPmdConfigHandle* getConfigHandler() const ;

         INT32 getResult () const { return _result ; }
         void  resetResult () { _result = SDB_OK ; }

         INT32 init( po::variables_map *pVMFile,
                     po::variables_map *pVMCMD ) ;

         INT32 restore( const BSONObj &objData,
                        po::variables_map *pVMCMD ) ;

         /*
            For reload
         */
         INT32 change( const BSONObj &objData,
                       BOOLEAN isWhole = FALSE ) ;

         /*
            For update in runtime
         */
         INT32 update( const BSONObj &userConfig,
                       BOOLEAN isWhole,
                       BSONObj &errorObj ) ;

         /*
            For delete in runtime
         */
         INT32 delete( const BSONObj &objData,
                       BSONObj &errorObj ) ;

         INT32 toBSON ( BSONObj &objData,
                        UINT32 mask = UTIL_CFG_MASK_SKIP_HIDEDFT ) ;
         INT32 toString( string &str,
                         UINT32 mask = UTIL_CFG_MASK_SKIP_HIDEDFT ) ;

         UINT32 getChangeID () const { return _changeID ; }

      public:
         /*
            Parse address line(192.168.20.106:5000,192.168.30.102:1000...) to
            utilAddrPair
         */
         INT32 parseAddressLine( const CHAR *pAddressLine,
                                 vector< utilAddrPair > &vecAddr,
                                 const CHAR *pItemSep = ",",
                                 const CHAR *pInnerSep = ":",
                                 UINT32 maxSize = 0 ) const ;

         string makeAddressLine( const vector< utilAddrPair > &vecAddr,
                                 CHAR chItemSep = ',',
                                 CHAR chInnerSep = ':' ) const ;

      protected:
         INT32 _addToFieldMap( const string &key, INT32 value,
                               BOOLEAN hasMapped = TRUE,
                               BOOLEAN hasField = TRUE ) ;
         INT32 _addToFieldMap( const string &key, const string &value,
                               BOOLEAN hasMapped = TRUE,
                               BOOLEAN hasField = TRUE ) ;
         void  _updateFieldMap( pmdCfgExchange *pEX,
                                const CHAR *pFieldName,
                                const CHAR *pValue,
                                UTIL_CFG_CHANGE changeLevel ) ;

         void  _purgeFieldMap( MAP_K2V &mapKeyField ) ;

         INT32 _saveUpdateChange( MAP_K2V &mapKeyField,
                                  MAP_K2V &mapColdKeyField,
                                  BSONObj &errorObj,
                                  BOOLEAN enableColdCompre ) ;

         INT32 _update( const BSONObj &userConfig,
                        BOOLEAN isWhole,
                        BSONObj &errorObj,
                        BOOLEAN enableColdCompre ) ;


      protected:
         virtual void  doDataExchange( pmdCfgExchange *pEX ) = 0 ;
         virtual INT32 postLoaded( UTIL_CFG_STEP step ) ;
         virtual INT32 preSaving() ;

      protected:
         INT32 rdxInt( pmdCfgExchange *pEX,
                       const CHAR *pFieldName,
                       INT32 &value,
                       BOOLEAN required,
                       UTIL_CFG_CHANGE changeLevel,
                       INT32 defaultValue,
                       BOOLEAN hideParam = FALSE ) ;

         INT32 rdxUInt( pmdCfgExchange *pEX,
                        const CHAR *pFieldName,
                        UINT32 &value,
                        BOOLEAN required,
                        UTIL_CFG_CHANGE changeLevel,
                        UINT32 defaultValue,
                        BOOLEAN hideParam = FALSE ) ;

         INT32 rdxShort( pmdCfgExchange *pEX,
                         const CHAR *pFieldName,
                         INT16 &value,
                         BOOLEAN required,
                         UTIL_CFG_CHANGE changeLevel,
                         INT16 defaultValue,
                         BOOLEAN hideParam = FALSE ) ;

         INT32 rdxUShort( pmdCfgExchange *pEX,
                          const CHAR *pFieldName,
                          UINT16 &value,
                          BOOLEAN required,
                          UTIL_CFG_CHANGE changeLevel,
                          UINT16 defaultValue,
                          BOOLEAN hideParam = FALSE ) ;

         INT32 rdxString( pmdCfgExchange *pEX,
                          const CHAR *pFieldName,
                          CHAR *pValue,
                          UINT32 len,
                          BOOLEAN required,
                          UTIL_CFG_CHANGE changeLevel,
                          const CHAR *pDefaultValue,
                          BOOLEAN hideParam = FALSE ) ;

         INT32 rdxPath( pmdCfgExchange *pEX,
                        const CHAR *pFieldName,
                        CHAR *pValue,
                        UINT32 len,
                        BOOLEAN required,
                        UTIL_CFG_CHANGE changeLevel,
                        const CHAR *pDefaultValue,
                        BOOLEAN hideParam = FALSE ) ;

         INT32 rdxPathRaw( pmdCfgExchange *pEX,
                           const CHAR *pFieldName,
                           CHAR *pValue,
                           UINT32 len,
                           BOOLEAN required,
                           UTIL_CFG_CHANGE changeLevel,
                           const CHAR *pDefaultValue,
                           BOOLEAN hideParam = FALSE ) ;

         INT32 rdxBooleanS( pmdCfgExchange *pEX,
                            const CHAR *pFieldName,
                            BOOLEAN &value,
                            BOOLEAN required,
                            UTIL_CFG_CHANGE changeLevel,
                            BOOLEAN defaultValue,
                            BOOLEAN hideParam = FALSE ) ;

         INT32 rdvMinMax( pmdCfgExchange *pEX,
                          UINT32 &value,
                          UINT32 minV,
                          UINT32 maxV,
                          BOOLEAN autoAdjust = TRUE ) ;

         INT32 rdvMinMax( pmdCfgExchange *pEX,
                          INT32 &value,
                          INT32 minV,
                          INT32 maxV,
                          BOOLEAN autoAdjust = TRUE ) ;

         INT32 rdvMinMax( pmdCfgExchange *pEX,
                          UINT16 &value,
                          UINT16 minV,
                          UINT16 maxV,
                          BOOLEAN autoAdjust = TRUE ) ;

         INT32 rdvMaxChar( pmdCfgExchange *pEX,
                           CHAR *pValue,
                           UINT32 maxChar,
                           BOOLEAN autoAdjust = TRUE ) ;

         INT32 rdvNotEmpty( pmdCfgExchange *pEX,
                            CHAR *pValue ) ;

      private:
         INT32 _rdxPath( pmdCfgExchange *pEX,
                         const CHAR *pFieldName,
                         CHAR *pValue,
                         UINT32 len,
                         BOOLEAN required,
                         UTIL_CFG_CHANGE changeLevel,
                         const CHAR *pDefaultValue,
                         BOOLEAN hideParam,
                         BOOLEAN addSep ) ;

      private:
         string                              _curFieldName ;
         INT32                               _result ;
         UINT32                              _changeID ;
         IPmdConfigHandle                    *_pConfigHander ;

         MAP_K2V                             _mapKeyValue ;
         MAP_K2V                             _mapColdKeyValue ;
      protected:
         ossSpinXLatch                       _mutex ;

   } ;
   typedef _utilCfgRecord utilCfgRecord ;

   /*
      _utilCfgBase define
   */
   class _utilCfgBase : public _utilCfgRecord
   {
      public:
         _utilCfgBase() ;
         virtual ~_utilCfgBase() ;

      public:

         void           setConfigFile( const CHAR *pFileOrPath ) ;
         const CHAR*    getConfigFile() const ;
         const CHAR*    getConfigPath() const ;

         INT32          initFromFile( const CHAR *pFileOrPath = NULL,
                                      BOOLEAN allowFileNotExist = FALSE,
                                      BOOLEAN allowUnReg = TRUE ) ;

         INT32          initFromArgs( INT32 argc,
                                      const CHAR **argv,
                                      BOOLEAN allowUnReg = TRUE ) ;

         INT32          saveToFile( UINT32 mask = UTIL_CFG_MASK_SKIP_UNFIELD,
                                    const CHAR *pPathFile = NULL ) ;

      protected:

         virtual const po::options_description& getOptDesc() const = 0 ;

         virtual const CHAR* getConfShortName() const = 0 ;
         virtual const CHAR* getDefaultPath() const = 0 ;
         virtual const CHAR* getConfPathKey() const = 0 ;

         virtual INT32 onPostParseArgs( const po::variables_map &vm ) const = 0 ;

      private:
         string         _confFile ;
         string         _confPath ;

   } ;
   typedef _utilCfgBase utilCfgBase ;

}

#endif //UTIL_PARAM_BASE_HPP__

