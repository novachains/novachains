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

   Source File Name = utilCommon.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/07/2018  TW  Initial Draft

   Last Changed =

*******************************************************************************/

#include "utilCommon.hpp"
#include "ossUtil.hpp"
#include "ossLatch.hpp"
#include "pdTrace.hpp"
#include "utilTrace.hpp"
#include <sstream>

using namespace bson ;

namespace engine
{

   PMD_NODE_ROLE utilGetRoleEnum( const CHAR *role )
   {
      if ( NULL == role )
         return PMD_NODE_ROLE_MAX;
      else if ( 0 == ossStrcasecmp( role, PMD_NODE_WITNESS_STR ) )
         return PMD_NODE_WITNESS ;
      else if ( 0 == ossStrcasecmp( role, PMD_NODE_FULL_STR ) )
         return PMD_NODE_FULL ;
      else if ( 0 == ossStrcasecmp( role, PMD_NODE_USER_STR ) )
         return PMD_NODE_USER ;
      else if ( 0 == ossStrcasecmp( role, PMD_NODE_SPV_STR ) )
         return PMD_NODE_SPV ;
      else
         return PMD_NODE_ROLE_MAX;
   }

   const CHAR* utilRole2Str( PMD_NODE_ROLE role )
   {
      switch ( role )
      {
         case PMD_NODE_WITNESS :
            return PMD_NODE_WITNESS_STR ;
         case PMD_NODE_FULL :
            return PMD_NODE_FULL_STR ;
         case PMD_NODE_USER :
            return PMD_NODE_USER_STR ;
         case PMD_NODE_SPV :
            return PMD_NODE_SPV_STR ;
         default :
            break ;
      }
      return "" ;
   }

   const CHAR* utilRoleShortStr( PMD_NODE_ROLE role )
   {
      switch ( role )
      {
         case PMD_NODE_WITNESS :
            return "W" ;
         case PMD_NODE_FULL :
            return "F" ;
         case PMD_NODE_USER :
            return "U" ;
         case PMD_NODE_SPV :
            return "S" ;
         default :
            break ;
      }
      return "" ;
   }

   PMD_NODE_ROLE utilShortStr2Role( const CHAR * role )
   {
      if ( NULL == role )
         return PMD_NODE_ROLE_MAX;
      if ( 0 == ossStrcasecmp( role, "W" ) )
         return PMD_NODE_WITNESS ;
      else if ( 0 == ossStrcasecmp( role, "F" ) )
         return PMD_NODE_FULL ;
      else if ( 0 == ossStrcasecmp( role, "U" ) )
         return PMD_NODE_USER ;
      else if ( 0 == ossStrcasecmp( role, "S" ) )
         return PMD_NODE_SPV ;
      else
         return PMD_NODE_ROLE_MAX;
   }

   PMD_NODE_TYPE utilGetNodeTypeEnum( const CHAR * type )
   {
      if ( NULL == type )
      {
         return PMD_NODE_TYPE_MAX ;
      }
      else if ( 0 == ossStrcasecmp( type, PMD_TYPE_DN_STR ) )
      {
         return PMD_TYPE_DN ;
      }
      else if ( 0 == ossStrcasecmp( type, PMD_TYPE_CM_STR ) )
      {
         return PMD_TYPE_CM ;
      }
      else
      {
         return PMD_NODE_TYPE_MAX ;
      }
   }

   const CHAR* utilNodeType2Str( PMD_NODE_TYPE type )
   {
      switch ( type )
      {
         case PMD_TYPE_DN :
            return PMD_TYPE_DN_STR ;
         case PMD_TYPE_CM :
            return PMD_TYPE_CM_STR ;
         default :
            break ;
      }
      return "Unknow" ;
   }

   PMD_NODE_TYPE utilRoleToType( PMD_NODE_ROLE role )
   {
      switch ( role )
      {
         case PMD_NODE_WITNESS :
         case PMD_NODE_FULL :
         case PMD_NODE_USER :
         case PMD_NODE_SPV :
            return PMD_TYPE_DN ;
         case PMD_NODE_CM :
            return PMD_TYPE_CM ;
         default :
            break ;
      }
      return PMD_NODE_TYPE_MAX ;
   }

