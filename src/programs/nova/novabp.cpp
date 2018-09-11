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

   Source File Name = novabp.cpp

   Descriptive Name =

   When/how to use:

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          12/1/2014  ly  Initial Draft

   Last Changed =

*******************************************************************************/

#include "pd.hpp"
#include "ossUtil.h"
#include "ossNPipe.hpp"
#include "ossMem.h"
#include "ossMem.hpp"
#include "ossPrimitiveFileOp.hpp"
#include "ossProc.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include "sptCommon.hpp"
#include "utilPipe.hpp"
#include "sptContainer.hpp"
#include "ossSignal.hpp"

using std::string ;
using namespace engine ;

#define NOVA_BP_LOG_FILE "novabp.log"

// #if !defined (SDB_SHELL)
//  #error "sdbbp should always have SDB_SHELL defined"
// #endif

// caller should free output in the case of success
// PD_TRACE_DECLARE_FUNCTION ( SDB_READFROMPIPE, "readFromPipe" )
static INT32 readFromPipe ( OSSNPIPE & npipe , CHAR ** output )
{
   CHAR     c   = 0 ;
   INT32    rc  = SDB_OK ;
   PD_TRACE_ENTRY ( SDB_READFROMPIPE );

   try
   {
      string   buf = "" ;
      while ( TRUE )
      {
         rc = ossReadNamedPipe ( npipe , &c , 1 , NULL ) ;
         if ( SDB_OK == rc )
         {
            buf += c ;
         }
         else if ( SDB_EOF == rc )
         {
            rc = SDB_OK ;
            break ;
         }
         else
         {
            goto error ;
         }
      }

      if ( ! output )
         goto done ;

      boost::algorithm::trim ( buf ) ;
      // output is freed by the caller
      *output = (CHAR *) SDB_OSS_MALLOC ( buf.size() + 1 ) ;
      if ( ! *output )
      {
         rc = SDB_OOM ;
         goto error ;
      }
      ossStrncpy ( *output , buf.c_str() , buf.size() + 1 ) ;
   }
   catch ( std::bad_alloc & )
   {
      rc = SDB_OOM ;
      goto error ;
   }

done :
   PD_TRACE_EXITRC ( SDB_READFROMPIPE, rc );
   return rc ;
error :
   goto done ;
}

// PD_TRACE_DECLARE_FUNCTION ( SDB_MONITOR_THREAD, "monitor_thread" )
void monitor_thread ( const OSSPID  shpid ,
                      const CHAR * f2bName ,
                      const CHAR * b2fName )
{
   PD_TRACE_ENTRY ( SDB_MONITOR_THREAD );
   while ( ossIsProcessRunning ( shpid ) )
   {
      ossSleep( OSS_ONE_SEC ) ;
   }

   // shell has exited, so just clean up and exit the whole program
   ossCleanNamedPipeByName ( f2bName ) ;
   ossCleanNamedPipeByName ( b2fName ) ;
   PD_TRACE_EXIT ( SDB_MONITOR_THREAD );
   exit (0) ;
}

// PD_TRACE_DECLARE_FUNCTION ( SDB_CREATESHMONTHREAD, "createShellMonitorThread" )
INT32 createShellMonitorThread ( const OSSPID & shpid ,
                                 const CHAR * f2bName ,
                                 const CHAR * b2fName )
{
   INT32 rc = SDB_OK ;
   PD_TRACE_ENTRY ( SDB_CREATESHMONTHREAD );

   try
   {
      boost::thread monitor ( monitor_thread , shpid , f2bName , b2fName ) ;
      monitor.detach() ;
   }
   catch ( boost::thread_resource_error & e )
   {
      rc = SDB_SYS ;
      goto error ;
   }

done :
   PD_TRACE_EXITRC ( SDB_CREATESHMONTHREAD, rc );
   return rc ;
error :
   goto done ;
}

