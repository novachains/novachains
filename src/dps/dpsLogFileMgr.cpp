/*******************************************************************************


   Copyright (C) 2011-2019 NOVACHAIN Ltd.

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

   Source File Name = dpsLogFileMgr.cpp

   Descriptive Name = Data Protection Service Log File Manager

   When/how to use: this program may be used on binary and text-formatted
   versions of data protection component. This file contains log file manager.


   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/15/2019  YW  Initial Draft

   Last Changed =

*******************************************************************************/
#include "stdio.h"
#include "dpsLogFileMgr.hpp"
#include "pd.hpp"
#include "dpsLogPage.hpp"
#include "dpsMessageBlock.hpp"
#include "dpsLogDef.hpp"
#include "dpsLogMgr.hpp"
#include "utilStr.hpp"
#include "pdTrace.hpp"

namespace engine
{

   #define LOG_LOOP_BEGIN( a ) \
           for ( UINT32 i = 0; i < a; i++ ) {\

   #define LOG_LOOP_FILE ( _files.at( i ) )

   #define LOG_LOOP_END }

   #define EACH i

   #define LOG_FILE( a ) ( _files.at((a)%_logFileNum) )

   #define WORK_FILE()  ( _files.at( _work ) )

   #define UPDATE_SUB( a ) { a = ++a % _files.size();}

   // constructor
   _dpsLogFileMgr::_dpsLogFileMgr( class _dpsLogMgr *logMgr ):_work(0),
   _logicalWork(0), _logFileNum(0)
   {
      SDB_ASSERT ( logMgr, "logMgr can't be NULL" ) ;
      _logMgr    = logMgr ;
      _logFileSz  = PMD_DFT_LOG_FILE_SZ * DPS_LOG_FILE_SIZE_UNIT ;

      _begin = 0 ;
      ossMemset(_path, 0, sizeof(_path)) ;
   }

   // destructor
   _dpsLogFileMgr::~_dpsLogFileMgr()
   {
      LOG_LOOP_BEGIN ( _files.size() )
      {
         if ( LOG_LOOP_FILE )
            SDB_OSS_DEL LOG_LOOP_FILE ;
      }
      LOG_LOOP_END
      _files.clear();
   }

   // initialize log file manager
   INT32 _dpsLogFileMgr::init( const CHAR *path )
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT( path, "path can not be NULL!") ;

      // make sure path + OSS_FILE_SEP + DPS_LOG_FILE_PREFIX + xxx + 0
      // is less or equal to OSS_MAX_PATHSIZE
      if ( ossStrlen ( path ) > DPS_LOG_FILE_MAX_PATHSIZE ) 
      {
         PD_LOG ( PDERROR, "Log Path is too long" ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      ossStrncpy( _path, path, ossStrlen ( path ) ) ;

      rc =  _createFile() ;

      if ( rc )
      {
         goto error;
      }

   done:
      return rc;
   error:
      // free memory if error occurs
      LOG_LOOP_BEGIN ( _files.size() )
         SDB_OSS_DEL LOG_LOOP_FILE ;
      LOG_LOOP_END
      _files.clear();
      goto done;
   }

   INT32 _dpsLogFileMgr::_createFile()
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT( _path, "path can not be NULL!") ;
      CHAR fileFullPath[ OSS_MAX_PATHSIZE+1 ] = {0} ;
      // temp buffer stores log file sequence up to 0xFFFFFFFF, which is
      // 4294967295 ( 10 bytes )
      CHAR tmp[11] = { 0 } ;

      // memory is free in destructor, or by end of error in this function
      _dpsLogFile *file = SDB_OSS_NEW _dpsLogFile();
      if ( NULL == file )
      {
         rc = SDB_OOM ;
         PD_LOG ( PDERROR, "Memory can't be allocated for dpsLogFile");
         goto error;
      }
      // push log file to vector array
      _files.push_back( file ) ;

      utilBuildFullPath( _path, DPS_LOG_FILE_PREFIX,
                         OSS_MAX_PATHSIZE, fileFullPath ) ;
      ossSnprintf ( tmp, sizeof(tmp), "%d", _logFileNum ) ;
      ossStrncat( fileFullPath, tmp, ossStrlen( tmp ) ) ;
      // initialize log file for each newly created one
      // we set readonly to FALSE, so that each log file is opened with
      // WRITEONLY option
      rc = file->init( fileFullPath, _logFileSz, _logFileNum );

      if ( rc )
      {
         PD_LOG ( PDERROR,"Failed to init log file rc = %d",  rc ) ;
         goto error;
      }

      _logFileNum++ ;

   done:
      return rc;
   error:
      if (file)
      {
         SDB_OSS_DEL file ;
      }
      _files.clear();
      goto done;
   }

   // write a log buffer into file
   // Note this function will change file pointer so the mb should always be
   // full ( length() = DPS_DEFAULT_PAGE_SIZE ), except during database shutdown
   // period. Otherwise it will cause serious log corruption
   // Again, NEVER flush a partial fulled page into file unless it's during
   // database shutdown
   INT32 _dpsLogFileMgr::flush( _dpsMessageBlock *mb,
                                BOOLEAN shutdown )
   {
      INT32 rc = SDB_OK;

      dpsLogFile *pWork = WORK_FILE() ;

      // first we get the number of bytes need to write into log file
      // by our design, the mb->length() should always be same as
      // DPS_DEFAULT_PAGE_SIZE, except during tearDown phase
      SDB_ASSERT ( shutdown || mb->length() == DPS_DEFAULT_PAGE_SIZE,
                   "mb length must be DPS_DEFAULT_PAGE_SIZE unless it's "
                   "shutdown" ) ;
      // since we always write every dps page, so we shouldn't write out of
      // bound, so we will hit idleSize = 0 when log file is filled up
      // new file will be created here
      if ( pWork->getIdleSize() == 0 )
      {
         // synchronization control, parallel creation is not allowed
         _mtx.get() ;
         if ( pWork->getIdleSize() == 0 )
         {
            rc = pWork->flushOffset() ;
            rc = _createFile() ;
            ++_work ;
            pWork = WORK_FILE() ;
         }
         _mtx.release() ;
      }

      if ( pWork->getIdleSize() == pWork->size() )
      {
         pWork->reset( _work, 0) ;
      }

      // write into log file for page size
      rc = pWork->write ( mb->startPtr(), DPS_DEFAULT_PAGE_SIZE ) ;
      rc = pWork->flushOffset();
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to write %d bytes into file, rc = %d",
                 DPS_DEFAULT_PAGE_SIZE, rc ) ;
         goto error ;
      }

