#include "lsRedis.h"

/**
 ** We need an init routine to support the value records but there is
 ** literally nothing to do.  Except return 0 (meaning everything is
 ** AOK).  So I guess that's something.
 */
static long value_init(int phase) {
  //
  // Really there is nothing to do as the normal redis process will do 99% of the work
  //
  return 0;
}


/** Initialize our redis value record
 */
static long value_init_longin_record( longinRecord *prec) {
  struct redisValueState *rvs;
  DBENTRY dbentry;
  DBENTRY *pdbentry = &dbentry;
  int status;
  char tmp[128];  // should be plenty big for reasonable redis keys
  int i;

  rvs = callocMustSucceed( 1, sizeof( *rvs), "redis value LONGIN init");

  rvs->nsv        = 64;
  rvs->stringVal  = callocMustSucceed( rvs->nsv, sizeof( char), "Redis init_record stringVal");

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
    fprintf( stderr, "value_init_longin_record: PV name is too short.  Name='%s', epicsPVBase='%s'\n", prec->name, rvs->epicsPVBase);
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
  fprintf( stderr, "redis value longin intit  pv '%s'  rediskey '%s'\n", prec->name, rvs->redisKey);

  return 0;
}

/** Tell epics we like i/o intr so much we are willing to tell it how.
 */
static long value_get_ioint_info( int dir, dbCommon* prec, IOSCANPVT* io) {
  struct redisValueState* priv=prec->dpvt;

  *io = priv->scan;
  return 0;
}


/** Copy the value that the redis worker gave us and take all the
 ** credit
 */
static long value_read_longin( longinRecord *prec) {
  struct redisValueState *rvs;
  rvs = prec->dpvt;

  epicsMutexMustLock( rvs->lock);
  prec->val = strtol( rvs->stringVal, NULL, 0);
  prec->udf = 0;
  epicsMutexUnlock( rvs->lock);

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
  DEVSUPFUN  read_longin;
} devLonginRedisValue = {
  5, /* space for 6 functions */
  NULL,
  value_init,
  value_init_longin_record,
  value_get_ioint_info,
  value_read_longin
};

epicsExportAddress(dset,devLonginRedisValue);

