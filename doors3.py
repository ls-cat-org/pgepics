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
        #
        # Our state.  Initial it's unknown so we won't be doing much
        # of anything at first.
        #
        # Note about the cover switches:
        #
        #  There are two switches, one indicates that the cover is
        #  open and the other indicates that the cover is closed.
        #  There is real and hopefully temporary state where the cover
        #  is neither opened nor closed make a three state system.
        #
        self.armReturn          = False         # When true allow moving back to data collection position
        self.coverClosedPV      = None          # PV opject to read position of the cover Closed switch (0=Closed, 1=Not Closed)
        self.coverOpenedPV      = None          # PV object to read the position of the cover Opened swtich (0 = Opened, 1 = Not Opened)
        self.coverOpenedSaved   = None          # Previous version of the cover Opened switch
        self.coverPV            = None          # PV object for controlling the covdf
        self.detectorName       = None          # Name of PV for the detector position
        self.detectorSaved      = None          # Detector position we'd like to restore
        self.doorPV             = None          # PV object for the door
        self.lastDoor           = None          # Previous value of the door state
        self.lastShutter        = None          # Previous value of the station shutter state
        self.lastUSUndulatorGap = None          # Previous value of the upstream undulator gap
        self.lastVoice          = None          # Previous value of the search voice state
        self.pvd                = None          # Dictionary of pv names (from init)
        self.readyAPV           = None          # PV object for the chain A station shutter permit
        self.readyBPV           = None          # PV object for the chain B station shutter permit
        self.shutterPV          = None          # PV object for the station shutter state
        self.voicePV            = None          # PV object for the search voice state
        self.waitingForCover    = False         # True when we are waiting for the cover to open before moving the detector back

        self.pvd = pvd
        self.db = db

        #
        # Initialize PV objects
        #
        self.voicePV = epics.PV(pvd["voice"])
        self.lastVoice = self.voicePV.value
        self.voicePV.add_callback( callback=self.voiceCB)

        self.shutterPV = epics.PV( pvd["shutter"])
        self.lastShutter = self.shutterPV.value

        self.doorPV = epics.PV( pvd["door"])
        self.lastDoor = self.doorPV.value
        self.doorPV.add_callback( callback=self.doorCB)

        self.readyAPV = epics.PV( pvd["readyA"])
        self.readyAPV.value
        self.readyAPV.add_callback( callback=self.readyCB)

        self.readyBPV = epics.PV( pvd["readyB"])
        self.readyBPV.value
        self.readyBPV.add_callback( callback=self.readyCB)

        #
        # Currently only the Eiger detectors have covers
        #
        if pvd.has_key("cover"):
            self.coverPV = epics.PV(pvd["cover"])
            self.coverPV.value

        if pvd.has_key("coverOpened"):
            self.coverOpenedPV    = epics.PV( pvd["coverOpened"]);
            self.coverOpenedSaved = self.coverOpenedPV.value
            self.coverOpenedPV.add_callback(callback=self.coverOpenedCB);

        if pvd.has_key("coverClosed"):
            self.coverClosedPV = epics.PV( pvd["coverClosed"])
            self.coverClosedPV.value

        #
        # Anyone know why we don't just ask the PV where the detector
        # is?  Because we only use the name of the detctor here since
        # moving motors that are left in the killed state is more
        # complicated that just passing a requested value.  The
        # database already has routines to do this so we just make
        # database calls.
        #
        self.detectorName = pvd["detector"]
        qr = self.db.prepare( "select epics.position( '%s') as p" % (self.detectorName))
        self.detectorSaved = float(qr.first())

    #
    # Called when the cover open switch changes state.  Used to move
    # the detector into data collection position after the cover has
    # opened.
    #
    def coverOpenedCB( self, **kw):
        value = int(kw["value"])
        if value == 1:
            strv = "Not Opened"
        else:
            strv = "Opened"

        print("[%s %s] lidOpenCB value=%d %s" % (self.station, time.strftime('%Y-%m-%d %H:%M:%S'), value, strv))

        if value == 0 and self.waitingForCover:
            qs = "select epics.moveit( '%s', %f)" % (self.detectorName, self.detectorSaved)
            print "    query: %s" % (qs)
            self.db.prepare( qs).first()

        #
        # Regardless of what happens above we are not waiting for the cover now.
        #
        # When the cover just changed from opened to not opened we
        # clearly are doing something that does not envovle us moving
        # the detector.  When the cover just changed from not opened
        # to opened while waiting for the detector then we just moved
        # the detector a couple of lines ago and now we're done with
        # it.  In either case we must leave this routine with
        # waitingForCover = False.
        self.waitingForCover = False


    #
    # Called when the station is being searched (and when it's no
    # longer being searched)
    #
    def voiceCB( self, **kw):
        value = kw["value"]
        print("[%s %s] voiceCB  voice: %d,  lastVoice: %d" % (self.station, time.strftime('%Y-%m-%d %H:%M:%S'), value, self.lastVoice))

        v = int(value)
        if v == 1 and self.lastVoice == 0:
            #
            # Here the station is being searched and we would like the
            # detector to move back into data collection position
            #
            self.armReturn = True

        self.lastVoice = v

    #
    # The door has just become closed or is no longer closed
    #
    # Value 0 = door is not closed
    # Value 1 = door is closed
    #
    def doorCB( self, **kw):
        value = int(kw["value"])
        print('[%s %s] doorCB  %d', (self.station, time.strftime('%Y-%m-%d %H:%M:%S'), value));

        #
        # maybe move the detector back
        #
        # Door is newly not closed
        #
        if value == 0 and self.lastDoor == 1:
            #
            # Save our current detector position so we might return to it later
            #
            qr = self.db.prepare( "select epics.position( '%s') as p" % (self.detectorName))
            self.detectorSaved = float(qr.first())

            #
            # Save cover state
            #
            if self.coverOpenedPV != None:
                self.coverOpenedSaved = self.coverOpenedPV.value


            print( '[%s %s] doorCB: %d  detectorSaved: %f' % (self.station, time.strftime('%Y-%m-%d %H:%M:%S'),value, self.detectorSaved))

            #
            # Don't move the detector back if it's fine where it is
            #
            if self.detectorSaved < self.IGNORE_DIST:
                self.db.prepare( "select epics.moveit( '%s', %f)" % (self.detectorName, self.MOVE_TO_DIST)).first()

                print('   doorCB moveit %f' % (self.MOVE_TO_DIST))

            #
            # Close the cover if we have one and it's not closed already
            #
            if self.coverClosedPV != None:
                if self.coverClosedPV.value != 0:
                    #
                    # close cover if it is not closed now
                    #
                    self.coverPV.value = 0

        if self.armReturn and value == 1 and self.lastDoor == 0:
            self.armReturn = False

            #
            # Move the detector back to data collection position when
            # the door is newly closed, the saved position is less
            # than the "door open" position, AND this program was
            # running when the station was last searched
            #
            if self.coverPV != None and self.coverOpenedSaved != None:
                #
                # The logic for the coverPV and the coverOpenedPV is
                # reversed so we restore the cover to value 0 when
                # coverOpenedSaved is 1 and vice versa.
                #
                self.coverPV.value =  1 - self.coverOpenedSaved
                #
                # Only wait for the cover if we are changing its
                # state.
                #
                if self.coverOpenedPV.value != self.coverOpenedSaved and self.detectorSaved < self.IGNORE_DIST:
                    self.waitingForCover = True

            if self.detectorSaved < self.IGNORE_DIST and not self.waitingForCover:

                qs = "select epics.moveit( '%s', %f)" % (self.detectorName, self.detectorSaved)
                print("    query: %s" % (qs))
                self.db.prepare( qs).first()

        self.lastDoor = value

    #
    # Track the station readiness.  Trigger the shutter open only when both chains are ready.
    #
    def readyCB( self, pv=None, **kw):

        print( "[%s %s]  Chain A Ready: %d  Chain B Reading: %d" % (self.station,time.strftime('%Y-%m-%d %H:%M:%S'), self.readyPV.value, self.readyPV.value))

        if self.readyAPV.value == 1 and self.readyBPV.value == 1 and (self.lastReadyA == 0 or self.lastReadyB == 0):
            self.shutterPV.value = 1
            print( "   Requesting shutter to open")

        self.lastReadyA = self.readyAPV.value
        self.lastReadyB = self.readyBPV.value


class Doors:
    db = None
    pvN = [
        { 'station' : '21-ID-D', 'voice' : 'PA:21ID:D_VOICE_SEARCHING',    'shutter' : 'PC:21ID:SDS_OPEN_REQUEST',
          'door' : 'PA:21ID:D_DOOR_2_CLOSED',    'detector' : '21:D1:DT:Z0', 'readyA' : 'PA:21ID:SDS_PERMIT', 'readyB' : 'PB:21ID:SDS_PERMIT',
          'cover' : '21:D1:pmac20:acc65e:1:bo15', 'detectorLowerLimit' : '21:D1:DT:Z0:RqsPos.DRVL', 'coverOpened': '21:D1:pmac20:acc65e:1:bi17',
          'coverClosed': '21:D1:pmac20:acc65e:1:bi16'},

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
