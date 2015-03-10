TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = redis
# redis.dbd will be created and installed
DBD += redis.dbd
# redis.dbd will be made up from these files:
redis_DBD += base.dbd
redis_DBD += redisdev.dbd

# Include dbd files from all support applications:
#redis_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#redis_LIBS += xxx

# redis_registerRecordDeviceDriver.cpp derives from redis.dbd
redis_SRCS += redis_registerRecordDeviceDriver.cpp
redis_SRCS += devLSRedis.c
redis_SRCS += lsRedisAISupport.c

# Build the main IOC entry point on workstation OSs.
redis_SRCS_DEFAULT += redisMain.cpp
redis_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#redis_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
redis_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

redis_SYS_LIBS_Linux += hiredis


.PHONY: printvars
printvars:
	@$(foreach V,$(sort $(.VARIABLES)), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V) ($(value $V)))))
