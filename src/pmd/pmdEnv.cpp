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

   Source File Name = pmdEnv.cpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          01/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "pmdEnv.hpp"
#include "ossUtil.hpp"
#include "ossVer.hpp"
#include "ossSocket.hpp"
#include "pd.hpp"
#include "pmdTrace.hpp"

namespace engine
{

   /*
      _pmdEnv implement
   */
   _pmdEnv::_pmdEnv()
   {
      _startTime = 0 ;
      _status = PMD_NODE_NORMAL ;
      _role = PMD_NODE_ROLE_MAX ;
      _shutdownCode = 0 ;

      /// hostname
      ossMemset( _hostName, 0, sizeof( _hostName ) ) ;
      ossGetHostName( _hostName, OSS_MAX_PATHSIZE ) ;

      /// start time
      _startTime = ossGetCurrentMilliseconds() ;

      /// data path
      ossMemset( _dbpath, 0, sizeof( _dbpath ) ) ;
   }

   _pmdEnv::~_pmdEnv()
   {
   }

   PMD_NODE_STATUS _pmdEnv::getNodeStatus() const
   {
      return _status ;
   }

   BOOLEAN _pmdEnv::isShutdown() const
   {
      return PMD_NODE_SHUTDOWN == _status ? TRUE : FALSE ;
   }

   BOOLEAN _pmdEnv::isNormal() const
   {
      return PMD_NODE_NORMAL == _status ? TRUE : FALSE ;
   }

   INT32 _pmdEnv::getShutdownCode() const
   {
      return _shutdownCode ;
   }

   PMD_NODE_ROLE _pmdEnv::getNodeRole() const
   {
      return _role ;
   }

   const CHAR* _pmdEnv::getHostName() const
   {
      return _hostName ;
   }

   UINT64  _pmdEnv::getStartTime() const
   {
      return _startTime ;
   }

   void _pmdEnv::getVersion( INT32 &ver,
                             INT32 &subVer,
                             INT32 &fixVer,
                             INT32 &release,
                             const CHAR **build ) const
   {
      ossGetVersion( &ver, &subVer, &fixVer, &release, build ) ;
   }

   const CHAR* _pmdEnv::getDataPath() const
   {
      return _dbpath ;
   }

   void _pmdEnv::shutdownNode( INT32 shutdownCode )
   {
      if ( !isShutdown() )
      {
         _status = PMD_NODE_SHUTDOWN ;
         _shutdownCode = shutdownCode ;
      }
   }

   void _pmdEnv::setNodeRole( PMD_NODE_ROLE role )
   {
      _role = role ;
   }

   void _pmdEnv::setDataPath( const CHAR* path )
   {
      if ( path )
      {
         ossStrncpy( _dbpath, path, OSS_MAX_PATHSIZE ) ;
      }
   }

}


