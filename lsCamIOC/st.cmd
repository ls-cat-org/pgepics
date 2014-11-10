errlogInit(20000)
epicsEnvSet("TOP",         "/usr/local/epics/synApps_5_7/support/areaDetector/ADProsilica/iocs/prosilicaIOC");
epicsEnvSet("ADCORE",      "/usr/local/epics/synApps_5_7/support/areaDetector/ADCore");
epicsEnvSet("ADPROSILICA", "/usr/local/epics/synApps_5_7/support/areaDetector/ADProsilica");
epicsEnvSet("CALC",        "/usr/local/epics/synApps_5_7/support/calc-3-2");
epicsEnvSet("SSCAN",       "/usr/local/epics/synApps_5_7/support/sscan-2-9");
epicsEnvSet("AUTOSAVE",    "/usr/local/epics/synApps_5_7/support/autosave-5-1");

dbLoadDatabase("$(TOP)/dbd/prosilicaApp.dbd")

prosilicaApp_registerRecordDeviceDriver(pdbbase) 

epicsEnvSet("PREFIX", "21:CAMS:")
epicsEnvSet("PORT1",  "PS1")
epicsEnvSet("PORT2",  "PS2")
epicsEnvSet("QSIZE",  "20")
epicsEnvSet("XSIZE",  "780")
epicsEnvSet("YSIZE",  "580")
epicsEnvSet("NCHANS", "2048")

# prosilicaConfig(portName,    # The name of the asyn port to be created
#                 cameraId,    # Unique ID, IP address, or IP name of the camera
#                 maxBuffers,  # Maximum number of NDArray buffers driver can allocate. 0=unlimited
#                 maxMemory,   # Maximum memory bytes driver can allocate. 0=unlimited
#                 priority,    # EPICS thread priority for asyn port driver 0=default
#                 stackSize,   # EPICS thread stack size for asyn port driver 0=default
#                 maxPvAPIFrames) # Number of frames to allocate in PvAPI driver. Default=2.
# The simplest way to determine the uniqueId of a camera is to run the Prosilica GigEViewer application, 
# select the camera, and press the "i" icon on the bottom of the main window to show the camera information for this camera. 
# The Unique ID will be displayed on the first line in the information window.

prosilicaConfig( "$(PORT1)", 10.1.253.239, 0, 0, 0, 0, 128)

prosilicaConfig( "$(PORT2)", 10.1.18.34, 0, 0, 0, 0, 128)

asynSetTraceIOMask("$(PORT1)",0,2)

asynSetTraceIOMask("$(PORT2)",0,2)

dbLoadRecords("$(ADCORE)/db/ADBase.template",   "P=$(PREFIX),R=cam29:,PORT=$(PORT1),ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/ADBase.template",   "P=$(PREFIX),R=cam34:,PORT=$(PORT2),ADDR=0,TIMEOUT=1")

# Note that prosilica.template must be loaded after NDFile.template to replace the file format correctly

dbLoadRecords("$(ADPROSILICA)/db/prosilica.template","P=$(PREFIX),R=cam29:,PORT=$(PORT1),ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADPROSILICA)/db/prosilica.template","P=$(PREFIX),R=cam34:,PORT=$(PORT2),ADDR=0,TIMEOUT=1")


# Create a standard arrays plugin, set it to get data from first Prosilica driver.
NDStdArraysConfigure("Image1", 5, 0, "$(PORT1)", 0, 0, 0, 0)
NDStdArraysConfigure("Image2", 5, 0, "$(PORT2)", 0, 0, 0, 0)

dbLoadRecords("$(ADCORE)/ADApp/Db/NDPluginBase.template","P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/ADApp/Db/NDPluginBase.template","P=$(PREFIX),R=image2:,PORT=Image2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")

# Use this line if you only want to use the Prosilica in 8-bit mode.  It uses an 8-bit waveform record
# NELEMENTS is set large enough for a 1360x1024x3 image size, which is the number of pixels in RGB images from the GC1380CH color camera. 
# Must be at least as big as the maximum size of your camera images

# cam29 is a Manta  46C with a resolution of  780 X  580 needing an array of 1,357,200 elements
# cam34 is a Manta 146C with a resolution of 1388 X 1038 needing an array of 4,322,232 elements

dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,TYPE=Int16,FTVL=SHORT,NELEMENTS=1357200")
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image2:,PORT=Image2,ADDR=0,TIMEOUT=1,TYPE=Int16,FTVL=SHORT,NELEMENTS=4322232")

#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,TYPE=Int8,FTVL=UCHAR,NELEMENTS=1357200")
#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image2:,PORT=Image2,ADDR=0,TIMEOUT=1,TYPE=Int8,FTVL=UCHAR,NELEMENTS=4322232")

< commonPlugins.cmd

set_requestfile_path("$(ADPROSILICA)/prosilicaApp/Db")

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30,"P=$(PREFIX)")
