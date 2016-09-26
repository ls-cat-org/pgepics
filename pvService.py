#! /usr/bin/python3
# 
#  Support for epics process variables
# 
#  Copyright 2008-2016 by Northwestern University
#  Author: Keith Brister
# 
#    This file is part of the LS-CAT Beamline Control Package which is
#    free software: you can redistribute it and/or modify it under the
#    terms of the GNU General Public License as published by the Free
#    Software Foundation, either version 3 of the License, or (at your
#    option) any later version.
# 
#    This software is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
# 
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
# 

import epics
import pg
import mmap
import select
import sys
import traceback
import os
import time
import signal

class PVServiceError( Exception):
    value = None

    def __init__( self, value):
        self.value = value
        print(sys.exc_info()[0], file=sys.stderr)
        print('-'*60, file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        print('-'*60, file=sys.stderr)

    def __str__( self):
        return repr( self.value)

class _Q:
    
    db = None   # our database connection

    def open( self):
        self.db = pg.connect( dbname="ls", host="contrabass.ls-cat.org", user="lsuser" )

    def close( self):
        self.db.close()

    def __init__( self):
        self.open()

    def reset( self):
        self.db.reset()

    def query( self, qs):
        if qs == '':
            return rtn
        if self.db.status == 0:
            self.reset()
        try:
            # ping the server
            qr = self.db.query(qs)
        except:
            if self.db.status == 1:
                print(sys.exc_info()[0], file=sys.stderr)
                print('-'*60, file=sys.stderr)
                traceback.print_exc(file=sys.stderr)
                print('-'*60, file=sys.stderr)
                return None
            # reset the connection, should
            # put in logic here to deal with transactions
            # as transactions are rolled back
            #
            self.db.reset()
            if self.db.status != 1:
                # Bad status even after a reset, bail
                raise PVServiceError( 'Database Connection Lost')

            qr = self.db.query( qs)

        return qr

    def dictresult( self, qr):
        return qr.dictresult()

    def e( self, s):
        return pg.escape_string( s)

    def fileno( self):
        return self.db.fileno()

    def getnotify( self):
        return self.db.getnotify()


class PvService:
    mmfn = "/mnt/pvs/pvService"
    workList = []
    connectionList = []

    def intHandler( self, signum, frame):
        print("Caught signal, exiting", file=sys.stderr)
        self.okToRun = False

    def updateCB( self, pvname=None, value=None, char_value=None, **kw):
        if not self.running:
            return

        tmp  = str(value)
        if tmp == None or len(tmp) == 0:
            tmp = str("505.505")   # SOS in leet

        self.workList.append({"pvname": pvname, "value": tmp})

    def updateConnectionCB( self, pvname=None, conn=None, **kw):
        print( "Connection %s: %s" % (pvname, repr(conn)))
        if conn:
            self.connectionList.append(pvname)


    def __init__( self):
        self.mms  = None
        self.running = False
        self.q = None
        self.pid = None
        self.pvList = {}
        self.putOnlyList = {}
        self.okToRun = True

        print("Initialization...\n", file=sys.stderr)

        self.q = _Q()

        #
        # Simple pid mechanism: only one process is allowed to run.
        # Older processes are told to quit.
        #
        qr = self.q.query( "select epics.iniCAMonitor() as pid")
        self.pid = int(qr.dictresult()[0]["pid"])

        #
        # get names and the index into the shared file
        #
        qr = self.q.query( "select index, name from epics.getpvnames( %d) order by index" % (self.pid))
        maxIndex = 0
        for r in qr.dictresult():
            print(r["index"], r["name"])
            self.pvList[r["name"]] = { "index" : int(r["index"]), "pv" : epics.PV( r["name"], callback=self.updateCB, connection_callback=self.updateConnectionCB)}
            if int( r["index"]) > maxIndex:
                maxIndex = int( r["index"])

        #
        # Initialize the file
        #
        tmpF = open( self.mmfn, "w")
        for i in range( maxIndex+1):
            for j in range( 256):
                tmpF.write( "\0")
        tmpF.close()
        #
        # We have to be sure that postgresql can read this.
        # Probably these permissions are not strict enough
        #
        os.chmod( self.mmfn, 0o666)

        #
        # now we'll open the file with the correct mode
        # and use the mmap module to treat the file like a string
        #
        self.mmf = open( self.mmfn, "a+")
        self.mms = mmap.mmap( self.mmf.fileno(), (maxIndex+1)*256)
        
        #
        # Setup callback to monitor the variables
        #
        #for p in list(self.pvList.keys()):
        #    self.pvList[p]["pv"].add_callback( callback=self.updateCB)

    def run( self):
        print("Running\n", file=sys.stderr)

        # Set up to capture ^C and exit "gracefully"
        #
        signal.signal( signal.SIGINT, self.intHandler)

        self.running = True

        tryAgain = False
        while self.okToRun:
            time.sleep(0.01)

            while len(self.connectionList):
                ci = self.connectionList.pop(0)
                value = str(self.pvList[ci]["pv"].get())
                self.workList.append({"pvname": ci, "value": value})

            while len(self.workList):
                wi = self.workList.pop(0)
                indx = self.pvList[wi["pvname"]]["index"]
                i    = 256 * indx
                qs = "select pg_advisory_lock( 14852, %d) as tal" % (indx)
                self.q.query( qs)
                self.mms.seek( i)
                if wi["value"] == None:
                    print("Bad value",wi, file=sys.stderr);
                else:
                    self.mms.write( str.encode(wi["value"]))

                self.mms.write( b'\x00')
                self.mms.flush()
                self.q.query( "select pg_advisory_unlock( 14852, %d)" % (indx))


            if tryAgain or self.q.getnotify() != None:
                qr = self.q.query( "select cappv, capval from epics.caputpop()")
                foundOne = False
                for r in qr.dictresult():
                    foundOne = True
                    nam = r["cappv"]
                    val = r["capval"]
                    print("moving ",nam," to ",val)
                    if nam in self.pvList:
                        self.pvList[nam]["pv"].put( val)
                    else:
                        if nam not in self.putOnlyList:
                            self.putOnlyList["nam"] = epics.PV( nam)
                        self.putOnlyList["nam"].put( val)
                    #
                    # pause to keep commands from bumping into one another
                    #
                tryAgain = not foundOne
        #
        # Remove callbacks
        #
        for p in list(self.pvList.keys()):
            self.pvList[p]["pv"].clear_callbacks()

if __name__ == "__main__":
    z = PvService()
    z.run()

