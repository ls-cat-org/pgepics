TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# For Kate Feng's implementation of devScaler.c, which uses Till
# Straumann's basicIoOps.h to bridge any mismatch between endianness
# of Joerger scaler hardware and endianness of processor.
#USR_CFLAGS += -DHAS_IOOPS_H

#=============================
# build an ioc application

PROD_IOC = lsredis

# Note that the order in which .dbd files and libraries are added to a list
# variable is important.  For .dbd files, things must be defined before they
# are referenced by a later .dbd file.  For libraries, things must be referenced
# before they are searched for; otherwise they won't be included.

# Custom stuff in development

##################################################################
# First, define lsredis_Common_DBD and lsredis_Common_LIBS from the modules
# defined in RELEASE
##################################################################
DBD += lsredis_Common.dbd
lsredis_Common_DBD += base.dbd lsredis.dbd

#ifdef CAPUTLOG
#       lsredis_Common_DBD += caPutLog.dbd
#       lsredis_Common_LIBS := caPutLog $(lsredis_Common_LIBS)
#endif

ifdef CAPUTRECORDER
        lsredis_Common_DBD += caputRecorder.dbd
        lsredis_Common_LIBS := caputRecorder $(lsredis_Common_LIBS)
endif

ifdef SNCSEQ
        ifeq (seq-2-1, $(findstring seq-2-1, $(SNCSEQ)))
                lsredis_Common_DBD += devSequencer.dbd
                lsredis_Common_LIBS := seqDev seq pv $(lsredis_Common_LIBS)
        else ifeq (seq-2.1, $(findstring seq-2.1, $(SNCSEQ)))
                lsredis_Common_DBD += devSequencer.dbd
                lsredis_Common_LIBS := seqDev seq pv $(lsredis_Common_LIBS)
        else
                # seq 2.2.
                lsredis_Common_LIBS := seq pv $(lsredis_Common_LIBS)
        endif
endif

ifdef ASYN
        lsredis_Common_DBD += asyn.dbd drvAsynSerialPort.dbd
        lsredis_Common_DBD += drvAsynIPPort.dbd drvVxi11.dbd devGpib.dbd
        lsredis_Common_LIBS := asyn $(lsredis_Common_LIBS)
endif

ifdef STD
        lsredis_Common_DBD += stdSupport.dbd
        lsredis_Common_LIBS := std $(lsredis_Common_LIBS)
endif

ifdef CALC
        lsredis_Common_DBD += calcSupport.dbd
        lsredis_Common_LIBS := calc $(lsredis_Common_LIBS)
endif

ifdef BUSY
        lsredis_Common_DBD += busySupport.dbd
        lsredis_Common_LIBS := busy $(lsredis_Common_LIBS)
endif

ifdef SSCAN
        lsredis_Common_DBD += sscanSupport.dbd
        lsredis_Common_LIBS := sscan $(lsredis_Common_LIBS)
        ifdef SNCSEQ
                lsredis_Common_DBD += sscanProgressSupport.dbd
                lsredis_Common_LIBS := scanProgress $(lsredis_Common_LIBS)
        endif
endif

ifdef AUTOSAVE
        lsredis_Common_DBD += asSupport.dbd
        lsredis_Common_LIBS := autosave $(lsredis_Common_LIBS)
endif

ifdef OPTICS
        lsredis_Common_DBD += opticsSupport.dbd
        lsredis_Common_LIBS := optics $(lsredis_Common_LIBS)
endif

ifdef MCA
        lsredis_Common_DBD += mcaSupport.dbd
        lsredis_Common_LIBS := mca $(lsredis_Common_LIBS)
endif

ifdef IP
        lsredis_Common_DBD += ipSupport.dbd
        lsredis_Common_LIBS := ip $(lsredis_Common_LIBS)
endif

ifdef MODBUS
        lsredis_Common_DBD += modbusSupport.dbd
        lsredis_Common_LIBS := modbus $(lsredis_Common_LIBS)
endif

