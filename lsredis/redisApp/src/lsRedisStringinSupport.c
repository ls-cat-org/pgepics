#include "lsRedis.h"

/**
 ** We need an init routine to support the value records but there is
 ** literally nothing to do.  Except return 0 (meaning everything is
 ** AOK).  So I guess that's something.
 */
static long value_init(int phase) {
  return 0;
}


/** Initialize our redis value record
 */
static long value_init_stringin_record( aoRecord *prec) {
  return value_init_record( (dbCommon *)prec, 0);
}

/** Tell epics we like i/o intr so much we are willing to tell it how.
 */
static long value_get_ioint_info( int dir, dbCommon* prec, IOSCANPVT* io) {
  redisValueState* rvs;

  rvs = prec->dpvt;
  *io = rvs->in_scan;
  return 0;
}


/** Copy the value that the redis worker gave us and take all the
 ** credit
 */
static long value_read_stringin( stringinRecord *prec) {
  redisValueState *rvs;
  rvs = prec->dpvt;

  epicsMutexMustLock( rvs->lock);
  strncpy( prec->val, rvs->stringVal, sizeof( prec->val) - 1);
  prec->val[sizeof( prec->val)-1] = 0;
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  return 0;
}

/** Send our value to redis
 **
 */
static long ca_read_stringin( stringinRecord *prec) {
  static char *id = "ca_read_stringin";
  char ourVal[MAX_STRING_SIZE];
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;
  if( rvs == NULL)
    return 1;


  dbGetLink( &prec->inp, DBR_STRING, ourVal, NULL, NULL);

  //
  // TODO: remove the need for the following SQL injection trap:
  //
  if( rvs->setter && strcmp( rvs->setter, "kvset")==0 && strchr( ourVal, '\'') != NULL) {
    fprintf( stderr, "%s: TODO: use prepared statements so we can process strings like this: %s\n", id, ourVal);
    return 0;
  }
  //
  //
  // Only process new values
  //
  if( !prec->udf && strcmp( prec->val, ourVal) == 0)
    return 0;

  strncpy( prec->val, ourVal, sizeof( prec->val) - 1);
  prec->val[sizeof( prec->val)-1] = 0;
  prec->udf = 0;

  epicsMutexMustLock( rvs->lock);
  if( strcmp( rvs->setter, "redis") == 0) {
    snprintf( tmp, sizeof(tmp)-1, "%s", ourVal);
    tmp[sizeof(tmp)-1] = 0;
  }

  //
  // TODO: escape characters or (preferred) use prepared statements
  //
  if( strcmp( rvs->setter, "kvset") == 0) {
    snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%s')", rvs->redisKey, ourVal);
    pgtmp[sizeof(pgtmp)-1] = 0;
  }
  epicsMutexUnlock( rvs->lock);

  //
  // Redis Acync callbacks not needed here because our subscriber
  // process will pick up the publication notice and mark this record
  // as no longer being active by setting pact = 0.
  //
  if( strcmp( rvs->setter, "redis") == 0) {
    epicsMutexMustLock(rvs->rs->lock);
    redisAsyncCommand( rvs->rs->wc, NULL, NULL, "MULTI");
    redisAsyncCommand( rvs->rs->wc, NULL, NULL, "HSET %s VALUE %s", rvs->redisKey, tmp);
    redisAsyncCommand( rvs->rs->wc, NULL, NULL, "PUBLISH UI-%s %s", rvs->redisConnector, rvs->redisKey);
    redisAsyncCommand( rvs->rs->wc, NULL, NULL, "EXEC");
    epicsMutexUnlock(  rvs->rs->lock);
    if( 1 != write( rvs->rs->notifyOut, "\n", 1))
      fprintf( stderr, "%s: unexpected response from notifyOut\n", id);

    // see note for AI support
    prec->pact = 0;		// Set back to one when we see that redis has published our new value
  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
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
} devStringinRedisSource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_stringin_record,
  value_get_ioint_info,
  value_read_stringin
};

epicsExportAddress(dset,devStringinRedisSource);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_stringin;
} devStringinCASource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_stringin_record,
  value_get_ioint_info,
  ca_read_stringin
};

epicsExportAddress(dset,devStringinCASource);

