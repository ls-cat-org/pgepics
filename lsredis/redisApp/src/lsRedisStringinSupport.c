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
    setRedis( rvs, tmp);

    // see note for AI support
    prec->pact = 1;		// Set back to one when we see that redis has published our new value
  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
  return 0;
}

/** Send our value to redis
 **
 */
static long val_read_stringin( stringinRecord *prec) {
  static char *id = "val_read_stringin";
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;
  int dontSet;

  rvs = prec->dpvt;
  if( rvs == NULL)
    return 1;

  dontSet = 0;

  epicsMutexMustLock( rvs->lock);
  if( prec->udf) {
    strncpy( prec->val, rvs->stringVal, sizeof( prec->val) - 1);
    prec->val[sizeof( prec->val) - 1] = 0;
    dontSet = 1;
  } else {
    if( strcmp( rvs->setter, "redis") == 0) {
      snprintf( tmp, sizeof(tmp)-1, "%s", prec->val);
      tmp[sizeof(tmp)-1] = 0;
    }
    
    //
    // TODO: use prepared statements to remove SQL injection risk
    //
    if( strstr( rvs->setter, "kvset") != NULL) {
      if( strchr( prec->val, '\'') != NULL) {
	fprintf( stderr, "%s: TODO: use prepared statements so we can process strings like this: %s\n", id, prec->val);
	return 0;
      }

      snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%s')", rvs->redisKey, prec->val);
      pgtmp[sizeof(pgtmp)-1] = 0;
    }
  }
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  if( dontSet)
    return 0;

  //
  // Redis Acync callbacks not needed here because our subscriber
  // process will pick up the publication notice and mark this record
  // as no longer being active by setting pact = 0.
  //
  if( strcmp( rvs->setter, "redis") == 0) {
    setRedis( rvs, tmp);

    // see note for AI support
    prec->pact = 0;		// Set back to one when we see that redis has published our new value
  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
  return 0;
}

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

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_stringin;
} devStringinVALSource = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_stringin_record,
  value_get_ioint_info,
  val_read_stringin
};

epicsExportAddress(dset,devStringinVALSource);

