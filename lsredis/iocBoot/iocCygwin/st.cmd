# Cygwin startup script

# For devIocStats
epicsEnvSet("ENGINEER","engineer")
epicsEnvSet("LOCATION","location")

< envPaths

# save_restore.cmd needs the full path to the startup directory, which
# envPaths currently does not provide
epicsEnvSet(STARTUP,$(TOP)/iocBoot/$(IOC))

# Increase size of buffer for error logging from default 1256
errlogInit(20000)

# Specify largest array CA will transport
# Note for N sscanRecord data points, need (N+1)*8 bytes, else MEDM
# plot doesn't display
#epicsEnvSet EPICS_CA_MAX_ARRAY_BYTES 64008

# set the protocol path for streamDevice
epicsEnvSet("STREAM_PROTOCOL_PATH", ".")

################################################################################
# Tell EPICS all about the record types, device-support modules, drivers,
# etc. in the software we just loaded (lsredis.munch)
dbLoadDatabase("../../dbd/ioclsredisCygwin.dbd")
ioclsredisCygwin_registerRecordDeviceDriver(pdbbase)

#< areaDetector.cmd

### save_restore setup
# We presume a suitable initHook routine was compiled into lsredis.munch.
# See also create_monitor_set(), after iocInit() .
< save_restore.cmd

# serial support
#< serial.cmd

# Motors
#dbLoadTemplate("basic_motor.substitutions")
#dbLoadTemplate("motor.substitutions")
dbLoadTemplate("softMotor.substitutions")
#dbLoadTemplate("pseudoMotor.substitutions")
< motorSim.cmd

### Allstop, alldone
#var motorUtil_debug,1
dbLoadRecords("$(MOTOR)/db/motorUtil.db", "P=lsredis:")

### streamDevice example
#dbLoadRecords("$(TOP)/lsredisApp/Db/streamExample.db","P=lsredis:,PORT=serial1")

### Scan-support software
# crate-resident scan.  This executes 1D, 2D, 3D, and 4D scans, and caches
# 1D data, but it doesn't store anything to disk.  (See 'saveData' below for that.)
dbLoadRecords("$(SSCAN)/sscanApp/Db/standardScans.db","P=lsredis:,MAXPTS1=2000,MAXPTS2=1000,MAXPTS3=10,MAXPTS4=10,MAXPTSH=2000")
dbLoadRecords("$(SSCAN)/sscanApp/Db/saveData.db","P=lsredis:")

# A set of scan parameters for each positioner.  This is a convenience
# for the user.  It can contain an entry for each scannable thing in the
# crate.
dbLoadTemplate("scanParms.substitutions")

### Slits
dbLoadRecords("$(OPTICS)/opticsApp/Db/2slit.db","P=lsredis:,SLIT=Slit1V,mXp=m3,mXn=m4")
dbLoadRecords("$(OPTICS)/opticsApp/Db/2slit.db","P=lsredis:,SLIT=Slit1H,mXp=m5,mXn=m6")

# X-ray Instrumentation Associates Huber Slit Controller
# supported by a customized version of the SNL program written by Pete Jemian
#dbLoadRecords("$(OPTICS)/opticsApp/Db/xia_slit.db", "P=lsredis:, HSC=hsc1:")
#dbLoadRecords("$(OPTICS)/opticsApp/Db/xia_slit.db", "P=lsredis:, HSC=hsc2:")
#dbLoadRecords("$(IP)/ipApp/Db/generic_serial.db", "P=lsredis:,C=0,SERVER=serial7")


### 2-post mirror
#dbLoadRecords("$(OPTICS)/opticsApp/Db/2postMirror.db","P=lsredis:,Q=M1,mDn=m18,mUp=m17,LENGTH=0.3")

