/*    Copyright 2012 SequoiaDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*    Copyright (C) 2011-2014 SequoiaDB Ltd.
 *    This program is free software: you can redistribute it and/or modify
 *    it under the term of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warrenty of
 *    MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program. If not, see <http://www.gnu.org/license/>.
 */


// This Header File is automatically generated, you MUST NOT modify this file anyway!
// On the contrary, you can modify the xml file "sequoiadb/misc/autogen/optlist.xml" if necessary!

#ifndef PMDOPTIONS_HPP_
#define PMDOPTIONS_HPP_

#include "pmdOptions.h"
#define PMD_COMMANDS_OPTIONS \
        ( PMD_COMMANDS_STRING (PMD_OPTION_DBPATH, ",d"), boost::program_options::value<string>(), "Database path" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_IDXPATH, ",i"), boost::program_options::value<string>(), "Index path" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_CONFPATH, ",c"), boost::program_options::value<string>(), "Configure file path" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_LOGPATH, ",l"), boost::program_options::value<string>(), "Log file path" ) \
        ( PMD_OPTION_DIAGLOGPATH, boost::program_options::value<string>(), "Diagnostic log file path" ) \
        ( PMD_OPTION_AUDITLOGPATH, boost::program_options::value<string>(), "Audit log file path" ) \
        ( PMD_OPTION_DIAGLOG_NUM, boost::program_options::value<int>(), "The max number of diagnostic log files, default:20, -1:unlimited" ) \
        ( PMD_OPTION_AUDIT_NUM, boost::program_options::value<int>(), "The max number of audit log files, default:20, -1:unlimited" ) \
        ( PMD_OPTION_BKUPPATH, boost::program_options::value<string>(), "Backup path" ) \
        ( PMD_OPTION_MAXPOOL, boost::program_options::value<int>(), "The maximum number of pooled agent,defalut:50, value range:[0,10000]" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_SVCNAME, ",p"), boost::program_options::value<string>(), "Local service name or port" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_REPLNAME, ",r"), boost::program_options::value<string>(), "Replication service name or port, default: 'svcname'+1" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_SHARDNAME, ",a"), boost::program_options::value<string>(), "Sharding service name or port, default: 'svcname'+2" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_CATANAME, ",x"), boost::program_options::value<string>(), "Catalog service name or port, default: 'svcname'+3" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_RESTNAME, ",s"), boost::program_options::value<string>(), "REST service name or port, default: 'svcname'+4" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_DIAGLEVEL, ",v"), boost::program_options::value<int>(), "Diagnostic level,default:3,value range:[0-5]" ) \
        ( PMD_OPTION_AUDIT_MASK, boost::program_options::value<string>(), "Audit mask,default:SYSTEM|DDL|DCL,value list: ACCESS,CLUSTER,SYSTEM,DML,DDL,DCL,DQL,INSERT,DELETE,UPDATE,OTHER, use '|' join multiple values, ALL enable all mask, NONE disable all mask" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_ROLE, ",o"), boost::program_options::value<string>(), "Role of the node (data/coord/catalog/standalone)" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_CATALOG_ADDR, ",t"), boost::program_options::value<string>(), "Catalog addr (hostname1:servicename1,hostname2:servicename2,...)" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_LOGFILESZ, ",f"), boost::program_options::value<int>(), "Log file size ( in MB ),default:64,value range:[64,2048]" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_LOGFILENUM, ",n"), boost::program_options::value<int>(), "Number of log files,default:20, value range:[1,11800]" ) \
        ( PMD_COMMANDS_STRING (PMD_OPTION_TRANSACTIONON, ",e"), boost::program_options::value<string>(), "Turn on transaction, default:FALSE" ) \
        ( PMD_OPTION_TRANSTIMEOUT, boost::program_options::value<int>(), "Timeout of waiting the transaction-lock(in second), default:60, value range:[0,3600]" ) \
        ( PMD_OPTION_NUMPRELOAD, boost::program_options::value<int>(), "The number of pre-loaders, default:0,value range:[0,100]" ) \
        ( PMD_OPTION_MAX_PREF_POOL, boost::program_options::value<int>(), "The maximum number of prefetchers, default:0, value range:[0,1000]" ) \
        ( PMD_OPTION_MAX_REPL_SYNC, boost::program_options::value<int>(), "The maximum number of repl-sync threads, default:10, value range:[0, 200], 0:disable concurrent repl-sync" ) \
        ( PMD_OPTION_LOGBUFFSIZE, boost::program_options::value<int>(), "The number of pages ( in 64KB ) for replica log memory ( default:1024, value range:[512,1024000] ), the size should be smaller than log space" ) \
        ( PMD_OPTION_DMS_TMPBLKPATH, boost::program_options::value<string>(), "The path of the temp files" ) \
        ( PMD_OPTION_SORTBUF_SIZE, boost::program_options::value<int>(), "Size of the sorting buf(MB), default:256, min value:128" ) \
        ( PMD_OPTION_HJ_BUFSZ, boost::program_options::value<int>(), "Size of the hash join buf(MB), default:128, min value:64" ) \
        ( PMD_OPTION_SYNC_STRATEGY, boost::program_options::value<string>(), "The control strategy of data sync in ReplGroup, value enumeration: none,keepnormal,keepall, default:keepnormal." ) \
        ( PMD_OPTION_PREFINST, boost::program_options::value<string>(), "Prefered instance for read request, default: 'M'. Could be single value in 'M', 'm', 'S', 's', 'A', 'a', 1-255. Or use ',' to join multiple values. Maximum lenght of option string is 256." ) \
        ( PMD_OPTION_PREFINST_MODE, boost::program_options::value<string>(), "The mode to choose query instance when multiple prefered instances are found. Could be 'random' or 'ordered'. Default is 'random'." ) \
        ( PMD_OPTION_INSTANCE_ID, boost::program_options::value<int>(), "Instance ID of node. Default is 0, which means the instance ID is the position of node in the replica group. Valid value could be 1-255." ) \
        ( PMD_OPTION_LOBPATH, boost::program_options::value<string>(), "Large object file path" ) \
        ( PMD_OPTION_LOBMETAPATH, boost::program_options::value<string>(), "Large object meta file path" ) \
        ( PMD_OPTION_DIRECT_IO_IN_LOB, boost::program_options::value<string>(), "Open direct io in large object" ) \
        ( PMD_OPTION_SPARSE_FILE, boost::program_options::value<string>(), "Extend the file as a sparse file" ) \
        ( PMD_OPTION_WEIGHT, boost::program_options::value<int>(), "The weight of election, default is 10, range [1, 100]" ) \
        ( PMD_OPTION_USESSL, boost::program_options::value<string>(), "Allow client to connect with SSL (enterprise edition only), default false" ) \
        ( PMD_OPTION_AUTH, boost::program_options::value<string>(), "Enable authentication. default is true" ) \
        ( PMD_OPTION_IS_ARBITER, boost::program_options::value<string>(), "Set node as an arbiter only. Default is false." ) \
        ( PMD_OPTION_PLAN_BUCKETS, boost::program_options::value<int>(), "The buckets of access plan cache. Sdb will not cache any access plan if it is 0. Default is 500, maximum is 4096. SequoiadDB will round up the value to 0, 128, 256, 512, 1024, 2048, 4096." ) \
        ( PMD_OPTION_OPERATOR_TIMEOUT, boost::program_options::value<int>(), "The timeout period for heartbeat in each operation ( in ms ), default:300000, 0 for no-timeout " ) \
        ( PMD_OPTION_OVER_FLOW_RATIO, boost::program_options::value<int>(), "The overflow ratio(%) of record when inserting, default is 12, value range:[0,10000]" ) \
        ( PMD_OPTION_OM_ADDR, boost::program_options::value<string>(), "Om addr (hostname:omservicename)" ) \
        ( PMD_OPTION_MAX_CACHE_SIZE, boost::program_options::value<int>(), "The node's max cache size, default:0, unit:MB(currently only applicable for Lob feature)" ) \
        ( PMD_OPTION_MAX_CACHE_JOB, boost::program_options::value<int>(), "The maximum number of cache threads, default:10, value range:[2, 200]" ) \
        ( PMD_OPTION_CACHE_MERGE_SIZE, boost::program_options::value<int>(), "The merge block size of collectionspace when use cache, default:0, value range:[0,64], unit:MB(currently only applicable for Lob feature)" ) \
        ( PMD_OPTION_PAGE_ALLOC_TIMEOUT, boost::program_options::value<int>(), "The timeout value for allocating page from cache, default:0, value range:[0,3600000], unit:ms" ) \
        ( PMD_OPTION_MAX_SYNC_JOB, boost::program_options::value<int>(), "The maximum number of data-sync threads, default:10, value range:[2, 200]" ) \
        ( PMD_OPTION_SYNC_INTERVAL, boost::program_options::value<int>(), "The interval time(unit:ms) of data-sync, default:10000, 0 means disable data-sync by time" ) \
        ( PMD_OPTION_SYNC_RECORDNUM, boost::program_options::value<int>(), "The trigger condition of record number of data-sync, default:0, 0 means disable data-sync by record" ) \
        ( PMD_OPTION_SYNC_DEEP, boost::program_options::value<string>(), "Turn on deep mode in data-sync, default:FALSE" ) \
        ( PMD_OPTION_ARCHIVE_ON, boost::program_options::value<string>(), "Turn on replica log archive, default false." ) \
        ( PMD_OPTION_ARCHIVE_COMPRESS_ON, boost::program_options::value<string>(), "Turn on replica log archive compression, default true." ) \
        ( PMD_OPTION_ARCHIVE_PATH, boost::program_options::value<string>(), "Archive log file path" ) \
        ( PMD_OPTION_ARCHIVE_TIMEOUT, boost::program_options::value<int>(), "The timeout interval for archiving( in seconds ), default:600, 0 for no-timeout." ) \
        ( PMD_OPTION_ARCHIVE_EXPIRED, boost::program_options::value<int>(), "The expired time for archive log file ( in hours ), default:240, 0 for no-expired." ) \
        ( PMD_OPTION_ARCHIVE_QUOTA, boost::program_options::value<int>(), "The disk quota for archive log directory ( in GB ), default:10, 0 for no limit." ) \
        ( PMD_OPTION_DATAERROR_OP, boost::program_options::value<int>(), "The node's operation when its data lost. Value is 0/1/2. 0 for none, 1 for fullsync, 2 for shutdown. Default is 1." ) \
        ( PMD_OPTION_MAX_CONN, boost::program_options::value<int>(), "Restrict the max number of clients connecting to the engine, range is [0,30000], default:0, 0:disable restriction" ) \
        ( PMD_OPTION_PLAN_CACHE_LEVEL, boost::program_options::value<int>(), "Specify the cache level of query plans, 0 for no cache, 1 for caching original query, 2 for caching normalized query, 3 for caching parameterized query, 4 for caching parameterized query with fuzzy-matched operators. Default is 3" ) \
        ( PMD_OPTION_SVCSCHEDULER, boost::program_options::value<int>(), "Specify the task scheduler type, 0 for None, 1 for FIFO, 2 for Priority, 3 for Container. Default is 0" ) \
        ( PMD_OPTION_SVC_MAX_CONCURRENCY, boost::program_options::value<int>(), "Specify the maximun concurrency number in scheduler. 0 for unlimited. default is 100. " ) \
        ( PMD_OPTION_TRANS_ISOLATION, boost::program_options::value<int>(), "Specify the isolation level of transaction, 0 for RU, 1 for RC. Default is 0" ) \

