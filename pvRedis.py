#! /usr/bin/python
#
import EpicsCA
import redis
import sys

print "1"

configList = [
    {
        "stn": 1,
        "rconnect": {
            "host": "vidalia.ls-cat.org",
            "port": 6379,
            "db":   0
            },
        "rpvs": [
            { "epics": "21:D1:DT:Z0:ActPos", "redis": "stns.1.detector.dist", "prec": 3}
            ]
        }
    ]

print "2"

class PvRedis:
    def updateCB( self, pv=None, **kw):
        k = self.pvList[pv.pvname]["rkey"]
        err = False
        try:
            ftmp = float( pv.get())
            tmp  = "%.*f" % (self.pvList[pv.pvname]["prec"], ftmp)

        except:
            err = True
            tmp = None

        if err or tmp == None or len(tmp) == 0:
            print >> sys.stderr, "missing value for %s on update", pv.pvname
            tmp = "Epics PV Not Connected"


        print "here i am", k, err, tmp
        self.r.hset( k, "VALUE", tmp)
        self.r.publish( "REDIS_PV_CONNECTOR", k)

    def __init__( self, rconnect, rpvs):
        self.r = redis.Redis( host=rconnect["host"], port=rconnect["port"], db=rconnect["db"])

        self.pvList = {}
        for rpv in rpvs:
            self.pvList[rpv["epics"]] = { "rkey": rpv["redis"], "pv": EpicsCA.PV( rpv["epics"]), "prec": rpv["prec"]}
            

        #
        # Initialize PV's
        #
        for p in self.pvList.keys():
            try:
                ftmp = float(self.pvList[p]["pv"].get())
                tmp  = "%.*f" % (self.pvList[p]["prec"], ftmp)
            except:
                print >> sys.stderr, "Error getting %s" % (p)
                tmp = "Epics PV Not Connected"

            if tmp == None or len(tmp) == 0:
                print >> sys.stderr, "missing value for %s" % (p)
                tmp = "Epics PV Not Connected"
                
            print p, tmp

            self.r.hset( self.pvList[p]["rkey"], "VALUE", tmp)
            self.r.publish( "REDIS_PV_CONNECTOR", self.pvList[p]["rkey"])

        for p in self.pvList.keys():
            self.pvList[p]["pv"].add_callback( callback=self.updateCB)
            print p, self.pvList[p]


    def run( self):
        print >> sys.stderr, "Running\n"

        while True:
            EpicsCA.pend_event( t=0.05)

print "3"

if __name__ == "__main__":
    print "4"
    z = PvRedis( configList[0]["rconnect"], configList[0]["rpvs"])
    z.run()


