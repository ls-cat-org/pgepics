#include "lsRedis.h"

/**
 ** We need an init routine to support the value records but there is
 ** literally nothing to do.  Except return 0 (meaning everything is
 ** AOK).  So I guess that's something.
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
static long value_init_stringout_record( stringoutRecord *prec) {
  value_init_record( (dbCommon *)prec, 1);
  return 2;
}



/** Copy the value that the redis worker gave us and take all the
 ** credit
 **
 ** Called from main thread
 */
static long value_write_stringout( stringoutRecord *prec) {
  static char *id = "value_write_stringout";
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;

  if( rvs == NULL)
    return 1;

  //
  // TODO: remove the need for the following SQL injection trap:
  //
  if( rvs->setter && strcmp( rvs->setter, "kvset")==0 && strchr( prec->val, '\'') != NULL) {
    fprintf( stderr, "%s: TODO: use prepared statements so we can process strings like this: %s\n", id, prec->val);
    return 0;
  }

  //
  //
  epicsMutexMustLock( rvs->lock);
  if( strcmp( rvs->setter, "redis") == 0) {
    snprintf( tmp, sizeof(tmp)-1, "%s", prec->val);
    tmp[sizeof(tmp)-1] = 0;
  }

  if( strcmp( rvs->setter, "kvset") == 0) {
    snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%s')", rvs->redisKey, prec->val);
    pgtmp[sizeof(pgtmp)-1] = 0;
  }
  epicsMutexUnlock( rvs->lock);

  //
  // Redis Acync callbacks not needed here because our subscriber
  // process will pick up the publication notice and mark this record
  // as no longer being active by setting pact = 0.
  //
  if( strcmp( rvs->setter, "redis") == 0) {
    setRedis( rvs, tmp);

    prec->pact = 1;		// Set back to zero when we see that redis has published our new value
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
  DEVSUPFUN  write_stringout;
} devStringoutRedisSource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_stringout_record,
  NULL,
  value_write_stringout
};

epicsExportAddress( dset, devStringoutRedisSource);

