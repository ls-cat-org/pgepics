#include "lsRedis.h"

static ELLLIST allredis = ELLLIST_INIT;



static void start_workers(initHookState state);

/** Register our start worker routine
 **
 ** Called from main thread
 */
static long init(int phase) {
  if(phase==0)
    initHookRegister(&start_workers);
  return 0;
}

/** Read socket support for hiredis events 
 **
 ** Called from worker thread **
 */
void devRedis_addRead( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLIN) == 0) {
    pfd->events |= POLLIN;
  }
}

/** (don't) Read socket support for hiredis events 
 **
 ** Called from worker thread **
 */
void devRedis_delRead( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLIN) != 0) {
    pfd->events &= ~POLLIN;
  }
}

/** Write socket support for hiredis events 
 **
 ** Called from worker thread **
 */
void devRedis_addWrite( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLOUT) == 0) {
    pfd->events |= POLLOUT;
  }
}

/** (don't) Write socket support for hiredis events 
 **
 ** Called from worker thread **
 */
void devRedis_delWrite( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  if( (pfd->events & POLLOUT) != 0) {
    pfd->events &= ~POLLOUT;
  }
}

/** hook to clean up hiredis socket events
 ** TODO: figure out what we are supposed to do here and do it
 **
 ** Called from worker thread **
 */
void devRedis_cleanup( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  pfd->fd = -1;

  if( (pfd->events & (POLLOUT | POLLIN)) != 0) {
    pfd->events &= ~(POLLOUT | POLLIN);
  }
}

/** Report when we've chaged databases
 **
 ** Called from worker thread
 **
 */
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


/** Perform all connection work once the redis state structure is ready
 **
 ** Called from main thread
 **
 */
static int connectToRedis( redisState *rs) {
  int err;
  redisPollFDData *prsfd;
  redisAsyncContext **c;
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
      //
      // Called from initialization routine before worker starts and,
      // hence, no mutex is needed.
      //
      err = redisAsyncCommand( *c, devRedisDBChangedCB, NULL, "SELECT %d", db);
      return err;
    }
  }

  return 0;
}

static void connectToPostgres( redisState *rs) {
  static char *id = "connectToPostgres";
  int connecting = 1;
  char tmp[128];
  int err;

  snprintf( tmp, sizeof(tmp)-1, "hostaddr=%s user=%s dbname=%s port=%d", rs->pgHost, rs->pgUser, rs->pgDb, rs->pgPort);
  tmp[sizeof(tmp)-1] = 0;

  fprintf( stderr, "%s: postgres connection string: '%s'\n", id, tmp);
  rs->q = PQconnectStart( tmp);

  err   = PQstatus( rs->q);
  if( err == CONNECTION_BAD) {
    fprintf( stderr, "%s: Cannot initiate connection to postgresql\n", id);
    exit( 1);
  }
  err   = PQsetnonblocking( rs->q, 1);
  if( err != 0) {
    fprintf( stderr, "%s: Cannot set the postgresql to non-blocking. Odd.\n", id);
    exit( 1);
  }
    
  rs->pgfd.fd     = PQsocket( rs->q);
  rs->pgfd.events = POLLOUT;
  

  while( connecting) {
    switch( PQconnectPoll( rs->q)) {
    case PGRES_POLLING_WRITING:
      rs->pgfd.events = POLLOUT;      
      fprintf( stderr, "%s: pgres writing\n", id);
      break;
      
    case PGRES_POLLING_READING:
      rs->pgfd.events = POLLIN;      
      fprintf( stderr, "%s: pgres reading\n", id);
      break;
      
    case PGRES_POLLING_OK:
      connecting = 0;
      rs->pgfd.events = POLLIN;
      fprintf( stderr, "%s: connection to postgresql server established\n", id);
      break;
      
    case PGRES_POLLING_FAILED:
    default:
      rs->pgfd.events = 0;
      fprintf( stderr, "%s: pgres failed\n", id);
      exit( 1);
      // TODO: initiate recovering the pg connection
      break;
    }
    
    if( !connecting)
      break;
    rs->pgfd.revents = 0;
    poll( &(rs->pgfd), 1, -1);
    
  }
}