ifdef MOTOR
        lsredis_Common_DBD += motorSupport.dbd devAcsMotor.dbd devAerotech.dbd
        lsredis_Common_DBD += devAttocube.dbd devFaulhaberMotor.dbd devImsMotor.dbd
        lsredis_Common_DBD += devKohzuMotor.dbd devMclennanMotor.dbd devMicos.dbd
        lsredis_Common_DBD += devMicroMo.dbd devNewFocus.dbd devNewport.dbd
        lsredis_Common_DBD += devOriel.dbd devPC6K.dbd devPIJena.dbd devPIMotor.dbd
        lsredis_Common_DBD += devSPiiPlus.dbd devSmartMotorMotor.dbd devSoftMotor.dbd
        lsredis_Common_DBD += devThorLabs.dbd motorSimSupport.dbd

        lsredis_Common_LIBS := motor $(lsredis_Common_LIBS)
        lsredis_Common_LIBS := Acs acsTech80 Aerotech Attocube Faulhaber Ims KohzuMotor $(lsredis_Common_LIBS)
        lsredis_Common_LIBS := Mclennan Micos MicroMo NewFocus Newport Oriel PI PIJena $(lsredis_Common_LIBS)
        lsredis_Common_LIBS := Parker SmartMotor ThorLabs softMotor motorSimSupport $(lsredis_Common_LIBS)
endif

ifdef STREAM
        lsredis_Common_DBD += stream.dbd streamSynApps.dbd
        lsredis_Common_LIBS := stream streamSynApps $(lsredis_Common_LIBS)
endif

ifdef DEVIOCSTATS
  ifneq ($(OS_CLASS), WIN32)
        lsredis_Common_DBD += devIocStats.dbd iocAdmin.dbd
        lsredis_Common_LIBS := devIocStats $(lsredis_Common_LIBS)
  endif
endif

ifdef ETHERIP
        lsredis_Common_DBD += ether_ip.dbd
        lsredis_Common_LIBS := ether_ip $(lsredis_Common_LIBS)
endif

ifdef ALIVE
        lsredis_Common_DBD += aliveSupport.dbd
        lsredis_Common_LIBS := alive $(lsredis_Common_LIBS)
endif

ifdef DELAYGEN
        lsredis_Common_DBD += delaygenSupport.dbd
        lsredis_Common_LIBS := delaygen $(lsredis_Common_LIBS)
endif

ifdef VAC
        lsredis_Common_DBD += vacSupport.dbd
        lsredis_Common_LIBS := vac $(lsredis_Common_LIBS)
endif

lsredis_Common_LIBS := $(lsredis_Common_LIBS) $(EPICS_BASE_IOC_LIBS)

##################################################################


ifeq (win32-x86,$(findstring win32-x86, $(T_A)))

        DBD  += ioclsredisWin32.dbd
        ioclsredisWin32_DBD += lsredis_Common.dbd

        ifdef MCA
                # If your host supports libpcap and libnet and you want to build
                # Canberra network support uncomment the following lines
                #ioclsredisWin32_DBD += mcaCanberraSupport.dbd
                #lsredis_LIBS_WIN32 += mcaCanberra
                #lsredis_LIBS_WIN32 += wpcap Packet
        endif

        ifdef DXP
                # If you want to build parallel port or USB interfaces to Saturn and/or
                # xMAP DXP, then uncomment the following lines
                ioclsredisWin32_DBD += dxpSupport.dbd
                lsredis_LIBS_WIN32 += dxp handel
                lsredis_LIBS_WIN32 += DLPORTIO PlxApi
                lsredis_SYS_LIBS_WIN32 += setupapi
        endif

endif

ifeq (windows-x64,$(findstring windows-x64, $(T_A)))
        DBD  += ioclsredisWin64.dbd
        ioclsredisWin64_DBD += lsredis_Common.dbd

        ifdef MCA
                # If your host supports libpcap and libnet and you want to build Canberra network
                # support uncomment the following line
                ioclsredisWin64_DBD += mcaCanberraSupport.dbd
                lsredis_LIBS_WIN32 += mcaCanberra
                lsredis_LIBS_WIN32 += wpcap Packet
        endif

endif

ifeq ($(EPICS_HOST_ARCH), cygwin-x86)
        DBD  += ioclsredisCygwin.dbd
        ioclsredisCygwin_DBD += lsredis_Common.dbd

        ifdef DXP
                # If you want to build parallel port or USB interfaces to Saturn and/or
                # xMAP DXP,then uncomment the following lines
                #ioclsredisCygwin_DBD += dxpSupport.dbd
                #lsredis_LIBS_cygwin32 += dxp handel
                #lsredis_SYS_LIBS_cygwin32 += DLPORTIO PlxApi setupapi
        endif

        lsredis_SYS_LIBS_cygwin32 += $(CYGWIN_RPC_LIB)

