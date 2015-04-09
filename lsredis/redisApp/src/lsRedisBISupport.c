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


/** Send our value to redis
 **
 */
static long ca_read_bi( biRecord *prec) {
  static char *id = "ca_read_bi";
  int ourVal;
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;


  if( rvs == NULL) {
    fprintf( stderr, "%s: PV %s   with missing rvs(!) \n", id, prec->name);
    return 1;
  }

  dbGetLink( &prec->inp, DBR_LONG, &ourVal, NULL, NULL);

  ourVal = ourVal == 0 ? 0 : 1;

  fprintf( stderr, "%s: %s = %d\n", id, prec->name, ourVal);

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
    setRedis( rvs, tmp);

    // TODO: see note for AI support
    prec->pact = 0;		// Set back to one when we see that redis has published our new value
  }
  
  if( strcmp( rvs->setter, "kvset") == 0) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
  return 2;
}

/** Send our value to redis
 **
 */
static long val_read_bi( biRecord *prec) {
  static char *id = "val_read_bi";
  int ourVal;
  char tmp[128];
  char pgtmp[128];
  redisValueState *rvs;
  int dontSet;

  rvs = prec->dpvt;

  if( rvs == NULL) {
    fprintf( stderr, "%s: PV %s   with missing rvs(!) \n", id, prec->name);
    return 1;
  }
  dontSet = 0;

  epicsMutexMustLock( rvs->lock);
  if( prec->udf) {
    // Pre-load the val field from redis and then switch back to normal passive behaviour
    //
    switch( rvs->stringVal[0]) {
    case 'Y':
    case 'y':
    case 'T':
    case 't':
      ourVal = 1;
      break;

    case 'N':
    case 'n':
    case 'F':
    case 'f':
      ourVal = 0;
      break;

    default:
      ourVal = strtol( rvs->stringVal, NULL, 0) == 0 ? 0 : 1;
    }

    prec->val = ourVal; 
    dontSet    = 1;
  } else {
    if( strstr( rvs->setter, "redis") != NULL) {
      snprintf( tmp, sizeof(tmp)-1, "%d", prec->val);
      tmp[sizeof(tmp)-1] = 0;
    }
    if( strstr( rvs->setter, "kvset") != NULL) {
      snprintf( pgtmp, sizeof( pgtmp)-1, "select px.kvset( -1, '%s', '%d')", rvs->redisKey, prec->val);
      pgtmp[sizeof(pgtmp)-1] = 0;
    }
  }
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

  if( dontSet)
    return 2;

  //
  // Redis Acync callbacks not needed here because our subscriber
  // process will pick up the publication notice and mark this record
  // as no longer being active by setting pact = 0.
  //
  if( strstr( rvs->setter, "redis") != NULL) {
    setRedis( rvs, tmp);

    // TODO: see note for AI support
    prec->pact = 0;		// Set back to one when we see that redis has published our new value
  }
  
  if( strstr( rvs->setter, "kvset") != NULL) {
    lsRedisSendQuery( rvs->rs, pgtmp);
    prec->pact = 0;		// TODO: set to one here and back to zero when PG acts on (or at least sees) the command
  }
  return 2;
}

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_bi;
} devBiCASource = {
  5, /* space for 5 functions */
  NULL,
  value_init,
  value_init_bi_record,
  value_get_ioint_info,
  ca_read_bi
};

epicsExportAddress(dset,devBiCASource);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_bi;
} devBiVALSource = {
  5, /* space for 5 functions */
  NULL,
  value_init,
  value_init_bi_record,
  value_get_ioint_info,
  val_read_bi
};

epicsExportAddress(dset,devBiVALSource);

