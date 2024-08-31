#! /usr/bin/python
import epics
import pg
import time

class RemoteControl:
    MOVE_TO_DIST = 700.0  # move back to here
    IGNORE_DIST  = 500.0  # don't move unless less than this

    def __init__( self, pvd, db):
        self.pvd             = None
        self.voicePv         = None
        self.shutterPv       = None
        self.doorPv          = None
        self.readyAPv        = None
        self.readyBPv        = None
        self.cover           = None
        self.coverPV         = None
        self.coverOpen       = None
        self.coverOpenPV     = None
        self.coverClosed     = None
        self.coverClosedPV   = None
        self.detector        = None
        self.detectorLLPV    = None
        self.lastVoice       = None
        self.lastShutter     = None
        self.lastDoor        = None
        self.coverOpenSaved  = None
        self.detectorSaved   = None
        self.armReturn       = False
        self.waitingForCover = False
        self.lastReadyA      = None
        self.lastReadyB      = None

        self.actionList = []
        self.pvd = pvd
        self.db = db

        self.detector = pvd["detector"]
        print self.detector
        qr = self.db.query( "select epics.position( '%s') as p" % (self.detector))
        self.detectorSaved = float(qr.dictresult()[0]["p"])

        self.voicePv = epics.PV(pvd["voice"])
        print("waiting for %s" % (pvd["voice"]))
        self.voicePv.wait_for_connection(1);
        print("done waiting for %s" % (pvd["voice"]))
        self.lastVoice = self.voicePv.value
        print("voicePv value:", self.lastVoice)
        self.voicePv.add_callback(callback=self.voiceCB)

        self.shutterPv = epics.PV( pvd["shutter"])
        print("waiting for %s" % (pvd["shutter"]))
        self.shutterPv.wait_for_connection(1);
        self.lastShutter = self.shutterPv.value
        print("done waiting.  Got value: ", self.lastShutter)

        self.doorPv = epics.PV( pvd["door"])
        self.lastDoor = self.doorPv.value
        self.doorPv.add_callback(callback=self.doorCB)


        self.readyAPv = epics.PV( pvd["readyA"])
        print("waiting for %s" % (pvd["readyA"]))
        self.readyAPv.wait_for_connection()
        print("Done waiting.  Value:", self.readyAPv.value)
        self.lastReadyA = self.readyAPv.value

        self.readyBPv = epics.PV( pvd["readyB"], callback=self.readyCB)
        print("waiting for %s" % (pvd["readyB"]))
        self.readyBPv.wait_for_connection()
        print("Done waiting.  Value:", self.readyBPv.value)
        self.lastReadyB = self.readyBPv.value

        self.readyAPv.add_callback(callback=self.readyCB)
        self.readyBPv.add_callback(callback=self.readyCB)
        
        if "detectorLowerLimit" in pvd:
            self.detectorLLPV = epics.PV( pvd["detectorLowerLimit"], callback=self.detectorLLCB)
            self.detectorLLPV.value

        if "cover" in pvd:
            self.cover = pvd["cover"]
            self.coverPV = epics.PV( self.cover)
            self.coverPV.value

        if "coverOpen" in pvd:
            self.coverOpen   = pvd["coverOpen"]
            self.coverOpenPV = epics.PV( self.coverOpen);
            self.coverOpenSaved = self.coverOpenPV.value

        if "coverClosed" in pvd:
            self.coverClosed   = pvd["coverClosed"]
            self.coverClosedPV = epics.PV( self.coverClosed)
            self.coverClosedPV.value


    def detectorLLAction(self, value=None, char_value=None):
        qs = "select epics.moveit( '%s', %f)" % (self.detector, self.detectorSaved)
        print("    query: %s" % (qs))
        self.db.query( qs)
        self.waitingForCover = False

    def detectorLLCB( self, pvname=None, value=None, char_value=None, **kw):
        print("detectorLLCB val: %d  waitingForCover: %s  detectorSaved: %f" % (value, self.waitingForCover, self.detectorSaved))

        if self.waitingForCover and self.detectorSaved < self.IGNORE_DIST:
            self.actionList.append((self.detectorLLAction, value, char_value))

    def voiceCB( self, pvname=None, value=None, char_value=None, **kw):
        print("[%s] voiceCB  voice: %d,  lastVoice: %d" % (time.strftime('%Y-%m-%d %H:%M:%S'), value, self.lastVoice))

        v = int(value)
        if v == 1 and self.lastVoice == 0:
            self.armReturn = True

        self.lastVoice = int(value)


    def coverOpenedCB( self, pvname=None, value=None, char_value=None, **kw):
        print('coverOpenedCB', value)
        if self.lastDoor == 1:
            self.coverOpenSaved = value


    def moveDetectorBackAction(self, value=None, char_value=None):
        qr = self.db.query( "select epics.position( '%s') as p" % (self.detector))
        self.detectorSaved = float(qr.dictresult()[0]["p"])

        if self.coverOpen != None:
            self.coverOpenSaved = self.coverOpenPV.value
                
        if self.detectorSaved < self.IGNORE_DIST:
            #
            # Only move the detector back if it is close to that some amount
            #
            self.db.query( "select epics.moveit( '%s', %f)" % (self.detector, self.MOVE_TO_DIST))
            
        if self.coverClosed != None:
            if self.coverClosedPV.value != 0:
                #
                # close cover if it is not closed now
                #
                self.coverPV.value = 0


    def moveDetectorForwardAction(self, value=None, charValue=None):
        if self.cover != None and self.coverOpenSaved != None:
            print('doorCB B')
            self.coverPV.value = ( 1 - self.coverOpenSaved)
            self.waitingForCover = True

        if self.detectorSaved < self.IGNORE_DIST and not self.waitingForCover:
            qs = "select epics.moveit( '%s', %f)" % (self.detector, self.detectorSaved)
            print("    query: %s" % (qs))
            self.db.query( qs)

        self.armReturn = False
        print('doorCB D')

    def doorCB( self, pvname=None, value=None, char_value=None, **kw):
        print('doorCB', value)

        #
        # maybe move the detector back
        #
        if value == 0 and self.lastDoor == 1:
            self.actionList.append((self.moveDetectorBackAction, value, char_value))
        
        #
        # maybe move the detector forward
        #
        if self.armReturn and value == 1 and self.lastDoor == 0:
            self.actionList.append((self.moveDetectorForwardAction, value, char_value))


        self.lastDoor = value


    def readyAction(self, value=None, charValue=None):
        print("opening shutter", self.shutterPv.pvname)
        self.shutterPv.put(1)
        

    def readyCB( self, pvname=None, value=None, char_value=None, **kw):
        print('readyCB', pvname, value, char_value)
        if self.readyAPv.value == 1 and self.readyBPv.value == 1 and (self.lastReadyA == 0 or self.lastReadyB == 0):
            self.actionList.append((self.readyAction, value, char_value))

        self.lastReadyA = self.readyAPv.value
        self.lastReadyB = self.readyBPv.value
        

