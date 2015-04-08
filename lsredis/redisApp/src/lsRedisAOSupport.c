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
static long value_init_ao_record( aoRecord *prec) {
  value_init_record( (dbCommon *)prec, 1);
  return 2;
}



/** Copy the value that the redis worker gave us and take all the
 ** credit
 **
 ** Called from main thread
 */
static long value_write_ao( aoRecord *prec) {
  //  static char *id = "value_write_ao";
  redisValueState *rvs;

  rvs = prec->dpvt;
  if( rvs == NULL)
    return 1;

  epicsMutexMustLock( rvs->lock);
  prec->val = strtod( rvs->stringVal, NULL);
  prec->udf = 0;
  if( rvs->inputPV && rvs->inputPV->udf)
    dbScanPassive( NULL, rvs->inputPV);
  epicsMutexUnlock( rvs->lock);
  
  return 0;
}


/** Tell epics we like i/o intr so much we are willing to tell it how.
 */
static long value_get_ioint_info( int dir, dbCommon* prec, IOSCANPVT* io) {
  redisValueState* rvs;

  rvs = prec->dpvt;
  *io = rvs->out_scan;
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
  DEVSUPFUN  write_ao;
  DEVSUPFUN  special_linconv;
} devAoRedisSource = {
  6, /* space for 6 functions */
  NULL,
  value_init,
  value_init_ao_record,
  value_get_ioint_info,
  value_write_ao,
  NULL
};

epicsExportAddress(dset,devAoRedisSource);