// PD_TRACE_DECLARE_FUNCTION ( SDB_ENTERDAEMONMODE, "enterDaemonMode" )
INT32 enterDaemonMode ( sptScope *scope ,
                        const OSSPID & shpid ,
                        const CHAR * waitName ,
                        const CHAR * f2bName ,
                        const CHAR * b2fName )
{
   INT32          rc          = SDB_OK ;
   PD_TRACE_ENTRY ( SDB_ENTERDAEMONMODE );
   CHAR *         code        = NULL ;
   BOOLEAN        exit        = FALSE ;
   //FILE           oldStdout   = *stdout ;
   INT32          fd          = -1 ;
   INT32          hOutFd      = -1 ;
   //FILE *         newStdout   = NULL ;
   //CHAR *         result      = NULL ;

   OSSNPIPE f2bPipe ;
   OSSNPIPE b2fPipe ;
   OSSNPIPE waitPipe ;

   ossMemset ( &f2bPipe , 0 , sizeof ( f2bPipe ) ) ;
   ossMemset ( &b2fPipe , 0 , sizeof ( b2fPipe ) ) ;
   ossMemset ( &waitPipe , 0 , sizeof ( waitPipe ) ) ;

   SDB_ASSERT ( scope , "invalid argument" ) ;
   SDB_ASSERT ( waitName && waitName[0] != '\0' , "invalid argument" ) ;
   SDB_ASSERT ( f2bName && f2bName[0] != '\0' , "invalid argument" ) ;
   SDB_ASSERT ( b2fName && b2fName[0] != '\0' , "invalid argument" ) ;

   rc = ossCreateNamedPipe( f2bName , 0 , 0 , OSS_NPIPE_INBOUND,
                            1 , 0 , f2bPipe ) ;
   SH_VERIFY_RC

   rc = ossCreateNamedPipe( b2fName , 0 , 0 , OSS_NPIPE_OUTBOUND ,
                            1 , 0 , b2fPipe ) ;
   SH_VERIFY_RC

   // tell front-end that initialzation finished
   rc = ossOpenNamedPipe ( waitName , OSS_NPIPE_OUTBOUND , 0 , waitPipe ) ;
   SH_VERIFY_RC

   rc = ossCloseNamedPipe ( waitPipe ) ;
   SH_VERIFY_RC

   rc = createShellMonitorThread ( shpid , f2bName , b2fName ) ;
   SH_VERIFY_RC

   while ( TRUE )
   {
      rc = ossConnectNamedPipe ( f2bPipe , OSS_NPIPE_INBOUND ) ;
      SH_VERIFY_RC

      // code is freed after evaluated or in done:
      rc = readFromPipe ( f2bPipe , &code )  ;
      SH_VERIFY_RC

      rc = ossDisconnectNamedPipe ( f2bPipe ) ;
      SH_VERIFY_RC

      rc = ossConnectNamedPipe ( b2fPipe , OSS_NPIPE_OUTBOUND ) ;
      SH_VERIFY_RC

      rc = ossNamedPipeToFd ( b2fPipe , &fd ) ;
      SH_VERIFY_RC

      // save the fd of sdbbp.log
      hOutFd = ossDup( 1 ) ;
      // redirect fd 1(it had been redirect to sdbbp.log) to b2fPipe
      rc = ossDup2( fd, 1 ) ;
      SH_VERIFY_RC

      if ( ossStrcmp ( CMD_QUIT , code ) == 0 )
         exit = TRUE ;

      if ( ! exit )
         scope->eval( code, ossStrlen( code ), "(novabp)", 1,
                      SPT_EVAL_FLAG_PRINT, NULL ) ;
      SAFE_OSS_FREE ( code ) ;
      // shell always have errno defined
      ossPrintf ( " %d", sdbGetErrno() ) ;
      //result = NULL ;

      //*stdout = oldStdout ;
      // close the copy fd of b2fPipe
      ossCloseFd( 1 ) ;
      // set 1 redirect back to sdbbp.log
      ossDup2( hOutFd, 1 ) ;
      // close the copy fd of sdbbp.log
      ossCloseFd( hOutFd ) ;

      // close name pipe
      // when we close this writen name pipe, the read name pipe
      // in front process will finish reading, and it will print
      // the contents
      rc = ossDisconnectNamedPipe ( b2fPipe ) ;
      SH_VERIFY_RC

      if ( exit )
      {
         break ;
      }
   }

done :
   SAFE_OSS_FREE ( code ) ;
   ossDeleteNamedPipe ( f2bPipe ) ;
   ossDeleteNamedPipe ( b2fPipe ) ;
   PD_TRACE_EXITRC ( SDB_ENTERDAEMONMODE, rc );
   return rc ;
error :
   ossCloseNamedPipe ( waitPipe ) ;
   goto done ;
}

// PD_TRACE_DECLARE_FUNCTION ( SDB_SDBBP_MAIN, "main" )
int main ( int argc , const char * argv[] )
{
   PD_TRACE_ENTRY ( SDB_SDBBP_MAIN );
   INT32             rc       = SDB_OK ;
   OSSPID            shpid    = OSS_INVALID_PID ;
   OSSPID            bppid    = OSS_INVALID_PID ;
   CHAR waitName[128]         = { 0 } ;
   CHAR f2bName[128]          = { 0 } ;
   CHAR b2fName[128]          = { 0 } ;
   engine::sptContainer container ;
   engine::sptScope *scope = NULL ;

   // redirect stdout to log file, so, when we printf error info,
   // thay will be redirected to log file
   if ( ! freopen ( NOVA_BP_LOG_FILE , "a" , stdout ) )
   {
      rc = ossResetTty() ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }
      // this can only display in linux, for in linux, father and son process
      // share the same terminal, but in window, the follow info display in 
      // a terminal where we can't see
      ossPrintf( "warning: failed to freopen stdout to log "
                 "file[%s]"OSS_NEWLINE, NOVA_BP_LOG_FILE ) ;
   }

#if defined( _LINUX )
   signal( SIGCHLD, SIG_IGN ) ;
#endif // _LINUX

   ossMemset ( waitName , 0 , sizeof ( waitName ) ) ;
   ossMemset ( f2bName , 0 , sizeof ( f2bName ) ) ;
   ossMemset ( b2fName , 0 , sizeof ( b2fName ) ) ;

   SH_VERIFY_COND ( argc >= 2 , SDB_INVALIDARG )

   shpid = (OSSPID) ossAtoi ( argv[1] ) ;
   bppid = ossGetCurrentProcessID() ;

   rc = getWaitPipeName ( shpid , waitName , sizeof ( waitName ) ) ;
   SH_VERIFY_RC

   rc = getPipeNames2 ( shpid , bppid , f2bName , sizeof ( f2bName ) ,
                                        b2fName , sizeof ( b2fName ) ) ;
   SH_VERIFY_RC

   rc = container.init() ;
   SH_VERIFY_RC

   // will purge engine in done:
   scope = container.newScope() ;
   SH_VERIFY_COND ( scope , SDB_SYS ) ;

   rc = enterDaemonMode ( scope , shpid , waitName , f2bName , b2fName ) ;
   SH_VERIFY_RC

done :
   if ( scope )
   {
      container.releaseScope( scope ) ;
   }
   container.fini() ;
   PD_TRACE_EXITRC ( SDB_SDBBP_MAIN, rc );
   return rc ;
error :
   goto done ;
}

