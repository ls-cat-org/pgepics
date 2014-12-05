#! /usr/bin/python
import EpicsCA
import pg
import time

class RemoteControl:
    MOVE_TO_DIST = 700.0  # move back to here
    IGNORE_DIST  = 500.0  # don't move unless less than this

    def __init__( self, pvd, db):
        self.pvd           = None
        self.voicePv       = None
        self.shutterPv     = None
        self.doorPv        = None
        self.readyAPv      = None
        self.readyBPv      = None
        self.detector      = None
        self.lastVoice     = None
        self.lastShutter   = None
        self.lastDoor      = None
        self.detectorSaved = None
        self.armReturn     = False

        self.pvd = pvd
        self.db = db
        self.voicePv = EpicsCA.PV(pvd["voice"])
        self.lastVoice = self.voicePv.get()
        self.voicePv.add_callback( callback=self.voiceCB)

        self.shutterPv = EpicsCA.PV( pvd["shutter"])
        self.lastShutter = self.shutterPv.get()

        self.doorPv = EpicsCA.PV( pvd["door"])
        self.lastDoor = self.doorPv.get()
        self.doorPv.add_callback( callback=self.doorCB)

        self.readyAPv = EpicsCA.PV( pvd["readyA"])
        self.readyAPv.get()
        self.readyAPv.add_callback( callback=self.readyCB)

        self.readyBPv = EpicsCA.PV( pvd["readyB"])
        self.readyBPv.get()
        self.readyBPv.add_callback( callback=self.readyCB)

        self.detector = pvd["detector"]

        qr = self.db.query( "select epics.position( '%s') as p" % (self.detector))
        self.detectorSaved = float(qr.dictresult()[0]["p"])

    def voiceCB( self, pv=None, **kw):
        print "[%s] voiceCB  voice: %d,  lastVoice: %d" % (time.strftime('%Y-%m-%d %H:%M:%S'), pv.get(), self.lastVoice)

        v = int(pv.get())
        if v == 1 and self.lastVoice == 0:
            self.armReturn = True

        self.lastVoice = int(pv.get())

    def doorCB( self, pv=None, **kw):
        if pv.get() == 0 and self.lastDoor == 1:
            qr = self.db.query( "select epics.position( '%s') as p" % (self.detector))
            self.detectorSaved = float(qr.dictresult()[0]["p"])

            if self.detectorSaved < self.IGNORE_DIST:
                #
                # Only move the detector back if it is close that some amount
                #
                self.db.query( "select epics.moveit( '%s', %f)" % (self.detector, self.MOVE_TO_DIST))

        if self.armReturn and pv.get() == 1 and self.lastDoor == 0:
            if self.detectorSaved < self.IGNORE_DIST:
                qs = "select epics.moveit( '%s', %f)" % (self.detector, self.detectorSaved)
                print "    query: %s" % (qs)
                self.db.query( qs)

            self.armReturn = False


        self.lastDoor = pv.get()

    def readyCB( self, pv=None, **kw):
        if self.readyAPv.get() == 1 and self.readyBPv.get() == 1 and (self.lastReadyA == 0 or self.lastReadyB == 0):
            self.shutterPv.put( 1)

        self.lastReadyA = self.readyAPv.get()
        self.lastReadyB = self.readyBPv.get()
        

class Doors:
    db = None
    pvN = [
        { 'station' : '21-ID-D', 'voice' : 'PA:21ID:D_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SDS_OPEN_REQUEST',
          'door' : 'PA:21ID:D_DOOR_2_CLOSED',    'detector' : '21:D1:DT:Z0', 'readyA' : 'PA:21ID:SDS_PERMIT', 'readyB' : 'PB:21ID:SDS_PERMIT'},

        { 'station' : '21-ID-E', 'voice' : 'PA:21ID:E_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SES_OPEN_REQUEST',
          'door' : 'PA:21ID:E_DOOR_1_CLOSED',    'detector' : '21:E1:DT:Z0', 'readyA' : 'PA:21ID:SES_PERMIT', 'readyB' : 'PB:21ID:SES_PERMIT'},

        { 'station' : '21-ID-F', 'voice' : 'PA:21ID:F_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SFS_OPEN_REQUEST',
          'door' : 'PA:21ID:F_DOOR_2_CLOSED',    'detector' : '21:F1:DT:D', 'readyA' : 'PA:21ID:SFS_PERMIT', 'readyB' : 'PB:21ID:SFS_PERMIT'},

        { 'station' : '21-ID-G', 'voice' : 'PA:21ID:G_VOICE_SEARCHING', 'shutter' : 'PC:21ID:SGS_OPEN_REQUEST',
          'door' : 'PA:21ID:G_DOOR_1_CLOSED', 'detector' : '21:G1:DT:D', 'readyA' : 'PA:21ID:SGS_PERMIT', 'readyB' : 'PB:21ID:SGS_PERMIT'}
           ]

    rcL = {}

    def __init__( self):
        self.db       = pg.connect(dbname='ls',user='lsuser', host='contrabass.ls-cat.org')
        for pvs in self.pvN:
            print pvs
            self.rcL[pvs["station"]] = RemoteControl( pvs, self.db)
            
    def run( self):
        while 1:
            EpicsCA.pend_event( t=1.1)


if __name__ == "__main__":
    doors = Doors()
    doors.run()
