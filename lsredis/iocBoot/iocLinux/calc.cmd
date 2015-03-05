### Stuff for user programming ###
dbLoadRecords("$(CALC)/calcApp/Db/userCalcs10.db","P=lsredis:")
appendToFile("$(BUILT_SETTINGS)", '$(P)userCalcEnable')

dbLoadRecords("$(CALC)/calcApp/Db/userCalcOuts10.db","P=lsredis:")
appendToFile("$(BUILT_SETTINGS)", '$(P)userCalcOutEnable')

dbLoadRecords("$(CALC)/calcApp/Db/userCalcOuts10more.db","P=lsredis:,N1=11,N2=12,N3=13,N4=14,N5=15,N6=16,N7=17,N8=18,N9=19,N10=20")

dbLoadRecords("$(CALC)/calcApp/Db/userStringCalcs10.db","P=lsredis:")
appendToFile("$(BUILT_SETTINGS)", '$(P)userStringCalcEnable')

dbLoadRecords("$(CALC)/calcApp/Db/userArrayCalcs10.db","P=lsredis:,N=8000")
appendToFile("$(BUILT_SETTINGS)", '$(P)userArrayCalcEnable')

dbLoadRecords("$(CALC)/calcApp/Db/userTransforms10.db","P=lsredis:")
appendToFile("$(BUILT_SETTINGS)", '$(P)userTranEnable')

dbLoadRecords("$(CALC)/calcApp/Db/userAve10.db","P=lsredis:")

# string sequence (sseq) records
dbLoadRecords("$(CALC)/calcApp/Db/userStringSeqs10.db","P=lsredis:")
appendToFile("$(BUILT_SETTINGS)", '$(P)userStringSeqEnable')

# editSseq - edit any sseq or seq record
dbLoadRecords("$(CALC)/calcApp/Db/editSseq.db", "P=lsredis:,Q=ES:")
doAfterIocInit("seq &editSseq, 'P=lsredis:,Q=ES:'")

# interpolation
dbLoadRecords("$(CALC)/calcApp/Db/interp.db", "P=lsredis:,N=2000")
dbLoadRecords("$(CALC)/calcApp/Db/interpNew.db", "P=lsredis:,Q=1,N=2000")

# pvHistory (in-crate archive of up to three PV's)
dbLoadRecords("$(STD)/stdApp/Db/pvHistory.db","P=lsredis:,N=1,MAXSAMPLES=1440")

# busy record
dbLoadRecords("$(BUSY)/busyApp/Db/busyRecord.db", "P=lsredis:,R=mybusy")
dbLoadRecords("$(BUSY)/busyApp/Db/busyRecord.db", "P=lsredis:,R=mybusy2")

# Soft function generator
#dbLoadRecords("$(CALC)/calcApp/Db/FuncGen.db","P=lsredis:,Q=fgen,OUT=lsredis:m7.VAL")
