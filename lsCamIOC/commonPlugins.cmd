# Create a netCDF file saving plugin.
NDFileNetCDFConfigure("FileNetCDF1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=netCDF1:,PORT=FileNetCDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=netCDF1:,PORT=FileNetCDF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileNetCDF.template","P=$(PREFIX),R=netCDF1:,PORT=FileNetCDF1,ADDR=0,TIMEOUT=1")

NDFileNetCDFConfigure("FileNetCDF2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=netCDF2:,PORT=FileNetCDF2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=netCDF2:,PORT=FileNetCDF2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileNetCDF.template","P=$(PREFIX),R=netCDF2:,PORT=FileNetCDF2,ADDR=0,TIMEOUT=1")

# Create a TIFF file saving plugin
NDFileTIFFConfigure("FileTIFF1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=TIFF1:,PORT=FileTIFF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=TIFF1:,PORT=FileTIFF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileTIFF.template",  "P=$(PREFIX),R=TIFF1:,PORT=FileTIFF1,ADDR=0,TIMEOUT=1")

NDFileTIFFConfigure("FileTIFF2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=TIFF2:,PORT=FileTIFF2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=TIFF2:,PORT=FileTIFF2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileTIFF.template",  "P=$(PREFIX),R=TIFF2:,PORT=FileTIFF2,ADDR=0,TIMEOUT=1")

# Create a JPEG file saving plugin
NDFileJPEGConfigure("FileJPEG1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=JPEG1:,PORT=FileJPEG1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=JPEG1:,PORT=FileJPEG1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileJPEG.template",  "P=$(PREFIX),R=JPEG1:,PORT=FileJPEG1,ADDR=0,TIMEOUT=1")

NDFileJPEGConfigure("FileJPEG2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=JPEG2:,PORT=FileJPEG2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=JPEG2:,PORT=FileJPEG2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileJPEG.template",  "P=$(PREFIX),R=JPEG2:,PORT=FileJPEG2,ADDR=0,TIMEOUT=1")

# Create a NeXus file saving plugin
NDFileNexusConfigure("FileNexus1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Nexus1:,PORT=FileNexus1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=Nexus1:,PORT=FileNexus1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileNexus.template", "P=$(PREFIX),R=Nexus1:,PORT=FileNexus1,ADDR=0,TIMEOUT=1")

NDFileNexusConfigure("FileNexus2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Nexus2:,PORT=FileNexus2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=Nexus2:,PORT=FileNexus2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileNexus.template", "P=$(PREFIX),R=Nexus2:,PORT=FileNexus2,ADDR=0,TIMEOUT=1")

# Create an HDF5 file saving plugin
NDFileHDF5Configure("FileHDF1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileHDF5.template",  "P=$(PREFIX),R=HDF1:,PORT=FileHDF1,ADDR=0,TIMEOUT=1")

NDFileHDF5Configure("FileHDF2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=HDF2:,PORT=FileHDF2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=HDF2:,PORT=FileHDF2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileHDF5.template",  "P=$(PREFIX),R=HDF2:,PORT=FileHDF2,ADDR=0,TIMEOUT=1")

# Create a Magick file saving plugin
NDFileMagickConfigure("FileMagick1", $(QSIZE), 0, "$(PORT1)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Magick1:,PORT=FileMagick1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=Magick1:,PORT=FileMagick1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileMagick.template","P=$(PREFIX),R=Magick1:,PORT=FileMagick1,ADDR=0,TIMEOUT=1")

