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
static long value_init_bi_record( aoRecord *prec) {
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
static long value_read_bi( biRecord *prec) {
  redisValueState *rvs;
  rvs = prec->dpvt;

  epicsMutexMustLock( rvs->lock);
  prec->val = strtol( rvs->stringVal, NULL, 0) == 0 ? 0 : 1;
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  return 2;
}

/** Send our value to redis
 **
 */
static long ca_read_bi( biRecord *prec) {
  static char *id = "ca_read_bi";
  epicsInt8 ourVal;
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;


  if( rvs == NULL) {
    fprintf( stderr, "%s: PV %s   with missing rvs(!) \n", id, prec->name);
    return 1;
  }

  dbGetLink( &prec->inp, DBR_CHAR, &ourVal, NULL, NULL);
  ourVal = ourVal == 0 ? 0 : 1;

  if( !prec->udf && prec->val == ourVal)
    return 2;

  prec->val = ourVal;
  prec->udf = 0;

  epicsMutexMustLock( rvs->lock);
  if( strcmp( rvs->setter, "redis") == 0) {
    snprintf( tmp, sizeof(tmp)-1, "%d", ourVal);
    tmp[sizeof(tmp)-1] = 0;
  }

  if( strcmp( rvs->setter, "kvset") == 0) {
    snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%d')", rvs->redisKey, ourVal);
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
      fprintf( stderr, "%s: unexpected response from notifyOut. This is probably bad\n", id);

    // TODO: see note for AI support
    prec->pact = 0;		// Set back to one when we see that redis has published our new value
  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
  return 2;
}


/** Interface to the IOC code that calls us
 */
struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_bi;
} devBiRedisSource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_bi_record,
  value_get_ioint_info,
  value_read_bi
};

epicsExportAddress(dset,devBiRedisSource);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_bi;
} devBiCASource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_bi_record,
  value_get_ioint_info,
  ca_read_bi
};

epicsExportAddress(dset,devBiCASource);

