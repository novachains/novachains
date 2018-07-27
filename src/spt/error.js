/* Error Constants */
const SDB_MAX_ERROR                     = 1024  ;
const SDB_MAX_WARNING                   = 1024  ;
const SDB_OK                            = 0     ;

/* Error Codes */
const SDB_IO                            = -1    ; // IO Exception;
const SDB_OOM                           = -2    ; // Out of Memory;
const SDB_PERM                          = -3    ; // Permission Error;
const SDB_FNE                           = -4    ; // File Not Exist;
const SDB_FE                            = -5    ; // File Exist;
const SDB_INVALIDARG                    = -6    ; // Invalid Argument;
const SDB_INVALIDSIZE                   = -7    ; // Invalid size;
const SDB_INTERRUPT                     = -8    ; // Interrupt;
const SDB_EOF                           = -9    ; // Hit end of file;
const SDB_SYS                           = -10   ; // System error;
const SDB_NOSPC                         = -11   ; // No space is left on disk;
const SDB_EDU_INVAL_STATUS              = -12   ; // EDU status is not valid;
const SDB_TIMEOUT                       = -13   ; // Timeout error;
const SDB_QUIESCED                      = -14   ; // Database is quiesced;
const SDB_NETWORK                       = -15   ; // Network error;
const SDB_NETWORK_CLOSE                 = -16   ; // Network is closed from remote;
const SDB_DATABASE_DOWN                 = -17   ; // Database is in shutdown status;
const SDB_APP_FORCED                    = -18   ; // Application is forced;
const SDB_INVALIDPATH                   = -19   ; // Given path is not valid;
const SDB_INVALID_FILE_TYPE             = -20   ; // Unexpected file type specified;
const SDB_DMS_NOSPC                     = -21   ; // There's no space for DMS;
const SDB_DMS_EXIST                     = -22   ; // Collection already exists;
const SDB_DMS_NOTEXIST                  = -23   ; // Collection does not exist;
const SDB_DMS_RECORD_TOO_BIG            = -24   ; // User record is too large;
const SDB_DMS_RECORD_NOTEXIST           = -25   ; // Record does not exist;
const SDB_DMS_OVF_EXIST                 = -26   ; // Remote overflow record exists;
const SDB_DMS_RECORD_INVALID            = -27   ; // Invalid record;
const SDB_DMS_SU_NEED_REORG             = -28   ; // Storage unit need reorg;
const SDB_DMS_EOC                       = -29   ; // End of collection;
const SDB_DMS_CONTEXT_IS_OPEN           = -30   ; // Context is already opened;
const SDB_DMS_CONTEXT_IS_CLOSE          = -31   ; // Context is closed;
const SDB_OPTION_NOT_SUPPORT            = -32   ; // Option is not supported yet;
const SDB_DMS_CS_EXIST                  = -33   ; // Collection space already exists;
const SDB_DMS_CS_NOTEXIST               = -34   ; // Collection space does not exist;
const SDB_DMS_INVALID_SU                = -35   ; // Storage unit file is invalid;
const SDB_RTN_CONTEXT_NOTEXIST          = -36   ; // Context does not exist;
const SDB_IXM_MULTIPLE_ARRAY            = -37   ; // More than one fields are array type;
const SDB_IXM_DUP_KEY                   = -38   ; // Duplicate key exist;
const SDB_IXM_KEY_TOO_LARGE             = -39   ; // Index key is too large;
const SDB_IXM_NOSPC                     = -40   ; // No space can be found for index extent;
const SDB_IXM_KEY_NOTEXIST              = -41   ; // Index key does not exist;
const SDB_DMS_MAX_INDEX                 = -42   ; // Hit max number of index;
const SDB_DMS_INIT_INDEX                = -43   ; // Failed to initialize index;
const SDB_DMS_COL_DROPPED               = -44   ; // Collection is dropped;
const SDB_IXM_IDENTICAL_KEY             = -45   ; // Two records get same key and rid;
const SDB_IXM_EXIST                     = -46   ; // Duplicate index name;
const SDB_IXM_NOTEXIST                  = -47   ; // Index name does not exist;
const SDB_IXM_UNEXPECTED_STATUS         = -48   ; // Unexpected index flag;
const SDB_IXM_EOC                       = -49   ; // Hit end of index;
const SDB_IXM_DEDUP_BUF_MAX             = -50   ; // Hit the max of dedup buffer;
const SDB_RTN_INVALID_PREDICATES        = -51   ; // Invalid predicates;
const SDB_RTN_INDEX_NOTEXIST            = -52   ; // Index does not exist;
const SDB_RTN_INVALID_HINT              = -53   ; // Invalid hint;
const SDB_DMS_NO_MORE_TEMP              = -54   ; // No more temp collections are avaliable;
const SDB_DMS_SU_OUTRANGE               = -55   ; // Exceed max number of storage unit;
const SDB_IXM_DROP_ID                   = -56   ; // $id index can't be dropped;
const SDB_DPS_LOG_NOT_IN_BUF            = -57   ; // Log was not found in log buf;
const SDB_DPS_LOG_NOT_IN_FILE           = -58   ; // Log was not found in log file;
const SDB_PMD_RG_NOT_EXIST              = -59   ; // Replication group does not exist;
const SDB_PMD_RG_EXIST                  = -60   ; // Replication group exists;
const SDB_INVALID_REQID                 = -61   ; // Invalid request id is received;
const SDB_PMD_SESSION_NOT_EXIST         = -62   ; // Session ID does not exist;
const SDB_PMD_FORCE_SYSTEM_EDU          = -63   ; // System EDU cannot be forced;
const SDB_NOT_CONNECTED                 = -64   ; // Database is not connected;
const SDB_UNEXPECTED_RESULT             = -65   ; // Unexpected result received;
const SDB_CORRUPTED_RECORD              = -66   ; // Corrupted record;
const SDB_BACKUP_HAS_ALREADY_START      = -67   ; // Backup has already been started;
const SDB_BACKUP_NOT_COMPLETE           = -68   ; // Backup is not completed;
const SDB_RTN_IN_BACKUP                 = -69   ; // Backup is in progress;
const SDB_BAR_DAMAGED_BK_FILE           = -70   ; // Backup is corrupted;
const SDB_RTN_NO_PRIMARY_FOUND          = -71   ; // No primary node was found;
const SDB_ERROR_RESERVED_1              = -72   ; // Reserved;
const SDB_PMD_HELP_ONLY                 = -73   ; // Engine help argument is specified;
const SDB_PMD_CON_INVALID_STATE         = -74   ; // Invalid connection state;
const SDB_CLT_INVALID_HANDLE            = -75   ; // Invalid handle;
const SDB_CLT_OBJ_NOT_EXIST             = -76   ; // Object does not exist;
const SDB_NET_ALREADY_LISTENED          = -77   ; // Listening port is already occupied;
const SDB_NET_CANNOT_LISTEN             = -78   ; // Unable to listen the specified address;
const SDB_NET_CANNOT_CONNECT            = -79   ; // Unable to connect to the specified address;
const SDB_NET_NOT_CONNECT               = -80   ; // Connection does not exist;
const SDB_NET_SEND_ERR                  = -81   ; // Failed to send;
const SDB_NET_TIMER_ID_NOT_FOUND        = -82   ; // Timer does not exist;
const SDB_NET_ROUTE_NOT_FOUND           = -83   ; // Route info does not exist;
const SDB_NET_BROKEN_MSG                = -84   ; // Broken msg;
const SDB_NET_INVALID_HANDLE            = -85   ; // Invalid net handle;
const SDB_DMS_INVALID_REORG_FILE        = -86   ; // Invalid reorg file;
const SDB_DMS_REORG_FILE_READONLY       = -87   ; // Reorg file is in read only mode;
const SDB_DMS_INVALID_COLLECTION_S      = -88   ; // Collection status is not valid;
const SDB_DMS_NOT_IN_REORG              = -89   ; // Collection is not in reorg state;
const SDB_REPL_GROUP_NOT_ACTIVE         = -90   ; // Replication group is not activated;
const SDB_REPL_INVALID_GROUP_MEMBER     = -91   ; // Node does not belong to the group;
const SDB_DMS_INCOMPATIBLE_MODE         = -92   ; // Collection status is not compatible;
const SDB_DMS_INCOMPATIBLE_VERSION      = -93   ; // Incompatible version for storage unit;
const SDB_REPL_LOCAL_G_V_EXPIRED        = -94   ; // Version is expired for local group;
const SDB_DMS_INVALID_PAGESIZE          = -95   ; // Invalid page size;
const SDB_REPL_REMOTE_G_V_EXPIRED       = -96   ; // Version is expired for remote group;
const SDB_CLS_VOTE_FAILED               = -97   ; // Failed to vote for primary;
const SDB_DPS_CORRUPTED_LOG             = -98   ; // Log record is corrupted;
const SDB_DPS_LSN_OUTOFRANGE            = -99   ; // LSN is out of boundary;
const SDB_UNKNOWN_MESSAGE               = -100  ; // Unknown mesage is received;
const SDB_NET_UPDATE_EXISTING_NODE      = -101  ; // Updated information is same as old one;
const SDB_CLS_UNKNOW_MSG                = -102  ; // Unknown message;
const SDB_CLS_EMPTY_HEAP                = -103  ; // Empty heap;
const SDB_CLS_NOT_PRIMARY               = -104  ; // Node is not primary;
const SDB_CLS_NODE_NOT_ENOUGH           = -105  ; // Not enough number of data nodes;
const SDB_CLS_NO_CATALOG_INFO           = -106  ; // Catalog information does not exist on data node;
const SDB_CLS_DATA_NODE_CAT_VER_OLD     = -107  ; // Catalog version is expired on data node;
const SDB_CLS_COORD_NODE_CAT_VER_OLD    = -108  ; // Catalog version is expired on coordinator node;
const SDB_CLS_INVALID_GROUP_NUM         = -109  ; // Exceeds the max group size;
const SDB_CLS_SYNC_FAILED               = -110  ; // Failed to sync log;
const SDB_CLS_REPLAY_LOG_FAILED         = -111  ; // Failed to replay log;
const SDB_REST_EHS                      = -112  ; // Invalid HTTP header;
const SDB_CLS_CONSULT_FAILED            = -113  ; // Failed to negotiate;
const SDB_DPS_MOVE_FAILED               = -114  ; // Failed to change DPS metadata;
const SDB_DMS_CORRUPTED_SME             = -115  ; // SME is corrupted;
const SDB_APP_INTERRUPT                 = -116  ; // Application is interrupted;
const SDB_APP_DISCONNECT                = -117  ; // Application is disconnected;
const SDB_OSS_CCE                       = -118  ; // Character encoding errors;
const SDB_COORD_QUERY_FAILED            = -119  ; // Failed to query on coord node;
const SDB_CLS_BUFFER_FULL               = -120  ; // Buffer array is full;
const SDB_RTN_SUBCONTEXT_CONFLICT       = -121  ; // Sub context is conflict;
const SDB_COORD_QUERY_EOC               = -122  ; // EOC message is received by coordinator node;
const SDB_DPS_FILE_SIZE_NOT_SAME        = -123  ; // Size of DPS files are not the same;
const SDB_DPS_FILE_NOT_RECOGNISE        = -124  ; // Invalid DPS log file;
const SDB_OSS_NORES                     = -125  ; // No resource is avaliable;
const SDB_DPS_INVALID_LSN               = -126  ; // Invalid LSN;
const SDB_OSS_NPIPE_DATA_TOO_BIG        = -127  ; // Pipe buffer size is too small;
const SDB_CAT_AUTH_FAILED               = -128  ; // Catalog authentication failed;
const SDB_CLS_FULL_SYNC                 = -129  ; // Full sync is in progress;
const SDB_CAT_ASSIGN_NODE_FAILED        = -130  ; // Failed to assign data node from coordinator node;
const SDB_PHP_DRIVER_INTERNAL_ERROR     = -131  ; // PHP driver internal error;
const SDB_COORD_SEND_MSG_FAILED         = -132  ; // Failed to send the message;
const SDB_CAT_NO_NODEGROUP_INFO         = -133  ; // No activated group information on catalog;
const SDB_COORD_REMOTE_DISC             = -134  ; // Remote-node is disconnected;
const SDB_CAT_NO_MATCH_CATALOG          = -135  ; // Unable to find the matched catalog information;
const SDB_CLS_UPDATE_CAT_FAILED         = -136  ; // Failed to update catalog;
const SDB_COORD_UNKNOWN_OP_REQ          = -137  ; // Unknown request operation code;
const SDB_COOR_NO_NODEGROUP_INFO        = -138  ; // Group information cannot be found on coordinator node;
const SDB_DMS_CORRUPTED_EXTENT          = -139  ; // DMS extent is corrupted;
const SDBCM_FAIL                        = -140  ; // Remote cluster manager failed;
const SDBCM_STOP_PART                   = -141  ; // Remote database services have been stopped;
const SDBCM_SVC_STARTING                = -142  ; // Service is starting;
const SDBCM_SVC_STARTED                 = -143  ; // Service has been started;
const SDBCM_SVC_RESTARTING              = -144  ; // Service is restarting;
const SDBCM_NODE_EXISTED                = -145  ; // Node already exists;
const SDBCM_NODE_NOTEXISTED             = -146  ; // Node does not exist;
const SDB_LOCK_FAILED                   = -147  ; // Unable to lock;
const SDB_DMS_STATE_NOT_COMPATIBLE      = -148  ; // DMS state is not compatible with current command;
const SDB_REBUILD_HAS_ALREADY_START     = -149  ; // Database rebuild is already started;
const SDB_RTN_IN_REBUILD                = -150  ; // Database rebuild is in progress;
const SDB_RTN_COORD_CACHE_EMPTY         = -151  ; // Cache is empty on coordinator node;
const SDB_SPT_EVAL_FAIL                 = -152  ; // Evalution failed with error;
const SDB_CAT_GRP_EXIST                 = -153  ; // Group already exist;
const SDB_CLS_GRP_NOT_EXIST             = -154  ; // Group does not exist;
const SDB_CLS_NODE_NOT_EXIST            = -155  ; // Node does not exist;
const SDB_CM_RUN_NODE_FAILED            = -156  ; // Failed to start the node;
const SDB_CM_CONFIG_CONFLICTS           = -157  ; // Invalid node configuration;
const SDB_CLS_EMPTY_GROUP               = -158  ; // Group is empty;
const SDB_RTN_COORD_ONLY                = -159  ; // The operation is for coord node only;
const SDB_CM_OP_NODE_FAILED             = -160  ; // Failed to operate on node;
const SDB_RTN_MUTEX_JOB_EXIST           = -161  ; // The mutex job already exist;
const SDB_RTN_JOB_NOT_EXIST             = -162  ; // The specified job does not exist;
const SDB_CAT_CORRUPTION                = -163  ; // The catalog information is corrupted;
const SDB_IXM_DROP_SHARD                = -164  ; // $shard index can't be dropped;
const SDB_RTN_CMD_NO_NODE_AUTH          = -165  ; // The command can't be run in the node;
const SDB_RTN_CMD_NO_SERVICE_AUTH       = -166  ; // The command can't be run in the service plane;
const SDB_CLS_NO_GROUP_INFO             = -167  ; // The group info not exist;
const SDB_CLS_GROUP_NAME_CONFLICT       = -168  ; // Group name is conflict;
const SDB_COLLECTION_NOTSHARD           = -169  ; // The collection is not sharded;
const SDB_INVALID_SHARDINGKEY           = -170  ; // The record does not contains valid sharding key;
const SDB_TASK_EXIST                    = -171  ; // A task that already exists does not compatible with the new task;
const SDB_CL_NOT_EXIST_ON_GROUP         = -172  ; // The collection does not exists on the specified group;
const SDB_CAT_TASK_NOTFOUND             = -173  ; // The specified task does not exist;
const SDB_MULTI_SHARDING_KEY            = -174  ; // The record contains more than one sharding key;
const SDB_CLS_MUTEX_TASK_EXIST          = -175  ; // The mutex task already exist;
const SDB_CLS_BAD_SPLIT_KEY             = -176  ; // The split key is not valid or not in the source group;
const SDB_SHARD_KEY_NOT_IN_UNIQUE_KEY   = -177  ; // The unique index must include all fields in sharding key;
const SDB_UPDATE_SHARD_KEY              = -178  ; // Sharding key cannot be updated;
const SDB_AUTH_AUTHORITY_FORBIDDEN      = -179  ; // Authority is forbidden;
const SDB_CAT_NO_ADDR_LIST              = -180  ; // There is no catalog address specified by user;
const SDB_CURRENT_RECORD_DELETED        = -181  ; // Current record has been removed;
const SDB_QGM_MATCH_NONE                = -182  ; // No records can be matched for the search condition;
const SDB_IXM_REORG_DONE                = -183  ; // Index page is reorged and the pos got different lchild;
const SDB_RTN_DUPLICATE_FIELDNAME       = -184  ; // Duplicate field name exists in the record;
const SDB_QGM_MAX_NUM_RECORD            = -185  ; // Too many records to be inserted at once;
const SDB_QGM_MERGE_JOIN_EQONLY         = -186  ; // Sort-Merge Join only supports equal predicates;
const SDB_PD_TRACE_IS_STARTED           = -187  ; // Trace is already started;
const SDB_PD_TRACE_HAS_NO_BUFFER        = -188  ; // Trace buffer does not exist;
const SDB_PD_TRACE_FILE_INVALID         = -189  ; // Trace file is not valid;
const SDB_DPS_TRANS_LOCK_INCOMPATIBLE   = -190  ; // Incompatible lock;
const SDB_DPS_TRANS_DOING_ROLLBACK      = -191  ; // Rollback operation is in progress;
const SDB_MIG_IMP_BAD_RECORD            = -192  ; // Invalid record is found during import;
const SDB_QGM_REPEAT_VAR_NAME           = -193  ; // Repeated variable name;
const SDB_QGM_AMBIGUOUS_FIELD           = -194  ; // Column name is ambiguous;
const SDB_SQL_SYNTAX_ERROR              = -195  ; // SQL syntax error;
const SDB_DPS_TRANS_NO_TRANS            = -196  ; // Invalid transactional operation;
const SDB_DPS_TRANS_APPEND_TO_WAIT      = -197  ; // Append to lock-wait-queue;
const SDB_DMS_DELETING                  = -198  ; // Record is deleted;
const SDB_DMS_INVALID_INDEXCB           = -199  ; // Index is dropped or invalid;
const SDB_COORD_RECREATE_CATALOG        = -200  ; // Unable to create new catalog when there's already one exists;
const SDB_UTIL_PARSE_JSON_INVALID       = -201  ; // Failed to parse JSON file;
const SDB_UTIL_PARSE_CSV_INVALID        = -202  ; // Failed to parse CSV file;
const SDB_DPS_LOG_FILE_OUT_OF_SIZE      = -203  ; // Log file size is too large;
const SDB_CATA_RM_NODE_FORBIDDEN        = -204  ; // Unable to remove the last node or primary in a group;
const SDB_CATA_FAILED_TO_CLEANUP        = -205  ; // Unable to clean up catalog, manual cleanup may be required;
const SDB_CATA_RM_CATA_FORBIDDEN        = -206  ; // Unable to remove primary catalog or catalog group for non-empty database;
const SDB_ERROR_RESERVED_2              = -207  ; // Reserved;
const SDB_CAT_RM_GRP_FORBIDDEN          = -208  ; // Unable to remove non-empty group;
const SDB_MIG_END_OF_QUEUE              = -209  ; // End of queue;
const SDB_COORD_SPLIT_NO_SHDIDX         = -210  ; // Unable to split because of no sharding index exists;
const SDB_FIELD_NOT_EXIST               = -211  ; // The parameter field does not exist;
const SDB_TOO_MANY_TRACE_BP             = -212  ; // Too many break points are specified;
const SDB_BUSY_PREFETCHER               = -213  ; // All prefetchers are busy;
const SDB_CAT_DOMAIN_NOT_EXIST          = -214  ; // Domain does not exist;
const SDB_CAT_DOMAIN_EXIST              = -215  ; // Domain already exists;
const SDB_CAT_GROUP_NOT_IN_DOMAIN       = -216  ; // Group is not in domain;
const SDB_CLS_SHARDING_NOT_HASH         = -217  ; // Sharding type is not hash;
const SDB_CLS_SPLIT_PERCENT_LOWER       = -218  ; // split percentage is lower then expected;
const SDB_TASK_ALREADY_FINISHED         = -219  ; // Task is already finished;
const SDB_COLLECTION_LOAD               = -220  ; // Collection is in loading status;
const SDB_LOAD_ROLLBACK                 = -221  ; // Rolling back load operation;
const SDB_INVALID_ROUTEID               = -222  ; // RouteID is different from the local;
const SDB_DUPLICATED_SERVICE            = -223  ; // Service already exists;
const SDB_UTIL_NOT_FIND_FIELD           = -224  ; // Field is not found;
const SDB_UTIL_CSV_FIELD_END            = -225  ; // csv field line end;
const SDB_MIG_UNKNOW_FILE_TYPE          = -226  ; // Unknown file type;
const SDB_RTN_EXPORTCONF_NOT_COMPLETE   = -227  ; // Exporting configuration does not complete in all nodes;
const SDB_CLS_NOTP_AND_NODATA           = -228  ; // Empty non-primary node;
const SDB_DMS_SECRETVALUE_NOT_SAME      = -229  ; // Secret value for index file does not match with data file;
const SDB_PMD_VERSION_ONLY              = -230  ; // Engine version argument is specified;
const SDB_SDB_HELP_ONLY                 = -231  ; // Help argument is specified;
const SDB_SDB_VERSION_ONLY              = -232  ; // Version argument is specified;
const SDB_FMP_FUNC_NOT_EXIST            = -233  ; // Stored procedure does not exist;
const SDB_ILL_RM_SUB_CL                 = -234  ; // Unable to remove collection partition;
const SDB_RELINK_SUB_CL                 = -235  ; // Duplicated attach collection partition;
const SDB_INVALID_MAIN_CL               = -236  ; // Invalid partitioned-collection;
const SDB_BOUND_CONFLICT                = -237  ; // New boundary is conflict with the existing boundary;
const SDB_BOUND_INVALID                 = -238  ; // Invalid boundary for the shard;
const SDB_HIT_HIGH_WATERMARK            = -239  ; // Hit the high water mark;
const SDB_BAR_BACKUP_EXIST              = -240  ; // Backup already exists;
const SDB_BAR_BACKUP_NOTEXIST           = -241  ; // Backup does not exist;
const SDB_INVALID_SUB_CL                = -242  ; // Invalid collection partition;
const SDB_TASK_HAS_CANCELED             = -243  ; // Task is canceled;
const SDB_INVALID_MAIN_CL_TYPE          = -244  ; // Sharding type must be ranged partition for partitioned-collection;
const SDB_NO_SHARDINGKEY                = -245  ; // There is no valid sharding-key defined;
const SDB_MAIN_CL_OP_ERR                = -246  ; // Operation is not supported on partitioned-collection;
const SDB_IXM_REDEF                     = -247  ; // Redefine index;
const SDB_DMS_CS_DELETING               = -248  ; // Dropping the collection space is in progress;
const SDB_DMS_REACHED_MAX_NODES         = -249  ; // Hit the limit of maximum number of nodes in the cluster;
const SDB_CLS_NODE_BSFAULT              = -250  ; // The node is not in normal status;
const SDB_CLS_NODE_INFO_EXPIRED         = -251  ; // Node information is expired;
const SDB_CLS_WAIT_SYNC_FAILED          = -252  ; // Failed to wait for the sync operation from secondary nodes;
const SDB_DPS_TRANS_DIABLED             = -253  ; // Transaction is disabled;
const SDB_DRIVER_DS_RUNOUT              = -254  ; // Data source is running out of connection pool;
const SDB_TOO_MANY_OPEN_FD              = -255  ; // Too many opened file description;
const SDB_DOMAIN_IS_OCCUPIED            = -256  ; // Domain is not empty;
const SDB_REST_RECV_SIZE                = -257  ; // The data received by REST is larger than the max size;
const SDB_DRIVER_BSON_ERROR             = -258  ; // Failed to build bson object;
const SDB_OUT_OF_BOUND                  = -259  ; // Stored procedure arguments are out of bound;
const SDB_REST_COMMON_UNKNOWN           = -260  ; // Unknown REST command;
const SDB_BUT_FAILED_ON_DATA            = -261  ; // Failed to execute command on data node;
const SDB_CAT_NO_GROUP_IN_DOMAIN        = -262  ; // The domain is empty;
const SDB_OM_PASSWD_CHANGE_SUGGUEST     = -263  ; // Changing password is required;
const SDB_COORD_NOT_ALL_DONE            = -264  ; // One or more nodes did not complete successfully;
const SDB_OMA_DIFF_VER_AGT_IS_RUNNING   = -265  ; // There is another OM Agent running with different version;
const SDB_OM_TASK_NOT_EXIST             = -266  ; // Task does not exist;
const SDB_OM_TASK_ROLLBACK              = -267  ; // Task is rolling back;
const SDB_LOB_SEQUENCE_NOT_EXIST        = -268  ; // LOB sequence does not exist;
const SDB_LOB_IS_NOT_AVAILABLE          = -269  ; // LOB is not useable;
const SDB_MIG_DATA_NON_UTF              = -270  ; // Data is not in UTF-8 format;
const SDB_OMA_TASK_FAIL                 = -271  ; // Task failed;
const SDB_LOB_NOT_OPEN                  = -272  ; // Lob does not open;
const SDB_LOB_HAS_OPEN                  = -273  ; // Lob has been open;
const SDBCM_NODE_IS_IN_RESTORING        = -274  ; // Node is in restoring;
const SDB_DMS_CS_NOT_EMPTY              = -275  ; // There are some collections in the collection space;
const SDB_CAT_LOCALHOST_CONFLICT        = -276  ; // 'localhost' and '127.0.0.1' cannot be used mixed with other hostname and IP address;
const SDB_CAT_NOT_LOCALCONN             = -277  ; // If use 'localhost' and '127.0.0.1' for hostname, coord and catalog must in the same host ;
const SDB_CAT_IS_NOT_DATAGROUP          = -278  ; // The special group is not data group;
const SDB_RTN_AUTOINDEXID_IS_FALSE      = -279  ; // can not update/delete records when $id index does not exist;
const SDB_CLS_CAN_NOT_STEP_UP           = -280  ; // can not step up when primary node exists or LSN is not the biggest;
const SDB_CAT_IMAGE_ADDR_CONFLICT       = -281  ; // Image address is conflict with the self cluster;
const SDB_CAT_GROUP_HASNOT_IMAGE        = -282  ; // The data group does not have image group;
const SDB_CAT_GROUP_HAS_IMAGE           = -283  ; // The data group has image group;
const SDB_CAT_IMAGE_IS_ENABLED          = -284  ; // The image is in enabled status;
const SDB_CAT_IMAGE_NOT_CONFIG          = -285  ; // The cluster's image does not configured;
const SDB_CAT_DUAL_WRITABLE             = -286  ; // This cluster and image cluster is both writable;
const SDB_CAT_CLUSTER_IS_READONLY       = -287  ; // This cluster is readonly;
const SDB_RTN_QUERYMODIFY_SORT_NO_IDX   = -288  ; // Sorting of 'query and modify' must use index;
const SDB_RTN_QUERYMODIFY_MULTI_NODES   = -289  ; // 'query and modify' can't use skip and limit in multiple nodes or sub-collections;
const SDB_DIR_NOT_EMPTY                 = -290  ; // Given path is not empty;
const SDB_IXM_EXIST_COVERD_ONE          = -291  ; // Exist one index which can cover this scene;
const SDB_CAT_IMAGE_IS_CONFIGURED       = -292  ; // The cluster's image has already configured;
const SDB_RTN_CMD_IN_LOCAL_MODE         = -293  ; // The command is in local mode;
const SDB_SPT_NOT_SPECIAL_JSON          = -294  ; // The object is not a special object in sdb shell;
const SDB_AUTH_USER_ALREADY_EXIST       = -295  ; // The specified user already exist;
const SDB_DMS_EMPTY_COLLECTION          = -296  ; // The collection is empty;
const SDB_LOB_SEQUENCE_EXISTS           = -297  ; // LOB sequence exists;
const SDB_OM_CLUSTER_NOT_EXIST          = -298  ; // cluster do not exist;
const SDB_OM_BUSINESS_NOT_EXIST         = -299  ; // business do not exist;
const SDB_AUTH_USER_NOT_EXIST           = -300  ; // user specified is not exist or password is invalid;
const SDB_UTIL_COMPRESS_INIT_FAIL       = -301  ; // Compression initialization failed;
const SDB_UTIL_COMPRESS_FAIL            = -302  ; // Compression failed;
const SDB_UTIL_DECOMPRESS_FAIL          = -303  ; // Decompression failed;
const SDB_UTIL_COMPRESS_ABORT           = -304  ; // Compression abort;
const SDB_UTIL_COMPRESS_BUFF_SMALL      = -305  ; // Buffer for compression is too small;
const SDB_UTIL_DECOMPRESS_BUFF_SMALL    = -306  ; // Buffer for decompression is too small;
const SDB_OSS_UP_TO_LIMIT               = -307  ; // Up to the limit;
const SDB_DS_NOT_ENABLE                 = -308  ; // data source is not enabled yet;
const SDB_DS_NO_REACHABLE_COORD         = -309  ; // No reachable coord notes;
const SDB_RULE_ID_IS_NOT_EXIST          = -310  ; // the record which exclusive ruleID is not exist;
const SDB_STRTGY_TASK_NAME_CONFLICT     = -311  ; // Task name conflict;
const SDB_STRTGY_TASK_NOT_EXISTED       = -312  ; // The task is not existed;
const SDB_DPS_LOG_NOT_ARCHIVED          = -313  ; // Replica log is not archived;
const SDB_DS_NOT_INIT                   = -314  ; // Data source has not been initialized;
const SDB_OPERATION_INCOMPATIBLE        = -315  ; // Operation is incompatible with the object;
const SDB_CAT_CLUSTER_IS_DEACTIVED      = -316  ; // This cluster is deactived;
const SDB_LOB_IS_IN_USE                 = -317  ; // LOB is in use;
const SDB_VALUE_OVERFLOW                = -318  ; // Data operation is overflowed;
const SDB_LOB_PIECESINFO_OVERFLOW       = -319  ; // LOB's pieces info is overflowed;
const SDB_LOB_LOCK_CONFLICTED           = -320  ; // LOB lock is conflicted;
const SDB_DMS_TRUNCATED                 = -321  ; // Collection is truncated;
const SDB_RTN_CONF_NOT_TAKE_EFFECT      = -322  ; // Some configuration changes didn't take effect;

