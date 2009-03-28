#! /usr/bin/python
import EpicsCA
import pg

class RemoteControl:
    pvd          = None
    voicePv      = None
    shutterPv    = None
    doorPv       = None
    readyAPv     = None
    readyBPv     = None
    detector     = None
    lastVoice    = None
    lastShutter  = None
    lastDoor     = None
    detectorSaved = None

    def __init__( self, pvd, db):
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
        self.detectorSaved = qr.dictresult()[0]["p"]

    def voiceCB( self, pv=None, **kw):
        print "voiceCB  voice: %d,  lastVoice: %d" % (pv.get(), self.lastVoice)

        v = int(pv.get())
        if v == 1 and self.lastVoice == 0:
            qs = "select epics.moveit( '%s', %s)" % (self.detector, self.detectorSaved)
            print "    query: %s" % (qs)
            self.db.query( qs)

        self.lastVoice = int(pv.get())

    def doorCB( self, pv=None, **kw):
        if pv.get() == 0 and self.lastDoor == 1:
            qr = self.db.query( "select epics.position( '%s') as p" % (self.detector))
            self.detectorSaved = qr.dictresult()[0]["p"]

            self.db.query( "select epics.moveit( '%s', 700)" % (self.detector))

        self.lastDoor = pv.get()

    def readyCB( self, pv=None, **kw):
        if self.readyAPv.get() == 1 and self.readyBPv.get() == 1 and (self.lastReadyA == 0 or self.lastReadyB == 0):
            self.shutterPv.put( 1)

        self.lastReadyA = self.readyAPv.get()
        self.lastReadyB = self.readyBPv.get()
        

class Doors:
    db = None
    pvN = [
        { 'station' : '21-ID-D', 'voice' : 'PA:21ID:OA_STA_D_VOICE_1',    'shutter' : 'PC:21ID:Rem_DshtrOpen',
          'door' : 'PA:21ID:IA_STA_D_DR2_CLOS.VAL',    'detector' : '21:D1:DT:Z0', 'readyA' : 'PA:21ID:STA_D_BEAM_READY', 'readyB' : 'PB:21ID:STA_D_BEAM_READY'},

        { 'station' : '21-ID-F', 'voice' : 'PA:21ID:OA_STA_F_VOICE_1',    'shutter' : 'PC:21ID:Rem_FshtrOpen',
          'door' : 'PA:21ID:IA_STA_F_DR2_CLOS.VAL',    'detector' : '21:F1:DT:D', 'readyA' : 'PA:21ID:STA_F_BEAM_READY', 'readyB' : 'PB:21ID:STA_F_BEAM_READY'},

        { 'station' : '21-ID-G', 'voice' : 'PA:21ID:OA_STA_FG_G_VOICE_1', 'shutter' : 'PC:21ID:Rem_GshtrOpen.VAL',
          'door' : 'PA:21ID:IA_STA_FG_G_DR1_CLOS.VAL', 'detector' : '21:G1:DT:D', 'readyA' : 'PA:21ID:STA_G_BEAM_READY', 'readyB' : 'PB:21ID:STA_G_BEAM_READY'}
           ]

    rcL = {}

    def __init__( self):
        self.db       = pg.connect(dbname='ls',user='lsuser', host='contrabass.ls-cat.org')
        for pvs in self.pvN:
            self.rcL[pvs["station"]] = RemoteControl( pvs, self.db)
            
    def run( self):
        while 1:
            EpicsCA.pend_event( t=1.1)


if __name__ == "__main__":
    doors = Doors()
    doors.run()