/** See if we need to increase the size of the hash table just before
 ** adding something to it.
 **
 ** Called from main thread.  (TODO: verify this)
 **
 */
static void lsRedisMaybeResizeHashTable( redisState *rs) {
  static char *id = "lsRedisMaybeResizeHashTable";
  ENTRY hte, *htrp;
  lsRedisHashData *hd;

  if( rs->nhashes > 0.75 * rs->hashTableSize) {
    //
    // Incrase the hash table size by destroying it and then
    // recreating it bigger than ever.  The table is more
    // efficient if it is not more than about 3/4 full: we should
    // never actually run out of room.
    //
    hdestroy_r( rs->htab);
    rs->hashTableSize *= 2;
    if( 0 == hcreate_r( rs->hashTableSize, rs->htab)) {
      fprintf( stderr, "%s: failed to allocate hash table of size %d\n", id, rs->hashTableSize);
      exit( 1);
    }
    //
    // Now that we've destroyed the old one and made the new one
    // bigger it's time to repopulate our hash table world.
    //
    for( hd = rs->lastHTEntry; hd != NULL; hd = hd->previous) {
      hte.key  = hd->redisKey;
      hte.data = hd;
      hsearch_r( hte, ENTER, &htrp, rs->htab);
      if( hd->rvs->outputPV) {
	hte.key  = (char *)hd->rvs->outputPV;
	hte.data = hd;
	hsearch_r( hte, ENTER, &htrp, rs->htab);
      }
    }
  }
}

/** return pointer to redisState given the name of the redis record.
 ** Allocate the storage and attach it to the redis record in
 ** case that record has not yet been initialized
 **
 ** Called from main thread.
 **
 */
static redisState *lsRedisGetRedisState( char *connectorName) {
  static char *id = "lsRedisgetRedisState";
  redisState **pprs, *rs;
  struct dbAddr rec;
  char tmp[128];
  int i;
  int sv[2];

  snprintf( tmp, sizeof( tmp)-1, "%s.DPVT", connectorName);
  tmp[sizeof(tmp)-1] = 0;

  if( dbNameToAddr( tmp, &rec)) {
    fprintf( stderr, "%s: could not find redis state pointer location\n", id);
    return NULL;
  }
  
  pprs = rec.pfield;
  rs   = *pprs;
  if( rs == NULL) {
    rs = callocMustSucceed( 1, sizeof( *rs), id);
    rs->htab = callocMustSucceed( 1, sizeof( struct hsearch_data), id);
    rs->hashTableSize = 4096;
    rs->nhashes       = 0;
    rs->lastHTEntry   = NULL;
    if( 0 == hcreate_r( rs->hashTableSize, rs->htab)) {
      fprintf( stderr, "%s: failed to allocate hash table\n", id);
      exit( 1);
    }
    rs->queueLock = epicsMutexMustCreate();
    rs->queueSize = sizeof( rs->queue)/sizeof( *rs->queue);
    rs->queueIn   = 0;
    rs->queueOut  = 0;

    if( -1 == socketpair( AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0, sv)) {
      fprintf( stderr, "%s: socketpair creation failed\n", id);
      exit( 1);
    }
    rs->pgIn  = sv[0];
    rs->pgOut = sv[1];
    rs->pgQueueSize = sizeof( rs->pgQueue)/sizeof( *rs->pgQueue);
    rs->pgQuerySize = 64;
    for( i=0; i < rs->pgQueueSize; i++) {
      rs->pgQueue[i] = callocMustSucceed( rs->pgQuerySize, sizeof( char), id);
    }
    rs->pgQueueIn   = 0;
    rs->pgQueueOut  = 0;
    rs->pgReadyForQuery = 1;

    *pprs = rs;
  }
  //fprintf( stderr, "%s: connectorName '%s'   rs: %p\n", id, connectorName, rs);
  return rs;
}


/** Send a query to the worker.
 **
 ** Called from main thread.
 **
 */
void lsRedisSendQuery( redisState *rs, char *qs) {
  static char *id = "lsRedisSendQuery";
  ssize_t  nchars;
  // TODO: handle the return values.
  nchars = write( rs->pgOut, qs, strlen( qs));
  if( nchars == -1) {
    fprintf( stderr, "%s: write error\n", id);
  }
}

