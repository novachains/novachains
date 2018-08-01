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

   Source File Name = sptConvertor.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of Script component. This file contains structures for javascript
   engine wrapper

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  YW Initial Draft

   Last Changed =

*******************************************************************************/

#include "ossUtil.hpp"
#include "sptConvertor.hpp"
#include "pd.hpp"
#include "ossMem.hpp"
//#include "sptConvertorHelper.hpp"
#include "sptObjDesc.hpp"
#include "sptSPObject.hpp"
#include <typeinfo>
using namespace bson ;
using namespace engine ;

#define SPT_CONVERTOR_SPE_OBJSTART  '$'
#define SPT_AGGREGATE_MATCHER       "$match"

namespace engine
{
   INT32 sptConvertor::toBson( JSObject *obj , bson::BSONObj &bsobj )
   {
      INT32 rc = SDB_OK ;
      BSONObjBuilder builder ;
      SDB_ASSERT( NULL != obj, "can not be NULL" ) ;

      _hasSetErrMsg = FALSE ;
      rc = _traverse( obj, builder ) ;
      if ( SDB_OK != rc )
      {
         _setErrMsg( "Failed to call traverse", FALSE ) ;
         goto error ;
      }
      bsobj = builder.obj() ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::toObjArray( JSObject *obj, vector< BSONObj > &bsArray )
   {
      INT32 rc = SDB_OK ;
      UINT32 length = 0 ;
      vector< BSONObj > tmpVec ;
      if ( NULL == obj )
      {
         goto done ;
      }
      if( !JS_IsArrayObject( _cx, obj ) )
      {
         rc = SDB_INVALIDARG ;
         _setErrMsg( "Object is not an array obj", FALSE ) ;
         goto error ;
      }
      if( !JS_GetArrayLength( _cx, obj, &length ) )
      {
         rc = SDB_INVALIDARG ;
         _setErrMsg( "Failed to get array length", FALSE ) ;
         goto error ;
      }

      for ( UINT32 i = 0; i < length; i++ )
      {
         BSONObj tmpObj ;
         JSObject *eleObj = NULL ;
         jsval val ;
         if( !JS_GetElement( _cx, obj, i, &val ) )
         {
            rc = SDB_INVALIDARG ;
            _setErrMsg( "Failed to get array element", FALSE ) ;
            goto error ;
         }
         if( JSTYPE_OBJECT != JS_TypeOfValue( _cx, val ) )
         {
            rc = SDB_INVALIDARG ;
            _setErrMsg( "Invalid element type", FALSE ) ;
            goto error ;
         }
         eleObj = JSVAL_TO_OBJECT( val ) ;
         rc = toBson( eleObj, tmpObj ) ;
         if( SDB_OK != rc )
         {
            goto error ;
         }
         tmpVec.push_back( tmpObj ) ;
      }
      bsArray.swap( tmpVec ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::_traverse( JSObject *obj , BSONObjBuilder &builder )
   {
      INT32 rc = SDB_OK ;
      JSIdArray *properties = NULL ;
      if ( NULL == obj )
      {
         goto done ;
      }

      properties = JS_Enumerate( _cx, obj ) ;
      if ( NULL == properties )
      {
         _setErrMsg( "System error", FALSE ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      for ( jsint i = 0; i < properties->length; i++ )
      {
         jsid id = properties->vector[i] ;
         jsval fieldName, fieldValue ;
         std::string name ;
         if ( !JS_IdToValue( _cx, id, &fieldName ) )
         {
            _setErrMsg( "Failed to get object field name", FALSE ) ;
            rc = SDB_INVALIDARG ;
            goto error ;
         }

         rc = _toString( fieldName, name ) ;
         if ( SDB_OK != rc )
         {
            _setErrMsg( "Failed to conversion field name", FALSE ) ;
            goto error ;
         }

         if ( !JS_GetProperty( _cx, obj, name.c_str(), &fieldValue ) )
         {
            _setErrMsg( "Failed to get object field value", FALSE ) ;
            rc = SDB_INVALIDARG ;
            goto error ;
         }

         rc = _appendToBson( name, fieldValue, builder ) ;
         if ( SDB_OK != rc )
         {
            _setErrMsg( "Failed to call appendToBson", FALSE ) ;
            goto error ;
         }
      }
   done:
      if ( properties )
      {
         JS_DestroyIdArray( _cx, properties ) ;
      }
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::_appendToBson( const std::string &name,
                                      const jsval &val,
                                      BSONObjBuilder &builder )
   {
      INT32 rc = SDB_OK ;
      switch ( JS_TypeOfValue( _cx, val ) )
      {
         case JSTYPE_VOID :
         {
            builder.appendUndefined( name ) ;
            break ;
         }
         case JSTYPE_NULL :
         {
            builder.appendNull( name ) ;
            break ;
         }
         case JSTYPE_NUMBER :
         {
            if ( JSVAL_IS_INT( val ) )
            {
               INT32 iN = 0 ;
               rc = _toInt( val, iN ) ;
               if ( SDB_OK != rc )
               {
                  _setErrMsg( "Failed to conversion int", FALSE ) ;
                  goto error ;
               }
               builder.append( name, iN ) ;
            }
            else
            {
               FLOAT64 fV = 0 ;
               rc = _toDouble( val, fV ) ;
               if ( SDB_OK != rc )
               {
                  _setErrMsg( "Failed to conversion double", FALSE ) ;
                  goto error ;
               }
               builder.append( name, fV ) ;
            }
            break ;
         }
         case JSTYPE_STRING :
         {
            std::string str ;
            rc = _toString( val, str ) ;
            if ( SDB_OK != rc )
            {
               _setErrMsg( "Failed to conversion string", FALSE ) ;
               goto error ;
            }
            builder.append( name, str ) ;
            break ;
         }
         case JSTYPE_BOOLEAN :
         {
            BOOLEAN bL = TRUE ;
            rc = _toBoolean( val, bL ) ;
            if ( SDB_OK != rc )
            {
               _setErrMsg( "Failed to conversion boolean", FALSE ) ;
               goto error ;
            }
            builder.appendBool( name, bL ) ;
            break ;
         }
         case JSTYPE_OBJECT :
         {
            if ( JSVAL_IS_NULL( val ) )
            {
               builder.appendNull( name ) ;
            }
            else
            {
               JSObject *obj = JSVAL_TO_OBJECT( val ) ;
               if ( NULL == obj )
               {
                  builder.appendNull( name ) ;
               }
               else
               {
                  BOOLEAN setMatcher = FALSE ;
                  const sptObjDesc *desc = NULL ;
                  BOOLEAN isSpecialObj = FALSE ;
                  rc = sptGetObjFactory()->getObjDesc( _cx, obj, isSpecialObj,
                                                       &desc ) ;
                  if( SDB_OK != rc )
                  {
                     goto error ;
                  }
                  if( desc )
                  {
                     string errMsg ;
                     rc = desc->cvtToBSON( name.c_str(), sptSPObject( _cx, obj ),
                                           isSpecialObj, builder, errMsg ) ;
                     if( SDB_OK == rc )
                     {
                        break ;
                     }
                     else if( SDB_SPT_NOT_SPECIAL_JSON == rc )
                     {
                        // $match could set strict = FALSE to ignore some errors
                        if ( TRUE == _strict )
                        {
                           rc = SDB_INVALIDARG ;
                           goto error ;
                        }
                     }
                     else
                     {
                        _setErrMsg( errMsg, FALSE ) ;
                        goto error ;
                     }
                  }
                  // Append as normal js obj or array
                  {
                     BSONObj tmpbs ;
                     rc = toBson( obj, tmpbs ) ;
                     if( SDB_OK != rc )
                     {
                        _setErrMsg( "Failed to convert js obj to BSONObj",
                                    FALSE ) ;
                        goto error ;
                     }
                     if ( setMatcher )
                     {
                        _inMatcher = FALSE ;
                     }
                     if( JS_IsArrayObject( _cx, obj ) )
                     {
                        builder.appendArray( name, tmpbs ) ;
                     }
                     else
                     {
                        builder.append( name, tmpbs ) ;
                     }
                  }
               }
            }
            break ;
         }
         case JSTYPE_FUNCTION :
         {
            std::string str ;
            rc = _toString( val, str ) ;
            if ( SDB_OK != rc )
            {
               _setErrMsg( "Failed to conversion function", FALSE ) ;
               goto error ;
            }
            builder.appendCode( name, str ) ;
            break ;
         }
         default :
         {
            _setErrMsg( "unexpected type", FALSE ) ;
            SDB_ASSERT( FALSE, "unexpected type" ) ;
            rc = SDB_INVALIDARG ;
            goto error ;
         }
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::_toString( const jsval &val,
                                  std::string &str )
   {
      return toString( _cx, val, str ) ;
   }

   INT32 sptConvertor::_toInt( const jsval &val, INT32 &iN )
   {
      INT32 rc = SDB_OK ;
      int32 ip = 0 ;
      if ( !JS_ValueToInt32( _cx, val, &ip ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      iN = ip ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::_toDouble( const jsval &val, FLOAT64 &fV )
   {
      INT32 rc = SDB_OK ;
      jsdouble dp = 0 ;
      if ( !JS_ValueToNumber( _cx, val, &dp ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      fV = dp ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::_toBoolean( const jsval &val, BOOLEAN &bL )
   {
      INT32 rc = SDB_OK ;
      JSBool bp = TRUE ;
      if ( !JS_ValueToBoolean( _cx, val, &bp ) )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      bL = bp ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 sptConvertor::toString( JSContext *cx,
                                  const jsval &val,
                                  std::string &str )
   {
      INT32 rc = SDB_OK ;
      //CHAR *utf8 = NULL ;
      SDB_ASSERT( NULL != cx, "impossible" ) ;
      size_t len = 0 ;
      JSString *jsStr = JS_ValueToString( cx, val ) ;
      if ( NULL == jsStr )
      {
         goto done ;
      }
      len = JS_GetStringLength( jsStr ) ;
      if ( 0 == len )
      {
         goto done ;
      }
      else
      {
         CHAR *p = JS_EncodeString ( cx , jsStr ) ;
         if ( NULL != p )
         {
            str.assign( p ) ;
            SAFE_JS_FREE( cx, p ) ;
         }
      }
   done:
      return rc ;
   }

   void sptConvertor::_setErrMsg( const string &msg, BOOLEAN isReplace )
   {
      if( _hasSetErrMsg == TRUE && isReplace == FALSE )
      {
         return ;
      }
      _hasSetErrMsg = TRUE ;
      _errMsg = msg ;
   }
}
