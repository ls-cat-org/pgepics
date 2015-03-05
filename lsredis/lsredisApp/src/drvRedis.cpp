/* drvRedis.cpp
 * Driver for redis support in epics
*/

#include <asynPortDriver.h>
#include <hiredis/hiredis.h>
#include <iocsh.h>
#include <epicsExport.h>
#include <string.h>

//static const char *driverName = "REDIS";

/** This is the class definition for the DAC128V class
  */
class REDIS : public asynPortDriver {
public:
  REDIS(const char *portName, const char *stnsName, const char *hostName, int port, int db);

  /* These are the methods that we override from asynPortDriver */
  virtual asynStatus writeOctet( asynUser *pasyncUser, const char *value, size_t nChars, size_t *nActual);
  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
  virtual void report(FILE *fp, int details);

protected:
  int REDIS_Key;          /**< (asynOctet, r/w) DAC output value in device units */
#define FIRST_REDIS_PARAM REDIS_Key
  int REDIS_Value;        /**< (asynOctet, r/w) DAC output value in device units but double */
  int BNP_FB_DETUNE;
  int BNP_FB_TARGET;
  int BNP_FB_FWHM;
  int BNP_FB_PEAKX;
  int BNP_FB_PEAKY;
  int BNP_FB_PARTIALSCAN;
  int BNP_FB_FULLSCAN;
  int BNP_FB_OFFON;
#define LAST_REDIS_PARAM BNP_FB_OFFON
    
private:
  redisContext *rc;
};


#define REDISKeyString         "KEY"
#define REDISValueString       "VALUE"
#define BNPFBDETUNEString      "BNP_FB_DETUNE"
#define BNPFBTARGETString      "BNP_FB_TARGET"
#define BNPFBFWHMString        "BNP_FB_FWHM"
#define BNPFBPEAKXString       "BNP_FB_PEAKX"
#define BNPFBPEAKYString       "BNP_FB_PEAKY"
#define BNPFBPARTIALSCANString "BNP_FB_PARTIALSCAN"
#define BNPFBFULLSCANString    "BNP_FB_FULLSCAN"
#define BNPFBOFFONString       "BNP_FB_OFFON"

#define NUM_PARAMS (&LAST_REDIS_PARAM - &FIRST_REDIS_PARAM + 1)

REDIS::REDIS(const char *portName, const char *stnsPrefix, const char *hostName, int port, int db) :
asynPortDriver(portName, 1, NUM_PARAMS, 
		   asynOctetMask | asynFloat64Mask | asynDrvUserMask | asynInt32Mask,
		   asynOctetMask | asynFloat64Mask | asynInt32Mask,
		   ASYN_CANBLOCK, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=1, autoConnect=1 */
		   0, 0)  /* Default priority and stack size */
{
  //static const char *functionName = "REDIS";
    redisReply *reply;
    createParam(REDISKeyString,         asynParamOctet,   &REDIS_Key);
    createParam(REDISValueString,       asynParamOctet,   &REDIS_Value);
    createParam(BNPFBDETUNEString,      asynParamFloat64, &BNP_FB_DETUNE);
    createParam(BNPFBFWHMString,        asynParamFloat64, &BNP_FB_FWHM);
    createParam(BNPFBTARGETString,      asynParamFloat64, &BNP_FB_TARGET);
    createParam(BNPFBPEAKXString,       asynParamFloat64, &BNP_FB_PEAKX);
    createParam(BNPFBPEAKYString,       asynParamFloat64, &BNP_FB_PEAKY);
    createParam(BNPFBPARTIALSCANString, asynParamInt32,   &BNP_FB_PARTIALSCAN);
    createParam(BNPFBFULLSCANString,    asynParamInt32,   &BNP_FB_FULLSCAN);
    createParam(BNPFBOFFONString,       asynParamInt32,   &BNP_FB_OFFON);
    
    rc = redisConnect( hostName, port);
    if( rc && rc->err) {
      // some sort of error raising stuff happens here
    }
    if( db != 0) {
      reply = (redisReply *)redisCommand( rc, "SELECT %d", db);
      freeReplyObject( reply);
    }
    fprintf( stderr, "REDIS constructor  REDIS_Key: %d   REDIS_Value: %d\n", REDIS_Key, REDIS_Value);


}

