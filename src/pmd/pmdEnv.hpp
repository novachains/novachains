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

   Source File Name = pmdEnv.hpp

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
#ifndef PMDENV_HPP_
#define PMDENV_HPP_

#include "pmdInterface.hpp"

namespace engine
{

   /*
      _pmdEnv define
   */
   class _pmdEnv : public IPmdEnv
   {
      public:
         _pmdEnv() ;
         virtual ~_pmdEnv() ;

         virtual PMD_NODE_STATUS    getNodeStatus() const ;
         virtual BOOLEAN            isShutdown() const ;
         virtual BOOLEAN            isNormal() const ;
         virtual INT32              getShutdownCode() const ;

         virtual PMD_NODE_ROLE      getNodeRole() const ;

         virtual const CHAR*        getHostName() const ;

         virtual UINT64             getStartTime() const ;

         virtual void               getVersion( INT32 &ver,
                                                INT32 &subVer,
                                                INT32 &fixVer,
                                                INT32 &release,
                                                const CHAR **build ) const ;

         virtual const CHAR*        getDataPath() const ;

         virtual void               shutdownNode( INT32 shutdownCode ) ;

      public:

         void                       setNodeRole( PMD_NODE_ROLE role ) ;
         void                       setDataPath( const CHAR* path ) ;

      private:
         UINT64                  _startTime ;
         PMD_NODE_STATUS         _status ;
         PMD_NODE_ROLE           _role ;
         INT32                   _shutdownCode ;
         CHAR                    _hostName[ OSS_MAX_PATHSIZE + 1 ] ;
         CHAR                    _dbpath[ OSS_MAX_PATHSIZE + 1 ] ;

   } ;
   typedef _pmdEnv pmdEnv ;

}

#endif //PMDENV_HPP_

