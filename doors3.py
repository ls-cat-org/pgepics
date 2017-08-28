#! /usr/bin/python3
#
# Copyright 2016 by Northwestern University
#
import epics
import postgresql
import time

open_fe_shutter = "PC:21ID:FES_OPEN_REQUEST"

class RemoteControl:
    MOVE_TO_DIST = 700.0                # move back to here
    IGNORE_DIST  = 500.0                # don't move unless less than this
    MINIMUM_DS_UNDULATOR_GAP = 21       # Close the shutter if the side station is below this number

    def __init__( self, pvd, db):
        self.armReturn          = False
        self.cover              = None
        self.coverClosed        = None
        self.coverClosedPV      = None
        self.coverOpen          = None
        self.coverOpenPV        = None
        self.coverOpenSaved     = None
        self.coverPV            = None
        self.detector           = None
        self.detectorLLPV       = None
        self.detectorSaved      = None
        self.doorPv             = None
        self.lastDoor           = None
        self.lastShutter        = None
        self.lastUSUndulatorGap = None
        self.lastVoice          = None
        self.pvd                = None
        self.readyAPv           = None
        self.readyBPv           = None
        self.shutterPv          = None
        self.voicePv            = None
        self.waitingForCover    = False

        self.pvd = pvd
        self.db = db
        self.voicePv = epics.PV(pvd["voice"])
        self.lastVoice = self.voicePv.value
        self.voicePv.add_callback( callback=self.voiceCB)

        self.shutterPv = epics.PV( pvd["shutter"])
        self.lastShutter = self.shutterPv.value

        self.doorPv = epics.PV( pvd["door"])
        self.lastDoor = self.doorPv.value
        self.doorPv.add_callback( callback=self.doorCB)

        self.readyAPv = epics.PV( pvd["readyA"])
        self.readyAPv.value
        self.readyAPv.add_callback( callback=self.readyCB)

        self.readyBPv = epics.PV( pvd["readyB"])
        self.readyBPv.value
        self.readyBPv.add_callback( callback=self.readyCB)

        
        if pvd.has_key("detectorLowerLimit"):
            self.detectorLLPV = epics.PV( pvd["detectorLowerLimit"])
            self.detectorLLPV.value
            self.detectorLLPV.add_callback( callback=self.detectorLLCB)

        if pvd.has_key("cover"):
            self.cover = pvd["cover"]
            self.coverPV = epics.PV( self.cover)
            self.coverPV.value

        if pvd.has_key("coverOpen"):
            self.coverOpen   = pvd["coverOpen"]
            self.coverOpenPV = epics.PV( self.coverOpen);
            self.coverOpenedSaved = self.coverOpenPV.value

        if pvd.has_key("coverClosed"):
            self.coverClosed   = pvd["coverClosed"]
            self.coverClosedPV = epics.PV( self.coverClosed)
            self.coverClosedPV.value

        self.detector = pvd["detector"]
        qr = self.db.prepare( "select epics.position( '%s') as p" % (self.detector))
        self.detectorSaved = float(qr.first())

    def detectorLLCB( self, **kw):
        value = kw["value"]
        print("detectorLLCB value: %d  waitingForCover: %s  detectorSaved: %f" % (value, self.waitingForCover, self.detectorSaved))

        if self.waitingForCover and self.detectorSaved < self.IGNORE_DIST:
            qs = "select epics.moveit( '%s', %f)" % (self.detector, self.detectorSaved)
            print "    query: %s" % (qs)
            self.db.prepare( qs).first()
            self.waitingForCover = False

    def voiceCB( self, **kw):
        value = kw["value"]
        print("[%s] voiceCB  voice: %d,  lastVoice: %d" % (time.strftime('%Y-%m-%d %H:%M:%S'), value, self.lastVoice))

        v = int(value)
        if v == 1 and self.lastVoice == 0:
            self.armReturn = True

        self.lastVoice = v)

    def coverOpenedCB( self, **kw):
        value = kw["value"]
        print 'coverOpenedCB', value
        if self.lastDoor == 1:
            self.coverOpenedSave = value


    def doorCB( self, **kw):
        value = kw["value"]
        print('doorCB  %d', (int(value)));

        #
        # maybe move the detector back
        #
        if value == 0 and self.lastDoor == 1:
            qr = self.db.prepare( "select epics.position( '%s') as p" % (self.detector))
            self.detectorSaved = float(qr.first())

            if self.coverOpen != None:
                self.coverOpenSaved = self.coverOpenPV.value
                

            print( 'doorCB', value, 'detectorSaved', self.detectorSaved)
            if self.detectorSaved < self.IGNORE_DIST:
                #
                # Only move the detector back if it is close to that some amount
                #
                self.db.prepare( "select epics.moveit( '%s', %f)" % (self.detector, self.MOVE_TO_DIST)).first()

                print( 'doorCB', value, 'moveit', self.MOVE_TO_DIST)

            if self.coverClosed != None:
                if self.coverClosedPV.value != 0:
                    #
                    # close cover if it is not closed now
                    #
                    self.coverPV.value = 0

        

        #
        # maybe move the detector forward
        #
        if self.armReturn and value == 1 and self.lastDoor == 0:
            if self.cover != None and self.coverOpenSaved != None:
                self.coverPV.value =  1 - self.coverOpenSaved
                self.waitingForCover = True

            if self.detectorSaved < self.IGNORE_DIST and not self.waitingForCover:

                qs = "select epics.moveit( '%s', %f)" % (self.detector, self.detectorSaved)
                print("    query: %s" % (qs))
                self.db.prepare( qs).first()

            self.armReturn = False


        self.lastDoor = value

    def readyCB( self, pv=None, **kw):
        if self.readyAPv.value == 1 and self.readyBPv.value == 1 and (self.lastReadyA == 0 or self.lastReadyB == 0):
            self.shutterPv.value = 1

        self.lastReadyA = self.readyAPv.value
        self.lastReadyB = self.readyBPv.value
        

