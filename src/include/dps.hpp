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

   Source File Name = dps.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-motionatted
   versions of DPS component. This file contains implement of DPS functions.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who    Description
   ====== =========== =====  ==============================================
          08/05/2019  Jiaqi  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef DPS_HPP_
#define DPS_HPP_

#include <queue>
#include "oss.hpp"
#include "ossAtomic.hpp"
#include "dpsDef.hpp"
#include "ossLatch.hpp"
#include "dms.hpp"
#include "dmsBlockData.hpp"
#include "dpsLogRecord.hpp"
#include "dpsMergeBlock.hpp"
#include "dpsLogMgr.hpp"
#include "sdbInterface.hpp"
#include "ossEvent.hpp"
#include "ossMemPool.hpp"
#include "pmdEDU.hpp"

using namespace bson ;

namespace engine
{
   class _pmdEDUCB ;

   class _dpsCB : public SDBObject
   {
      public:
         _dpsCB() ;
         ~_dpsCB() ;

      public:

         dpsLogMgr * mgr(){ return _pLogMgr ; }

         INT32 init( const CHAR *path, UINT32 pageNum ) ;

         INT32 writeLog( dpsMergeInfo &info,
                         pmdEDUCB *cb,
                         UINT32 csID,
                         dmsMBContext *context,
                         dmsExtentID extLID,
                         BOOLEAN needUnLock,
                         DMS_FILE_TYPE type,
                         UINT32 *clLID ) ;

         INT32 writeLog( const CHAR *fullName,
                         const BSONObj &obj) ;

      private:
         dpsLogMgr         *_pLogMgr ;
         
   } ;
   typedef _dpsCB dpsCB ;


   /*
    DPS TOOL FUNCTIONS
   */

   INT32 checkAndAddTimeInfo( dpsLogRecord &record ) ;

   INT32 dpsInsert2Record( const CHAR *fullName,
                           const BSONObj &obj,
                           dpsLogRecord &record ) ;

}

#endif // DPS_HPP_
