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
#include <aiRecord.h>
#include <dbStaticLib.h>
#include <string.h>

static ELLLIST allredis = ELLLIST_INIT;

struct redisState {
  ELLNODE node;
  unsigned int seed;
  unsigned int lastnum;
  epicsMutexId lock;
  IOSCANPVT scan;
  epicsThreadId generator;
  redisAsyncContext *rc;	// our read context
  struct pollfd     rcfd;
  redisAsyncContext *wc;	// our write context
  struct pollfd     wcfd;
  redisAsyncContext *sc;	// our subscriber context
  struct pollfd     scfd;
};

struct redisPollFDData {
  struct redisState *prs;
  struct pollfd     *ppfd;
};

static void start_workers(initHookState state);

static long init(int phase) {
  if(phase==0)
    initHookRegister(&start_workers);
  return 0;
}

static void findOurRecords( struct redisState *rs) {
  int status;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  if (!pdbbase) {
    printf("No database loaded\n");
    return;
  }
  status = 0;
  dbInitEntry(pdbbase, pdbentry);
  status = dbFindRecordType(pdbentry, "ai");
  while (!status) {
    status=dbFirstRecord(pdbentry);
    while (!status) {
      fprintf( stderr, "record name: %s\n", dbGetRecordName( pdbentry));
      status = dbNextRecord(pdbentry);
    }
  }
  dbFinishEntry(pdbentry);
}



/** hook to mange read events                                                                                                                                                                                                                                                 
 */
void devRedis_addRead( void *data) {
  struct pollfd *pfd;
  struct redisPollFDData *prsfd;
  
  prsfd = (struct redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLIN) == 0) {
    pfd->events |= POLLIN;
  }
}

/** hook to manage "don't need to read" events                                                                                                                                                                                                                                
 */
void devRedis_delRead( void *data) {
  struct pollfd *pfd;
  struct redisPollFDData *prsfd;
  
  prsfd = (struct redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLIN) != 0) {
    pfd->events &= ~POLLIN;
  }
}

/** hook to manage write events                                                                                                                                                                                                                                               
 */
void devRedis_addWrite( void *data) {
  struct pollfd *pfd;
  struct redisPollFDData *prsfd;
  
  prsfd = (struct redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLOUT) == 0) {
    pfd->events |= POLLOUT;
  }
}

/** hook to manage "don't need to write anymore" events                                                                                                                                                                                                                       
 */
void devRedis_delWrite( void *data) {
  struct pollfd *pfd;
  struct redisPollFDData *prsfd;
  
  prsfd = (struct redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLOUT) != 0) {
    pfd->events &= ~POLLOUT;
  }
}

/** hook to clean up                                                                                                                                                                                                                                                          
 * TODO: figure out what we are supposed to do here and do it                                                                                                                                                                                                                 
 */
void devRedis_cleanup( void *data) {
  struct pollfd *pfd;
  struct redisPollFDData *prsfd;
  
  prsfd = (struct redisPollFDData *)data;
  pfd = prsfd->ppfd;

  pfd->fd = -1;

  if( (pfd->events & (POLLOUT | POLLIN)) != 0) {
    pfd->events &= ~(POLLOUT | POLLIN);
  }
}

static void devRedisDBChangedCB( redisAsyncContext *c, void *reply, void *privdata) {
  redisReply *r;
  
  r = reply;
  fprintf( stderr, "DB Changed with type %d: '%s'\n", r->type, r->str);

  if( r->type == REDIS_REPLY_ERROR) {
    //
    // TODO: at this point our context is unusable, should probably do something about that.
    //
  }
}


static int connectToRedis( redisAsyncContext **c, struct redisState *rs, struct pollfd *fdp, const char *host, const int port, const int db) {
  int err;
  struct redisPollFDData *prsfd;

  *c = redisAsyncConnect( host, port);
  if( !(*c) || (*c)->err) {
    fprintf( stderr, "Unsuccessful attempt at creating redis context.\n");
    return 1;
  }
  prsfd = callocMustSucceed( 1, sizeof( *prsfd), "connectToRedis");
  prsfd->prs  = rs;
  prsfd->ppfd = fdp;

  fdp->fd           = (*c)->c.fd;
  fdp->events       = 0;
  (*c)->ev.data     = prsfd;			// this is all so self referal
  (*c)->ev.addRead  = devRedis_addRead;
  (*c)->ev.delRead  = devRedis_delRead;
  (*c)->ev.addWrite = devRedis_addWrite;
  (*c)->ev.delWrite = devRedis_delWrite;
  (*c)->ev.cleanup  = devRedis_cleanup;

  if( db != 0) {
    err = redisAsyncCommand( *c, devRedisDBChangedCB, NULL, "SELECT %d", db);
    return err;
  }

  return 0;
}

