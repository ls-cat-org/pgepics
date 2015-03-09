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
#include <stringinRecord.h>
#include <dbStaticLib.h>
#include <string.h>

static ELLLIST allredis = ELLLIST_INIT;

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

struct redisPollFDData {
  struct redisState *prs;
  struct pollfd     *ppfd;
};

struct redisValueAIState {
  double       nextValue;	// supplied by the redis file service thread
  epicsMutexId lock;		// Keep things thread safe
  IOSCANPVT    scan;		// So we can request that our record get processed
  char *epicsPVBase;
  char *redisKeyBase;
  char *redisKey;		// What we are called in the redis world
};


static void start_workers(initHookState state);

static long init(int phase) {
  if(phase==0)
    initHookRegister(&start_workers);
  return 0;
}

/*
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

  for( status = dbFirstRecordType(pdbentry); !status; status = dbNextRecordType(pdbentry)) {
    for( status = dbFirstRecord( pdbentry); !status; status = dbNextRecord( pdbentry)) {
      dbFindField( pdbentry, "DTYP");
      if( dbFoundField( pdbentry) && strcmp(dbGetString( pdbentry),"Redis Value")==0)
	fprintf( stderr, "      record name: %s\n", dbGetRecordName( pdbentry));
    }
  }

  dbFinishEntry(pdbentry);
}
*/


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
  //fprintf( stderr, "DB Changed with type %d: '%s'\n", r->type, r->str);

  if( r->type == REDIS_REPLY_ERROR) {
    //
    // TODO: at this point our context is unusable, should probably do something about that.
    //
  }
}


static int connectToRedis( struct redisState *rs) {
  int err;
  struct redisPollFDData *prsfd;
  struct redisAsyncContext **c;
  struct pollfd *fdp;
  char *host;
  int   port;
  int   db;
  int i;
  
  for( i=0; i<3; i++) {
    switch( i) {
    case 0:
      c    = &(rs->wc);
      host = rs->writeHost;
      port = rs->writePort;
      db   = rs->writeDb;
      fdp  = &(rs->wcfd);
      break;
      
    case 1:
      c    = &(rs->sc);
      host = rs->subHost;
      port = rs->subPort;
      db   = rs->subDb;
      fdp  = &(rs->scfd);
      break;
      
    case 2:
    default:			// default to keep gcc from whining about uninitialize variables.
      c    = &(rs->rc);
      host = rs->readHost;
      port = rs->readPort;
      db   = rs->readDb;
      fdp  = &(rs->rcfd);
      break;
    }
    
    //fprintf( stderr, "connectToRedis: host='%s'  port=%d  db=%d\n", host, port, db);

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
  }

  return 0;
}

