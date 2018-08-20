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

   Source File Name = utilParam.hpp

   Descriptive Name =

   When/how to use: str util

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

******************************************************************************/

#ifndef UTILPARAM_HPP__
#define UTILPARAM_HPP__

#include "core.hpp"
#include <string>
#include "ossProc.hpp"
#include "ossUtil.h"
#include "utilStr.hpp"

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options ;
using namespace std ;

namespace engine
{

   INT32 utilReadConfigureFile( const CHAR *file,
                                const po::options_description &desc,
                                po::variables_map &vm,
                                BOOLEAN allowUnReg = TRUE ) ;

   INT32 utilReadCommandLine( INT32 argc, CHAR **argv,
                              const po::options_description &desc,
                              po::variables_map &vm,
                              BOOLEAN allowUnreg = TRUE ) ;

   INT32 utilWriteConfigFile( const CHAR * pFile,
                              const CHAR * pData,
                              BOOLEAN createOnly = FALSE ) ;

   INT32 utilGetServiceByConfigPath( const string& confPath,
                                     string &svcname,
                                     const string &defaultName,
                                     BOOLEAN allowFileNotExist = TRUE ) ;

   INT32 utilGetRoleByConfigPath( const string& confPath,
                                  INT32 &role,
                                  BOOLEAN allowFileNotExist = TRUE ) ;

   INT32 utilGetDBPathByConfigPath( const string& confPath,
                                    string &dbPath,
                                    BOOLEAN allowFileNotExist = TRUE ) ;

   INT32 utilSetAndCheckUlimit() ;

}

#endif // UTILPARAM_HPP__

