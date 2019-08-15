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

   Source File Name = dpsLogFileMgr.hpp

   Descriptive Name = Data Protection Services Log File Manager Header

   When/how to use: this program may be used on binary and text-formatted
   versions of DPS component. This file contains declare for log file manager

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/15/2019  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef DPSLOGFILEMGR_HPP__
#define DPSLOGFILEMGR_HPP__

#include "core.hpp"
#include "oss.hpp"
#include "dpsLogFile.hpp"
#include "pmdDef.hpp"
#include <vector>
using namespace std;

namespace engine
{

   #define DPS_LOG_FILE_PREFIX "novachainLog."
   #define DPS_LOG_FILE_MAX_PATHSIZE \
           ( OSS_MAX_PATHSIZE - sizeof(DPS_LOG_FILE_PREFIX) - 12 )
   #define PMD_DFT_LOG_FILE_SZ 64


   class _dpsLogPage;
   class _dpsMessageBlock;
   class _dpsLogMgr ;

   /*
      _dpsLogFileMgr define
   */
   class _dpsLogFileMgr : public SDBObject
   {
   private:
      vector<_dpsLogFile *>   _files ;
      UINT32                  _work ;
      UINT32                  _logicalWork ;
      UINT32                  _begin ;

      UINT32                  _logFileSz ;
      UINT32                  _logFileNum ;
      ossSpinSLatch           _mtx ;
      _dpsLogMgr             *_logMgr ;
      CHAR _path [DPS_LOG_FILE_MAX_PATHSIZE+1]  ;

   public:
      _dpsLogFileMgr( class _dpsLogMgr *logMgr );
      ~_dpsLogFileMgr();

      INT32 init( const CHAR *path );

      INT32 flush( _dpsMessageBlock *mb,
                   BOOLEAN shutdown = FALSE );

      void setLogFileSz ( UINT64 logFileSz )
      {
         _logFileSz = logFileSz ;
      }

      UINT32 getLogFileSz ()
      {
         return _logFileSz ;
      }
      void setLogFileNum ( UINT32 logFileNum )
      {
         _logFileNum = logFileNum ;
      }
      UINT32 getLogFileNum ()
      {
         return _logFileNum ;
      }
      _dpsLogFile * getWorkLogFile()
      {
         return _files[_work] ;
      }

      _dpsLogFile* getLogFile( UINT32 fileId )
      {
         if ( fileId >= _logFileNum )
         {
            return NULL ;
         }

         return _files[ fileId ] ;
      }

      UINT32 getWorkPos() const { return _work ; }
      UINT32 getLogicalWorkPos() const { return _logicalWork ; }

      INT32 sync() ;

   protected:
      INT32    _createFile () ;
      UINT32   _incFileID ( UINT32 fileID ) ;
      UINT32   _decFileID ( UINT32 fileID ) ;
      void     _incLogicalFileID () ;

   };
   typedef class _dpsLogFileMgr dpsLogFileMgr ;

}

#endif // DPSLOGFILEMGR_HPP__

