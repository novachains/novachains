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

#include "dmsBlockUnit.hpp"

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
UINT32  LOOPNUM     = 100000 ;
UINT32  TESTCREATETABLES = 10 ;
CHAR *  defaultCollectionName = "foo" ;
CHAR *  COLLECTIONNAME = defaultCollectionName ;
dmsBlockUnit *myUnit = NULL ;
BSONObj sampleObj = BSON("name"<<"insert block data") ;

int main ()
{

   cout << "dmsMetadataBlock size is " << sizeof(dmsMetadataBlock) <<endl ;
   INT32 rc = 0 ;
   UINT32 pages = rand()%99+1 ;
   myUnit = new dmsBlockUnit( COLLECTIONNAME ,1, 0 ) ;

   if ( !myUnit )
   {
      printf ("Failed to allocate memory for myUnit\n" );
      return 0 ;
   }
   rc = myUnit->open( "./") ;
   if ( rc )
   {
      printf("Failed to open SU, rc = %d\n", rc );
      return 0 ;
   }

   cout<< "open success" << endl;
   rc = myUnit->data()->addCollection( COLLECTIONNAME, NULL,
                                       UTIL_UNIQUEID_NULL, 0, NULL,
                                       pages ) ;
   if ( rc )
   {
      printf("Failed to create collection, rc=%d\n", rc ) ;
   }
   cout<< "add collection success" << endl;

   for(unsigned int i=0; i<LOOPNUM; i++)
   {
      rc = myUnit->insertRecord( COLLECTIONNAME, sampleObj,
                              NULL, FALSE ) ;
      if ( rc )
      {
         printf("Failed to append record, rc=%d\n", rc);
         return 0 ;
      }
   }
   cout<< "insert success" << endl;

}
