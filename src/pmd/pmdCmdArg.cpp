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

   Source File Name = pmdCmdArg.cpp

   Descriptive Name = Process MoDel

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains kernel control block object.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdCmdArg.hpp"
#include "pmdTrace.hpp"
#include "pd.hpp"

namespace engine
{

   /*
      _pmdCmdArg implement
   */
   _pmdCmdArg::_pmdCmdArg()
   {
   }

   _pmdCmdArg::~_pmdCmdArg()
   {
   }

   INT32 _pmdCmdArg::getArgc() const
   {
      return _argc ;
   }

   const CHAR** _pmdCmdArg::getArgv() const
   {
      return _argv ;
   }

   const CHAR* _pmdCmdArg::getCmdLine() const
   {
      return _cmdstr.c_str() ;
   }

   void _pmdCmdArg::init( INT32 argc, const CHAR *argv[] )
   {
      _argc = argc ;
      _argv = argv ;
      _cmdstr.clear() ;

      for ( INT32 i = 0 ; i < argc ; ++i )
      {
         if ( i != 0 )
         {
            _cmdstr += " " ;
         }
         _cmdstr += argv[ 0 ] ;
      }
   }

}

