#include <stdlib.h>
#include <epicsExport.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recSup.h>
#include <recGbl.h>
#include <callback.h>

#include <aiRecord.h>

void redis_cb( CALLBACK *cb);

static long init_record(aiRecord *pao);
static long read_ai(aiRecord *pao);

struct redisState {
  unsigned int seed;
  CALLBACK cb; /* New */
};

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiRedisAsync = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  init_record,
  NULL,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiRedisAsync); /* change name */

void prng_cb(CALLBACK* cb);

static long init_record(aiRecord *pao)
{
  struct redisState* priv;
  unsigned long start;

  priv=malloc(sizeof(struct redisState));
  if(!priv){
    recGblRecordError(S_db_noMemory, (void*)pao,
		      "devRedisAsync failed to allocate private struct");
    return S_db_noMemory;
  }

  /* New */
  callbackSetCallback(redis_cb,&priv->cb);
  callbackSetPriority(priorityLow,&priv->cb);
  callbackSetUser(pao,&priv->cb);
  priv->cb.timer=NULL;

  recGblInitConstantLink(&pao->inp,DBF_ULONG,&start);

  priv->seed=start;
  pao->dpvt=priv;

  return 0;
}


static long read_ai(aiRecord *pao)
{
  struct redisState* priv=pao->dpvt;

  if( ! pao->pact ){
    /* start async operation */
    pao->pact=TRUE;
    callbackSetUser(pao,&priv->cb);
    callbackRequestDelayed(&priv->cb,0.1);
    return 0;
  }else{
    /* complete operation */
    pao->pact=FALSE;
    return 0;
  }
}


void redis_cb(CALLBACK* cb)
{
  aiRecord* prec;
  struct redisState* priv;
  struct rset* prset;
  epicsInt32 raw;

  callbackGetUser(prec,cb);
  prset=(struct rset*)prec->rset;
  priv=prec->dpvt;

  raw=rand_r(&priv->seed);

  dbScanLock((dbCommon*)prec);
  prec->rval=raw;
  (*prset->process)(prec);
  dbScanUnlock((dbCommon*)prec);
}

