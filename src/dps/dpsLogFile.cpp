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

   Source File Name = dpsLogFile.cpp

   Descriptive Name = Data Protection Service Log File

   When/how to use: this program may be used on binary and text-formatted
   versions of data protection component. This file contains code logic for
   DPS transaction log file basic operations

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who   Description
   ====== =========== ===== ==============================================
          07/29/2019  jiaqi Initial Draft

   Last Changed =

*******************************************************************************/
#include "dpsLogFile.hpp"
#include "dpsLogPage.hpp"
#include "dpsLogFileMgr.hpp"
#include "pd.hpp"
#include "pdTrace.hpp"
#include "dpsLogRecord.hpp"

namespace engine
{
   #define DPS_LOGFILE_READ_TIMEOUT          ( 30000 )

   _dpsLogFile::_dpsLogFile()
   {
      _file = NULL ;
      _fileSize = 0 ;
      _fileNum  = 0 ;
      _idleSize = 0 ;
      _inRestore= FALSE ;
      _dirty = FALSE ;
   }

   _dpsLogFile::~_dpsLogFile()
   {
      if ( NULL != _file )
      {
         close();
         SDB_OSS_DEL _file;
      }

      _file = NULL;
   }

   string _dpsLogFile::toString() const
   {
      stringstream ss ;
      ss << "FileSize: " << _fileSize
         << ", IdleSize: " << _idleSize
         << ", LogID: " << _logHeader._logID ;
      return ss.str() ;
   }

   // initialize a log file, file size max 4GB
   INT32 _dpsLogFile::init( const CHAR *path, UINT32 size, UINT32 fileNum )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN created = FALSE ;

      SDB_ASSERT ( 0 == ( _fileSize % DPS_DEFAULT_PAGE_SIZE ),
                   "Size must be multiple of DPS_DEFAULT_PAGE_SIZE bytes" ) ;

      _fileSize = size ;
      _fileNum  = fileNum ;
      _idleSize = _fileSize ;
      _path = string( path ) ;

      // allocate OSS_FILE, free in destructor
      _file = SDB_OSS_NEW _OSS_FILE();
      if ( !_file )
      {
         rc = SDB_OOM;
         PD_LOG ( PDERROR, "new _OSS_FILE failed!" );
         goto error;
      }

      // if the file exist, restore
      if ( SDB_OK == ossAccess( path ) )
      {
         rc = ossOpen ( path, OSS_READWRITE|OSS_SHAREWRITE, OSS_RWXU, *_file ) ;
         if ( rc == SDB_OK )
         {
            rc = _restore () ;
            if ( rc == SDB_OK )
            {
               UINT32 startOffset = 0 ;
               if ( DPS_INVALID_LSN_OFFSET != _logHeader._offset )
               {
                  startOffset = (UINT32)( _logHeader._offset %
                                          _fileSize ) ;
               }
               PD_LOG ( PDEVENT, "Restore dps log file[%s] succeed, "
                        "firstLsn[%lld], idle space: %u, start offset: %d",
                        path, _logHeader._offset, getIdleSize(),
                        startOffset ) ;
               goto done ;
            }
            else
            {
               close () ;
               PD_LOG ( PDEVENT, "Restore dps log file[%s] failed[rc:%d]",
                        path, rc ) ;
               goto error ;
            }
         }
      }

      if ( SDB_OK == ossAccess( path ) )
      {
         rc = ossDelete ( path );
         if ( SDB_IO == rc )
         {
            PD_LOG ( PDERROR, "Failed to delete file at %s", path ) ;
            goto error;
         }
      }