static long init_record(aiRecord *prec) {
  struct redisState *rs;
  unsigned long start;

  rs = callocMustSucceed( 1, sizeof( *rs), "redisintr");
  
  recGblInitConstantLink( &prec->inp, DBF_ULONG, &start);

  rs->seed      = start;
  scanIoInit( &rs->scan);
  rs->lock      = epicsMutexMustCreate();
  rs->generator = NULL;
  ellAdd( &allredis, &rs->node);
  prec->dpvt    = rs;


  //
  // Initialize contexts
  //
  if( connectToRedis( &(rs->rc), rs, &(rs->rcfd), "10.1.253.14", 6379, 0) ||
      connectToRedis( &(rs->wc), rs, &(rs->wcfd), "10.1.253.14", 6379, 0) ||
      connectToRedis( &(rs->sc), rs, &(rs->scfd), "10.1.253.14", 6379, 0)) {

    fprintf( stderr, "Record Init Failed: redis connection initialization failed\n");
    return 1;
  }


  findOurRecords( rs);

  return 0;
}


void devRedis_debugCB( redisAsyncContext *ac, void *reply, void *privdata) {
  static int indentlevel = 0;
  redisReply *r;
  int i;

  r = (redisReply *)reply;

  if( r == NULL) {
    fprintf( stderr, "Null reply.  Odd\n");
    return;
  }

  switch( r->type) {
  case REDIS_REPLY_STATUS:
    fprintf( stderr, "%*sSTATUS: %s\n", indentlevel*4,"", r->str);
    break;

  case REDIS_REPLY_ERROR:
    fprintf( stderr, "%*sERROR: %s\n", indentlevel*4, "", r->str);
    break;

  case REDIS_REPLY_INTEGER:
    fprintf( stderr, "%*sInteger: %lld\n", indentlevel*4, "", r->integer);
    break;

  case REDIS_REPLY_NIL:
    fprintf( stderr, "%*s(nil)\n", indentlevel*4, "");
    break;

  case REDIS_REPLY_STRING:
    fprintf( stderr, "%*sSTRING: %s\n", indentlevel*4, "", r->str);
    break;

  case REDIS_REPLY_ARRAY:
    fprintf( stderr, "%*sARRAY of %d elements\n", indentlevel*4, "", (int)r->elements);
    indentlevel++;
    for( i=0; i<(int)r->elements; i++)
      devRedis_debugCB( ac, r->element[i], NULL);
    indentlevel--;
    break;

  default:
    fprintf( stderr, "%*sUnknown type %d\n", indentlevel*4,"", r->type);

  }
}

static struct dbAddr *redisKeyToPV( const char *key) {
  struct dbAddr *paddr;
  char tmp[128];
  int i, j;

  if( strlen( key) < 7) {
    return NULL;
  }

  paddr = callocMustSucceed( 1, sizeof( *paddr), "redisKeyToPv");
  strncpy( tmp, "21:orange:", sizeof( tmp)-1);
  tmp[sizeof(tmp)-1] = 0;

  for( i=strlen(tmp), j=7; key[j] && j<sizeof(tmp)-1; i++, j++) {
    if( key[j] == '.') {
      tmp[i] = ':';
    } else {
      tmp[i] = key[j];
    }
  }
  if( i < sizeof( tmp))
    tmp[i] = 0;

  if( dbNameToAddr( tmp, paddr)) {
    free( paddr);
    paddr = NULL;
  }
  return paddr;
}