   const CHAR* utilNodeStatusStr( PMD_NODE_STATUS status )
   {
      switch( status )
      {
         case PMD_NODE_NORMAL :
            return PMD_NODE_NORMAL_STR ;
         case PMD_NODE_SHUTDOWN :
            return PMD_NODE_SHUTDOWN_STR ;
         case PMD_NODE_REBUILDING :
            return PMD_NODE_REBUILDING_STR ;
         case PMD_NODE_PULLINGUP :
            return PMD_NODE_PULLINGUP_STR ;
         default :
            break ;
      }
      return "Unknow" ;
   }

   PMD_NODE_STATUS utilGetNodeStatusEnum( const CHAR *status )
   {
      if ( NULL == status )
      {
         return PMD_NODE_STATUS_MAX ;
      }
      else if ( 0 == *status ||
                0 == ossStrcasecmp( status, PMD_NODE_NORMAL_STR ) )
      {
         return PMD_NODE_NORMAL ;
      }
      else if ( 0 == ossStrcasecmp( status, PMD_NODE_SHUTDOWN_STR ) )
      {
         return PMD_NODE_SHUTDOWN ;
      }
      else if ( 0 == ossStrcasecmp( status, PMD_NODE_REBUILDING_STR ) )
      {
         return PMD_NODE_REBUILDING ;
      }
      else if ( 0 == ossStrcasecmp( status, PMD_NODE_PULLINGUP_STR ) )
      {
         return PMD_NODE_PULLINGUP ;
      }
      else
      {
         return PMD_NODE_STATUS_MAX ;
      }
   }

   const CHAR* utilDataStatusStr( BOOLEAN dataIsOK, PMD_NODE_STATUS status )
   {
      if ( dataIsOK )
      {
         return PMD_DATA_NORMAL_STR ;
      }
      if ( PMD_NODE_REBUILDING == status || PMD_NODE_REBUILDING == status )
      {
         return PMD_DATA_REPAIR_STR ;
      }
      return PMD_DATA_FAULT_STR ;
   }

   std::string utilNodeMode2Str( UINT32 mode )
   {
      std::stringstream ss ;
      BOOLEAN hasAdd = FALSE ;

      if ( PMD_NODE_MODE_READONLY & mode )
      {
         ss << PMD_NODE_MODE_READONLY_STR ;
         hasAdd = TRUE ;
      }
      if ( PMD_NODE_MODE_DEACTIVATED & mode )
      {
         if ( hasAdd )
         {
            ss << " | " ;
         }
         ss << PMD_NODE_MODE_DEACTIVATED_STR ;
      }
      return ss.str() ;
   }

   UINT32 utilGetNodeModeFlag( const string &mode )
   {
      UINT32 modeFlag = 0 ;
      vector< string > parsed ;
      utilSplitStr( mode, parsed, "| \t" ) ;

      for ( UINT32 i = 0 ; i < parsed.size() ; ++i )
      {
         if ( 0 == ossStrcasecmp( PMD_NODE_MODE_READONLY_STR,
                                  parsed[ i ].c_str() ) )
         {
            modeFlag |= PMD_NODE_MODE_READONLY ;
         }
         else if ( 0 == ossStrcasecmp( PMD_NODE_MODE_DEACTIVATED_STR,
                                       parsed[ i ].c_str() ) )
         {
            modeFlag |= PMD_NODE_MODE_DEACTIVATED ;
         }
      }

      return modeFlag ;
   }

