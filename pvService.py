#! /usr/bin/python

import EpicsCA, pg, mmap, select, sys, traceback, os, time

class PVServiceError( Exception):
    value = None

    def __init__( self, value):
        self.value = value
        print >> sys.stderr, sys.exc_info()[0]
        print >> sys.stderr, '-'*60
        traceback.print_exc(file=sys.stderr)
        print >> sys.stderr, '-'*60

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
                print >> sys.stderr, sys.exc_info()[0]
                print >> sys.stderr, '-'*60
                traceback.print_exc(file=sys.stderr)
                print >> sys.stderr, '-'*60
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
    q = None
    pid = None
    pvList = {}
    putOnlyList = {}
    mmfn = "/mnt/pvs/pvService"
    mmf  = None
    mms  = None
    running = False

    def __init__( self):
        self.q = _Q()

        qr = self.q.query( "select epics.iniCAMonitor() as pid")
        self.pid = int(qr.dictresult()[0]["pid"])

        qr = self.q.query( "select index, name from epics.getpvnames( %d) order by index" % (self.pid))
        maxIndex = 0
        for r in qr.dictresult():
            self.pvList[r["name"]] = { "index" : int(r["index"]), "pv" : EpicsCA.PV( r["name"])}
            if int( r["index"]) > maxIndex:
                maxIndex = int( r["index"])

        #self.mmfn = "%s.%d" % (self.mmfn, self.pid)
        tmpF = open( self.mmfn, "w")
        for i in range( maxIndex+1):
            for j in range( 256):
                tmpF.write( "\0")
        tmpF.close()
        os.chmod( self.mmfn, 0666)

        self.mmf = open( self.mmfn, "a+")
        self.mms = mmap.mmap( self.mmf.fileno(), (maxIndex+1)*256)
        
        for p in self.pvList.keys():
            try:
                tmp = str(self.pvList[p]["pv"].get())
            except:
                tmp = str("505.505")
                pass
            if tmp == None or len(tmp) == 0:
                tmp = str("505.505")  # SOS in leet

            i = 256 * self.pvList[p]["index"]
            self.mms.seek( i)
            self.mms.write( tmp)
            self.mms.write( "\0")
            
        self.mms.flush()

        for p in self.pvList.keys():
            self.pvList[p]["pv"].add_callback( callback=self.updateCB)

    def updateCB( self, pv=None, **kw):
        if not self.running:
            return
        indx = self.pvList[pv.pvname]["index"]
        i = 256*indx
        tmp  = str( pv.get())
        if tmp == None or len(tmp) == 0:
            tmp = str("505.505")   # SOS in leet

        qs = "select pg_advisory_lock( 14852, %d) as tal" % (indx)
        self.q.query( qs)
        self.mms.seek( i)
        self.mms.write( tmp)
        self.mms.write( "\0")
        self.mms.flush()
        self.q.query( "select pg_advisory_unlock( 14852, %d)" % (indx))

    def run( self):
        self.running = True
        tryAgain = False
        while True:
            EpicsCA.pend_event( t=0.05)
            if tryAgain or self.q.getnotify() != None:
                qr = self.q.query( "select cappv, capval from epics.caputpop()")
                foundOne = False
                for r in qr.dictresult():
                    foundOne = True
                    nam = r["cappv"]
                    val = r["capval"]
                    print "moving ",nam," to ",val
                    if self.pvList.has_key( nam):
                        self.pvList[nam]["pv"].put( val)
                    else:
                        if not self.putOnlyList.has_key( nam):
                            self.putOnlyList["nam"] = EpicsCA.PV( nam)
                        self.putOnlyList["nam"].put( val)
                    #
                    # pause to keep commands from bumping into one another
                    #
                tryAgain = not foundOne

if __name__ == "__main__":
    z = PvService()
    z.run()

