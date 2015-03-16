#include "lsRedis.h"

/**
 ** We need an init routine to support the value records but there is
 ** literally nothing to do.  Except return 0 (meaning everything is
 ** BOK).  So I guess that's something.
 **
 ** Called from main thread
 */
static long value_init(int phase) {
  return 0;
}


/** Initialize our redis value record
 **
 ** Called from main thread
 */
static long value_init_bo_record( boRecord *prec) {
  value_init_record( (dbCommon *)prec, 1);
  return 2;
}



/** Copy the value that the redis worker gave us and take all the
 ** credit
 **
 ** Called from main thread
 */
static long value_write_bo( boRecord *prec) {
  //  static char *id = "value_write_bo";
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;

  if( rvs == NULL)
    return 1;

  epicsMutexMustLock( rvs->lock);
  if( strcmp( rvs->setter, "redis") == 0) {
    snprintf( tmp, sizeof(tmp)-1, "%d", prec->val == 0 ? 0 : 1);
    tmp[sizeof(tmp)-1] = 0;
  }

  if( strcmp( rvs->setter, "kvset") == 0) {
    snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%d')", rvs->redisKey, prec->val == 0 ? 0 : 1);
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

    prec->pact = 1;		// Set back to zero when we see that redis has published our new value

    //    fprintf( stderr, "%s: using redis to set value of '%s' to '%s'\n", id, rvs->redisKey, tmp);

  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command

    //fprintf( stderr, "%s: using kvset to set value of '%s' to '%s'\n", id, rvs->redisKey, pgtmp);
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
  DEVSUPFUN  write_bo;
} devBoRedisSource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_bo_record,
  NULL,
  value_write_bo
};

epicsExportAddress( dset, devBoRedisSource);