/** Add an item to the query queue
 **
 ** Called from worker
 **
 */
static void readQueryService( redisState *rs) {
  static char *id = "readQueryService";
  char *buf;
  ssize_t nchars;

  buf = rs->pgQueue[rs->pgQueueIn++ % rs->pgQueueSize];

  nchars = read( rs->pgIn, buf, rs->pgQuerySize - 1);
  buf[rs->pgQuerySize - 1] = 0;
  if( nchars <= 0) {
    fprintf( stderr, "%s: read returned %d, this is probably bad.\n", id, (int)nchars);
  }

  fprintf( stderr, "%s: read %d bytes.  query: %s\n", id, (int)nchars, buf);
}

/** perhaps queue a new query and set the fd events send it
 ** Otherwise do not mess with the fd events
 **
 */
static void lsRedisPGQueryNext( redisState *rs) {
  char *qs;

  qs = NULL;
  if( rs->pgReadyForQuery) {
    if( rs->pgQueueIn != rs->pgQueueOut)
      qs = rs->pgQueue[ rs->pgQueueOut++ % rs->pgQueueSize];

    if( qs != NULL) {
      PQsendQuery( rs->q, qs);
      rs->pgReadyForQuery = 0;
      rs->pgfd.events = POLLIN | POLLOUT;
    }
  }
}

/** Return a valid redis value state.  Create it as well as the redis
 ** state if need be.  If there are both input and ouput PVs with the
 ** sample redis key then they'll both have the same redis value
 ** state.
 **
 ** Called from main thread.
 */
static void lsRedisSetRedisValueState( char *connectorName, char *redisKey, char *inputPVName, char *outputPVName) {
  static char *id = "lsRedisGetRedisValueState";
  redisState *rs;
  struct dbAddr rec;
  ENTRY  hte, *htrp;
  lsRedisHashData *hd;
  void *inRecord, *outRecord;
  redisValueState *rvs;
  dbCommon *prec;

  fprintf( stderr, "%s: starting\n", id);
  rs = lsRedisGetRedisState( connectorName);

  // Don't go on if we were given nothing to do
  if( !redisKey || !redisKey[0] || !rs || ( (!inputPVName || !inputPVName[0]) && (!outputPVName || !outputPVName[0])))
    return;

  inRecord = NULL;
  if( inputPVName && inputPVName[0]) {
    if( dbNameToAddr( inputPVName, &rec)) {
      fprintf( stderr, "%s could not find record '%s'\n", id, inputPVName);
      return;
    }
    inRecord = rec.precord;
  }
  
  outRecord = NULL;
  if( outputPVName && outputPVName[0]) {
    if( dbNameToAddr( outputPVName, &rec)) {
      fprintf( stderr, "%s could not find record '%s'\n", id, outputPVName);
      return;
    }
    outRecord = rec.precord;
  }

  // At this point we should one of the input and output pv records in
  // hand.


  // See if we already have an entry for this redis key
  hte.key  = redisKey;
  hte.data = NULL;
  if( !hsearch_r( hte, FIND, &htrp, rs->htab)) {
    //
    // the key was not found.  This means we'll need to create both
    // entry for the hash table and the redis value structure itself.
    //
    lsRedisMaybeResizeHashTable( rs);
    //
    // Add a key
    //
    fprintf( stderr, "%s: adding key '%s'\n", id, redisKey);
    rvs = callocMustSucceed( 1, sizeof( *rvs), id);
    rvs->redisConnector = strdup( connectorName);
    rvs->redisKey  = strdup( redisKey);
    rvs->inputPV   = inRecord;
    rvs->outputPV  = outRecord;
    rvs->rs        = rs;
    rvs->nsv       = 64;
    rvs->stringVal = callocMustSucceed( rvs->nsv, sizeof( char), id);
    rvs->lock      = epicsMutexMustCreate();

    hd = callocMustSucceed( 1, sizeof( *hd), id);
    hd->previous    = rs->lastHTEntry;
    rs->lastHTEntry = hd;
    hd->redisKey    = rvs->redisKey;
    hd->rvs         = rvs;

    hte.key         = hd->redisKey;
    hte.data        = hd;
    hsearch_r( hte, ENTER, &htrp, rs->htab);
    rs->nhashes++;
  }
  //
  // If you are still following along then at this point you'll see
  // that htrp now points to our hash table entry either because we
  // found it or because we entered it.
  //
  hd  = htrp->data;
  rvs = hd->rvs;

  if( inRecord != NULL) {
    scanIoInit( &(rvs->in_scan));
    rvs->inputPV  = inRecord;
    prec = (dbCommon *)inRecord;
    prec->dpvt = rvs;
  }

  if( outRecord != NULL) {
    scanIoInit( &(rvs->out_scan));
    rvs->outputPV = outRecord;
    prec = (dbCommon *)outRecord;
    prec->dpvt = rvs;
  }

  fprintf( stderr, "%s: done  redisKey %s  inPV %s at %p  outPV %s at %p  rs %p  rvs %p\n",
	   id, redisKey, inputPVName, inRecord, outputPVName, outRecord, rs, rvs);

  return;
}

