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

   Source File Name = pmdTool.cpp

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

#include "pmdTool.hpp"
#include "ossEDU.hpp"
#include "pmdSignalHandler.hpp"
//#include "pmdEDUMgr.hpp"
#include "pd.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"

namespace engine
{
   static OSS_THREAD_LOCAL PMD_ON_EDU_EXIT_FUNC __eduHookFunc = NULL ;

   PMD_ON_EDU_EXIT_FUNC pmdSetEDUHook( PMD_ON_EDU_EXIT_FUNC hookFunc )
   {
      PMD_ON_EDU_EXIT_FUNC oldFunc = __eduHookFunc ;
      __eduHookFunc = hookFunc ;
      return oldFunc ;
   }

   PMD_ON_EDU_EXIT_FUNC pmdGetEDUHook()
   {
      return __eduHookFunc ;
   }

   pmdSysInfo* pmdGetSysInfo()
   {
      static pmdSysInfo s_sysInfo ;
      return &s_sysInfo ;
   }

   BOOLEAN pmdIsQuitApp()
   {
      return pmdGetSysInfo()->_quitFlag ;
   }

   void pmdSetQuit()
   {
      pmdGetSysInfo()->_quitFlag = TRUE ;
   }

   INT32& pmdGetSigNum()
   {
      static INT32 s_sigNum = -1 ;
      return s_sigNum ;
   }

   ossProcLimits* pmdGetLimit()
   {
      return &( pmdGetSysInfo()->_limitInfo ) ;
   }

   UINT64 pmdAcquireGlobalID()
   {
      return pmdGetSysInfo()->_globalID.inc() ;
   }

   const CHAR* pmdNodeStatus2String( PMD_NODE_STATUS status )
   {
      const CHAR *str = "Uknown" ;

      switch( status )
      {
         case PMD_NODE_NORMAL:
            str = "Normal" ;
            break ;
         case PMD_NODE_SHUTDOWN :
            str = "Shutdown" ;
            break ;
         default:
            break ;
      }

      return str ;
   }

   const CHAR* pmdNodeRole2String( PMD_NODE_ROLE role )
   {
      const CHAR *str = "Uknown" ;

      switch( role )
      {
         case PMD_NODE_WITNESS :
            str = "Witness" ;
            break ;
         case PMD_NODE_FULL :
            str = "Full" ;
            break ;
         case PMD_NODE_SPV :
            str = "SPV" ;
            break ;
         default :
            break ;
      }

      return str ;
   }

#if defined (_LINUX)

   // PD_TRACE_DECLARE_FUNCTION ( SDB_PMDSIGHND, "pmdSignalHandler" )
   void pmdSignalHandler ( INT32 sigNum )
   {
      PD_TRACE_ENTRY ( SDB_PMDSIGHND ) ;

      static BOOLEAN s_closeStdFds = FALSE ;

      // set signum
      PMD_SIGNUM = sigNum ;

      if ( ossGetSignalShieldFlag() )
      {
         ossGetPendingSignal() = sigNum ;
         goto done ;
      }

      if ( sigNum > 0 && sigNum <= OSS_MAX_SIGAL )
      {
         if ( SIGPIPE == sigNum && !s_closeStdFds &&
              1 == ossGetCurrentProcessID() )
         {
            /// close std fds
            ossCloseStdFds() ;
            s_closeStdFds = TRUE ;
         }

         PD_LOG ( PDEVENT, "Recieve signal[%d:%s, %s]",
                  sigNum, pmdGetSignalInfo( sigNum )._name,
                  pmdGetSignalInfo( sigNum )._handle ? "QUIT" : "IGNORE" ) ;
         if ( pmdGetSignalInfo( sigNum )._handle ) // quit
         {
            pmdGetSysInfo()->_quitFlag = TRUE ;
            if ( pmdGetSysInfo()->_pQuitFunc )
            {
               (*pmdGetSysInfo()->_pQuitFunc)() ;
            }
         }
      }

   done:
      PD_TRACE_EXIT ( SDB_PMDSIGHND ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_PMDEDUUSERTRAPHNDL, "pmdEDUUserTrapHandler" )
   void pmdEDUUserTrapHandler( OSS_HANDPARMS )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_PMDEDUUSERTRAPHNDL );
      oss_edu_data * pEduData = NULL ;
      const CHAR *dumpPath = ossGetTrapExceptionPath () ;
      if ( ossGetSignalShieldFlag() )
      {
         ossGetPendingSignal() = signum ;
         goto done ;
      }
      else if ( !dumpPath )
      {
         goto done ;
      }
      /*else if ( !pmdGetEDUMgr() )
      {
         goto done ;
      }*/

