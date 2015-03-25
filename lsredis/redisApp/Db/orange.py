import redis
configList = {
    "stns.2" : [
        { "inp": "S:SRcurrentAI",                "key": "current",                     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:E1:DT:Z0:ActPos",           "key": "detector.dist",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:E1:TB:H:ActPos",            "key": "table.y.position",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 3},
        { "inp": "21:E1:TB:L:ActPos",            "key": "table.x.position",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 3},
        { "inp": "21:ECAMS:Edge1:VerticalFound", "key": "capDetected",                 "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "EPS:21:ID:BLEPS:SPER",         "key": "pss.bleps_fe_shutter_permit", "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "EPS:21:ID:BLEPS:VACPER",       "key": "pss.bleps_fe_vacuum_permit",  "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PC:21ID:SES_BLEPS_PERMIT",     "key": "pss.bleps_station_permit",    "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:ACIS_FES_PERMIT",      "key": "pss.acis_permit",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:ACIS_GLOBAL_ONLINE",   "key": "pss.global_online",           "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_APS_KEY",            "key": "pss.a_enabled",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_DOOR_1_CLOSED",      "key": "pss.a_door_1_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_DOOR_2_CLOSED",      "key": "pss.a_door_2_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_SEARCHED",           "key": "pss.a_searched",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_USER_KEY",           "key": "pss.a_user_key",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_VOICE_SEARCHING",    "key": "pss.a_searching",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_APS_KEY",            "key": "pss.b_enabled",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_DOOR_1_CLOSED",      "key": "pss.b_door_1_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_DOOR_2_CLOSED",      "key": "pss.b_door_2_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_SEARCHED",           "key": "pss.b_searched",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_USER_KEY",           "key": "pss.b_enabled",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_VOICE_SEARCHING",    "key": "pss.b_searching",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_APS_KEY",            "key": "pss.c_enabled",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_DOOR_1_CLOSED",      "key": "pss.c_door_1_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_SEARCHED",           "key": "pss.c_searched",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_USER_KEY",           "key": "pss.c_user_key",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_VOICE_SEARCHING",    "key": "pss.c_searching",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:DIW_FLOW_1_LOW_SPT",   "key": "pss.di_water_flow1_trip",     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:DIW_FLOW_2_LOW_SPT",   "key": "pss.di_water_flow2_trip",     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:DIW_FLOW_3_LOW_SPT",   "key": "pss.di_water_flow3_trip",     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:DIW_FLOW_4_LOW_SPT",   "key": "pss.di_water_flow4_trip",     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_APS_KEY",            "key": "pss.e_enabled",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_DOOR_1_CLOSED",      "key": "pss.e_door_1_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_DOOR_2_CLOSED",      "key": "pss.e_door_2_a",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_SEARCHED",           "key": "pss.e_searched",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_USER_KEY",           "key": "pss.e_user_key",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_VOICE_SEARCHING",    "key": "pss.e_searching",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:FAULT_EXISTS",         "key": "pss.chain_a_fault",           "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:FES_BLOCKING_BEAM",    "key": "pss.fe_shutter",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:SES_BLOCKING_BEAM",    "key": "pss.stn_shutter",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:TRIP_EXISTS",          "key": "pss.chain_a_trip",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:A_DOOR_1_CLOSED",      "key": "pss.a_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:A_DOOR_2_CLOSED",      "key": "pss.a_door_2_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:B_DOOR_1_CLOSED",      "key": "pss.b_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:B_DOOR_2_CLOSED",      "key": "pss.b_door_2_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:C_DOOR_1_CLOSED",      "key": "pss.c_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:E_DOOR_1_CLOSED",      "key": "pss.door_1_b",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:E_DOOR_2_CLOSED",      "key": "pss.door_2_b",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:FAULT_EXISTS",         "key": "pss.chain_b_fault",           "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:TRIP_EXISTS",          "key": "pss.chain_b_trip",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_BEAM_PRESENT",       "key": "pss.a_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_BEAM_PRESENT",       "key": "pss.b_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_BEAM_PRESENT",       "key": "pss.c_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_BEAM_PRESENT",       "key": "pss.beam_present",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PC:21ID:FEEPS_FES_PERMIT",     "key": "pss.fe_eps",                  "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_CRASH_BUTTON_1",     "key": "pss.a_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_CRASH_BUTTON_1",     "key": "pss.b_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_CRASH_BUTTON_2",     "key": "pss.b_crash_button_2",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_CRASH_BUTTON_1",     "key": "pss.c_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_CRASH_BUTTON_2",     "key": "pss.c_crash_button_2",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_CRASH_BUTTON_1",     "key": "pss.crash_button_1",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_CRASH_BUTTON_2",     "key": "pss.crash_button_2",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:E_CRASH_BUTTON_3",     "key": "pss.crash_button_3",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
#
#
        { "pv": "align.x",                       "key": "align.x.position",              "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "align.x.position",              "key": "align.x.position",                                 "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "align.y",                       "key": "align.y.position",              "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "align.y.position",              "key": "align.y.position",                                 "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "align.z",                       "key": "align.z.position",              "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "align.z.position",              "key": "align.z.position",                                 "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "appy",                          "key": "appy.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "appy.position",                 "key": "appy.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "appz",                          "key": "appz.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "appz.position",                 "key": "appz.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "backLight.factor",              "key": "backLight.factor.position",     "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "backLight.factor.position",     "key": "backLight.factor.position",                        "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "1"},
        { "pv": "backLight.intensity",           "key": "backLight.intensity.position",  "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "backLight.intensity.position",  "key": "backLight.intensity.position",                     "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "0"},
        { "pv": "backLight",                     "key": "backLight.position",            "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Off", "onam": "On"},
        { "pv": "backLight.position",            "key": "backLight.position",                               "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "pv": "cam.zoom",                      "key": "cam.zoom.position",             "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "cam.zoom.position",             "key": "cam.zoom.position",                                "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "0"},
        { "pv": "capy",                          "key": "capy.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "capy.position",                 "key": "capy.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "capz",                          "key": "capz.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "capz.position",                 "key": "capz.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "centering.x",                   "key": "centering.x.position",          "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "centering.x.position",          "key": "centering.x.position",                             "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "centering.y",                   "key": "centering.y.position",          "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "centering.y.position",          "key": "centering.y.position",                             "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "cryo",                          "key": "cryo.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "cryo.position",                 "key": "cryo.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Out",    "onam": "In"},
        { "pv": "dryer",                         "key": "dryer.position",                "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Out",    "onam": "In"},
        { "pv": "dryer.position",                "key": "dryer.position",                                   "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Off",    "onam": "On"},
        { "pv": "fastShutter",                   "key": "fastShutter.position",          "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "fastShutter.position",          "key": "fastShutter.position",                             "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Closed", "onam": "Open"},
        { "pv": "fluo",                          "key": "fluo.position",                 "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Out",    "onam": "In"},
        { "pv": "fluo.position",                 "key": "fluo.position",                                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Out",    "onam": "In"},
        { "pv": "frontLight.factor",             "key": "frontLight.factor.position",    "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "frontLight.factor.position",    "key": "frontLight.factor.position",                       "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "1"},
        { "pv": "frontLight.intensity",          "key": "frontLight.intensity.position", "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "frontLight.intensity.position", "key": "frontLight.intensity.position",                    "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "0"},
        { "pv": "frontLight",                    "key": "frontLight.position",           "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "frontLight.position",           "key": "frontLight.position",                              "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Off",    "onam": "On"},
        { "pv": "kappa",                         "key": "kappa.position",                "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "kappa.position",                "key": "kappa.position",                                   "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "lightPolar",                    "key": "lightPolar.position",           "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "lightPolar.position",           "key": "lightPolar.position",                              "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "omega",                         "key": "omega.position",                "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "omega.position",                "key": "omega.position",                                   "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "phi",                           "key": "phi.position",                  "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "phi.position",                  "key": "phi.position",                                     "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "scint.focus",                   "key": "scint.focus.position",          "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "scint.focus.position",          "key": "scint.focus.position",                             "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "1"},
        { "pv": "scint",                         "key": "scint.position",                "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "scint.position",                "key": "scint.position",                                   "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "ai", "prec": "3"},
        { "pv": "smartMagnet",                   "key": "smartMagnet.position",          "setter": "kvset", "oscan": "Passive",  "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "smartMagnet.position",          "key": "smartMagnet.position",                             "iscan": "I/O Intr", "dtyp": "Redis Source", "ri": "bi", "znam": "Off",    "onam": "On"},
        #
        #
        #
        { "key": "esaf",               "ri": "longin",   "iscan": "I/O Intr", "dtyp": "Redis Source"},
        { "key": "robot.path",         "ri": "stringin", "iscan": "I/O Intr", "dtyp": "Redis Source"},
        { "key": "collection.running", "ri": "bi",       "iscan": "I/O Intr", "dtyp": "Redis Source"}
    ]
}

ndx    = "stns.2"
for obj in configList[ndx]:
    k = ndx+'.'+obj["key"]

    if obj.has_key( 'ro'):
        print 'HSET %s DTYP "%s"' % ( k, obj["dtyp"])
        print 'HSET %s OUT_RECORD %s' % ( k, obj["ro"])
        if obj.has_key( 'inp'):
            print 'HSET %s INP %s' % ( k, obj["inp"])
            
        if obj.has_key( 'znam'):
            print 'HSET %s ZNAM %s' %( k, obj["znam"])

        if obj.has_key( 'onam'):
            print 'HSET %s ONAM %s' %( k, obj["onam"])

        print 'HSET %s OUT_SCAN "%s"' % ( k, obj["oscan"])

        if obj.has_key( 'pv'):
            print 'HSET %s OUT_PV %s' % ( k, obj["pv"])
        else:
            print 'HSET %s OUT_PV %s' % ( k, k.replace( '.', ':'))


    if obj.has_key( 'ri'):
        if obj.has_key( 'setter'):
            print 'HSET %s SETTER %s' % ( k, obj["setter"])
    
        print 'HSET %s DTYP "%s"' % ( k, obj["dtyp"])
        print 'HSET %s IN_RECORD %s' % ( k, obj["ri"])
        if obj.has_key( 'inp'):
            print 'HSET %s INP %s' % ( k, obj["inp"])
            
        if obj.has_key( 'znam'):
            print 'HSET %s ZNAM %s' %( k, obj["znam"])

        if obj.has_key( 'onam'):
            print 'HSET %s ONAM %s' %( k, obj["onam"])

        print 'HSET %s IN_SCAN "%s"' % ( k, obj["iscan"])

        if obj.has_key( 'pv'):
            print 'HSET %s IN_PV %s' % ( k, obj["pv"])
        else:
            print 'HSET %s IN_PV %s' % ( k, obj["key"].replace( '.', ':'))