asynStatus REDIS::writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual) {
  //static const char *functionName = "writeOctet";
  int theParam = pasynUser->reason;
  asynStatus status;
  char key[64];
  redisReply *reply;

  fprintf( stderr, "writeOctet: reason=%d, value='%s'\n", pasynUser->reason, value);

  if( theParam == REDIS_Key) {
    status = asynPortDriver::writeOctet( pasynUser, value, nChars, nActual);
    return status;
  }

  getStringParam( REDIS_Key, sizeof( key)-1, key);
  key[sizeof(key)-1] = 0;
  
  if( key[0] != 0) {
    reply = (redisReply *)redisCommand( rc, "HSET %s VALUE %s", key, value);
    freeReplyObject( reply);

    fprintf( stderr, "in write key: %s,  value: %s\n", key, value);
  }

  status = asynPortDriver::writeOctet( pasynUser, value, nChars, nActual);
  
  return(status);
}

asynStatus REDIS::readOctet(asynUser *pasynUser, char *value, size_t nChars, size_t *nActual, int *eomReason) {
  //static const char *functionName = "readOctet";
    redisReply *reply;
    char key[64];
    int i;
    

    fprintf( stderr, "readOctet value: '%s'\n", value);

    getStringParam( REDIS_Key, sizeof( key)-1, key);
    key[sizeof(key)-1] = 0;
    
    reply = (redisReply *)redisCommand( rc, "HGET %s VALUE", key);
    fprintf( stderr, "got reply type %d for key %s\n", reply->type, key);
    if( reply->type == REDIS_REPLY_STRING) {
      for( i=0; i < (int)nChars-1 && i<reply->len; i++) {
	value[i] = reply->str[i];
      }
      value[nChars - 1] = 0;
      if( i < (int)nChars)
	value[i] = 0;
    }
    *eomReason = ASYN_EOM_END;
    return(asynSuccess);
}

/* Report  parameters */
void REDIS::report(FILE *fp, int details) {
    asynPortDriver::report(fp, details);

    if (details >= 1) {
      fprintf(fp, "  Redis report goes here\n");
    }
}

/** Configuration command, called directly or from iocsh */
 extern "C" int initREDIS(const char *portName, const char *stnsPrefix, const char *hostName, int port, int db) {
   REDIS *pREDIS = new REDIS( portName, stnsPrefix, hostName, port, db);
   pREDIS = NULL;  /* This is just to avoid compiler warnings */
   return(asynSuccess + (pREDIS == NULL ? 0 : 0));
 }


 static const iocshArg initArg0 = { "Port name", iocshArgString};
 static const iocshArg initArg1 = { "Stns name", iocshArgString};
 static const iocshArg initArg2 = { "Host Name", iocshArgString};
 static const iocshArg initArg3 = { "Port",      iocshArgInt};
 static const iocshArg initArg4 = { "Db",        iocshArgInt};
 static const iocshArg * const initArgs[5] = {
  &initArg0,
  &initArg1,
  &initArg2,
  &initArg3,
  &initArg4
};

static const iocshFuncDef initFuncDef = {"initREDIS",5, initArgs};
static void initCallFunc(const iocshArgBuf *args) {
  initREDIS( args[0].sval, args[1].sval, args[2].sval, args[3].ival, args[4].ival);
}

void drvREDISRegister(void) {
  iocshRegister( &initFuncDef, initCallFunc);
}

extern "C" {
epicsExportRegistrar( drvREDISRegister);
}
