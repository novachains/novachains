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

   Source File Name = ossMem.hpp

   Descriptive Name = Operating System Services Memory Header

   When/how to use: this program may be used on binary and text-formatted
   versions of OSS component. This file contains declares for all memory
   allocation/free operations.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2012  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef OSSMEM_HPP_
#define OSSMEM_HPP_
#include "ossMem.h"
#include <new>

#define SDB_OSS_MEMDUMPNAME      "memdump.info"
#define SDB_OSS_NEW              new(__FILE__,__LINE__,std::nothrow)
#define SDB_OSS_DEL              delete

#define SAFE_OSS_DELETE(p) \
   do {                    \
      if (p) {             \
         SDB_OSS_DEL p ;   \
         p = NULL ;        \
      }                    \
   } while (0)

/*
void* operator new (size_t size, const CHAR *file, UINT32 line ) ;

void operator delete ( void *p, const CHAR *file, UINT32 line ) ;*/
#endif