/** Initialize redis support
 **
 ** Called from main thread.
 **
 */
static long init_record( stringinRecord *prec) {
  static char *id = "devLSRedis init_record";
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  redisState *rs;

  rs = lsRedisGetRedisState( prec->name);

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
    fprintf( stderr, "%s could not find db entry for '%s'\n", id, prec->name);
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
  
  rs->pgHost       = strdup( dbGetInfo( pdbentry, "pgHost"));
  rs->pgPort       = strtol( dbGetInfo( pdbentry, "pgPort"),         NULL, 0);
  rs->pgUser       = strdup( dbGetInfo( pdbentry, "pgUser"));
  rs->pgDb         = strdup( dbGetInfo( pdbentry, "pgDb"));


  dbFinishEntry( pdbentry);

  //
  // Initialize contexts
  //
  if( connectToRedis( rs)) {
    fprintf( stderr, "%s: redis connection initialization failed\n", id);
    return 1;
  }

  connectToPostgres( rs);

  fprintf( stderr, "%s: rs %p   rc %p   wc %p   sc %p\n", id, rs, rs->rc, rs->wc, rs->sc);

  //  findOurRecords( rs);

  return 0;
}

/** General purpuse callback for debuging redis replies
 **
 ** Called from worker thread.
 **
 */
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


/** Given a redis key name return the state structure associated with
 ** the corresponding PV (or null if there is not one)
 **
 ** Called from worker thread.
 **
 */
static redisValueState *redisKeyToRedisValueState( redisState *rs, char *key) {
  lsRedisHashData *hdp;
  ENTRY hte, *phte;

  hte.key  = key;
  hte.data = NULL;
  if( !hsearch_r( hte, FIND, &phte, rs->htab))
    return NULL;

  hdp = phte->data;
  return hdp->rvs;
}


/** redis callback to set the value of a pv.  Or really, tell the main thread to change it to the value we are passing to it.
 **
 ** Called from worker thread.
 **
 */
static void devRedis_setPVCB( redisAsyncContext *c, void *reply, void *privdata) {
  static char *id = "devRedis_setPVCB";
  redisReply             *r;
  redisValueState *rvs;

  r = reply;
  rvs   = (redisValueState *)privdata;

  epicsMutexMustLock( rvs->lock);
  if( rvs->nsv < strlen( r->str)-1) {
    rvs->nsv = strlen( r->str) + 32;
    free( rvs->stringVal);
    rvs->stringVal = callocMustSucceed( rvs->nsv, sizeof( char), id);
  }
  strncpy( rvs->stringVal, r->str, rvs->nsv - 1);
  rvs->stringVal[rvs->nsv - 1] = 0;

  epicsMutexUnlock(   rvs->lock);

  fprintf( stderr, "%s: rcvd string '%s'\n", id, r->str);

  scanIoRequest( rvs->in_scan);
}

/** Service a message comming from the subscription connection
 **
 ** Called from worker thread.
 **
 */