      // open the file with "create only" and "read write" mode, for rx-r-----
      rc = ossOpen( path, OSS_CREATEONLY |OSS_READWRITE | OSS_SHAREWRITE,
                    OSS_RWXU, *_file );

      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to open log file %s, rc = %d", path, rc ) ;
         goto error;
      }

      created = TRUE ;

      // increase the file size to the given size plus log file header
      rc = ossExtendFile( _file, (SINT64)_fileSize + DPS_LOG_HEAD_LEN );
      if ( rc )
      {
         close() ;
         PD_LOG ( PDERROR, "Failed to extend log file size to %d, rc = %d",
                 size + DPS_LOG_HEAD_LEN, rc ) ;
         goto error;
      }

      _initHead ( DPS_INVALID_LOG_FILE_ID ) ;
      rc = _flushHeader () ;
      if ( rc )
      {
         close () ;
         PD_LOG ( PDERROR, "Failed to flush header, rc = %d", rc ) ;
         goto error ;
      }
      // Currently let's just skip head
      rc = ossSeek ( _file, DPS_LOG_HEAD_LEN, OSS_SEEK_SET ) ;
      if ( rc )
      {
         close() ;
         PD_LOG ( PDERROR, "Failed to seek to %d offset in log file, rc = %d",
                 DPS_LOG_HEAD_LEN, rc ) ;
         goto error ;
      }

   done:
      return rc;
   error:
      if ( NULL != _file )
      {
         SDB_OSS_DEL _file;
         _file = NULL ;
      }
      if ( created )
      {
         INT32 rcTmp = SDB_OK ;
         rcTmp = ossDelete( path ) ;
         if ( SDB_OK != rcTmp )
         {
            PD_LOG( PDERROR, "failed to remove new file[%s], rc:%d",
                    path, rc ) ;
         }
      }
      goto done;
   }

   INT32 _dpsLogFile::_restore ()
   {
      INT32 rc = SDB_OK ;
      INT64 fileSize = 0 ;
      dpsLogRecordHeader lsnHeader ;
      CHAR *lastRecord = NULL ;

      _inRestore = TRUE ;

      //Judge the length is right
      rc = ossGetFileSize( _file, &fileSize ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      if ( fileSize < (INT64)( _fileSize + sizeof(dpsLogHeader) ) )
      {
         PD_LOG ( PDERROR, "DPS file size[%d] is smaller than config[%d]",
                  fileSize - sizeof(dpsLogHeader), _fileSize ) ;
         rc = SDB_DPS_FILE_SIZE_NOT_SAME ;
         goto error ;
      }

      //Init header
      rc = _readHeader() ;
      if ( SDB_OK != rc )
      {
         PD_LOG ( PDERROR, "Fail to read dps file header[rc:%d]", rc ) ;
         goto error ;
      }

      // check header info
      if ( ossStrncmp( _logHeader._eyeCatcher, DPS_LOG_HEADER_EYECATCHER,
                       sizeof( _logHeader._eyeCatcher ) ) != 0 )
      {
         PD_LOG( PDERROR, "DPS file eye catcher error" ) ;
         rc = SDB_DPS_FILE_NOT_RECOGNISE ;
         goto error ;
      }
      else if ( _logHeader._fileSize != 0 &&
                _logHeader._fileSize != _fileSize )
      {
         PD_LOG( PDERROR, "DPS file's meta size[%d] is not the same with "
                 "config[%d]", _logHeader._fileSize, _fileSize ) ;
         rc = SDB_DPS_FILE_SIZE_NOT_SAME ;
         goto error ;
      }
      else if ( _logHeader._fileNum != 0 &&
                _logHeader._fileNum != _fileNum )
      {
         PD_LOG( PDERROR, "DPS file's meta file num[%d] is not the same with "
                 "config[%d]", _logHeader._fileNum, _fileNum ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      // check the real size
      if ( fileSize > (INT64)( _fileSize + sizeof(dpsLogHeader) ) )
      {
         PD_LOG( PDERROR, "DPS file real size[%d] is not the same with "
                 "config[%d]", fileSize - sizeof(dpsLogHeader),
                 _fileSize ) ;
      }

      // upgrade the header
      if ( _logHeader._version != DPS_LOG_FILE_VERSION1 )
      {
         _logHeader._version = DPS_LOG_FILE_VERSION1 ;
         _logHeader._fileSize = _fileSize ;
         _logHeader._fileNum  = _fileNum ;

         rc = _flushHeader() ;
         PD_RC_CHECK( rc, PDERROR, "Failed to flush header, rc: %d", rc ) ;
      }

      if ( _logHeader._logID == DPS_INVALID_LOG_FILE_ID )
      {
         goto done ;
      }

      _idleSize = _fileSize - _logHeader._offset ;

   done:
      _inRestore = FALSE ;
      SAFE_OSS_FREE( lastRecord ) ;
      return rc ;
   error:
      goto done ;
   }

   INT32 _dpsLogFile::reset ( UINT32 logID, const UINT64 &offset )
   {
      _logHeader._offset = offset ;
      _logHeader._logID = logID ;
      _idleSize = _fileSize ;
      _dirty = FALSE ;

      INT32 rc = _flushHeader () ;
      return rc ;
   }

   INT32 _dpsLogFile::flushOffset()
   {
     _logHeader._offset = _fileSize - _idleSize ;
     INT32 rc  = _flushHeader() ;
     return rc ;
   }

   INT32 _dpsLogFile::_flushHeader ()
   {
      INT32 rc = SDB_OK ;
      SINT64 writtenLen = 0;
      UINT32 written = 0 ;
      CHAR *buff = (CHAR*)&_logHeader ;

      while ( written < DPS_LOG_HEAD_LEN )
      {
         rc = ossSeekAndWrite( _file, written, &buff[written],
                               DPS_LOG_HEAD_LEN - written, &writtenLen ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to write into file header, rc = %d", rc ) ;
            goto error;
         }
         written += (UINT32)writtenLen ;
      }

      _dirty = TRUE ;
   done:
      return rc;
   error:
      goto done;
   }

   INT32 _dpsLogFile::_readHeader ()
   {
      INT32 rc = SDB_OK ;
      SINT64 readLen = 0 ;
      SINT64 read = 0 ;
      CHAR *buff = (CHAR*)&_logHeader ;

      while ( read < DPS_LOG_HEAD_LEN )
      {
         rc = ossSeekAndRead ( _file, read, &buff[read],
                              DPS_LOG_HEAD_LEN-read, &readLen ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR,  "Failed to ossSeekAndRead, rc = %d", rc ) ;
            goto error;
         }
         read += ( UINT32 )readLen;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

   // write data into log file
   INT32 _dpsLogFile::write( const CHAR *content, UINT32 len )
   {
      INT32 rc = SDB_OK;
      _dirty = TRUE ;

      if ( len <= _idleSize && _idleSize <= _fileSize )
      {
         SINT64 writtenLen = 0 ;
         UINT32 written = 0 ;

         while ( written < len )
         {
            // write data into the file
            // _fileSize - _idleSize is the offset for the current position
            rc = ossSeekAndWrite( _file, DPS_LOG_HEAD_LEN + _fileSize -
                                  _idleSize + written, &content[written],
                                  len - written, &writtenLen ) ;
            if ( SDB_OK == rc )
            {
               written += (UINT32)writtenLen ;
            }
            else
            {
               PD_LOG ( PDERROR, "Failed to write page data into file[%s], "
                        "written: %u, len: %u, rc = %d",
                        toString().c_str(), written, len, rc ) ;
               goto error;
            }
         }
         _idleSize -= len ;
      }
      else if ( len > _idleSize )
      {
         PD_LOG( PDERROR, "Write page data failed, len[%u] grater than "
                 "idle size in log file[%s]", len, toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      else
      {
         PD_LOG( PDERROR, "Wrong idle in log file[%s]", toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      _writeEvent.signalAll() ;

   done:
      return rc;
   error:
      ossPanic() ;
      goto done;
   }

   // read data from a given offset
   INT32 _dpsLogFile::read ( const UINT32 &lOffset, UINT32 len, CHAR *buf )
   {
      INT32 rc = SDB_OK;
      SINT64 readLen = 0;
      UINT32 read = 0 ;
      UINT32 readTimerCounter = 0 ;
      UINT32 offset = lOffset % _fileSize ;
      // make sure we don't read out of range
      SDB_ASSERT ( offset + len <= _fileSize,
                   "Read out of range" ) ;
      // make sure the LSN is within the range
      if ( lOffset < _logHeader._offset ||
           lOffset > _logHeader._offset + _fileSize )
      {
         PD_LOG ( PDERROR, "Unable to find LSN %lld in the file", lOffset ) ;
         rc = SDB_DPS_LOG_NOT_IN_FILE ;
         goto error ;
      }
      /// make sure the read data is all flushed
      while ( !_inRestore && offset + len > getLength() )
      {
         if ( SDB_OK == _writeEvent.wait( OSS_ONE_SEC ) )
         {
            continue ;
         }
         readTimerCounter += OSS_ONE_SEC ;
         if ( readTimerCounter >= DPS_LOGFILE_READ_TIMEOUT )
         {
            break ;
         }
      }

      while ( read < len )
      {
         // seeks to given offset and read
         rc = ossSeekAndRead ( _file, offset + DPS_LOG_HEAD_LEN + read,
                               &buf[read], len-read, &readLen ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to ossSeekAndRead, rc = %d", rc ) ;
            goto error;
         }
         read += ( UINT32 )readLen;
      }
   done:
      return rc;
   error:
      goto done;
   }

   // close the file
   INT32 _dpsLogFile::close()
   {
      return ossClose( *_file );
   }

   INT32 _dpsLogFile::invalidateData()
   {
      INT32 rc = SDB_OK ;
      dpsLogRecordHeader header ;
      const CHAR *pData = ( const CHAR* )&header ;
      UINT32 len = sizeof( header ) ;
      UINT32 idleSize = _idleSize ;

      if ( 0 == idleSize )
      {
         goto done ;
      }
      else if ( len <= idleSize && idleSize <= _fileSize )
      {
         _dirty = TRUE ;

         SINT64 writtenLen = 0 ;
         UINT32 written = 0 ;

         while ( written < len )
         {
            // write data into the file
            // _fileSize - _idleSize is the offset for the current position
            rc = ossSeekAndWrite( _file, DPS_LOG_HEAD_LEN + _fileSize -
                                  idleSize + written, &pData[written],
                                  len - written, &writtenLen ) ;
            if ( SDB_OK == rc )
            {
               written += (UINT32)writtenLen ;
            }
            else
            {
               PD_LOG ( PDERROR, "Failed to write dummy data to file[%s], "
                        "written: %u, len: %u, idleSize: %u , rc = %d",
                        toString().c_str(), written, len, idleSize, rc ) ;
               goto error;
            }
         }
      }
      else
      {
         PD_LOG( PDERROR, "Wrong idle in log file[%s]", toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

   done:
      return rc;
   error:
      goto done;
   }

   INT32 _dpsLogFile::sync()
   {
      INT32 rc = SDB_OK ;
      if ( NULL == _file )
      {
         PD_LOG( PDERROR, "file has not been initialized yet" ) ;
         rc = SDB_SYS ;
         goto error ;
      }
      rc = ossFsync( _file ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "failed to sync file, file no:%d, rc:%d",
                 _fileNum, rc ) ;
         goto error ;
      }      
      _dirty = FALSE ;

   done:
      return rc ;
   error:
      goto done ;
   }
}