endif

ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
        DBD  += ioclsredisVX.dbd
        ioclsredisVX_DBD += lsredis_Common.dbd

        ifdef ALLEN_BRADLEY
                lsredis_LIBS_vxWorks += allenBradley
        endif

        ifdef QUADEM
                ioclsredisVX_DBD += drvAHlsredis.dbd
                ioclsredisVX_DBD += drvAPS_EM.dbd
                lsredis_LIBS_vxWorks += quadEM
        endif

        ifdef AREA_DETECTOR
                # areaDetector provides a Makefile for things it needs and supplies
                PROD_NAME = AD
                -include $(ADCORE)/ADApp/commonDriverMakefile
                ioclsredisVX_DBD += ADSupport.dbd NDPluginSupport.dbd
                lsredis_LIBS_vxWorks += $(AD_LIBS)
                # To add the simDetector uncomment the following lines
                ioclsredisVX_DBD += simDetectorSupport.dbd
                lsredis_LIBS_vxWorks += simDetector
        endif

        ifdef VME
                ioclsredisVX_DBD += vmeVXSupport.dbd
                lsredis_LIBS_vxWorks += vme
        endif

        ifdef STD
                ioclsredisVX_DBD += stdVXSupport.dbd
        endif

        ifdef IP
                ioclsredisVX_DBD += ipVXSupport.dbd
        endif

        ifdef MCA
                ioclsredisVX_DBD += SIS38XXSupport.dbd
                ioclsredisVX_DBD += mcaCanberraSupport.dbd
                lsredis_LIBS_vxWorks += SIS38XX
                lsredis_LIBS_vxWorks += mcaCanberra
        endif

        ifdef IPUNIDIG
                ioclsredisVX_DBD += ipUnidigSupport.dbd
                lsredis_LIBS_vxWorks += ipUnidig
        endif

        ifdef DAC128V
                ioclsredisVX_DBD += dac128VSupport.dbd
                lsredis_LIBS_vxWorks += dac128V
        endif

        ifdef IP330
                ioclsredisVX_DBD += ip330Support.dbd
                lsredis_LIBS_vxWorks += ip330
        endif

        ifdef CAMAC
                ioclsredisVX_DBD += camacSupport.dbd
                ioclsredisVX_DBD += devCamacSupport.dbd
                lsredis_LIBS_vxWorks += camac devCamac
        endif

        ifdef ALLEN_BRADLEY
                ioclsredisVX_DBD += allenBradley.dbd
        endif

        ifdef LOVE
                ioclsredisVX_DBD += devLove.dbd
                lsredis_LIBS_vxWorks += love
        endif

        ifdef IPAC
                ioclsredisVX_DBD += tyGSOctal.dbd
                ioclsredisVX_DBD += drvIpac.dbd
                lsredis_LIBS_vxWorks += TyGSOctal Ipac
        endif

        ifdef ASYN
                ioclsredisVX_DBD += drvGsIP488.dbd
        endif

        ifdef SOFTGLUE
                ioclsredisVX_DBD += softGlueSupport.dbd
                lsredis_LIBS_vxWorks += softGlue
        endif

        ifdef MOTOR
                ioclsredisVX_DBD += devDeltaTau.dbd
                ioclsredisVX_DBD += devOms.dbd
                lsredis_LIBS_vxWorks += oms DeltaTau
        endif

        ifdef DEVIOCSTATS
                ioclsredisVX_DBD += devVxStats.dbd
        endif
endif

ifneq (,$(findstring linux, $(T_A)))
        DBD  += ioclsredisLinux.dbd
        ioclsredisLinux_DBD += lsredis_Common.dbd

