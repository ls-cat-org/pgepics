import redis

keyBase = 'stns.2'

r = redis.Redis()
keys = r.keys( '%s.*' % (keyBase))

nams = [
    'ZNAM',
    'ONAM'
    ]


print """
record(stringout,"$(P)") {
  field( DTYP,         "Redis Connector")
  field( DESC,         "Manage redis supplied PVs")
  field( SCAN,         "I/O Intr")
  field( OMSL,         "supervisory")
  info(  redisKeyBase,  "$(S)")
  info(  epicsPVBase,   "$(P)")
  info(  redisReadHost, "$(RRH)")
  info(  redisReadPort, "$(RRP)")
  info(  redisReadDb,   "$(RRDB)")
  info(  redisWriteHost,"$(RWH)")
  info(  redisWritePort,"$(RWP)")
  info(  redisWriteDb,  "$(RWDB)")
  info(  redisSubHost,  "$(RRH)")
  info(  redisSubPort,  "$(RRP)")
  info(  redisSubDb,    "$(RRDB)")
  info(  pgHost,        "$(pgHost)")
  info(  pgPort,        "$(pgPort)")
  info(  pgUser,        "$(pgUser)")
  info(  pgDb,          "$(pgDb)")
}
"""

keys.sort()

for k in keys:
    obj = r.hgetall( k)
    kk = k[ len( keyBase)+1:]

    if obj.has_key( 'OUT_RECORD') and obj.has_key( 'OUT_DTYP'):
        if obj.has_key( 'OUT_PV'):
            pv = obj['OUT_PV']
        else:
            pv = kk.replace( '.', ':')
        print 'record( %s, "$(P):%s") {' % ( obj["OUT_RECORD"], pv)
        print '  field( DTYP,           "%s")' % (obj["OUT_DTYP"])
        print '  field( DESC,           "Reids Bridge")'
        print '  field( SCAN,           "%s")' % (obj["OUT_SCAN"])
        for n in nams:
            if obj.has_key(n):
                print '  field( %s,           "%s")' % (n, obj[n])
        if obj.has_key( 'OUT'):
            print  '  field( OUT,            "%s")' % (obj["OUT"])
        if obj.has_key( 'PREC'):
            print '  field( PREC,            "%s")' % (obj["PREC"])
        if obj["OUT_RECORD"] == 'ao':
            print '  field( LINR,           "NO CONVERSION")'
        print '  info(  redisKey,       "$(S).%s")'  % (kk)
        print '  info(  redisConnector, "$(P)")'
        print '}'

    if obj.has_key( 'IN_RECORD') and obj.has_key( 'IN_DTYP'):
        if obj.has_key( 'IN_PV'):
            pv = obj['IN_PV']
        else:
            pv = kk.replace( '.', ':')
        print 'record( %s, "$(P):%s") {' % ( obj["IN_RECORD"], pv)
        print '  field( DTYP,           "%s")' % (obj["IN_DTYP"])
        print '  field( DESC,           "Reids Bridge")'
        print '  field( SCAN,           "%s")' % (obj["IN_SCAN"])
        for n in nams:
            if obj.has_key(n):
                print '  field( %s,           "%s")' % (n, obj[n])
        if obj.has_key( 'INP'):
            print  '  field( INP,            "%s")' % (obj["INP"])
        if obj.has_key( 'PREC'):
            print '  field( PREC,            "%s")' % (obj["PREC"])
        if obj["IN_RECORD"] == 'ai':
            print '  field( LINR,           "NO CONVERSION")'
        print '  info(  redisKey,       "$(S).%s")'  % (kk)
        print '  info(  redisConnector, "$(P)")'
        if obj.has_key( "SETTER"):
            print '  info(  setter,         "%s")' % (obj["SETTER"])
        print '}'

