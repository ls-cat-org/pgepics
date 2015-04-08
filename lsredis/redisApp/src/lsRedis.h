#define _GNU_SOURCE
#include <stdlib.h>
#include <epicsExport.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>
#include <dbScan.h>
#include <dbDefs.h>
#include <ellLib.h>
#include <cantProceed.h>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <initHooks.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <poll.h>
#include <stringinRecord.h>
#include <stringoutRecord.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <biRecord.h>
#include <boRecord.h>
#include <longinRecord.h>
#include <longoutRecord.h>
#include <dbStaticLib.h>
#include <string.h>
#include <search.h>
#include <errno.h>
#include <postgresql/libpq-fe.h>
#include <sys/socket.h>
#include <unistd.h>

/** Everything we need to keep track of
 */
typedef struct redisStateStruct {
  ELLNODE node;
  epicsMutexId queueLock;		// queue is manipulated by more than one thread
  struct redisValueStateStruct *queue[64];		// queue of our output records that need processing
  int queueSize;			// in preparation for a variable queue length (hopefully never)
  unsigned int queueIn;			// Next open position to add an entry
  unsigned int queueOut;		// Next position to read from
  struct hsearch_data *htab;
  int hashTableSize;
  int nhashes;
  struct lsRedisHashDataStruct *lastHTEntry;	// Hash table entries are also in a linked list so the HT can be resized.
  int notifyIn;			// This is what the worker listens to to see if hiredis waits us to look at its state
  int notifyOut;		// Socket to tell us when the hiredis state has possibly changed
  char *basePVName;		// Our related PV's start with this.  (Not currently used)
  char *redisKeyBase;		// Our redis world key base for our variables (Not currently used)
  epicsMutexId lock;
  IOSCANPVT scan;
  epicsThreadId generator;
  int   pgIn;			// socket to read from to receive query requests
  int   pgOut;			// socket to write queries to
  char *pgHost;			// our postgres server location
  int   pgPort;			// and its port
  char *pgUser;			// postgres user name
  char *pgDb;			// name of the postgres database to use
  PGconn *q;			// our postgres conneciton
  struct pollfd pgfd;		// poll object for postgress
  char *pgQueue[64];		// our query queue
  int   pgQuerySize;		// Our fixed length query string size
  int   pgQueueSize;
  unsigned int   pgQueueIn;	// Worker listens for queries on this socket
  unsigned int   pgQueueOut;	// Main thread sends queries here
  int   pgReadyForQuery;	// flag to indicate we can send off another query
  char *readHost;		// redis host for read requests
  int   readPort;		// redis port on redis host for read requests
  int   readDb;			// redis database number for read requests
  char *writeHost;		// redis host for write requests
  int   writePort;		// redis port on redis host for write requests
  int   writeDb;		// redis database number for write requests
  char *subHost;		// redis host for subscriptions (normally the same as the read host)
  int   subPort;		// redis port on redis host for write requests
  int   subDb;			// redis database number for write requests
  redisAsyncContext *rc;	// our read context
  struct pollfd     rcfd;
  redisAsyncContext *wc;	// our write context
  struct pollfd     wcfd;
  redisAsyncContext *sc;	// our subscriber context
  struct pollfd     scfd;
} redisState;

/** This structure allows us to pass the redis state structure to
 ** routines that need a particular pollfd
 */
typedef struct redisPollFDDataStruct {
  struct redisStateStruct *prs;
  struct pollfd     *ppfd;
} redisPollFDData;


/** Use the same state structure for all the various record types to be a little less crazy
 */
typedef struct redisValueStateStruct {
  dbCommon    *inputPV;		// the PV that services inputs from redis to epics
  dbCommon    *outputPV;	// the PV that services outputs from epics to redis
  const char  *redisKey;        // Saves a lookup or two
  redisState  *rs;		// the redis state that services this variable
  char        *stringVal;	// pointer to the string we need to convert in the record support routine.
  int          nsv;		// Number of characters in stringVal.
  const char  *setter;		// name of the setter routine.  Currently the choices are "kvset" and "redis"
				// Allocate more than needed and we'll
				// need to increase the buffer rarely
  epicsMutexId lock;		// Keep things thread safe
  IOSCANPVT    in_scan;		// So we can request that our record get processed
  IOSCANPVT    out_scan;	// So we can request that our record get processed
  const char  *redisConnector;
  int          writeWasPending;	// detect if write pending has changed
  int          writePending;		// 0 if we are not waiting for a write callback, 1 if we are
  int          readPending;
} redisValueState;

typedef struct lsRedisHashDataStruct {
  struct lsRedisHashDataStruct *previous;
  const char *redisKey;	// the key in redis land
  redisValueState *rvs;	// our private data
} lsRedisHashData;

extern long value_init_record( dbCommon *prec, int inout);
extern void lsRedisSendQuery( redisState *rs, char *qs);

extern void setRedis( redisValueState *rvs, char *value);