static long init_record( stringinRecord *prec) {
  struct redisState *rs;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;

  rs = callocMustSucceed( 1, sizeof( *rs), "Redis Connector Init Recorod");

  scanIoInit( &rs->scan);
  rs->basePVName = strdup( prec->name);
  rs->lock       = epicsMutexMustCreate();
  rs->generator  = NULL;
  ellAdd( &allredis, &rs->node);
  prec->dpvt     = rs;

  //
  // Get the extra strings we should have attached through the info mechanism
  //
  status = 0;
  dbInitEntry( pdbbase, pdbentry);
  status = dbFindRecord( pdbentry, prec->name);
  if( status) {
    fprintf( stderr, "Redis Connector Init Record could not find db entry for '%s'\n", prec->name);
    return 1;
  }

  rs->basePVName   = strdup( dbGetInfo( pdbentry, "epicsPVBase"));
  rs->redisKeyBase = strdup( dbGetInfo( pdbentry, "redisKeyBase"));
  rs->readHost     = strdup( dbGetInfo( pdbentry, "redisReadHost"));
  rs->readPort     = strtol( dbGetInfo( pdbentry, "redisReadPort"),  NULL, 0);
  rs->readDb       = strtol( dbGetInfo( pdbentry, "redisReadDb"),    NULL, 0);
  rs->writeHost    = strdup( dbGetInfo( pdbentry, "redisWriteHost"));
  rs->writePort    = strtol( dbGetInfo( pdbentry, "redisWritePort"), NULL, 0);
  rs->writeDb      = strtol( dbGetInfo( pdbentry, "redisWriteDb"),   NULL, 0);
  rs->subHost      = strdup( dbGetInfo( pdbentry, "redisSubHost"));
  rs->subPort      = strtol( dbGetInfo( pdbentry, "redisSubPort"),   NULL, 0);
  rs->subDb        = strtol( dbGetInfo( pdbentry, "redisSubDb"),     NULL, 0);

  //
  // Initialize contexts
  //
  if( connectToRedis( rs)) {
    fprintf( stderr, "Record Init Failed: redis connection initialization failed\n");
    return 1;
  }


  //  findOurRecords( rs);

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


static struct redisValueAIState *redisKeyToPV( struct redisState *rs, const char *key) {
  struct redisValueAIState *rtn;
  struct dbAddr rec;
  char tmp[128];
  int i, j;

  if( strlen( key) < 7) {
    return NULL;
  }

  snprintf( tmp, sizeof( tmp)-1, "%s:", rs->basePVName);
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

  //fprintf( stderr, "redisValueAIState: key='%s'  PV?: '%s'\n", key, tmp);

  if( dbNameToAddr( tmp, &rec)) {
    return NULL;
  }
  
  rtn = (struct redisValueAIState *)((aiRecord *)(rec.precord)->dpvt);
  return rtn;
}


static void devRedis_setPVCB( redisAsyncContext *c, void *reply, void *privdata) {
  redisReply             *r;
  struct redisValueAIState *rvs;
  r = reply;

  rvs   = (struct redisValueAIState *)privdata;

  epicsMutexMustLock( rvs->lock);
  rvs->nextValue = strtod( r->str, NULL);
  epicsMutexUnlock(   rvs->lock);

  scanIoRequest( rvs->scan);
}

static void devRedisSubscribeCB( redisAsyncContext *c, void *reply, void *privdata) {
  redisReply *r;
  char *key;
  struct redisState *rs;
  struct redisValueAIState *rvs;

  r  = reply;
  rs = privdata;

  if( r->type != REDIS_REPLY_ARRAY || r->elements != 4)
    return;
  
  key = r->element[3]->str;

  //fprintf( stderr, "devRedisSubscribeCB: key='%s'\n", key);

  rvs = redisKeyToPV( rs, key);

  if( rvs) {
    redisAsyncCommand( rs->rc, devRedis_setPVCB, rvs, "HGET %s VALUE", key);
  }
}



static void worker(void *raw) {
  struct redisState *rs = raw;
  struct redisValueAIState *rvs;
  struct pollfd fds[3];
  redisAsyncContext *racs[3]; 
  int pollReturn;
  int i;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  
  status = 0;
  dbInitEntry( pdbbase, pdbentry);
  for( status = dbFirstRecordType(pdbentry); !status; status = dbNextRecordType(pdbentry)) {
    for( status = dbFirstRecord( pdbentry); !status; status = dbNextRecord( pdbentry)) {
      dbFindField( pdbentry, "DTYP");
      if( dbFoundField( pdbentry) && strcmp(dbGetString( pdbentry),"Redis Value")==0) {
	dbFindField( pdbentry, "DPVT");
	if( dbFoundField( pdbentry)) {
	  rvs = ((aiRecord *)(pdbentry->precnode->precord))->dpvt;
	  redisAsyncCommand( rs->rc, devRedis_setPVCB, rvs, "HGET %s VALUE", rvs->redisKey);
	  //fprintf( stderr, "      record name: %s   redisKey: %s\n", dbGetRecordName( pdbentry), rvs->redisKey);
	}
      }
    }
  }
  dbFinishEntry(pdbentry);


  //
  // Get initial values
  //
  

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

static long read_stringin( stringinRecord *prec)
{
  //
  // Really, the value of this record is not used.
  // We only the this record to process other records

  return 0;
}

static long value_init(int phase) {
  //
  // Really there is nothing to do as the normal redis process will do 99% of the work
  //
  return 0;
}


static long value_init_ai_record( aiRecord *prec) {
  struct redisValueAIState *rvs;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  char tmp[128];  // should be plenty big for reasonable redis keys
  int i;

  rvs = callocMustSucceed( 1, sizeof( *rvs), "redis value AI init");

  scanIoInit( &rvs->scan);
  rvs->lock     = epicsMutexMustCreate();
  rvs->redisKey = NULL;
  prec->dpvt = rvs;

  //
  // Get the extra strings we should have attached through the info mechanism
  //
  status = 0;
  dbInitEntry( pdbbase, pdbentry);
  status = dbFindRecord( pdbentry, prec->name);
  if( status) {
    fprintf( stderr, "Redis Value Init Record could not find db entry for '%s'\n", prec->name);
    return 1;
  }
  rvs->epicsPVBase  = strdup( dbGetInfo( pdbentry, "epicsPVBase"));
  rvs->redisKeyBase = strdup( dbGetInfo( pdbentry, "redisKeyBase"));

  if( strlen( rvs->epicsPVBase) >= sizeof(prec->name)+1) {
    fprintf( stderr, "value_init_ai_record: PV name is too short.  Name='%s', epicsPVBase='%s'\n", prec->name, rvs->epicsPVBase);
    return 1;
  }
  if( strlen( rvs->redisKeyBase) + strlen( prec->name) + 2 >= sizeof( tmp)) {
    fprintf( stderr, "Resulting redis key too long.  '%s.%s'\n", rvs->redisKeyBase, &(prec->name[strlen(rvs->epicsPVBase)+1]));
    return 1;
  }

  snprintf( tmp, sizeof(tmp)-1, "%s%s", rvs->redisKeyBase, &(prec->name[strlen(rvs->epicsPVBase)+1]));

  tmp[sizeof(tmp)-1]=0;
  
  for( i=0; tmp[i]; i++) {
    if( tmp[i] == ':')
      tmp[i] = '.';
  }

  rvs->redisKey = strdup(tmp);
  //  fprintf( stderr, "redis value ai intit  pv '%s'  rediskey '%s'\n", prec->name, rvs->redisKey);

  return 0;
}

static long value_get_ioint_info( int dir, dbCommon* prec, IOSCANPVT* io) {
  struct redisValueAIState* priv=prec->dpvt;

  *io = priv->scan;
  return 0;
}


static long value_read_ai( aiRecord *prec) {
  struct redisValueAIState *rvs;
  rvs = prec->dpvt;

  epicsMutexMustLock( rvs->lock);
  prec->val = rvs->nextValue;
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  return 2;
}


struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_stringin;
} devStringinRedisConnector = {
  5, /* space for 6 functions */
  NULL,
  init,
  init_record,
  get_ioint_info,
  read_stringin
};

epicsExportAddress( dset, devStringinRedisConnector);

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
  value_init,
  value_init_ai_record,
  value_get_ioint_info,
  value_read_ai,
  NULL
};

epicsExportAddress(dset,devAiRedisValue);

