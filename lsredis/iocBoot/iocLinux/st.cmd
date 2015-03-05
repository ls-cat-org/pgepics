# Linux startup script

# For devIocStats
epicsEnvSet("ENGINEER","engineer")
epicsEnvSet("LOCATION","location")

epicsEnvSet("STREAM_PROTOCOL_PATH", ".")

< envPaths

epicsEnvSet(STARTUP,$(TOP)/iocBoot/$(IOC))

# Increase size of buffer for error logging from default 1256
errlogInit(20000)

# Specify largest array CA will transport
# Note for N doubles, need N*8 bytes+some overhead
epicsEnvSet EPICS_CA_MAX_ARRAY_BYTES 64010

################################################################################
# Tell EPICS all about the record types, device-support modules, drivers,
# etc. in the software we just loaded (lsredis.munch)
dbLoadDatabase("../../dbd/ioclsredisLinux.dbd")
ioclsredisLinux_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("$(TOP)/db/lsredis.template","P=21:orange:,R=redis:,PORT=Redis,ADDR=1,TIMEOUT=1")
initREDIS("Redis","stns.2", "127.0.0.1",6379,2);

###############################################################################
iocInit
###############################################################################

# write all the PV names to a local file
dbl > dbl-all.txt

# Report  states of database CA links
dbcar(*,1)

# print the time our boot was finished
date

