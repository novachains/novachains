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

   Source File Name = sptBsonobjArray.hpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef SPT_BSONOBJ_ARRAY_HPP_
#define SPT_BSONOBJ_ARRAY_HPP_

#include "sptApi.hpp"
#include "../bson/bson.hpp"
#include <vector>

using namespace bson ;
using namespace std ;

namespace engine
{

   /*
      _sptBsonobjArray define
   */
   class _sptBsonobjArray : public SDBObject
   {
   JS_DECLARE_CLASS( _sptBsonobjArray )

   public:
      _sptBsonobjArray() ;
      _sptBsonobjArray( const vector< BSONObj > &vecObjs ) ;
      virtual ~_sptBsonobjArray() ;

      const vector< BSONObj >& getBsonArray() const { return _vecObj ; }

   public:

      static INT32 help( const _sptArguments &arg,
                         _sptReturnVal &rval,
                         bson::BSONObj &detail ) ;

      INT32 construct( const _sptArguments &arg,
                       _sptReturnVal &rval,
                       bson::BSONObj &detail ) ;
      INT32 destruct() ;

      INT32 size( const _sptArguments &arg,
                  _sptReturnVal &rval,
                  bson::BSONObj &detail ) ;

      INT32 more( const _sptArguments &arg,
                  _sptReturnVal &rval,
                  bson::BSONObj &detail ) ;

      INT32 next( const _sptArguments &arg,
                  _sptReturnVal &rval,
                  bson::BSONObj &detail ) ;

      INT32 pos( const _sptArguments &arg,
                 _sptReturnVal &rval,
                 bson::BSONObj &detail ) ;

      INT32 getIndex( const _sptArguments &arg,
                      _sptReturnVal &rval,
                      bson::BSONObj &detail ) ;

      INT32 resolve( const _sptArguments &arg,
                     UINT32 opcode,
                     BOOLEAN &processed,
                     string &callFunc,
                     BOOLEAN &setIDProp,
                     _sptReturnVal &rval,
                     bson::BSONObj &detail ) ;

   private:
      vector< BSONObj >          _vecObj ;
      UINT32                     _curPos ;

   } ;
   typedef _sptBsonobjArray sptBsonobjArray ;
}

#endif // SPT_BSONOBJ_ARRAY_HPP_

