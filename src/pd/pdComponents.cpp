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

   Source File Name = pdComponents.cpp

   Descriptive Name = Problem Determination

   When/how to use: this program may be used on binary and text-formatted
   versions of PD component. This file contains functions for components

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#include "core.hpp"
#include "pdTrace.hpp"

// Whenever we add a new component, we have to modify
// 1) pdTrace.hpp ( the component list )
// 2) pdComponents.cpp ( component list string )
// 4) add a new directory with EXACT same component name
// 5) create <comp>Trace.hpp which will include <comp>Trace.h
// There will be <comp>Trace.h automatically created in include directory
// Note there are max 32 components can be added into list
const CHAR *_pdTraceComponentDir[] = {
   "msg",    // PD_TRACE_COMPONENT_MSG
   "net",    // PD_TRACE_COMPONENT_NET
   "oss",    // PD_TRACE_COMPONENT_OSS
   "pd",     // PD_TRACE_COMPONENT_PD
   "tools",  // PD_TRACE_COMPONENT_TOOL
   "mon",    // PD_TRACE_COMPONENT_MON
   "pmd",    // PD_TRACE_COMPONENT_PMD
   "rest",   // PD_TRACE_COMPONENT_REST
   "spt",    // PD_TRACE_COMPONENT_SPT
   "util",   // PD_TRACE_COMPONENT_UTIL
   "spd",    // PD_TRACE_COMPONENT_SPD
} ;

UINT32 pdGetCompoentSize()
{
   return sizeof( _pdTraceComponentDir ) / sizeof( const CHAR* ) ;
}

const CHAR *pdGetTraceComponent ( UINT32 id )
{
   if ( (INT32)id >= pdGetCompoentSize() )
   {
      return NULL ;
   }
   return _pdTraceComponentDir[id] ;
}