      pEduData = ossGetThreadEDUData() ;

      if ( NULL == pEduData )
      {
         goto done ;
      }

      if ( OSS_AM_I_INSIDE_SIGNAL_HANDLER( pEduData ) )
      {
         goto done ;
      }
      OSS_ENTER_SIGNAL_HANDLER( pEduData ) ;

      if ( signum == OSS_STACK_DUMP_SIGNAL )
      {
         PD_LOG ( PDEVENT, "Signal %d is received, "
                  "prepare to dump stack for all threads", signum ) ;

         /*pmdEDUMgr *pMgr = pmdGetEDUMgr() ;
         pMgr->killByThreadID( OSS_STACK_DUMP_SIGNAL_INTERNAL ) ;*/

         ossMemTrace ( dumpPath ) ;
      }
      else if ( signum == OSS_STACK_DUMP_SIGNAL_INTERNAL )
      {
         PD_LOG ( PDEVENT, "Signal %d is received, "
                  "prepare to dump stack for %u:%u", signum,
                  ossGetCurrentProcessID(),
                  ossGetCurrentThreadID() ) ;
         ossStackTrace( OSS_HANDARGS, dumpPath ) ;
      }
      else
      {
         PD_LOG ( PDWARNING, "Unexpected signal is received: %d",
                  signum ) ;
      }
      OSS_LEAVE_SIGNAL_HANDLER( pEduData ) ;
   done :
      PD_TRACE1 ( SDB_PMDEDUUSERTRAPHNDL, PD_PACK_INT(rc) );
      PD_TRACE_EXIT ( SDB_PMDEDUUSERTRAPHNDL ) ;
      return ;
   }

   INT32 pmdEnableSignalEvent( const CHAR *filepath,
                               PMD_ON_QUIT_FUNC pFunc,
                               INT32 *pDelSig )
   {
      INT32 rc = SDB_OK ;
      ossSigSet sigSet ;
      struct sigaction newact ;
      ossMemset ( &newact, 0, sizeof(newact)) ;
      sigemptyset ( &newact.sa_mask ) ;

      // set trap file path
      if ( filepath )
      {
         ossSetTrapExceptionPath ( filepath ) ;
      }
      pmdGetSysInfo()->_pQuitFunc = pFunc ;

      // SIGSEGV( 11 )
      newact.sa_sigaction = ( OSS_SIGFUNCPTR ) ossEDUCodeTrapHandler ;
      newact.sa_flags |= SA_SIGINFO ;
      newact.sa_flags |= SA_ONSTACK ;
      if ( sigaction ( SIGSEGV, &newact, NULL ) )
      {
         PD_LOG ( PDERROR, "Failed to setup signal handler for SIGSEGV" ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      if ( sigaction ( SIGBUS, &newact, NULL ) )
      {
         PD_LOG ( PDERROR, "Failed to setup signal handler for SIGBUS" ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      // stack dump signals: SIGURG( 23 )
      newact.sa_sigaction = ( OSS_SIGFUNCPTR ) pmdEDUUserTrapHandler ;
      newact.sa_flags |= SA_SIGINFO ;
      newact.sa_flags |= SA_ONSTACK ;
      if ( sigaction ( OSS_STACK_DUMP_SIGNAL, &newact, NULL ) )
      {
         PD_LOG ( PDERROR, "Failed to setup signal handler for dump signal" ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      // capture the internal user stack dump signal
      if ( sigaction ( OSS_STACK_DUMP_SIGNAL_INTERNAL, &newact, NULL ) )
      {
         PD_LOG ( PDERROR, "Failed to setup signal handler for dump signal" ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      /// ignore the SIGPIPE
      signal( SIGPIPE, SIG_IGN ) ;

      // other signal
      sigSet.fillSet () ;
      sigSet.sigDel ( SIGSEGV ) ;
      sigSet.sigDel ( SIGBUS ) ;
      sigSet.sigDel ( SIGALRM ) ;
      sigSet.sigDel ( SIGPROF ) ;
      sigSet.sigDel ( SIGPIPE ) ;
      sigSet.sigDel ( OSS_STACK_DUMP_SIGNAL ) ;
      sigSet.sigDel ( OSS_STACK_DUMP_SIGNAL_INTERNAL ) ;

      if ( pDelSig )
      {
         UINT32 i = 0 ;
         while ( 0 != pDelSig[ i ] )
         {
            sigSet.sigDel( pDelSig[ i ] ) ;
            ++i ;
         }
      }

      rc = ossRegisterSignalHandle( sigSet, (SIG_HANDLE)pmdSignalHandler ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG ( PDWARNING, "Failed to register signals, rc = %d", rc ) ;
         // we do not abort startup process if any signal handler can't be
         // installed
         rc = SDB_OK ;
      }

   done :
      return rc ;
   error :
      goto done ;
   }

#else

   // PD_TRACE_DECLARE_FUNCTION ( SDB_PMDCTRLHND, "pmdCtrlHandler" )
   BOOL pmdCtrlHandler( DWORD fdwCtrlType )
   {
      BOOLEAN ret = FALSE ;
      PD_TRACE_ENTRY ( SDB_PMDCTRLHND );
      switch( fdwCtrlType )
      {
      // Handle the CTRL-C signal.
      case CTRL_C_EVENT:
         printf( "Ctrl-C event\n\n" ) ;
         pmdGetSysInfo()->_quitFlag = TRUE ;
         if ( pmdGetSysInfo()->_pQuitFunc )
         {
            (*pmdGetSysInfo()->_pQuitFunc)() ;
         }
         Beep( 750, 300 );
         ret = TRUE ;
         goto done ;

      // CTRL-CLOSE: confirm that the user wants to exit.
      case CTRL_CLOSE_EVENT:
         Beep( 600, 200 );
         printf( "Ctrl-Close event\n\n" ) ;
         ret = TRUE ;
         goto done ;

      // Pass other signals to the next handler.
      case CTRL_BREAK_EVENT:
         Beep( 900, 200 );
         printf( "Ctrl-Break event\n\n" ) ;
         ret = FALSE ;
         goto done ;

      case CTRL_LOGOFF_EVENT:
         Beep( 1000, 200 );
         printf( "Ctrl-Logoff event\n\n" ) ;
         ret = FALSE ;
         goto done ;

      case CTRL_SHUTDOWN_EVENT:
         Beep( 750, 500 );
         printf( "Ctrl-Shutdown event\n\n" ) ;
         ret = FALSE ;
         goto done ;

      default:
         ret = FALSE ;
         goto done ;
      }
   done :
      PD_TRACE1 ( SDB_PMDCTRLHND, PD_PACK_INT(ret) ) ;
      PD_TRACE_EXIT ( SDB_PMDCTRLHND ) ;
      return ret ;
   }

   INT32 pmdEnableSignalEvent( const CHAR * filepath,
                               PMD_ON_QUIT_FUNC pFunc,
                               INT32 *pDelSig )
   {
      // set trap file path
      if ( filepath )
      {
         ossSetTrapExceptionPath ( filepath ) ;
      }
      pmdGetSysInfo()->_pQuitFunc = pFunc ;

      // install ctrl event handler
      SetConsoleCtrlHandler( (PHANDLER_ROUTINE)pmdCtrlHandler, TRUE ) ;

      return SDB_OK ;
   }

#endif // _LINUX


}


