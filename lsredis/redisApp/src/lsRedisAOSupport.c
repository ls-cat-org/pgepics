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
static long value_init_ao_record( aoRecord *prec) {
  value_init_record( (dbCommon *)prec, 1);
  return 2;
}


/** Tell epics we like i/o intr so much we are willing to tell it how.
 */
static long value_get_ioint_info( int dir, dbCommon* prec, IOSCANPVT* io) {
  redisValueState *rvs;
  
  rvs = prec->dpvt;
  *io = rvs->out_scan;

  return 0;
}


static void lsRedis_writeAOCB( redisAsyncContext *c, void *reply, void *privdata) {
  redisValueState *rvs;

  rvs = privdata;

  epicsMutexMustLock( rvs->lock);
  rvs->writePending   = 0;
  rvs->outputPV->pact = 0;
  epicsMutexUnlock(   rvs->lock);

  scanIoRequest( rvs->out_scan);

}

/** Copy the value that the redis worker gave us and take all the
 ** credit
 */
static long value_write_ao( aoRecord *prec) {
  static char *id = "value_write_ao";
  char tmp[128];
  redisValueState *rvs;

  rvs = prec->dpvt;

  if( rvs == NULL)
    return 1;

  epicsMutexMustLock( rvs->lock);

  // Don't do anything if we are waiting for the redis callback
  //
  if( !rvs->writePending) {
    if( rvs->writeWasPending) {
      // Finish up from previous write
      //
      rvs->writeWasPending = 0;
      prec->pact           = 0;
      fprintf( stderr, "%s: finished writing to redis\n", id);
    } else {
      snprintf( tmp, sizeof(tmp)-1, "%.*f", prec->prec, prec->val);
      tmp[sizeof(tmp)-1] = 0;
      redisAsyncCommand( rvs->rs->wc, lsRedis_writeAOCB, rvs, "HSET %s VALUE %s", rvs->redisKey, tmp);
      rvs->writePending    = 1;
      rvs->writeWasPending = 1;
      prec->pact           = 1;
      fprintf( stderr, "%s: writing to '%s'  value %.*f\n", id, rvs->redisKey, prec->prec, prec->val);
    }
  }

  epicsMutexUnlock( rvs->lock);


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
} devAoRedisValue = {
  6, /* space for 6 functions */
  NULL,
  value_init,
  value_init_ao_record,
  value_get_ioint_info,
  value_write_ao,
  NULL
};

epicsExportAddress(dset,devAoRedisValue);

