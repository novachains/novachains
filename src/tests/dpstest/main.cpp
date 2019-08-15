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
*******************************************************************************/
/* FVT test program for dps component */
#include <string>
#include "dps.hpp"

#define DFT_FILENAME "testDPS.log"
#define DFT_SEGSIZE  1024*1024
#define DFT_NUMSEG   50

#define MAXTHREADS 1024

using namespace engine ;
using namespace bson ;
using namespace std ;

UINT32  TESTTHREADS = 10 ;
UINT32  LOOPNUM     = 10000 ;
UINT32  TESTCREATETABLES = 10 ;
CHAR *  defaultCollectionName = "foo" ;
dpsCB * cb ;
pmdEDUCB * edu ;
//dpsMergeInfo info ;
//dpsLogRecord &logRecord = info.getMergeBlock().record() ;
UINT32 clLID = 1 ;

BSONObj sampleObj = BSON("name"<<"insert log data") ;

void flushDisk( dpsLogMgr *logMgr )
{
   int rc = 0 ;
   while (TRUE)
   {
      rc = logMgr->run(edu) ;
      if (rc)
      {
         cout<<"flush disk failed"<<endl ;
      }
   }
}

int main()
{
  INT32 rc = 0;
  int i = 0 ;
  cb = SDB_OSS_NEW dpsCB() ;

  dpsLogMgr *mgr = cb->mgr() ;
//  boost::thread thread(boost::bind(&flushDisk, mgr)) ;

  cb->init("./", 64 ) ;
  if (rc)
  {
     cout<<"log file init fail"<<endl ;
     goto done ;
  }

  while ( i < 1000 )
  {
     dpsMergeInfo info ;
     dpsLogRecord &logRecord = info.getMergeBlock().record() ;
     rc = dpsInsert2Record( defaultCollectionName, sampleObj, logRecord) ;
     if (rc)
     {
        cout<<"log creation fail"<<endl ;
        goto done ;
     }

     rc = cb->writeLog( info, edu, 1, NULL, 1, FALSE, 1, &clLID ) ;
     if (rc)
     {
        cout<<"write log failed"<<endl ;
        goto done ;
     }
   i++ ;
   }
   mgr->run(edu) ;

//  thread.join() ;

done:
  return 0 ;

}