      /// if the page is not whole, reset file pointer to the page begin pos
      if ( DPS_DEFAULT_PAGE_SIZE != mb->length() )
      {
         pWork->idleSize( pWork->getIdleSize() +
                          DPS_DEFAULT_PAGE_SIZE ) ;
      }

   done:
      return rc;
   error:
      goto done;
   }

   void _dpsLogFileMgr::_incLogicalFileID ()
   {
      ++_logicalWork ;
   }

    // PD_TRACE_DECLARE_FUNCTION ( SDB__DPSLGFILEMGR_SYNC, "_dpsLogFileMgr::sync" )
   INT32 _dpsLogFileMgr::sync()
   {
      INT32 rc = SDB_OK ;

      /// flush files from the oldest file
      UINT32 j = _work + 1 ;
      for ( UINT32 i = 0 ; i <= _files.size(); ++i, ++j )
      {
         _dpsLogFile *file = LOG_FILE( j ) ;
         if ( !file->isDirty() )
         {
            continue ;
         }
         rc = file->sync() ;
         if ( SDB_OK != rc )
         {
            PD_LOG( PDERROR, "Failed to sync log file: %d", rc ) ;
            goto error ;
         }
      }

   done:
      PD_TRACE_EXITRC( SDB__DPSLGFILEMGR_SYNC, rc ) ;
      return rc ;
   error:
      goto done ;
   }

}

