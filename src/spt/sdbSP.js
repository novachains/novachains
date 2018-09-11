/*******************************************************************************
   Copyright (C) 2011-2018 SequoiaDB Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*******************************************************************************/
// File open option define
const SDB_FILE_CREATEONLY =    0x00000001 ;
const SDB_FILE_REPLACE =       0x00000002 ;
const SDB_FILE_CREATE =        SDB_FILE_CREATEONLY | SDB_FILE_REPLACE ;
const SDB_FILE_SHAREREAD =     0x00000010 ;
const SDB_FILE_SHAREWROTE =    SDB_FILE_SHAREREAD | 0x00000020 ;
const SDB_FILE_READONLY =      0x00000004 | SDB_FILE_SHAREREAD ;
const SDB_FILE_WRITEONLY =     0x00000008 ;
const SDB_FILE_READWRITE =     0x00000004 | SDB_FILE_WRITEONLY ;
var SDB_PRINT_JSON_FORMAT        = true ;

function jsonFormat(pretty) {
   if (pretty == undefined){
      pretty = true;
   }
   SDB_PRINT_JSON_FORMAT = pretty;
}

function println ( val ) {
   if ( arguments.length > 0 )
      print ( val ) ;
   print ( '\n' ) ;
}
// return a double number between 0 and 1
function rand () {
   return Math.random() ;
}

// BSONObj
BSONObj.prototype.toObj = function() {
   return JSON.parse( this.toJson() ) ;
}

BSONObj.prototype.toString = function() {
   if ( typeof(SDB_PRINT_JSON_FORMAT) == "undefined" ||
        SDB_PRINT_JSON_FORMAT )
   {
      try
      {
         var obj = this.toObj();
         var str = JSON.stringify ( obj, undefined, 2 ) ;
         return str ;
      }
      catch ( e )
      {
         return this.toJson() ;
      }
   }
   else
   {
      return this.toJson() ;
   }
}
// end BSONObj

// BSONArray
BSONArray.prototype.toArray = function() {
   if ( this._arr )
      return this._arr;

   var a = [];
   while ( true ) {
      var bs = this.next();
      if ( ! bs ) break ;
      var json = bs.toJson () ;
      try
      {
         var stf = JSON.stringify(JSON.parse(json), undefined, 2) ;
         a.push ( stf ) ;
      }
      catch ( e )
      {
         a.push ( json ) ;
      }
   }
   this._arr = a ;
   return this._arr ;
}

BSONArray.prototype.arrayAccess = function( idx ) {
   return this.toArray()[idx] ;
}

BSONArray.prototype.toString = function() {
   //return this.toArray().join('\n') ;
   var array = this ;
   var record = undefined ;
   var returnRecordNum = 0 ;
   while ( ( record = array.next() ) != undefined )
   {
      returnRecordNum++ ;
      try
      {
         println ( record ) ;
      }
      catch ( e )
      {
         var json = record.toJson () ;
         println ( json ) ;
      }
   }
   println("Return "+returnRecordNum+" row(s).") ;
   return "" ;
}

BSONArray.prototype._formatStr = function() {

   var bsonObj = this.toArray() ;
   var objArr = new Array() ;
   var eleArr = new Array() ;
   var maxSizeArr = new Array() ;
   var outStr = "" ;
   var colNameArr = new Array() ;
   var objNum ;

   for ( var i in bsonObj )
   {
      objArr.push( JSON.parse( bsonObj[i] ) ) ;
   }

   var objNum = objArr.length ;

   if ( objNum > 0 )
   {
      // get row name and init maxSizeArr
      for( var index in objArr )
      {
         for ( var eleKey in objArr[ index ] )
         {
            if ( -1 == colNameArr.indexOf( eleKey ) )
            {
               colNameArr.push( eleKey ) ;
               maxSizeArr[ eleKey ] = eleKey.length ;
            }
         }
      }

      for ( var index in objArr )
      {
         var localArr = new Array() ;
         for( var ele in objArr[ index ] )
         {
            var localEle = objArr[ index ][ ele ].toString() ;
            localArr[ ele ] = localEle ;
            if ( maxSizeArr[ ele ] < localEle.length )
            {
               maxSizeArr[ ele ] = localEle.length ;
            }
         }
         eleArr.push( localArr ) ;
      }

      for( var index in maxSizeArr )
      {
         maxSizeArr[ index ] = maxSizeArr[ index ] + 1 ;
      }
      for ( var index in colNameArr )
      {
         var localRowName = colNameArr[ index ] ;
         outStr += " " + localRowName ;
         for ( var k = 0; k < maxSizeArr[ localRowName ] - localRowName.length ;
               k++ )
         {
            outStr += " " ;
         }
      }
      outStr += "\n" ;

      for ( var index in eleArr )
      {
         var arr = eleArr[ index ] ;
         for ( var ele in colNameArr )
         {
            var localRowName = colNameArr[ ele ] ;
            var localEle = arr[ localRowName ] ;
            if ( undefined == localEle )
            {
               localEle = "" ;
            }
            outStr += " " + localEle ;
            for ( var k = 0; k < maxSizeArr[ localRowName ] - localEle.length;
                  k++ )
            {
               outStr += " " ;
            }
         }
         outStr += "\n" ;
      }
   }
   return outStr ;
}
// end BSONArray


// end File
