#!SQL!# CREATE OR REPLACE FUNCTION epics._caget(pvi int) returns text as $$

if not "mmap" in GD:
    plpy.execute("select pg_advisory_lock(14850)")
    import mmap
    GD["mmap"] = mmap
    plpy.execute("select pg_advisory_unlock(14850)")
mmap = GD["mmap"]

if not "mm" in GD:
    plpy.execute("select pg_advisory_lock(14850)")
    mm = {}
    GD["mm"] = mm
    plpy.execute("select pg_advisory_unlock(14850)")
mm = GD["mm"]

if not "fd" in mm:
    plpy.execute("select pg_advisory_lock(14850)")
    try:
        f = open("/mnt/pvs/pvService", "r+", encoding="utf-8")
        s  = mmap.mmap( f.fileno(), 0)
    except:
        plpy.warning("something bad happend while trying to create mm")
        plpy.execute("select pg_advisory_unlock(14850)")
        return None

    mm["fd"] = f.fileno()
    mm["s"]  = s
    plpy.execute("select pg_advisory_unlock(14850)")

try:
    plpy.execute("select pg_advisory_lock(14852, %d)" % (pvi))
    mm["s"].seek(256*pvi)
    readFlag = True
    val = ""
    while readFlag:
        tmp = mm["s"].read_byte()
        if tmp != 0:
            val += chr(tmp)
        else:
            readFlag = False
except:
    plpy.warning("something bad happend while trying to read the pv value")
    del GD["mm"]
    plpy.execute("select pg_advisory_unlock(14852, %d)" % (pvi))
    return None

plpy.execute("select pg_advisory_unlock(14852, %d)" % (pvi))
if val==None or val=="None":
    val=str("505.911")

return val

#!SQL!# $$ LANGUAGE plpython3u SECURITY DEFINER;
#!SQL!# ALTER FUNCTION epics._caget( int) OWNER TO administrators;