class Doors:
    db = None
    pvN = [
        { 'station' : '21-ID-D', 'voice' : 'PA:21ID:D_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SDS_OPEN_REQUEST',
          'door' : 'PA:21ID:D_DOOR_2_CLOSED',    'detector' : '21:D1:DT:Z0', 'readyA' : 'PA:21ID:SDS_PERMIT', 'readyB' : 'PB:21ID:SDS_PERMIT',
          'cover' : '21:D1:pmac20:acc65e:1:bo15', 'detectorLowerLimit' : '21:D1:DT:Z0:RqsPos.DRVL', 'coverOpen': '21:D1:pmac20:acc65e:1:bi17', 'coverClosed': '21:D1:pmac20:acc65e:1:bi16'},

        #{ 'station' : '21-ID-E', 'voice' : 'PA:21ID:E_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SES_OPEN_REQUEST',
        #  'door' : 'PA:21ID:E_DOOR_1_CLOSED',    'detector' : '21:E1:DT:Z0', 'readyA' : 'PA:21ID:SES_PERMIT', 'readyB' : 'PB:21ID:SES_PERMIT'},

        { 'station' : '21-ID-F', 'voice' : 'PA:21ID:F_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SFS_OPEN_REQUEST',
          'door' : 'PA:21ID:F_DOOR_2_CLOSED',    'detector' : '21:F1:DT:D', 'readyA' : 'PA:21ID:SFS_PERMIT', 'readyB' : 'PB:21ID:SFS_PERMIT'},

        { 'station' : '21-ID-G', 'voice' : 'PA:21ID:G_VOICE_SEARCHING', 'shutter' : 'PC:21ID:SGS_OPEN_REQUEST',
          'door' : 'PA:21ID:G_DOOR_1_CLOSED', 'detector' : '21:G1:DT:D', 'readyA' : 'PA:21ID:SGS_PERMIT', 'readyB' : 'PB:21ID:SGS_PERMIT'}
           ]

    rcL = {}

    def __init__( self):
        self.db       = pg.connect(dbname='ls',user='lsuser', host='postgres.ls-cat.net')
        for pvs in self.pvN:
            print(pvs)
            self.rcL[pvs["station"]] = RemoteControl( pvs, self.db)
            
    def run( self):
        while True:
            time.sleep(0.01)

            for pvs in self.pvN:
                actionList = self.rcL[pvs["station"]].actionList
                while len(actionList):
                    (action, value, char_value) = actionList.pop(0)
                    action(value, char_value)

if __name__ == "__main__":
    doors = Doors()
    doors.run()