static void devRedisSubscribeCB( redisAsyncContext *c, void *reply, void *privdata) {
  //  static char *id = "devRedisSubscribeCB";
  redisReply *r;
  char *key;
  char *publisher;
  redisState *rs;
  redisValueState *rvs;
  redisPollFDData *prsfd;

  r  = reply;

  if( r->type != REDIS_REPLY_ARRAY || r->elements != 4)
    return;
  
  prsfd = c->ev.data;
  rs    = prsfd->prs;

  //
  // TODO: Ignore the entry when we are the publisher
  //
  // TODO: Ignore the change when it will not alter the value of the
  // PV
  //

  publisher = r->element[2]->str;
  key       = r->element[3]->str;

  rvs = redisKeyToRedisValueState( rs, key);

  if( rvs) {
    //
    // is this self published?
    //
    if( strstr( publisher, rvs->redisConnector)) {
      //
      // put this on our queue for the main thread to request lower
      // the pact flag
      //
      epicsMutexMustLock( rvs->rs->queueLock);
      rvs->rs->queue[ (rvs->rs->queueIn++ % rvs->rs->queueSize)] = rvs;
      epicsMutexUnlock( rvs->rs->queueLock);
      
      scanIoRequest( rvs->rs->scan);
    }
    rvs->readPending    = 1;

    epicsMutexMustLock( rs->lock);
    redisAsyncCommand( rvs->rs->rc, devRedis_setPVCB, rvs, "HGET %s VALUE", key);
    epicsMutexUnlock( rs->lock);
  }
}

// called from worker
static void postgresRead( redisState *rs) {
  int err;
  PGresult *pgr;

  err = PQconsumeInput( rs->q);
  if( err == 0) {
    // TODO: handle the error
  }
  if( !PQisBusy( rs->q)) {
    pgr = PQgetResult( rs->q);
    if( pgr == NULL) {
      rs->pgReadyForQuery = 1;
    } else {
      // TODO: Surely there is something useful coming back.  Do
      // something with the responses.
      //
      PQclear( pgr);
      rs->pgReadyForQuery = 0;
    } 
  }
}

// called from worker
static void postgresWrite( redisState *rs) {
  int status;

  status = PQflush( rs->q);

  switch( status) {
  case 1:
    // There is more to write;
    rs->pgfd.events = POLLIN | POLLOUT;
    break;
    
  case 0:
    // There is nothing more to write at this time
    rs->pgfd.events = POLLIN;
    break;

  case -1:
  default:
    // TODO: Figure out what this really means and handle it
    rs->pgfd.events = POLLIN;
  }
}

/**  Our worker thread.  Handles communication with redis
 **
 ** Called from worker thread.  (This is the worker.)
 **
 */
static void worker(void *raw) {
  static char *id = "worker";
  redisState *rs = raw;
  redisValueState *rvs;
  struct pollfd fds[5];
  redisAsyncContext *racs[3]; 
  int pollReturn;
  int i;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  
  //
  // Queue initial values
  // How many can we send off before the request queue fills up?  Should we start the event loop first?
  //

  status = 0;
  dbInitEntry( pdbbase, pdbentry);
  for( status = dbFirstRecordType(pdbentry); !status; status = dbNextRecordType(pdbentry)) {
    for( status = dbFirstRecord( pdbentry); !status; status = dbNextRecord( pdbentry)) {
      dbFindField( pdbentry, "DTYP");
      if( dbFoundField( pdbentry) && strcmp(dbGetString( pdbentry),"Redis Value")==0) {
	dbFindField( pdbentry, "DPVT");
	if( dbFoundField( pdbentry)) {
	  rvs = ((aiRecord *)(pdbentry->precnode->precord))->dpvt;

	  epicsMutexMustLock( rs->lock);
	  redisAsyncCommand( rs->rc, devRedis_setPVCB, rvs, "HGET %s VALUE", rvs->redisKey);
	  epicsMutexUnlock(   rs->lock);
	}
      }
    }
  }
  dbFinishEntry(pdbentry);

  //
  // Start up the subscriber
  //
  epicsMutexMustLock( rs->lock);
  status = redisAsyncCommand( rs->sc, devRedisSubscribeCB, rs, "PSUBSCRIBE *");
  epicsMutexUnlock( rs->lock);

  if( status != REDIS_OK) {
    fprintf( stderr, "Queuing subscribe command failed.\n");
    return;
  }
  
  //
  // set up array of contexts to simplify things after poll
  //
  racs[0] = rs->rc;
  racs[1] = rs->wc;
  racs[2] = rs->sc;

  fprintf( stderr, "%s: starting event loop\n", id);

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

    lsRedisPGQueryNext( rs);		// maybe queues the next query
    fds[3].fd      = rs->pgfd.fd;
    fds[3].events  = rs->pgfd.events;
    fds[3].revents = 0;

    fds[4].fd      = rs->pgIn;
    fds[4].events  = POLLIN;
    fds[4].revents = 0;

    //    epicsMutexUnlock( rs->lock);
    //
    // Wait for as long as it takes
    //
    pollReturn = poll( fds, 5, -1);

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
    if( fds[3].revents & POLLIN)
      postgresRead( rs);

    if( fds[3].revents & POLLOUT)
      postgresWrite( rs);

    if( fds[4].revents & POLLIN)
      readQueryService( rs);

  }
}