static void devRedis_setPVCB( redisAsyncContext *c, void *reply, void *privdata) {
  struct dbAddr *paddr;
  redisReply *r;
  struct redisState *rs;
  struct redisPollFDData *prsfd;
  r = reply;

  paddr = (struct dbAddr *)privdata;
  prsfd = (struct redisPollFDData *)c->ev.data;
  rs    = prsfd->prs;

  epicsMutexMustLock( rs->lock);
  dbPutField( paddr, DBR_STRING, r->str, 1);
  epicsMutexUnlock( rs->lock);
  free( paddr);
}

static void devRedisSubscribeCB( redisAsyncContext *c, void *reply, void *privdata) {
  redisReply *r;
  char *key;
  struct redisState *rs;
  struct dbAddr *paddr;

  r  = reply;
  rs = privdata;

  if( r->type != REDIS_REPLY_ARRAY || r->elements != 4)
    return;
  
  key = r->element[3]->str;

  paddr = redisKeyToPV( key);

  if( paddr)
    redisAsyncCommand( rs->rc, devRedis_setPVCB, paddr, "HGET %s VALUE", key);
}



static void worker(void *raw) {
  struct redisState *rs = raw;
  struct pollfd fds[3];
  redisAsyncContext *racs[3]; 
  int pollReturn;
  int i;

  //
  // Start up the subscriber
  //
  if( redisAsyncCommand( rs->sc, devRedisSubscribeCB, rs, "PSUBSCRIBE *") != REDIS_OK) {
    fprintf( stderr, "Queuing subscribe command failed.\n");
    return;
  }
  
  //
  // set up array of contexts to simplify things after poll
  //
  racs[0] = rs->rc;
  racs[1] = rs->wc;
  racs[2] = rs->sc;


  while(1) {
    //
    // Looks dumb but should work
    //
    fds[0].fd      = rs->rcfd.fd;
    fds[0].events  = rs->rcfd.events;
    fds[0].revents = 0;

    fds[1].fd      = rs->wcfd.fd;
    fds[1].events  = rs->wcfd.events;
    fds[1].revents = 0;

    fds[2].fd      = rs->scfd.fd;
    fds[2].events  = rs->scfd.events;
    fds[2].revents = 0;

    //    epicsMutexUnlock( rs->lock);
    //
    // Wait for as long as it takes
    //
    pollReturn = poll( fds, 3, -1);

    //    epicsMutexMustLock( rs->lock);
    //
    // pollReturn == number of fds with an event ready
    //            or  -1 on error
    if( pollReturn == -1) {
      //
      // TODO: do something interesting when poll returns an error.
      // Probably a system interupt occured and if this is a common
      // thing then we should probably look at ppool.
      //
    }

    for( i=0; i<3; i++) {
      if( fds[i].revents) {
	  if(  fds[i].revents & POLLIN)
	    redisAsyncHandleRead( racs[i]);
	  if(  fds[i].revents & POLLOUT)
	    redisAsyncHandleWrite( racs[i]);
      }
    }
    
    rs->lastnum = rand_r(&rs->seed);

  }
}

static void start_workers(initHookState state)
{
  ELLNODE *cur;
  if(state!=initHookAfterInterruptAccept)
    return;
  
  for(cur=ellFirst(&allredis); cur; cur=ellNext(cur)) {
    struct redisState *priv = CONTAINER(cur, struct redisState, node);
    priv->generator = epicsThreadMustCreate("redisworker",
                                            epicsThreadPriorityMedium,
                                            epicsThreadGetStackSize( epicsThreadStackMedium),
                                            &worker, priv);
  }
}


static long get_ioint_info(int dir,dbCommon* prec,IOSCANPVT* io)
{
  struct redisState* priv=prec->dpvt;

  *io = priv->scan;
  return 0;
}

static long read_ai(aiRecord *prec)
{
  struct redisState* priv=prec->dpvt;

  epicsMutexMustLock(priv->lock);
  prec->rval = priv->lastnum;
  epicsMutexUnlock(priv->lock);

  return 0;
}



static long value_read_ai( aiRecord *prec) {
  return 0;
}


struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiRedisIntr = {
  6, /* space for 6 functions */
  NULL,
  init,
  init_record,
  get_ioint_info,
  read_ai,
  NULL
};

epicsExportAddress(dset,devAiRedisIntr);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiRedisValue = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  NULL,
  NULL,
  value_read_ai,
  NULL
};

epicsExportAddress(dset,devAiRedisValue);