   BSONObj utilGetErrorBson( INT32 flags, const CHAR *detail )
   {
      static BSONObj _retObj [SDB_MAX_ERROR + SDB_MAX_WARNING + 1] ;
      static BOOLEAN _init = FALSE ;
      static ossSpinXLatch _lock ;

      // init retobj
      if ( FALSE == _init )
      {
         _lock.get() ;
         if ( FALSE == _init )
         {
            for ( SINT32 i = -SDB_MAX_ERROR; i <= SDB_MAX_WARNING ; i ++ )
            {
               BSONObjBuilder berror ;
               berror.append ( OP_ERRNOFIELD, i ) ;
               berror.append ( OP_ERRDESP_FIELD, getErrDesp ( i ) ) ;
               berror.append ( OP_ERR_DETAIL, "" ) ;
               _retObj[ i + SDB_MAX_ERROR ] = berror.obj() ;
            }
            _init = TRUE ;
         }
         _lock.release() ;
      }

      // check flags
      if ( flags < -SDB_MAX_ERROR || flags > SDB_MAX_WARNING )
      {
         PD_LOG ( PDERROR, "Error code error[rc:%d]", flags ) ;
         flags = SDB_SYS ;
      }

      // return new obj
      if ( detail && *detail != 0 )
      {
         BSONObjBuilder bb ;
         bb.append ( OP_ERRNOFIELD, flags ) ;
         bb.append ( OP_ERRDESP_FIELD, getErrDesp ( flags ) ) ;
         bb.append ( OP_ERR_DETAIL, detail ) ;
         return bb.obj() ;
      }
      // return fix obj
      return _retObj[ SDB_MAX_ERROR + flags ] ;
   }

   struct _utilShellRCItem
   {
      UINT32      _src ;
      INT32       _rc ;
      UINT32      _end ;
   } ;
   typedef _utilShellRCItem utilShellRCItem ;

   #define MAP_SHELL_RC_ITEM( src, rc )   { src, rc, 0 },

   utilShellRCItem* utilGetShellRCMap()
   {
      static utilShellRCItem s_srcMap[] = {
         // map begin
         MAP_SHELL_RC_ITEM( SDB_SRC_SUC, SDB_OK )
         MAP_SHELL_RC_ITEM( SDB_SRC_IO, SDB_IO )
         MAP_SHELL_RC_ITEM( SDB_SRC_PERM, SDB_PERM )
         MAP_SHELL_RC_ITEM( SDB_SRC_OOM, SDB_OOM )
         MAP_SHELL_RC_ITEM( SDB_SRC_INTERRUPT, SDB_INTERRUPT )
         MAP_SHELL_RC_ITEM( SDB_SRC_SYS, SDB_SYS )
         MAP_SHELL_RC_ITEM( SDB_SRC_NOSPC, SDB_NOSPC )
         MAP_SHELL_RC_ITEM( SDB_SRC_TIMEOUT, SDB_TIMEOUT )
         MAP_SHELL_RC_ITEM( SDB_SRC_NETWORK, SDB_NETWORK )
         MAP_SHELL_RC_ITEM( SDB_SRC_INVALIDPATH, SDB_INVALIDPATH )
         MAP_SHELL_RC_ITEM( SDB_SRC_CANNOT_LISTEN, SDB_NET_CANNOT_LISTEN )
         MAP_SHELL_RC_ITEM( SDB_SRC_CAT_AUTH_FAILED, SDB_CAT_AUTH_FAILED )
         MAP_SHELL_RC_ITEM( SDB_SRC_INVALIDARG, SDB_INVALIDARG )
         // map end
         { 0, 0, 1 }
      } ;
      return &s_srcMap[0] ;
   }

   UINT32 utilRC2ShellRC( INT32 rc )
   {
      utilShellRCItem *pEntry = utilGetShellRCMap() ;
      while( 0 == pEntry->_end )
      {
         if ( rc == pEntry->_rc )
         {
            return pEntry->_src ;
         }
         ++pEntry ;
      }
      if ( rc >= 0 )
      {
         return rc ;
      }
      return SDB_SRC_SYS ;
   }

   INT32 utilShellRC2RC( UINT32 src )
   {
      utilShellRCItem *pEntry = utilGetShellRCMap() ;
      while( 0 == pEntry->_end )
      {
         if ( src == pEntry->_src )
         {
            return pEntry->_rc ;
         }
         ++pEntry ;
      }
      if ( (INT32)src <= 0 )
      {
         return src ;
      }
      return SDB_SYS ;
   }

}

