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

   Source File Name = pmdTool.hpp

   Descriptive Name = Process MoDel Main

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for SequoiaDB,
   and all other process-initialization code.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/08/2018  XJH Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef PMD_TOOL_HPP_
#define PMD_TOOL_HPP_

#include "pmdInterface.hpp"
#include "pmdDef.hpp"
#include "ossAtomic.hpp"
#include "ossUtil.hpp"

namespace engine
{

   /*
      When recieve quit event or signal, will call
   */
   typedef  void (*PMD_ON_QUIT_FUNC)() ;

   /*
      Be called when pmdEDU exit
   */
   typedef  void (*PMD_ON_EDU_EXIT_FUNC)() ;

   PMD_ON_EDU_EXIT_FUNC pmdSetEDUHook( PMD_ON_EDU_EXIT_FUNC hookFunc ) ;
   PMD_ON_EDU_EXIT_FUNC pmdGetEDUHook() ;

   /*
      pmd system info define
   */

   typedef struct _pmdSysInfo
   {
      BOOLEAN                       _quitFlag ;
      PMD_ON_QUIT_FUNC              _pQuitFunc ;

      /// global id
      ossAtomic64                   _globalID ;

      /// ulimit info
      ossProcLimits                 _limitInfo ;

      _pmdSysInfo()
      :_globalID( 1 )
      {
         _quitFlag      = FALSE ;
         _pQuitFunc     = NULL ;
      }
   } pmdSysInfo ;

   void           pmdSetQuit() ;
   BOOLEAN        pmdIsQuitApp() ;

   UINT64         pmdAcquireGlobalID() ;

   ossProcLimits* pmdGetLimit() ;

   pmdSysInfo*    pmdGetSysInfo () ;

   /*
      pmd trap functions
   */

   INT32    pmdEnableSignalEvent( const CHAR *filepath,
                                  PMD_ON_QUIT_FUNC pFunc,
                                  INT32 *pDelSig = NULL ) ;

   INT32&   pmdGetSigNum() ;

   /*
      Env define
   */
   #define  PMD_SIGNUM                 pmdGetSigNum()

}

#endif //PMD_TOOL_HPP_

