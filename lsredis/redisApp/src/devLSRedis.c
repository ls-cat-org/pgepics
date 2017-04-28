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

/** Hook to clean up hiredis socket events. Called by hiredis at the
 ** tail end of closing up shop.  This is where we should be freeing
 ** up any memory that we have allocated.
 **
 ** Called from worker thread **
 */
void devRedis_cleanup( void *data) {
  struct pollfd *pfd;
  redisPollFDData *prsfd;
  
  prsfd = (redisPollFDData *)data;
  pfd = prsfd->ppfd;

  pfd->fd     = -1;
  pfd->events = 0;

}

/** Report when we've chaged databases
 **
 ** Called from worker thread
 **
 */
static void devRedisDBChangedCB( redisAsyncContext *c, void *reply, void *privdata) {
  static const char *id = __FILE__ " " "devRedisDBChangedCB";
  redisReply *r;
  
  if (reply == NULL) {
    fprintf(stderr, "%s: NULL reply: %s, host: %s\n", id, c->errstr, c->c.tcp.host);
    return;
  }

  r = reply;
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

/** Set a redis variable
 **
 ** Called by Main Thread
 */
void setRedis( redisValueState *rvs, char *value) {
  static char *id = "setRedis";

  epicsMutexMustLock( rvs->rs->lock);

  // KLUDGE: we are assuming REDIS_OK = 0.  If not then there will be a problem here.
  if( redisAsyncCommand(    rvs->rs->wc, NULL, NULL, "MULTI")
      || redisAsyncCommand( rvs->rs->wc, NULL, NULL, "HSET %s VALUE %s", rvs->redisKey,       value)
      || redisAsyncCommand( rvs->rs->wc, NULL, NULL, "PUBLISH UI-%s %s", rvs->redisConnector, rvs->redisKey)
      || redisAsyncCommand( rvs->rs->wc, NULL, NULL, "EXEC")) {
    fprintf( stderr, "%s: redisAsyncCommand returned an error.  This should be unusual.  Key '%s'   Value '%s'\n", id, rvs->redisKey, value);
  }

  epicsMutexUnlock( rvs->rs->lock);

  if( 1 != write( rvs->rs->notifyOut, "\n", 1))
    fprintf( stderr, "%s: unexpected response from notifyOut.  Key: %s   Value: %s\n", id, rvs->redisKey, value);
}

static void connectToPostgres( redisState *rs) {
  static char *id = "connectToPostgres";
  int connecting = 1;
  char tmp[128];
  int err;

  snprintf( tmp, sizeof(tmp)-1, "hostaddr=%s user=%s dbname=%s port=%d", rs->pgHost, rs->pgUser, rs->pgDb, rs->pgPort);
  tmp[sizeof(tmp)-1] = 0;

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

  while( connecting) {
    switch( PQconnectPoll( rs->q)) {
    case PGRES_POLLING_WRITING:
      rs->pgfd.events = POLLOUT;      
      break;
      
    case PGRES_POLLING_READING:
      rs->pgfd.events = POLLIN;      
      break;
      
    case PGRES_POLLING_OK:
      connecting = 0;
      rs->pgfd.events = POLLIN;
      break;
      
    case PGRES_POLLING_FAILED:
    default:
      rs->pgfd.events = 0;
      exit( 1);
      // TODO: initiate recovering the pg connection
      break;
    }
    
    if( !connecting)
      break;
    rs->pgfd.revents = 0;
    poll( &(rs->pgfd), 1, -1);
    
  }
  //
  // Default state is listening
  rs->pgfd.events = POLLIN;
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
      hte.key  = (char *)hd->redisKey;
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
static redisState *lsRedisGetRedisState( const char *connectorName) {
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
    if( -1 == socketpair( AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK, 0, sv)) {
      fprintf( stderr, "%s: socketpair creation failed\n", id);
      exit( 1);
    }
    rs->notifyIn      = sv[0];
    rs->notifyOut     = sv[1];
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
    rs->pgQuerySize = 256;
    for( i=0; i < rs->pgQueueSize; i++) {
      rs->pgQueue[i] = callocMustSucceed( rs->pgQuerySize, sizeof( **(rs->pgQueue)), id);
    }
    rs->pgQueueIn   = 0;
    rs->pgQueueOut  = 0;
    rs->pgReadyForQuery = 1;

    *pprs = rs;
  }
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
}

/** perhaps queue a new query and set the fd events send it
 ** Otherwise do not mess with the fd events
 **
 */
static void lsRedisPGQueryNext( redisState *rs) {
  static char *id = "lsRedisPGQueryNext";
  char *qs;

  qs = NULL;
  if( rs->pgReadyForQuery) {
    if( rs->pgQueueIn != rs->pgQueueOut) {
      qs = rs->pgQueue[ rs->pgQueueOut++ % rs->pgQueueSize];

      if( qs != NULL) {
	PQsendQuery( rs->q, qs);
	fprintf( stderr, "%s: sending query %s\n", id, qs);
	rs->pgReadyForQuery = 0;
	rs->pgfd.events = POLLIN | POLLOUT;	// enable sending the query
      }
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
static void lsRedisSetRedisValueState( const char *connectorName,
				       const char *redisKey,
				       const char *inputPVName,
				       const char *outputPVName,
				       const char *setter) {
  static char *id = "lsRedisGetRedisValueState";
  redisState *rs;
  struct dbAddr rec;
  ENTRY  hte, *htrp;
  lsRedisHashData *hd;
  void *inRecord, *outRecord;
  redisValueState *rvs;
  dbCommon *prec;

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
  hte.key  = (char *)redisKey;
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
    rvs = callocMustSucceed( 1, sizeof( *rvs), id);
    rvs->redisConnector = strdup( connectorName);
    rvs->redisKey  = strdup( redisKey);
    rvs->inputPV   = inRecord;
    rvs->outputPV  = outRecord;
    rvs->rs        = rs;
    rvs->nsv       = 128;
    rvs->stringVal = callocMustSucceed( rvs->nsv, sizeof( *(rvs->stringVal)), id);
    rvs->lock      = epicsMutexMustCreate();

    hd = callocMustSucceed( 1, sizeof( *hd), id);
    hd->previous    = rs->lastHTEntry;
    rs->lastHTEntry = hd;
    hd->redisKey    = rvs->redisKey;
    hd->rvs         = rvs;

    hte.key         = (char *)hd->redisKey;
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

  if( setter && (rvs->setter == NULL || rvs->setter[0] == 0))
    rvs->setter = setter;

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

  return;
}

/** Initialize redis support
 **
 ** Called from main thread.
 **
 */
static long init_record( stringoutRecord *prec) {
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

  fprintf( stderr, "%s: initialized\n", id);
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
  redisReply  *r;
  redisValueState *rvs;

  if (reply == NULL) {
    fprintf(stderr, "%s: NULL reply: %s, host: %s\n", id, c->errstr, c->c.tcp.host);
    return;
  }

  r = reply;
  rvs   = (redisValueState *)privdata;

  if( r->type != REDIS_REPLY_STRING) {
    fprintf( stderr, "%s: problem with key '%s'  type: %d\n", id, rvs->redisKey, r->type);
    return;
  }

  epicsMutexMustLock( rvs->lock);
  if( rvs->nsv < strlen( r->str)-1) {
    rvs->nsv = strlen( r->str) + 32;
    if( rvs->stringVal)
      free( rvs->stringVal);
    rvs->stringVal = callocMustSucceed( rvs->nsv, sizeof( *(rvs->stringVal)), id);
  }
  strncpy( rvs->stringVal, r->str, rvs->nsv - 1);
  rvs->stringVal[rvs->nsv - 1] = 0;
  epicsMutexUnlock(   rvs->lock);

  fprintf( stderr, "%s: adding %s with value %s\n", id, rvs->redisKey, rvs->stringVal);

  //  if( rvs->inputPV && rvs->inputPV->scan == SCAN_PASSIVE)
  //    dbScanPassive( NULL, rvs->inputPV);

  if( rvs->inputPV && rvs->inputPV->scan == SCAN_IO_EVENT)
    scanIoRequest( rvs->in_scan);

  if( rvs->outputPV && rvs->outputPV->scan == SCAN_IO_EVENT)
    scanIoRequest( rvs->out_scan);
}

/** Service a message comming from the subscription connection
 **
 ** Called from worker thread.
 **
 */
static void devRedisSubscribeCB( redisAsyncContext *c, void *reply, void *privdata) {
  static char *id = __FILE__ " " "devRedisSubscribeCB";
  redisReply *r;
  char *key;
  char *publisher;
  redisState *rs;
  redisValueState *rvs;
  redisPollFDData *prsfd;

  if (reply == NULL) {
    fprintf(stderr, "%s: NULL reply: %s, host: %s\n", id, c->errstr, c->c.tcp.host);
    return;
  }

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
    if( strstr( publisher, rvs->redisConnector) != NULL) {
      //
      // put this on our queue for the main thread to request lower
      // the pact flag
      //
      fprintf( stderr, "%s: queuing %s\n", id, rvs->redisKey);
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
  static char *id = "postgresRead";
  int err;
  PGresult *pgr;

  err = PQconsumeInput( rs->q);
  if( err == 0) {
    fprintf( stderr, "%s: PQconsumeInput returned an error\n", id);
    // TODO: handle the error
  }
  while( !PQisBusy( rs->q)) {
    pgr = PQgetResult( rs->q);
    if( pgr == NULL) {
      rs->pgReadyForQuery = 1;
      break;
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
  static char *id = "postgresWrite";
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
    fprintf( stderr, "%s: PQflush returned an error\n", id);
    rs->pgfd.events = POLLIN;
  }
}

// Service the signal that hiredis may have changed state We currently
// do nothing with whatever is sent.  We are only sent 1 character for
// each request and other routines are responsible for the state.  If
// we somehow do not read all the input then we get it next time
// around.
static void lsRedisService( redisState *rs) {
  static char *id = "lsRedisService";
  char dummy[128];

  if( 0 >= read( rs->notifyIn, dummy, sizeof( dummy)))
    fprintf( stderr, "%s: error reading from notifyIn socket\n", id);
}

/**  Our worker thread.  Handles communication with redis
 **
 ** Called from worker thread.  (This is the worker.)
 **
 */
static void worker(void *raw) {
  //  static char *id = "worker";
  redisState *rs = raw;
  redisValueState *rvs;
  struct pollfd fds[6];
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
      dbFindField( pdbentry, "DPVT");
      if( dbFoundField( pdbentry)) {
	rvs = ((struct dbCommon *)(pdbentry->precnode->precord))->dpvt;

	dbFindField( pdbentry, "DTYP");
	//
	// For these the initial value is stored in redis
	//
	if( dbFoundField( pdbentry) && (strcmp( dbGetString( pdbentry),"Redis Source")==0 || strcmp(dbGetString( pdbentry), "VAL Source")==0)) {
	  epicsMutexMustLock( rs->lock);
	  redisAsyncCommand( rs->rc, devRedis_setPVCB, rvs, "HGET %s VALUE", rvs->redisKey);
	  epicsMutexUnlock(   rs->lock);
	}
	/*
	if( dbFoundField( pdbentry) && (strcmp( dbGetString( pdbentry), "CA Source")==0 )) {
	  if( rvs->inputPV && rvs->inputPV->scan == SCAN_PASSIVE)
	    dbScanPassive( NULL, rvs->inputPV);
	}
	*/
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

    lsRedisPGQueryNext( rs);		// maybe dequeues and sends the next query
    fds[3].fd      = rs->pgfd.fd;
    fds[3].events  = rs->pgfd.events;
    fds[3].revents = 0;

    fds[4].fd      = rs->pgIn;
    fds[4].events  = POLLIN;
    fds[4].revents = 0;

    fds[5].fd      = rs->notifyIn;
    fds[5].events  = POLLIN;
    fds[5].revents = 0;

    //    epicsMutexUnlock( rs->lock);
    //
    // Wait for as long as it takes
    //
    pollReturn = poll( fds, 6, -1);

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
	if(  fds[i].revents & POLLIN) {
	  epicsMutexMustLock( rs->lock);
	  redisAsyncHandleRead( racs[i]);
	  epicsMutexUnlock( rs->lock);
	}
	if(  fds[i].revents & POLLOUT) {
	  epicsMutexMustLock( rs->lock);
	  redisAsyncHandleWrite( racs[i]);
	  epicsMutexUnlock( rs->lock);
	}
      }
    }
    if( fds[3].revents & POLLIN)
      postgresRead( rs);

    if( fds[3].revents & POLLOUT)
      postgresWrite( rs);

    if( fds[4].revents & POLLIN)
      readQueryService( rs);

    if( fds[5].revents & POLLIN)
      lsRedisService( rs);
  }
}


/** Epics tells us we can now start some workers.
 ** We just have one.
 **
 ** Called from main thread.
 **
 */
static void start_workers(initHookState state) {
  //static char *id = "start_workers";
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

/** How to read a string for our redis record.
 **
 ** Called from main thread.
 **
 */
static long write_stringout( stringoutRecord *prec) {
  //  static char *id = "write_stringout";
  redisState      *rs;
  redisValueState *rvs;
  int udf;

  rs = prec->dpvt;

  epicsMutexMustLock( rs->queueLock);
  prec->udf = 0;
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
    if( rvs->outputPV) {
      epicsMutexMustLock( rvs->lock);
      rvs->outputPV->pact = 0;		// Allow the record to get processed later.
      epicsMutexUnlock( rvs->lock);
    }
    if( rvs->inputPV) {
      epicsMutexMustLock( rvs->lock);
      rvs->inputPV->pact  = 0;
      udf = rvs->inputPV->udf;
      epicsMutexUnlock( rvs->lock);
      if( udf)
	dbScanPassive( NULL, rvs->inputPV);
    }

  }
  prec->pact = 0;
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
  DEVSUPFUN  write_stringout;
} devStringoutRedisConnector = {
  5, /* space for 5 functions */
  NULL,
  init,
  init_record,
  get_ioint_info,
  write_stringout
};

epicsExportAddress( dset, devStringoutRedisConnector);


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
  const char *redisConnector;
  const char *redisKey;
  const char *setter;

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
  redisConnector = dbGetInfo( pdbentry, "redisConnector");
  redisKey       = dbGetInfo( pdbentry, "redisKey");
  setter         = dbGetInfo( pdbentry, "setter");
  if( !setter)
    setter = strdup( "");

  //
  // the redis value state is shared between the output and input records
  if( inout) {
    lsRedisSetRedisValueState( redisConnector, redisKey, NULL, prec->name, setter);
  } else {
    lsRedisSetRedisValueState( redisConnector, redisKey, prec->name, NULL, setter);
  }
  return 0;
}
