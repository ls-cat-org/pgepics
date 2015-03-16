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

epicsExportAddress( dset, devBiRedisSource);

