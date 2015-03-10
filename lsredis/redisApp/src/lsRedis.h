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
#include <biRecord.h>
#include <longinRecord.h>
#include <dbStaticLib.h>
#include <string.h>

/** Everything we need to keep track of
 */
struct redisState {
  ELLNODE node;
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
};

/** This structure allows us to pass the redis state structure to
 ** routines that need a particular pollfd
 */
struct redisPollFDData {
  struct redisState *prs;
  struct pollfd     *ppfd;
};


/** Use the same state structure for all the various record types to be a little less crazy
 */
struct redisValueState {
  char         *stringVal;	// pointer to the string we need to convert in the record support routine.
  int          nsv;		// Number of characters in stringVal.
				// Allocate more than needed and we'll
				// need to increase the buffer rarely
  epicsMutexId lock;		// Keep things thread safe
  IOSCANPVT    scan;		// So we can request that our record get processed
  char *epicsPVBase;
  char *redisKeyBase;
  char *redisKey;		// What we are called in the redis world
};

