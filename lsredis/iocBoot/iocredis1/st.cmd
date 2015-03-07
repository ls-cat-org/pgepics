#!../../bin/linux-x86_64/redis

## You may have to change redis to something else
## everywhere it appears in this file

#< envPaths
epicsEnvSet("TOP","/usr/local/epics/synApps_5_7/support/kbdev")

## Register all support components
dbLoadDatabase("$(TOP)/dbd/redis.dbd",0,0)
redis_registerRecordDeviceDriver(pdbbase) 


dbLoadRecords("$(TOP)/db/redis.db","P=test:redis,D=Random,S=324235")
dbLoadRecords("$(TOP)/db/redis.db","P=test:redisasync,D=Random Async,S=324235")
dbLoadRecords("$(TOP)/db/redis.db","P=test:redisintr,D=Random Intr,S=324235")

## Load record instances
#dbLoadRecords("$(TOP)/db/redis.db","user=brister")

iocInit()

## Start any sequence programs
#seq sncredis,"user=brister"
