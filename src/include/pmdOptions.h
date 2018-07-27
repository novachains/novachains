/*    Copyright (C) 2011-2018 SequoiaDB Ltd.
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


// This Header File is automatically generated, you MUST NOT modify this file anyway!
// On the contrary, you can modify the xml file "sequoiadb/misc/autogen/optlist.xml" if necessary!

#ifndef PMDOPTIONS_H_
#define PMDOPTIONS_H_
#define PMD_OPTION_HELP                                                 "help"
#define PMD_OPTION_VERSION                                              "version"
#define PMD_OPTION_DBPATH                                               "dbpath"
#define PMD_OPTION_IDXPATH                                              "indexpath"
#define PMD_OPTION_CONFPATH                                             "confpath"
#define PMD_OPTION_LOGPATH                                              "logpath"
#define PMD_OPTION_DIAGLOGPATH                                          "diagpath"
#define PMD_OPTION_AUDITLOGPATH                                         "auditpath"
#define PMD_OPTION_DIAGLOG_NUM                                          "diagnum"
#define PMD_OPTION_AUDIT_NUM                                            "auditnum"
#define PMD_OPTION_BKUPPATH                                             "bkuppath"
#define PMD_OPTION_WWWPATH                                              "wwwpath"
#define PMD_OPTION_MAXPOOL                                              "maxpool"
#define PMD_OPTION_SVCNAME                                              "svcname"
#define PMD_OPTION_REPLNAME                                             "replname"
#define PMD_OPTION_SHARDNAME                                            "shardname"
#define PMD_OPTION_CATANAME                                             "catalogname"
#define PMD_OPTION_RESTNAME                                             "httpname"
#define PMD_OPTION_OMNAME                                               "omname"
#define PMD_OPTION_DIAGLEVEL                                            "diaglevel"
#define PMD_OPTION_AUDIT_MASK                                           "auditmask"
#define PMD_OPTION_ROLE                                                 "role"
#define PMD_OPTION_CATALOG_ADDR                                         "catalogaddr"
#define PMD_OPTION_LOGFILESZ                                            "logfilesz"
#define PMD_OPTION_LOGFILENUM                                           "logfilenum"
#define PMD_OPTION_TRANSACTIONON                                        "transactionon"
#define PMD_OPTION_TRANSTIMEOUT                                         "transactiontimeout"
#define PMD_OPTION_NUMPRELOAD                                           "numpreload"
#define PMD_OPTION_MAX_PREF_POOL                                        "maxprefpool"
#define PMD_OPTION_MAX_SUB_QUERY                                        "maxsubquery"
#define PMD_OPTION_MAX_REPL_SYNC                                        "maxreplsync"
#define PMD_OPTION_LOGBUFFSIZE                                          "logbuffsize"
#define PMD_OPTION_DMS_TMPBLKPATH                                       "tmppath"
#define PMD_OPTION_SORTBUF_SIZE                                         "sortbuf"
#define PMD_OPTION_HJ_BUFSZ                                             "hjbuf"
#define PMD_OPTION_SYNC_STRATEGY                                        "syncstrategy"
#define PMD_OPTION_REPL_BUCKET_SIZE                                     "replbucketsize"
#define PMD_OPTION_MEMDEBUG                                             "memdebug"
#define PMD_OPTION_MEMDEBUGSIZE                                         "memdebugsize"
#define PMD_OPTION_CATALIST                                             "cataloglist"
#define PMD_OPTION_DPSLOCAL                                             "dpslocal"
#define PMD_OPTION_TRACEON                                              "traceon"
#define PMD_OPTION_TRACEBUFSZ                                           "tracebufsz"
#define PMD_OPTION_SHARINGBRK                                           "sharingbreak"
#define PMD_OPTION_INDEX_SCAN_STEP                                      "indexscanstep"
#define PMD_OPTION_START_SHIFT_TIME                                     "startshifttime"
#define PMD_OPTION_PREFINST                                             "preferedinstance"
#define PMD_OPTION_PREFINST_MODE                                        "preferedinstancemode"
#define PMD_OPTION_INSTANCE_ID                                          "instanceid"
#define PMD_OPTION_LOBPATH                                              "lobpath"
#define PMD_OPTION_LOBMETAPATH                                          "lobmetapath"
#define PMD_OPTION_DIRECT_IO_IN_LOB                                     "directioinlob"
#define PMD_OPTION_SPARSE_FILE                                          "sparsefile"
#define PMD_OPTION_CLUSTER_NAME                                         "clustername"
#define PMD_OPTION_BUSINESS_NAME                                        "businessname"
#define PMD_OPTION_USERTAG                                              "usertag"
#define PMD_OPTION_WEIGHT                                               "weight"
#define PMD_OPTION_USESSL                                               "usessl"
#define PMD_OPTION_AUTH                                                 "auth"
#define FAP_OPTION_NAME                                                 "fap"
#define PMD_OPTION_IS_ARBITER                                           "arbiter"
#define PMD_OPTION_PLAN_BUCKETS                                         "planbuckets"
#define PMD_OPTION_OPERATOR_TIMEOUT                                     "optimeout"
#define PMD_OPTION_OVER_FLOW_RATIO                                      "overflowratio"
#define PMD_OPTION_EXTEND_THRESHOLD                                     "extendthreshold"
#define PMD_OPTION_SIGNAL_INTERVAL                                      "signalinterval"
#define PMD_OPTION_OM_ADDR                                              "omaddr"
#define PMD_OPTION_MAX_CACHE_SIZE                                       "maxcachesize"
#define PMD_OPTION_MAX_CACHE_JOB                                        "maxcachejob"
#define PMD_OPTION_CACHE_MERGE_SIZE                                     "cachemergesz"
#define PMD_OPTION_PAGE_ALLOC_TIMEOUT                                   "pagealloctimeout"
#define PMD_OPTION_MAX_SYNC_JOB                                         "maxsyncjob"
#define PMD_OPTION_SYNC_INTERVAL                                        "syncinterval"
#define PMD_OPTION_SYNC_RECORDNUM                                       "syncrecordnum"
#define PMD_OPTION_SYNC_DEEP                                            "syncdeep"
#define PMD_OPTION_ARCHIVE_ON                                           "archiveon"
#define PMD_OPTION_ARCHIVE_COMPRESS_ON                                  "archivecompresson"
#define PMD_OPTION_ARCHIVE_PATH                                         "archivepath"
#define PMD_OPTION_ARCHIVE_TIMEOUT                                      "archivetimeout"
#define PMD_OPTION_ARCHIVE_EXPIRED                                      "archiveexpired"
#define PMD_OPTION_ARCHIVE_QUOTA                                        "archivequota"
#define PMD_OPTION_DATAERROR_OP                                         "dataerrorop"
#define PMD_OPTION_DMS_CHK_INTERVAL                                     "dmschkinterval"
#define PMD_OPTION_PERF_STAT                                            "perfstat"
#define PMD_OPTION_OPT_COST_THRESHOLD                                   "optcostthreshold"
#define PMD_OPTION_MAX_CONN                                             "maxconn"
#define PMD_OPTION_ENABLE_MIX_CMP                                       "enablemixcmp"
#define PMD_OPTION_PLAN_CACHE_LEVEL                                     "plancachelevel"
#define PMD_OPTION_SVCSCHEDULER                                         "svcscheduler"
#define PMD_OPTION_SVC_MAX_CONCURRENCY                                  "svcmaxconcurrency"
#define PMD_OPTION_TRANS_ISOLATION                                      "transisolation"
#endif