### User filters
#dbLoadRecords("$(OPTICS)/opticsApp/Db/filterMotor.db","P=lsredis:,Q=fltr1:,MOTOR=m1,LOCK=fltr_1_2:")
#dbLoadRecords("$(OPTICS)/opticsApp/Db/filterMotor.db","P=lsredis:,Q=fltr2:,MOTOR=m2,LOCK=fltr_1_2:")
#dbLoadRecords("$(OPTICS)/opticsApp/Db/filterLock.db","P=lsredis:,Q=fltr2:,LOCK=fltr_1_2:,LOCK_PV=lsredis:DAC1_1.VAL")

# XIA shutter
#dbLoadRecords("$(OPTICS)/opticsApp/Db/XIA_shutter.db","P=lsredis:,S=shutter1,PORT=serial2,ADDRESS=0123")

### Optical tables
#tableRecordDebug=1
dbLoadRecords("$(OPTICS)/opticsApp/Db/table.db","P=lsredis:,Q=Table1,T=table1,M0X=m1,M0Y=m2,M1Y=m3,M2X=m4,M2Y=m5,M2Z=m6,GEOM=SRI")

# Io calculation
#dbLoadRecords("$(OPTICS)/opticsApp/Db/Io.db","P=lsredis:Io:")

### Monochromator support ###
# Kohzu and PSL monochromators: Bragg and theta/Y/Z motors
# standard geometry (geometry 1)
#dbLoadRecords("$(OPTICS)/opticsApp/Db/kohzuSeq.db","P=lsredis:,M_THETA=m9,M_Y=m10,M_Z=m11,yOffLo=17.4999,yOffHi=17.5001")
# modified geometry (geometry 2)
#dbLoadRecords("$(OPTICS)/opticsApp/Db/kohzuSeq.db","P=lsredis:,M_THETA=m9,M_Y=m10,M_Z=m11,yOffLo=4,yOffHi=36")

# Spherical grating monochromator
#dbLoadRecords("$(OPTICS)/opticsApp/Db/SGM.db","P=lsredis:,N=1,M_x=m7,M_rIn=m6,M_rOut=m8,M_g=m9")

# 4-bounce high-resolution monochromator
#dbLoadRecords("$(OPTICS)/opticsApp/Db/hrSeq.db","P=lsredis:,N=1,M_PHI1=m9,M_PHI2=m10")
#dbLoadRecords("$(OPTICS)/opticsApp/Db/hrSeq.db","P=lsredis:,N=2,M_PHI1=m11,M_PHI2=m12")

### Orientation matrix, four-circle diffractometer (see seq program 'orient' below)
#dbLoadRecords("$(OPTICS)/opticsApp/Db/orient.db", "P=lsredis:,O=1,PREC=4")
#dbLoadTemplate("orient_xtals.substitutions")

# Load single element Canberra AIM MCA and ICB modules
#< canberra_1.cmd

# Load 13 element detector software
#< canberra_13.cmd

# Load 3 element detector software
#< canberra_3.cmd

### Stuff for user programming ###
dbLoadRecords("$(CALC)/calcApp/Db/userCalcs10.db","P=lsredis:")
dbLoadRecords("$(CALC)/calcApp/Db/userCalcOuts10.db","P=lsredis:")
dbLoadRecords("$(CALC)/calcApp/Db/userStringCalcs10.db","P=lsredis:")
dbLoadRecords("$(CALC)/calcApp/Db/userArrayCalcs10.db","P=lsredis:,N=2000")
dbLoadRecords("$(CALC)/calcApp/Db/userTransforms10.db","P=lsredis:")
dbLoadRecords("$(CALC)/calcApp/Db/userAve10.db","P=lsredis:")
# string sequence (sseq) records
dbLoadRecords("$(CALC)/calcApp/Db/userStringSeqs10.db","P=lsredis:")
# 4-step measurement
dbLoadRecords("$(STD)/stdApp/Db/4step.db", "P=lsredis:")
# interpolation
dbLoadRecords("$(CALC)/calcApp/Db/interp.db", "P=lsredis:,N=2000")
# array test
dbLoadRecords("$(CALC)/calcApp/Db/arrayTest.db", "P=lsredis:,N=2000")

