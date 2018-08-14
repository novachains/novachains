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

   Source File Name = utilCommon.hpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef UTILCOMMON_HPP_
#define UTILCOMMON_HPP_

#include "core.hpp"
#include "msgDef.h"
#include "pmdDef.hpp"
#include "utilStr.hpp"
#include "pmdInterface.hpp"

#include "../bson/bson.h"

using namespace bson ;

namespace engine
{

   /*
      PMD_NODE_ROLE ENUM AND STRING TRANSFER
   */
   PMD_NODE_ROLE utilGetRoleEnum( const CHAR *role ) ;
   const CHAR* utilRole2Str( PMD_NODE_ROLE role ) ;

   const CHAR* utilRoleShortStr( PMD_NODE_ROLE role ) ;
   PMD_NODE_ROLE utilShortStr2Role( const CHAR *role ) ;

   /*
      PMD_NODE_TYPE ENUM AND STRING TRANSFER
   */
   PMD_NODE_TYPE utilGetNodeTypeEnum( const CHAR *type ) ;
   const CHAR* utilNodeType2Str( PMD_NODE_TYPE type ) ;

   PMD_NODE_TYPE utilRoleToType( PMD_NODE_ROLE role ) ;

   /*
      PMD_NODE_STATUS AND STRING TRANSFER
   */
   const CHAR* utilNodeStatusStr( PMD_NODE_STATUS status ) ;
   PMD_NODE_STATUS utilGetNodeStatusEnum( const CHAR *status ) ;

   /*
      PMD_NODE_STATUS AND STRING TRANSFER
   */
   const CHAR* utilDataStatusStr( BOOLEAN dataIsOK, PMD_NODE_STATUS status ) ;

   /*
      PMD_NODE_MODE AND STRING TRANSFER
   */
   string      utilNodeMode2Str( UINT32 mode ) ;
   UINT32      utilGetNodeModeFlag( const string &mode ) ;

   /*
      util get error bson
   */
   BSONObj        utilGetErrorBson( INT32 flags, const CHAR *detail ) ;

   /*
      util rc to shell return code
   */
   UINT32         utilRC2ShellRC( INT32 rc ) ;
   INT32          utilShellRC2RC( UINT32 src ) ;

}



#endif //UTILCOMMON_HPP_

