/*******************************************************************************

   Copyright (C) 2011-2018 NOVACHAIN Ltd.

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
/* FVT test program for dms component */
#include <string>
#include "dmsBlockUnit.hpp"
#include "dmsScanner.hpp"

#define DFT_FILENAME "testDMS.dat"
#define DFT_SEGSIZE  1024*1024
#define DFT_NUMSEG   50

#define MAXTHREADS 1024

#define BIG_COLLECTION_NAME "big"
#define SMALL_COLLECTION_NAME "small"
using namespace engine ;
using namespace bson ;
using namespace std ;

UINT32  TESTTHREADS = 10 ;
UINT32  LOOPNUM     = 10000 ;
UINT32  TESTCREATETABLES = 10 ;
CHAR *  defaultCollectionName = "foo" ;
CHAR *  COLLECTIONNAME = defaultCollectionName ;
dmsRecordGenerator generator ;
dmsRecordID recordID ;
ossValuePtr recordDataPtr = 0 ;

BSONObj sampleObj = BSON("name"<<"insert block data") ;

class processor
{
   public:
      processor() ;
      ~processor() ;

      INT32 prepare() ;
      INT32 insert() ;
      INT32 query() ;
      void run() ;
   private:
      dmsBlockUnit *pSu ;
} ;

processor::processor()
{
   pSu = NULL ;
}

processor::~processor()
{
   if ( pSu )
   {
      SDB_OSS_DEL pSu ;
   }
}

INT32 processor::prepare()
{
   INT32 rc = 0 ;
   UINT32 pages = rand()%99+1 ;
   CHAR * errorMsg ;
   if (!pSu)
   {
      pSu = SDB_OSS_NEW dmsBlockUnit( COLLECTIONNAME ,1, 0 ) ;

      if ( pSu )
      {
         rc = pSu->open( "./") ;
         if ( !rc )
         {
            rc = pSu->data()->addCollection( COLLECTIONNAME, NULL,
                                             UTIL_UNIQUEID_NULL, 0, NULL,
                                             pages ) ;
            if ( rc )
            {
               errorMsg = "Failed to create collection\n" ;
            }
         }
         else
         {
            errorMsg = "Failed to open storage\n" ;
         }  
      }
      else
      {
         errorMsg = "Failed to allocate memory for pSu\n" ;
         rc = -1 ;
      }
   }

   if (rc)
   {
      cout<<errorMsg<<endl ;
   }
   else
   {
      cout<< "storage unit open succeed, now you can insert" <<endl ;
   }

   return rc ;
}

INT32 processor::insert()
{
   INT32 rc = 0 ;
   if( pSu )
   {
      rc = pSu->insertRecord( COLLECTIONNAME, sampleObj,
                              NULL, FALSE ) ;
      if ( rc )
      {
         cout<<"Failed to insert record, rc="<<rc<<endl ;
      }
      else
      {
         cout<<"insert succeed"<<endl ;
      }
   }
   else
   {
      cout<<"no storage unit exists, please do prepare first"<<endl ;
   }
   return rc ;
}

INT32 processor::query()
{
   INT32 rc = 0 ;
   dmsExtScanner *pScanner = NULL;
   dmsMBContext *pContext = NULL ;

   rc = pSu->data()->getMBContext( &pContext, COLLECTIONNAME, SHARED ) ;
   if (!rc)
   {
      pScanner = SDB_OSS_NEW dmsExtScanner( pSu->data(), pContext, pContext->mb()->_firstExtentID ) ;
      if ( !pScanner )
      {
         cout<<"failed to allocate canner"<<endl ;
         rc = -1 ;
      }
   }
   else
   {
      cout<<"failed to get mbContext"<<endl;
   }

   if ( !rc )
   {
      while ( SDB_OK == ( rc = pScanner->advance( recordID, generator, NULL ) ) )
      {
         generator.getDataPtr( recordDataPtr ) ;
         BSONObj obj( (const CHAR*)recordDataPtr ) ;
         cout<< "data is: " << obj.toString().c_str()<< endl;
      }
      if ( SDB_DMS_EOC != rc )
      {
         cout<<"extent scan failed and rc is "<<rc<<endl ;
      }
      else
      {
         rc = 0 ;
         cout<<"query finished"<< endl ;
      }
   }

   if ( pContext )
   {
      pSu->data()->releaseMBContext(pContext) ;
   }

   if ( pScanner )
   {
      SDB_OSS_DEL pScanner ;
   }

   return rc ;
}

void processor::run()
{
   INT32 rc = 0 ;
   char request[10] ;


   while (true)
   {
      cin.get(request, 10).get() ;

        if (!strcmp(request, "prepare"))
        {
           rc = prepare() ;
        }
        else if (!strcmp(request, "insert"))
        {
           rc = insert() ;
        }
        else if (!strcmp(request, "query"))
        {
           rc = query() ;
        }
        else if (!strcmp(request, "quit"))
        {
           cout<<"terminate program"<<endl ;
           break ;
        }
        else
        {
           cout<<"invalid request"<<endl ;
           rc = -1 ;
        }

     if (rc)
     {
        break ;
     }
   }
}

int main()
{
   processor pro ;
   pro.run() ;
}