# pvHistory (in-crate archive of up to three PV's)
dbLoadRecords("$(STD)/stdApp/Db/pvHistory.db","P=lsredis:,N=1,MAXSAMPLES=1440")

# resettable timer
dbLoadRecords("$(STD)/stdApp/Db/timer.db","P=lsredis:,N=1")

# Slow feedback
dbLoadTemplate "pid_slow.substitutions"

# PID-based feedback
#dbLoadTemplate "fb_epid.substitutions"

# Miscellaneous PV's, such as burtResult
dbLoadRecords("$(STD)/stdApp/Db/misc.db","P=lsredis:")
#dbLoadRecords("$(STD)/stdApp/Db/VXstats.db","P=lsredis:")
dbLoadRecords("$(DEVIOCSTATS)/db/ioc.db","IOCNAME=lsredis, TODFORMAT=%m/%d/%Y %H:%M:%S")

### Load database record for alive heartbeating support.
# RHOST specifies the IP address that receives the heartbeats.
#dbLoadRecords("$(ALIVE)/aliveApp/Db/alive.db", "P=lsredis:,RHOST=X.X.X.X")

###############################################################################
iocInit
###############################################################################

# write all the PV names to a local file
dbl > dbl-all.txt

### startup State Notation Language programs
#seq &kohzuCtl, "P=lsredis:, M_THETA=m9, M_Y=m10, M_Z=m11, GEOM=1, logfile=kohzuCtl.log"
#seq &hrCtl, "P=lsredis:, N=1, M_PHI1=m9, M_PHI2=m10, logfile=hrCtl1.log"

# Keithley 2000 series DMM
# channels: 10, 20, or 22;  model: 2000 or 2700
#seq &Keithley2kDMM,("P=lsredis:, Dmm=D1, channels=22, model=2700")

# X-ray Instrumentation Associates Huber Slit Controller
# supported by a SNL program written by Pete Jemian and modified (TMM) for use with the
# sscan record
#seq  &xia_slit, "name=hsc1, P=lsredis:, HSC=hsc1:, S=lsredis:seriala[6]"

# Orientation-matrix
#seq &orient, "P=lsredis:orient1:,PM=lsredis:,mTTH=m9,mTH=m10,mCHI=m11,mPHI=m12"

# Io calculation
#seq &Io, "P=lsredis:Io:,MONO=lsredis:BraggEAO,VSC=lsredis:scaler1"

# Start PF4 filter sequence program
#seq pf4,"name=pf1,P=lsredis:,H=pf4:,B=A,M=lsredis:BraggEAO,B1=lsredis:Unidig1Bo3,B2=lsredis:Unidig1Bo4,B3=lsredis:Unidig1Bo5,B4=lsredis:Unidig1Bo6"
#seq pf4,"name=pf2,P=lsredis:,H=pf4:,B=B,M=lsredis:BraggEAO,B1=lsredis:Unidig1Bo7,B2=lsredis:Unidig1Bo8,B3=lsredis:Unidig1Bo9,B4=lsredis:Unidig1Bo10"

# Start Femto amplifier sequence programs
#seq femto,"name=fem1,P=lsredis:,H=fem01:,F=seq01:,G1=lsredis:Unidig1Bo6,G2=lsredis:Unidig1Bo7,G3=lsredis:Unidig1Bo8,NO=lsredis:Unidig1Bo10"

### Start up the autosave task and tell it what to do.
# The task is actually named "save_restore".
# Note that you can reload these sets after creating them: e.g., 
# reload_monitor_set("auto_settings.req",30,"P=lsredis:")
#save_restoreDebug=20
#
# save positions every five seconds
create_monitor_set("auto_positions.req",5,"P=lsredis:")
# save other things every thirty seconds
create_monitor_set("auto_settings.req",30,"P=lsredis:")

### Start the saveData task.
saveData_Init("saveData.req", "P=lsredis:")

# motorUtil (allstop & alldone)
motorUtilInit("lsredis:")

dbcar(0,1)

# print the time our boot was finished
date