#define PMD_HIDDEN_COMMANDS_OPTIONS \
        ( PMD_COMMANDS_STRING (PMD_OPTION_HELP, ",h"), "help" ) \
        ( PMD_OPTION_VERSION, "Database version" ) \
        ( PMD_OPTION_WWWPATH, boost::program_options::value<string>(), "Web service root path" ) \
        ( PMD_OPTION_OMNAME, boost::program_options::value<string>(), "OM service name or port, default: 'svcname'+5" ) \
        ( PMD_OPTION_MAX_SUB_QUERY, boost::program_options::value<int>(), "The maximum number of sub-query for each SQL request, default:10, min value is 0, max value can't more than param 'maxprefpool'" ) \
        ( PMD_OPTION_REPL_BUCKET_SIZE, boost::program_options::value<int>(), "Repl bucket size ( must be the power of 2 ), default is 32, value range:[1,4096]" ) \
        ( PMD_OPTION_MEMDEBUG, boost::program_options::value<string>(), "Enable memory debug, default:FALSE" ) \
        ( PMD_OPTION_MEMDEBUGSIZE, boost::program_options::value<int>(), "Memory debug segment size,default:0, if not zero, the value range:[256,4194304]" ) \
        ( PMD_OPTION_CATALIST, boost::program_options::value<string>(), "Catalog node list(json)" ) \
        ( PMD_OPTION_DPSLOCAL, boost::program_options::value<string>(), "Log the operation from local port, default:FALSE" ) \
        ( PMD_OPTION_TRACEON, boost::program_options::value<string>(), "Turn on trace when starting, default:FALSE" ) \
        ( PMD_OPTION_TRACEBUFSZ, boost::program_options::value<int>(), "Trace buffer size(MB), default:256, value range:[1,1024]" ) \
        ( PMD_OPTION_SHARINGBRK, boost::program_options::value<int>(), "The timeout period for heartbeat in each replica group ( in ms ), default:7000, value range:[5000,300000] " ) \
        ( PMD_OPTION_INDEX_SCAN_STEP, boost::program_options::value<int>(), "Index scan step, default is 100, range:[1, 10000]" ) \
        ( PMD_OPTION_START_SHIFT_TIME, boost::program_options::value<int>(), "Nodes starting shift time(sec), default:600, value range:[0,7200]" ) \
        ( PMD_OPTION_CLUSTER_NAME, boost::program_options::value<string>(), "Cluster name which belonging to" ) \
        ( PMD_OPTION_BUSINESS_NAME, boost::program_options::value<string>(), "Business name which belonging to" ) \
        ( PMD_OPTION_USERTAG, boost::program_options::value<string>(), "User defined tag" ) \
        ( FAP_OPTION_NAME, boost::program_options::value<string>(), "Enable foreign access protocol.Only module name 'fapmongo' is supportted" ) \
        ( PMD_OPTION_EXTEND_THRESHOLD, boost::program_options::value<int>(), "The threshold size( in MB, must be power of 2 or 0 ) of pre extended segment, default is 32, 0 means disable pre-extend, value range:[0, 128]" ) \
        ( PMD_OPTION_SIGNAL_INTERVAL, boost::program_options::value<int>(), "Signal test interval(second), default:0, 0 or less value means disable signal test" ) \
        ( PMD_OPTION_DMS_CHK_INTERVAL, boost::program_options::value<int>(), "The interval(hour) for Cluster to check data storage files ( in hour ), default:0, 0 means disable checking, value range:[0, 240]" ) \
        ( PMD_OPTION_PERF_STAT, boost::program_options::value<string>(), "Whether enable performance statistics or not" ) \
        ( PMD_OPTION_OPT_COST_THRESHOLD, boost::program_options::value<int>(), "The optimizer estimates a number of data pages are in memory, and ignore their IO cost when estimating access plans, default: 20, 0 means to estimate IO cost always, -1 means never to estimate cost" ) \
        ( PMD_OPTION_ENABLE_MIX_CMP, boost::program_options::value<string>(), "Specify the compare mode of query matcher. Enabled means query could match with different BSON canonical types. Otherwise, it means query could only match with the same BSON canonical type. Default is false" ) \

#endif /* PMDOPTIONS_HPP_ */