class Doors:
    db = None
    pvN = [
        { 'station' : '21-ID-D', 'voice' : 'PA:21ID:D_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SDS_OPEN_REQUEST',
          'door' : 'PA:21ID:D_DOOR_2_CLOSED',    'detector' : '21:D1:DT:Z0', 'readyA' : 'PA:21ID:SDS_PERMIT', 'readyB' : 'PB:21ID:SDS_PERMIT',
          'cover' : '21:D1:pmac20:acc65e:1:bo15', 'detectorLowerLimit' : '21:D1:DT:Z0:RqsPos.DRVL', 'coverOpen': '21:D1:pmac20:acc65e:1:bi17', 'coverClosed': '21:D1:pmac20:acc65e:1:bi16'
      },

        { 'station' : '21-ID-E', 'voice' : 'PA:21ID:E_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SES_OPEN_REQUEST',
          'door' : 'PA:21ID:E_DOOR_1_CLOSED',    'detector' : '21:E1:DT:Z0', 'readyA' : 'PA:21ID:SES_PERMIT', 'readyB' : 'PB:21ID:SES_PERMIT'},

        { 'station' : '21-ID-F', 'voice' : 'PA:21ID:F_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SFS_OPEN_REQUEST',
          'door' : 'PA:21ID:F_DOOR_2_CLOSED',    'detector' : '21:F1:DT:D', 'readyA' : 'PA:21ID:SFS_PERMIT', 'readyB' : 'PB:21ID:SFS_PERMIT'},

        { 'station' : '21-ID-G', 'voice' : 'PA:21ID:G_VOICE_SEARCHING', 'shutter' : 'PC:21ID:SGS_OPEN_REQUEST',
          'door' : 'PA:21ID:G_DOOR_1_CLOSED', 'detector' : '21:G1:DT:D', 'readyA' : 'PA:21ID:SGS_PERMIT', 'readyB' : 'PB:21ID:SGS_PERMIT'}
           ]

    rcL = {}

    def __init__( self):
        self.db       = postgresql.open("pq://lsuser@10.1.0.3/ls")
        for pvs in self.pvN:
            print pvs
            self.rcL[pvs["station"]] = RemoteControl( pvs, self.db)
            
    def run( self):
        while 1:
            time.sleep(0.01)


if __name__ == "__main__":
    doors = Doors()
    doors.run()
