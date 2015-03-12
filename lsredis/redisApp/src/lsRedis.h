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
#include <aiRecord.h>
#include <aoRecord.h>
#include <biRecord.h>
#include <longinRecord.h>
#include <dbStaticLib.h>
#include <string.h>
#include <search.h>
#include <errno.h>


/** Everything we need to keep track of
 */
typedef struct redisStateStruct {
  ELLNODE node;
  struct hsearch_data *htab;
  int hashTableSize;
  int nhashes;
  struct lsRedisHashDataStruct *lastHTEntry;	// Hash table entries are also in a linked list so the HT can be resized.
  char *basePVName;
  char *redisKeyBase;
  epicsMutexId lock;
  IOSCANPVT scan;
  epicsThreadId generator;
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
  char        *redisKey;        // Saves a lookup or two
  redisState  *rs;		// the redis state that services this variable
  char        *stringVal;	// pointer to the string we need to convert in the record support routine.
  int          nsv;		// Number of characters in stringVal.
				// Allocate more than needed and we'll
				// need to increase the buffer rarely
  epicsMutexId lock;		// Keep things thread safe
  IOSCANPVT    in_scan;		// So we can request that our record get processed
  IOSCANPVT    out_scan;	// So we can request that our record get processed
  char       * redisConnector;
  int          writeWasPending;	// detect if write pending has changed
  int          writePending;		// 0 if we are not waiting for a write callback, 1 if we are
  int          readPending;
} redisValueState;

typedef struct lsRedisHashDataStruct {
  struct lsRedisHashDataStruct *previous;
  char *redisKey;	// the key in redis land
  redisValueState *rvs;	// our private data
} lsRedisHashData;

extern long value_init_record( dbCommon *prec, int inout);