NDFileMagickConfigure("FileMagick2", $(QSIZE), 0, "$(PORT2)", 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Magick2:,PORT=FileMagick2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDFile.template",      "P=$(PREFIX),R=Magick2:,PORT=FileMagick2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDFileMagick.template","P=$(PREFIX),R=Magick2:,PORT=FileMagick2,ADDR=0,TIMEOUT=1")

# Create 4 ROI plugins
NDROIConfigure("ROI1", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI2", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI2:,  PORT=ROI2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI2:,  PORT=ROI2,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI3", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI3:,  PORT=ROI3,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI3:,  PORT=ROI3,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI4", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI4:,  PORT=ROI4,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI4:,  PORT=ROI4,ADDR=0,TIMEOUT=1")

NDROIConfigure("ROI21", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI21:,  PORT=ROI21,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI21:,  PORT=ROI21,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI22", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI22:,  PORT=ROI22,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI22:,  PORT=ROI22,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI23", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI23:,  PORT=ROI23,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI23:,  PORT=ROI23,ADDR=0,TIMEOUT=1")
NDROIConfigure("ROI24", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=ROI24:,  PORT=ROI24,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDROI.template",       "P=$(PREFIX),R=ROI24:,  PORT=ROI24,ADDR=0,TIMEOUT=1")

# Create a processing plugin
NDProcessConfigure("PROC1", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDProcess.template",   "P=$(PREFIX),R=Proc1:,  PORT=PROC1,ADDR=0,TIMEOUT=1")

NDProcessConfigure("PROC2", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Proc2:,  PORT=PROC2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDProcess.template",   "P=$(PREFIX),R=Proc2:,  PORT=PROC2,ADDR=0,TIMEOUT=1")

# Create 5 statistics plugins
NDStatsConfigure("STATS1", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats1:,  PORT=STATS1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI1,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats1:,  PORT=STATS1,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS2", $(QSIZE), 0, "ROI1", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats2:,  PORT=STATS2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI2,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats2:,  PORT=STATS2,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS3", $(QSIZE), 0, "ROI2", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats3:,  PORT=STATS3,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI3,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats3:,  PORT=STATS3,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS4", $(QSIZE), 0, "ROI3", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats4:,  PORT=STATS4,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI4,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats4:,  PORT=STATS4,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS5", $(QSIZE), 0, "ROI4", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats5:,  PORT=STATS5,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats5:,  PORT=STATS5,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")

NDStatsConfigure("STATS21", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats21:,  PORT=STATS21,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI1,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats21:,  PORT=STATS21,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS22", $(QSIZE), 0, "ROI1", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats22:,  PORT=STATS22,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI2,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats22:,  PORT=STATS22,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS23", $(QSIZE), 0, "ROI2", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats23:,  PORT=STATS23,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI3,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats23:,  PORT=STATS23,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS24", $(QSIZE), 0, "ROI3", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats24:,  PORT=STATS24,ADDR=0,TIMEOUT=1,NDARRAY_PORT=ROI4,NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats24:,  PORT=STATS24,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")
NDStatsConfigure("STATS25", $(QSIZE), 0, "ROI4", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Stats25:,  PORT=STATS25,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDStats.template",     "P=$(PREFIX),R=Stats25:,  PORT=STATS25,ADDR=0,TIMEOUT=1,HIST_SIZE=256,XSIZE=$(XSIZE),YSIZE=$(YSIZE),NCHANS=$(NCHANS)")

# Create a transform plugin
NDTransformConfigure("TRANS1", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Trans1:,  PORT=TRANS1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDTransform.template", "P=$(PREFIX),R=Trans1:,  PORT=TRANS1,ADDR=0,TIMEOUT=1")

NDTransformConfigure("TRANS2", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Trans2:,  PORT=TRANS2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDTransform.template", "P=$(PREFIX),R=Trans2:,  PORT=TRANS2,ADDR=0,TIMEOUT=1")

# Create an overlay plugin with 8 overlays
NDOverlayConfigure("OVER1", $(QSIZE), 0, "$(PORT1)", 0, 8, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Over1:, PORT=OVER1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDOverlay.template",   "P=$(PREFIX),R=Over1:, PORT=OVER1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:1:,NAME=ROI1,   SHAPE=1,O=Over1:,XPOS=$(PREFIX)ROI1:MinX_RBV,YPOS=$(PREFIX)ROI1:MinY_RBV,XSIZE=$(PREFIX)ROI1:SizeX_RBV,YSIZE=$(PREFIX)ROI1:SizeY_RBV,PORT=OVER1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:2:,NAME=ROI2,   SHAPE=1,O=Over1:,XPOS=$(PREFIX)ROI2:MinX_RBV,YPOS=$(PREFIX)ROI2:MinY_RBV,XSIZE=$(PREFIX)ROI2:SizeX_RBV,YSIZE=$(PREFIX)ROI2:SizeY_RBV,PORT=OVER1,ADDR=1,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:3:,NAME=ROI3,   SHAPE=1,O=Over1:,XPOS=$(PREFIX)ROI3:MinX_RBV,YPOS=$(PREFIX)ROI3:MinY_RBV,XSIZE=$(PREFIX)ROI3:SizeX_RBV,YSIZE=$(PREFIX)ROI3:SizeY_RBV,PORT=OVER1,ADDR=2,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:4:,NAME=ROI4,   SHAPE=1,O=Over1:,XPOS=$(PREFIX)ROI4:MinX_RBV,YPOS=$(PREFIX)ROI4:MinY_RBV,XSIZE=$(PREFIX)ROI4:SizeX_RBV,YSIZE=$(PREFIX)ROI4:SizeY_RBV,PORT=OVER1,ADDR=3,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:5:,NAME=Cursor1,SHAPE=1,O=Over1:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER1,ADDR=4,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:6:,NAME=Cursor2,SHAPE=1,O=Over1:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER1,ADDR=5,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:7:,NAME=Box1,   SHAPE=1,O=Over1:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER1,ADDR=6,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over1:8:,NAME=Box2,   SHAPE=1,O=Over1:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER1,ADDR=7,TIMEOUT=1")

NDOverlayConfigure("OVER2", $(QSIZE), 0, "$(PORT2)", 0, 8, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template","P=$(PREFIX),R=Over2:, PORT=OVER2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDOverlay.template",   "P=$(PREFIX),R=Over2:, PORT=OVER2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:1:,NAME=ROI21,   SHAPE=1,O=Over2:,XPOS=$(PREFIX)ROI21:MinX_RBV,YPOS=$(PREFIX)ROI21:MinY_RBV,XSIZE=$(PREFIX)ROI21:SizeX_RBV,YSIZE=$(PREFIX)ROI21:SizeY_RBV,PORT=OVER2,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:2:,NAME=ROI22,   SHAPE=1,O=Over2:,XPOS=$(PREFIX)ROI22:MinX_RBV,YPOS=$(PREFIX)ROI22:MinY_RBV,XSIZE=$(PREFIX)ROI22:SizeX_RBV,YSIZE=$(PREFIX)ROI22:SizeY_RBV,PORT=OVER2,ADDR=1,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:3:,NAME=ROI23,   SHAPE=1,O=Over2:,XPOS=$(PREFIX)ROI23:MinX_RBV,YPOS=$(PREFIX)ROI23:MinY_RBV,XSIZE=$(PREFIX)ROI23:SizeX_RBV,YSIZE=$(PREFIX)ROI23:SizeY_RBV,PORT=OVER2,ADDR=2,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:4:,NAME=ROI24,   SHAPE=1,O=Over2:,XPOS=$(PREFIX)ROI24:MinX_RBV,YPOS=$(PREFIX)ROI24:MinY_RBV,XSIZE=$(PREFIX)ROI24:SizeX_RBV,YSIZE=$(PREFIX)ROI24:SizeY_RBV,PORT=OVER2,ADDR=3,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:5:,NAME=Cursor21,SHAPE=1,O=Over2:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER2,ADDR=4,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:6:,NAME=Cursor22,SHAPE=1,O=Over2:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER2,ADDR=5,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:7:,NAME=Box21,   SHAPE=1,O=Over2:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER2,ADDR=6,TIMEOUT=1")
dbLoadRecords("$(ADCORE)/db/NDOverlayN.template","P=$(PREFIX),R=Over2:8:,NAME=Box22,   SHAPE=1,O=Over2:,XPOS=junk,                  YPOS=junk,                  XSIZE=junk,                   YSIZE=junk,                   PORT=OVER2,ADDR=7,TIMEOUT=1")

# Create 2 color conversion plugins
NDColorConvertConfigure("CC1", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template",   "P=$(PREFIX),R=CC1:,  PORT=CC1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDColorConvert.template", "P=$(PREFIX),R=CC1:,  PORT=CC1,ADDR=0,TIMEOUT=1")
NDColorConvertConfigure("CC2", $(QSIZE), 0, "$(PORT1)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template",   "P=$(PREFIX),R=CC2:,  PORT=CC2,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT1),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDColorConvert.template", "P=$(PREFIX),R=CC2:,  PORT=CC2,ADDR=0,TIMEOUT=1")

NDColorConvertConfigure("CC21", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template",   "P=$(PREFIX),R=CC21:,  PORT=CC21,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDColorConvert.template", "P=$(PREFIX),R=CC21:,  PORT=CC21,ADDR=0,TIMEOUT=1")
NDColorConvertConfigure("CC22", $(QSIZE), 0, "$(PORT2)", 0, 0, 0)
dbLoadRecords("$(ADCORE)/db/NDPluginBase.template",   "P=$(PREFIX),R=CC22:,  PORT=CC22,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT2),NDARRAY_ADDR=0")
dbLoadRecords("$(ADCORE)/db/NDColorConvert.template", "P=$(PREFIX),R=CC22:,  PORT=CC22,ADDR=0,TIMEOUT=1")

# Load scan records
dbLoadRecords("$(SSCAN)/sscanApp/Db/scan.db", "P=$(PREFIX),MAXPTS1=2000,MAXPTS2=200,MAXPTS3=20,MAXPTS4=10,MAXPTSH=10")

# Load sseq record for acquisition sequence
dbLoadRecords("$(CALC)/calcApp/Db/yySseq.db", "P=$(PREFIX), S=AcquireSequence")

# Calc record reset frame count
dbLoadRecords("$(CALC)/calcApp/Db/userCalcOuts10.db", "P=$(PREFIX)")


set_requestfile_path("./")
set_requestfile_path("$(ADCORE)/ADApp/Db")
set_requestfile_path("$(CALC)/calcApp/Db")
set_requestfile_path("$(SSCAN)/sscanApp/Db")
set_savefile_path("./autosave")
set_pass0_restoreFile("auto_settings.sav")
set_pass1_restoreFile("auto_settings.sav")
save_restoreSet_status_prefix("$(PREFIX)")
dbLoadRecords("$(AUTOSAVE)/asApp/Db/save_restoreStatus.db", "P=$(PREFIX)")
