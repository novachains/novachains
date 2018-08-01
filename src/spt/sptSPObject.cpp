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

   Source File Name = sptSPObject.cpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  WJM  Initial Draft

   Last Changed =

*******************************************************************************/
#include "sptSPObject.hpp"
#include "sptConvertor.hpp"
#include "sptConvertorHelper.hpp"
#include "utilStr.hpp"
#include "pd.hpp"
#include <sstream>
namespace engine
{
   sptSPObject::sptSPObject( JSContext *cx, JSObject *obj ):
      _cx( cx ), _obj( obj )
   {
   }

   sptSPObject::~sptSPObject()
   {
   }

   INT32 sptSPObject::getObjectField( const std::string &fieldName,
                                      sptObject **obj ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      JSObject *jsObj = NULL ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      if( !JSVAL_IS_OBJECT( val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      jsObj = JSVAL_TO_OBJECT( val ) ;
      if( NULL == jsObj )
      {
         rc = SDB_SYS ;
         goto error ;
      }
      *obj = SDB_OSS_NEW sptSPObject( _cx, jsObj ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getBoolField( const std::string &fieldName, BOOLEAN &rval,
                                    INT32 mask ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else
      {
         SPT_JS_TYPE type ;
         rc = _getTypeOfVal( val, type ) ;
         if( SDB_OK != rc )
         {
            goto error ;
         }
         switch( type )
         {
            case SPT_JS_TYPE_BOOLEAN:
               if( mask & SPT_CVT_FLAGS_FROM_BOOL )
               {
                  rval = static_cast< BOOLEAN >( JSVAL_TO_BOOLEAN( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_INT:
               if( mask & SPT_CVT_FLAGS_FROM_INT )
               {
                  rval = static_cast< BOOLEAN >( JSVAL_TO_INT( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_DOUBLE:
               if( mask & SPT_CVT_FLAGS_FROM_DOUBLE )
               {
                  rval = static_cast< BOOLEAN >( JSVAL_TO_DOUBLE( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_STRING:
               if( mask & SPT_CVT_FLAGS_FROM_STRING )
               {
                  string str = static_cast< CHAR* >(
                     JS_EncodeString( _cx, JSVAL_TO_STRING( val ) ) ) ;
                  if( str == "TRUE" || str == "true" ||
                      str == "T" || str == "t"  )
                  {
                     rval = TRUE ;
                  }
                  else if( str == "FALSE" || str == "false" ||
                           str == "F" || str == "f" )
                  {
                     rval = FALSE ;
                  }
                  else
                  {
                     rc = SDB_INVALIDARG ;
                     goto error ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_OBJECT:
               if( mask & SPT_CVT_FLAGS_FROM_OBJECT )
               {
                  JSObject *tmpObj = JSVAL_TO_OBJECT( val ) ;
                  BOOLEAN isSpecialObj = FALSE ;
                  const _sptObjDesc *objDesc = NULL ;
                  rc = _getObjectDesc( tmpObj, isSpecialObj, &objDesc ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
                  rc = objDesc->cvtToBool( sptSPObject( _cx, tmpObj ),
                                           isSpecialObj, rval ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
         }
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getIntField( const std::string &fieldName, INT32 &rval,
                                   INT32 mask ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else
      {
         SPT_JS_TYPE type ;
         rc = _getTypeOfVal( val, type ) ;
         if( SDB_OK != rc )
         {
            goto error ;
         }
         switch( type )
         {
            case SPT_JS_TYPE_BOOLEAN:
               if( mask & SPT_CVT_FLAGS_FROM_BOOL )
               {
                  rval = static_cast< INT32 >( JSVAL_TO_BOOLEAN( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_INT:
               if( mask & SPT_CVT_FLAGS_FROM_INT )
               {
                  rval = JSVAL_TO_INT( val ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_DOUBLE:
               if( mask & SPT_CVT_FLAGS_FROM_DOUBLE )
               {
                  rval = static_cast< INT32 >( JSVAL_TO_DOUBLE( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_STRING:
               if( mask & SPT_CVT_FLAGS_FROM_STRING )
               {
                  const CHAR* pStr = NULL ;
                  pStr = static_cast< CHAR* >
                     ( JS_EncodeString( _cx, JSVAL_TO_STRING( val ) ) ) ;
                  if( NULL == pStr )
                  {
                     rc = SDB_INVALIDARG ;
                     goto error ;
                  }
                  rc = utilStr2Num( pStr, rval ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_OBJECT:
               if( mask & SPT_CVT_FLAGS_FROM_OBJECT )
               {
                  JSObject *tmpObj = JSVAL_TO_OBJECT( val ) ;
                  BOOLEAN isSpecialObj = FALSE ;
                  const sptObjDesc *objDesc = NULL ;
                  rc = _getObjectDesc( tmpObj, isSpecialObj, &objDesc ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
                  rc = objDesc->cvtToInt( sptSPObject( _cx, tmpObj ),
                                           isSpecialObj, rval ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
         }
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getDoubleField( const std::string &fieldName,
                                      FLOAT64 &rval,
                                      INT32 mask ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else
      {
         SPT_JS_TYPE type ;
         rc = _getTypeOfVal( val, type ) ;
         if( SDB_OK != rc )
         {
            goto error ;
         }
         switch( type )
         {
            case SPT_JS_TYPE_BOOLEAN:
               if( mask & SPT_CVT_FLAGS_FROM_BOOL )
               {
                  rval = static_cast< FLOAT64 >( JSVAL_TO_BOOLEAN( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_INT:
               if( mask & SPT_CVT_FLAGS_FROM_INT )
               {
                  rval = static_cast< FLOAT64 >( JSVAL_TO_INT( val ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_DOUBLE:
               if( mask & SPT_CVT_FLAGS_FROM_DOUBLE )
               {
                  rval = JSVAL_TO_DOUBLE( val ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_STRING:
               if( mask & SPT_CVT_FLAGS_FROM_STRING )
               {
                  const CHAR* pStr = NULL ;
                  pStr = static_cast< CHAR* >
                     ( JS_EncodeString( _cx, JSVAL_TO_STRING( val ) ) ) ;
                  if( NULL == pStr )
                  {
                     rc = SDB_INVALIDARG ;
                     goto error ;
                  }
                  {
                     stringstream strConvertor( pStr ) ;
                     if( !( strConvertor >> rval ) )
                     {
                        rc = SDB_INVALIDARG ;
                        goto error ;
                     }
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_OBJECT:
               if( mask & SPT_CVT_FLAGS_FROM_OBJECT )
               {
                  JSObject *tmpObj = JSVAL_TO_OBJECT( val ) ;
                  BOOLEAN isSpecialObj = FALSE ;
                  const sptObjDesc *objDesc = NULL ;
                  rc = _getObjectDesc( tmpObj, isSpecialObj, &objDesc ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
                  rc = objDesc->cvtToDouble( sptSPObject( _cx, tmpObj ),
                                             isSpecialObj, rval ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
         }
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getStringField( const std::string &fieldName,
                                       string &rval, INT32 mask ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else
      {
         SPT_JS_TYPE type ;
         rc = _getTypeOfVal( val, type ) ;
         if( SDB_OK != rc )
         {
            goto error ;
         }
         switch( type )
         {
            case SPT_JS_TYPE_BOOLEAN:
               if( mask & SPT_CVT_FLAGS_FROM_BOOL )
               {
                  stringstream ss ;
                  ss << JSVAL_TO_BOOLEAN( val ) ;
                  rval = ss.str() ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_INT:
               if( mask & SPT_CVT_FLAGS_FROM_INT )
               {
                  stringstream ss ;
                  ss << JSVAL_TO_INT( val ) ;
                  rval = ss.str() ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_DOUBLE:
               if( mask & SPT_CVT_FLAGS_FROM_DOUBLE )
               {
                  stringstream ss ;
                  ss << JSVAL_TO_DOUBLE( val ) ;
                  rval = ss.str() ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_STRING:
               if( mask & SPT_CVT_FLAGS_FROM_STRING )
               {
                  rval = static_cast< CHAR* >
                        ( JS_EncodeString( _cx, JSVAL_TO_STRING( val ) ) ) ;
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
            case SPT_JS_TYPE_OBJECT:
               if( mask & SPT_CVT_FLAGS_FROM_OBJECT )
               {
                  JSObject *tmpObj = JSVAL_TO_OBJECT( val ) ;
                  BOOLEAN isSpecialObj = FALSE ;
                  const sptObjDesc *objDesc = NULL ;
                  rc = _getObjectDesc( tmpObj, isSpecialObj, &objDesc ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
                  if( objDesc )
                  {
                     rc = objDesc->cvtToString( sptSPObject( _cx, tmpObj ),
                                                isSpecialObj, rval ) ;
                     if( SDB_OK != rc )
                     {
                        goto error ;
                     }
                  }
                  else
                  {
                     CHAR *tmpStr = NULL ;
                     tmpStr = convertJsvalToString( _cx, val ) ;
                     if( NULL == tmpStr )
                     {
                        rc = SDB_SYS ;
                        goto error ;
                     }
                     rval = tmpStr ;
                  }
               }
               else
               {
                  rc = SDB_INVALIDARG ;
                  goto error ;
               }
               break ;
         }
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getUserObj( const sptObjDesc &objDesc,
                                  const void** value ) const
   {
      SDB_ASSERT( NULL != value, "value must not be null" ) ;
      INT32 rc = SDB_OK ;
      if( string( objDesc.getJSClassName() ) !=
            sptGetObjFactory()->getClassName( _cx, _obj ) )
      {
         rc = SDB_INVALIDARG ;
         PD_LOG( PDERROR, "JsObj className must be: %s", objDesc.getJSClassName() ) ;
         goto error ;
      }

      *value = JS_GetPrivate( _cx, _obj  ) ;
      if( *value == NULL )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Failed to convert jsobj to user obj:%d", rc ) ;
         goto error ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::toBSON( bson::BSONObj &rval, BOOLEAN strict ) const
   {
      INT32 rc = SDB_OK ;
      sptConvertor cvt( _cx, strict ) ;
      rc = cvt.toBson( _obj, rval ) ;
      if( SDB_OK != rc )
      {
         goto error ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::toString( std::string &rval ) const
   {
      INT32 rc = SDB_OK ;
      CHAR* pStr = NULL ;
      pStr = convertJsvalToString( _cx, OBJECT_TO_JSVAL( _obj ) ) ;
      if( NULL == pStr )
      {
         rc = SDB_SYS ;
         goto error ;
      }
      rval.assign( pStr ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::getFieldType( const std::string &fieldName,
                                    SPT_JS_TYPE &type ) const
   {
      INT32 rc = SDB_OK ;
      jsval val ;
      JSType jsType ;
      if( !JS_GetProperty( _cx, _obj, fieldName.c_str(), &val ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      jsType = JS_TypeOfValue( _cx, val ) ;
      switch( jsType )
      {
         case JSTYPE_OBJECT:
            type = SPT_JS_TYPE_OBJECT ;
            break ;
         case JSTYPE_FUNCTION:
            type = SPT_JS_TYPE_OBJECT ;
            break ;
         case JSTYPE_STRING:
            type = SPT_JS_TYPE_STRING ;
            break ;
         case JSTYPE_NUMBER:
            if( JSVAL_IS_INT( val ) )
            {
               type = SPT_JS_TYPE_INT ;
            }
            else
            {
               type = SPT_JS_TYPE_DOUBLE ;
            }
            break ;
         case JSTYPE_BOOLEAN:
            type = SPT_JS_TYPE_BOOLEAN ;
            break ;
         default:
            rc = SDB_INVALIDARG ;
            goto error ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   BOOLEAN sptSPObject::isFieldExist( const std::string &fieldName ) const
   {
      INT32 ret = TRUE ;
      UINT32 attr = 0 ;
      JSBool found = JS_FALSE ;

      if( !JS_GetPropertyAttributes( _cx, _obj, fieldName.c_str(), &attr,
                                     &found ) || JS_FALSE == found )
      {
         ret = FALSE ;
      }
      return ret ;
   }

   INT32 sptSPObject::getFieldNumber( UINT32 &number ) const
   {
      INT32 rc = SDB_OK ;
      JSIdArray *properties = JS_Enumerate( _cx, _obj ) ;
      if ( NULL == properties )
      {
         rc = SDB_SYS ;
         goto error ;
      }
      number = (UINT32) properties->length ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::_getTypeOfVal( jsval val, SPT_JS_TYPE &type ) const
   {
      INT32 rc = SDB_OK ;
      JSType jsType ;
      jsType = JS_TypeOfValue( _cx, val ) ;
      switch( jsType )
      {
         case JSTYPE_OBJECT:
            type = SPT_JS_TYPE_OBJECT ;
            break ;
         case JSTYPE_FUNCTION:
            type = SPT_JS_TYPE_OBJECT ;
            break ;
         case JSTYPE_STRING:
            type = SPT_JS_TYPE_STRING ;
            break ;
         case JSTYPE_NUMBER:
            if( JSVAL_IS_INT( val ) )
            {
               type = SPT_JS_TYPE_INT ;
            }
            else
            {
               type = SPT_JS_TYPE_DOUBLE ;
            }
            break ;
         case JSTYPE_BOOLEAN:
            type = SPT_JS_TYPE_BOOLEAN ;
            break ;
         default:
            rc = SDB_INVALIDARG ;
            goto error ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptSPObject::_getObjectDesc( JSObject* obj, BOOLEAN &isSpecialObj,
                                      const sptObjDesc **pDesc ) const
   {
      return sptGetObjFactory()->getObjDesc( _cx, obj, isSpecialObj, pDesc) ;
   }
}
