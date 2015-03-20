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
static long value_init_ai_record( aoRecord *prec) {
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
static long value_read_ai( aiRecord *prec) {
  redisValueState *rvs;
  rvs = prec->dpvt;

  epicsMutexMustLock( rvs->lock);
  prec->val = strtod( rvs->stringVal, NULL);
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  return 2;
}

/** Send our value to redis
 **
 */
static long ca_read_ai( aiRecord *prec) {
  static char *id = "ca_read_ai";
  double ourVal;
  char tmp[128];
  char pgtmp[128];

  redisValueState *rvs;

  rvs = prec->dpvt;

  if( rvs == NULL) {
    fprintf( stderr, "%s: Missing redis value state for %s\n", id, prec->name);
    return 1;
  }

  dbGetLink( &prec->inp, DBR_DOUBLE, &ourVal, NULL, NULL);

  epicsMutexMustLock( rvs->lock);
  snprintf( tmp, sizeof(tmp)-1, "%.*f", prec->prec, ourVal);
  tmp[sizeof(tmp)-1] = 0;

  if( strcmp( rvs->setter, "kvset") == 0) {
    snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%f')", rvs->redisKey, prec->val);
    pgtmp[sizeof(pgtmp)-1] = 0;
  }
  epicsMutexUnlock( rvs->lock);

  //
  // compare values only to the precision that we are interested in
  //
  ourVal = strtod( tmp, NULL);

  if( !prec->udf && ourVal == prec->val)
    return 2;

  prec->val = ourVal;
  prec->udf = 0;

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
      fprintf( stderr, "%s: notifyOut write error\n", id);

    // TODO: fix that fact that this flag is not being reset when
    // redis sees the new value leading to buffer overruns in hiredis
    // when PV's are flying at us faster than we can process them.
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
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiRedisSource = {
  6, /* space for 6 functions */
  NULL,
  value_init,
  value_init_ai_record,
  value_get_ioint_info,
  value_read_ai,
  NULL
};

epicsExportAddress(dset,devAiRedisSource);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiCASource = {
  6, /* space for 6 functions */
  NULL,
  value_init,
  value_init_ai_record,
  value_get_ioint_info,
  ca_read_ai,
  NULL
};

epicsExportAddress(dset,devAiCASource);