function _getErr (errCode) {
   var errDesp = [ 
      "Succeed",
      "IO Exception",
      "Out of Memory",
      "Permission Error",
      "File Not Exist",
      "File Exist",
      "Invalid Argument",
      "Invalid size",
      "Interrupt",
      "Hit end of file",
      "System error",
      "No space is left on disk",
      "EDU status is not valid",
      "Timeout error",
      "Database is quiesced",
      "Network error",
      "Network is closed from remote",
      "Database is in shutdown status",
      "Application is forced",
      "Given path is not valid",
      "Unexpected file type specified",
      "There's no space for DMS",
      "Collection already exists",
      "Collection does not exist",
      "User record is too large",
      "Record does not exist",
      "Remote overflow record exists",
      "Invalid record",
      "Storage unit need reorg",
      "End of collection",
      "Context is already opened",
      "Context is closed",
      "Option is not supported yet",
      "Collection space already exists",
      "Collection space does not exist",
      "Storage unit file is invalid",
      "Context does not exist",
      "More than one fields are array type",
      "Duplicate key exist",
      "Index key is too large",
      "No space can be found for index extent",
      "Index key does not exist",
      "Hit max number of index",
      "Failed to initialize index",
      "Collection is dropped",
      "Two records get same key and rid",
      "Duplicate index name",
      "Index name does not exist",
      "Unexpected index flag",
      "Hit end of index",
      "Hit the max of dedup buffer",
      "Invalid predicates",
      "Index does not exist",
      "Invalid hint",
      "No more temp collections are avaliable",
      "Exceed max number of storage unit",
      "$id index can't be dropped",
      "Log was not found in log buf",
      "Log was not found in log file",
      "Replication group does not exist",
      "Replication group exists",
      "Invalid request id is received",
      "Session ID does not exist",
      "System EDU cannot be forced",
      "Database is not connected",
      "Unexpected result received",
      "Corrupted record",
      "Backup has already been started",
      "Backup is not completed",
      "Backup is in progress",
      "Backup is corrupted",
      "No primary node was found",
      "Reserved",
      "Engine help argument is specified",
      "Invalid connection state",
      "Invalid handle",
      "Object does not exist",
      "Listening port is already occupied",
      "Unable to listen the specified address",
      "Unable to connect to the specified address",
      "Connection does not exist",
      "Failed to send",
      "Timer does not exist",
      "Route info does not exist",
      "Broken msg",
      "Invalid net handle",
      "Invalid reorg file",
      "Reorg file is in read only mode",
      "Collection status is not valid",
      "Collection is not in reorg state",
      "Replication group is not activated",
      "Node does not belong to the group",
      "Collection status is not compatible",
      "Incompatible version for storage unit",
      "Version is expired for local group",
      "Invalid page size",
      "Version is expired for remote group",
      "Failed to vote for primary",
      "Log record is corrupted",
      "LSN is out of boundary",
      "Unknown mesage is received",
      "Updated information is same as old one",
      "Unknown message",
      "Empty heap",
      "Node is not primary",
      "Not enough number of data nodes",
      "Catalog information does not exist on data node",
      "Catalog version is expired on data node",
      "Catalog version is expired on coordinator node",
      "Exceeds the max group size",
      "Failed to sync log",
      "Failed to replay log",
      "Invalid HTTP header",
      "Failed to negotiate",
      "Failed to change DPS metadata",
      "SME is corrupted",
      "Application is interrupted",
      "Application is disconnected",
      "Character encoding errors",
      "Failed to query on coord node",
      "Buffer array is full",
      "Sub context is conflict",
      "EOC message is received by coordinator node",
      "Size of DPS files are not the same",
      "Invalid DPS log file",
      "No resource is avaliable",
      "Invalid LSN",
      "Pipe buffer size is too small",
      "Catalog authentication failed",
      "Full sync is in progress",
      "Failed to assign data node from coordinator node",
      "PHP driver internal error",
      "Failed to send the message",
      "No activated group information on catalog",
      "Remote-node is disconnected",
      "Unable to find the matched catalog information",
      "Failed to update catalog",
      "Unknown request operation code",
      "Group information cannot be found on coordinator node",
      "DMS extent is corrupted",
      "Remote cluster manager failed",
      "Remote database services have been stopped",
      "Service is starting",
      "Service has been started",
      "Service is restarting",
      "Node already exists",
      "Node does not exist",
      "Unable to lock",
      "DMS state is not compatible with current command",
      "Database rebuild is already started",
      "Database rebuild is in progress",
      "Cache is empty on coordinator node",
      "Evalution failed with error",
      "Group already exist",
      "Group does not exist",
      "Node does not exist",
      "Failed to start the node",
      "Invalid node configuration",
      "Group is empty",
      "The operation is for coord node only",
      "Failed to operate on node",
      "The mutex job already exist",
      "The specified job does not exist",
      "The catalog information is corrupted",
      "$shard index can't be dropped",
      "The command can't be run in the node",
      "The command can't be run in the service plane",
      "The group info not exist",
      "Group name is conflict",
      "The collection is not sharded",
      "The record does not contains valid sharding key",
      "A task that already exists does not compatible with the new task",
      "The collection does not exists on the specified group",
      "The specified task does not exist",
      "The record contains more than one sharding key",
      "The mutex task already exist",
      "The split key is not valid or not in the source group",
      "The unique index must include all fields in sharding key",
      "Sharding key cannot be updated",
      "Authority is forbidden",
      "There is no catalog address specified by user",
      "Current record has been removed",
      "No records can be matched for the search condition",
      "Index page is reorged and the pos got different lchild",
      "Duplicate field name exists in the record",
      "Too many records to be inserted at once",
      "Sort-Merge Join only supports equal predicates",
      "Trace is already started",
      "Trace buffer does not exist",
      "Trace file is not valid",
      "Incompatible lock",
      "Rollback operation is in progress",
      "Invalid record is found during import",
      "Repeated variable name",
      "Column name is ambiguous",
      "SQL syntax error",
      "Invalid transactional operation",
      "Append to lock-wait-queue",
      "Record is deleted",
      "Index is dropped or invalid",
      "Unable to create new catalog when there's already one exists",
      "Failed to parse JSON file",
      "Failed to parse CSV file",
      "Log file size is too large",
      "Unable to remove the last node or primary in a group",
      "Unable to clean up catalog, manual cleanup may be required",
      "Unable to remove primary catalog or catalog group for non-empty database",
      "Reserved",
      "Unable to remove non-empty group",
      "End of queue",
      "Unable to split because of no sharding index exists",
      "The parameter field does not exist",
      "Too many break points are specified",
      "All prefetchers are busy",
      "Domain does not exist",
      "Domain already exists",
      "Group is not in domain",
      "Sharding type is not hash",
      "split percentage is lower then expected",
      "Task is already finished",
      "Collection is in loading status",
      "Rolling back load operation",
      "RouteID is different from the local",
      "Service already exists",
      "Field is not found",
      "csv field line end",
      "Unknown file type",
      "Exporting configuration does not complete in all nodes",
      "Empty non-primary node",
      "Secret value for index file does not match with data file",
      "Engine version argument is specified",
      "Help argument is specified",
      "Version argument is specified",
      "Stored procedure does not exist",
      "Unable to remove collection partition",
      "Duplicated attach collection partition",
      "Invalid partitioned-collection",
      "New boundary is conflict with the existing boundary",
      "Invalid boundary for the shard",
      "Hit the high water mark",
      "Backup already exists",
      "Backup does not exist",
      "Invalid collection partition",
      "Task is canceled",
      "Sharding type must be ranged partition for partitioned-collection",
      "There is no valid sharding-key defined",
      "Operation is not supported on partitioned-collection",
      "Redefine index",
      "Dropping the collection space is in progress",
      "Hit the limit of maximum number of nodes in the cluster",
      "The node is not in normal status",
      "Node information is expired",
      "Failed to wait for the sync operation from secondary nodes",
      "Transaction is disabled",
      "Data source is running out of connection pool",
      "Too many opened file description",
      "Domain is not empty",
      "The data received by REST is larger than the max size",
      "Failed to build bson object",
      "Stored procedure arguments are out of bound",
      "Unknown REST command",
      "Failed to execute command on data node",
      "The domain is empty",
      "Changing password is required",
      "One or more nodes did not complete successfully",
      "There is another OM Agent running with different version",
      "Task does not exist",
      "Task is rolling back",
      "LOB sequence does not exist",
      "LOB is not useable",
      "Data is not in UTF-8 format",
      "Task failed",
      "Lob does not open",
      "Lob has been open",
      "Node is in restoring",
      "There are some collections in the collection space",
      "'localhost' and '127.0.0.1' cannot be used mixed with other hostname and IP address",
      "If use 'localhost' and '127.0.0.1' for hostname, coord and catalog must in the same host ",
      "The special group is not data group",
      "can not update/delete records when $id index does not exist",
      "can not step up when primary node exists or LSN is not the biggest",
      "Image address is conflict with the self cluster",
      "The data group does not have image group",
      "The data group has image group",
      "The image is in enabled status",
      "The cluster's image does not configured",
      "This cluster and image cluster is both writable",
      "This cluster is readonly",
      "Sorting of 'query and modify' must use index",
      "'query and modify' can't use skip and limit in multiple nodes or sub-collections",
      "Given path is not empty",
      "Exist one index which can cover this scene",
      "The cluster's image has already configured",
      "The command is in local mode",
      "The object is not a special object in sdb shell",
      "The specified user already exist",
      "The collection is empty",
      "LOB sequence exists",
      "cluster do not exist",
      "business do not exist",
      "user specified is not exist or password is invalid",
      "Compression initialization failed",
      "Compression failed",
      "Decompression failed",
      "Compression abort",
      "Buffer for compression is too small",
      "Buffer for decompression is too small",
      "Up to the limit",
      "data source is not enabled yet",
      "No reachable coord notes",
      "the record which exclusive ruleID is not exist",
      "Task name conflict",
      "The task is not existed",
      "Replica log is not archived",
      "Data source has not been initialized",
      "Operation is incompatible with the object",
      "This cluster is deactived",
      "LOB is in use",
      "Data operation is overflowed",
      "LOB's pieces info is overflowed",
      "LOB lock is conflicted",
      "Collection is truncated",
      "Some configuration changes didn't take effect"
   ]; 
   var index = -errCode ;
   if ( index < 0 || index >= errDesp.length ) {
      return "unknown error"
   }
   return errDesp[index] ;
}
function getErr (errCode) {
   return _getErr ( errCode ) ;
}