ifeq ($(HAVE_HDF5), YES)
ifeq ($(HAVE_SZIP), YES)

        ifdef QUADEM
                ioclsredisLinux_DBD += drvAHlsredis.dbd
                lsredis_LIBS_Linux += quadEM
        endif

        ifdef AREA_DETECTOR
                # areaDetector provides a Makefile for things it needs and supplies
                PROD_NAME = AD
                -include $(ADCORE)/ADApp/commonDriverMakefile
                ioclsredisLinux_DBD += ADSupport.dbd NDPluginSupport.dbd
                lsredis_LIBS_Linux += $(AD_LIBS)
                # To add the simDetector uncomment the following lines
                ioclsredisLinux_DBD += simDetectorSupport.dbd
                lsredis_LIBS_Linux += simDetector
        endif

        ifdef DXP
                # If your host supports parallel port or USB interfaces to Saturn DXP, set
                # LINUX_USB_INSTALLED to YES in <synApps>/configure/CONFIG_SITE.
                ifeq ($(LINUX_USB_INSTALLED), YES)
                        ioclsredisLinux_DBD += dxpSupport.dbd
                        lsredis_LIBS_Linux  += dxp handel
                        lsredis_SYS_LIBS_Linux += usb
                endif
        endif

endif
endif

        ifdef MCA
                # If your host supports libpcap and libnet and you want to build Canberra network
                # support, set LINUX_NET_INSTALLED to YES in <synApps>/configure/CONFIG_SITE.
                ifeq ($(LINUX_NET_INSTALLED), YES)
                        ioclsredisLinux_DBD += mcaCanberraSupport.dbd
                        lsredis_LIBS_Linux += mcaCanberra
                        lsredis_SYS_LIBS_Linux += net pcap
                endif
        endif

        ifdef IPAC
                ioclsredisLinux_DBD += drvIpac.dbd
                lsredis_LIBS_Linux += Ipac
        endif

        ifdef SOFTGLUE
                ioclsredisLinux_DBD += softGlueSupport.dbd
                lsredis_LIBS_Linux += softGlue
        endif

        lsredis_SYS_LIBS_Linux += hiredis
endif

ifeq ($(OS_CLASS), Darwin)
        DBD  += ioclsredisDarwin.dbd
        ioclsredisDarwin_DBD += lsredis_Common.dbd
endif



# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
lsredis_SRCS_vxWorks  += ioclsredisVX_registerRecordDeviceDriver.cpp
lsredis_SRCS_Linux    += ioclsredisLinux_registerRecordDeviceDriver.cpp     lsredisMain.cpp drvRedis.cpp
ifeq (win32-x86,$(findstring win32-x86, $(T_A)))
        lsredis_SRCS_WIN32    += ioclsredisWin32_registerRecordDeviceDriver.cpp     lsredisMain.cpp
endif
ifeq (windows-x64,$(findstring windows-x64, $(T_A)))
        lsredis_SRCS_WIN32    += ioclsredisWin64_registerRecordDeviceDriver.cpp     lsredisMain.cpp
endif
lsredis_SRCS_cygwin32 += ioclsredisCygwin_registerRecordDeviceDriver.cpp    lsredisMain.cpp
lsredis_SRCS_solaris  += ioclsredis_solaris_registerRecordDeviceDriver.cpp  lsredisMain.cpp
lsredis_SRCS_Darwin   += ioclsredisDarwin_registerRecordDeviceDriver.cpp    lsredisMain.cpp



#The following adds support from base/src/vxWorks
lsredis_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

lsredis_LIBS_vxWorks  += $(lsredis_Common_LIBS)
lsredis_LIBS_Linux    += $(lsredis_Common_LIBS)
lsredis_LIBS_solaris  += $(lsredis_Common_LIBS)
lsredis_LIBS_cygwin32 += $(lsredis_Common_LIBS)
lsredis_LIBS_WIN32    += $(lsredis_Common_LIBS)
lsredis_LIBS_Darwin   += $(lsredis_Common_LIBS)


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

$(COMMON_DIR)/ioclsredisWin32Include.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisWin64Include.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisCygwinInclude.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisVXInclude.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisLinuxInclude.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisDarwinInclude.dbd: $(COMMON_DIR)/lsredis_Common.dbd

$(COMMON_DIR)/ioclsredisWin32.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisWin64.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisCygwin.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisVX.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisLinux.dbd: $(COMMON_DIR)/lsredis_Common.dbd
$(COMMON_DIR)/ioclsredisDarwin.dbd: $(COMMON_DIR)/lsredis_Common.dbd

ioclsredisWin32.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
ioclsredisWin64.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
ioclsredisCygwin.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
ioclsredisVX.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
ioclsredisLinux.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
ioclsredisDarwin.dbd$(DEP): $(COMMON_DIR)/lsredis_Common.dbd