/** Epics tells us we can now start some workers.
 ** We just have one.
 **
 ** Called from main thread.
 **
 */
static void start_workers(initHookState state) {
  ELLNODE *cur;
  if(state!=initHookAfterInterruptAccept)
    return;
  
  for(cur=ellFirst(&allredis); cur; cur=ellNext(cur)) {
    redisState *priv = CONTAINER(cur, redisState, node);
    priv->generator = epicsThreadMustCreate("redisworker",
                                            epicsThreadPriorityMedium,
                                            epicsThreadGetStackSize( epicsThreadStackMedium),
                                            &worker, priv);
  }
}


/** Tell epics how to scan us (whatever "scan" means.  Probably the
 ** most overused work in data collection environments.)
 **
 ** Called from main thread.
 **
 */
static long get_ioint_info(int dir,dbCommon* prec,IOSCANPVT* io) {
  redisState* priv=prec->dpvt;

  *io = priv->scan;
  return 0;
}

/** How to read a string for our redis record We need to have such a
 ** routine to make everyone happy (and not have to write our own
 ** redis record) BUT it makes no sense to give this record a value.
 ** Does that make this really just a device driver rather than a
 ** device?  If it really matters then there is a bit of rewritting to
 ** do.
 **
 ** Called from main thread.
 **
 */
static long read_stringin( stringinRecord *prec) {
  //static char *id = "read_stringin";
  redisState      *rs;
  redisValueState *rvs;

  rs = prec->dpvt;

  epicsMutexMustLock( rs->queueLock);
  while( rs->queueIn != rs->queueOut) {
    rvs = rs->queue[rs->queueOut++ % rs->queueSize];
    //
    // Show which redis key is being processed
    //
    strncpy( prec->val, rvs->redisKey, sizeof( prec->val)-1);
    prec->val[sizeof( prec->val)-1] = 0;
    
    //
    // get epics to do some work
    //
    rvs->outputPV->pact = 0;		// Allow the record to get processed later.
  }
  epicsMutexUnlock( rs->queueLock);
  return 0;
}


/** Interface to the IOC code that calls us
 */
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


/** Initialize our redis value record
 ** inout = 0 for an input record, 1 for an output
 **
 ** Called from main thread.
 **
 */
long value_init_record( dbCommon *prec, int inout) {
  static char *id = "value_init_record";
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  char *redisConnector;
  char *redisKey;

  //
  // Get the extra strings we should have attached through the info mechanism
  //
  status = 0;
  dbInitEntry( pdbbase, pdbentry);
  status = dbFindRecord( pdbentry, prec->name);
  if( status) {
    fprintf( stderr, "%s could not find db entry for '%s'\n", id, prec->name);
    return 1;
  }
  redisConnector  = strdup( dbGetInfo( pdbentry, "redisConnector"));
  redisKey        = strdup( dbGetInfo( pdbentry, "redisKey"));
  
  //
  // the redis value state is shared between the output and input records
  if( inout)
    lsRedisSetRedisValueState( redisConnector, redisKey, NULL, prec->name);
  else
    lsRedisSetRedisValueState( redisConnector, redisKey, prec->name, NULL);

  return 0;
}
