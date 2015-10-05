import redis
configList = {
    "stns.1" : [
        #
        # EPICS originated pvs
        #
        { "inp": "S:SRcurrentAI",                "key": "current",                     "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:D1:AttAct",                 "key": "attenuation.actual",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:D1:AttRqs",                 "key": "attenuation.request",         "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:D1:AttUnit",                "key": "attenuation.unit",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Factor", "onam": "Percent"},
        { "inp": "21:D1:2:userCalc2",            "key": "I0",                          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 0},
        { "inp": "21:D1:DT:Z0:ActPos",           "key": "detector.dist",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 1},
        { "inp": "21:D1:TB:H:ActPos",            "key": "table.y.position",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 3},
        { "inp": "21:D1:TB:L:ActPos",            "key": "table.x.position",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "ai", "prec": 3},
        { "inp": "21:DCAMS:Edge1:VerticalFound", "key": "capDetected",                 "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
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
        { "inp": "PA:21ID:D_APS_KEY",            "key": "pss.enabled",                 "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_DOOR_1_CLOSED",      "key": "pss.door_1_a",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_DOOR_2_CLOSED",      "key": "pss.door_2_a",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_SEARCHED",           "key": "pss.searched",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_USER_KEY",           "key": "pss.user_key",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_VOICE_SEARCHING",    "key": "pss.searching",               "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:FAULT_EXISTS",         "key": "pss.chain_a_fault",           "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:FES_BLOCKING_BEAM",    "key": "pss.fe_shutter",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:SES_BLOCKING_BEAM",    "key": "pss.stn_shutter",             "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:TRIP_EXISTS",          "key": "pss.chain_a_trip",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:A_DOOR_1_CLOSED",      "key": "pss.a_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:A_DOOR_2_CLOSED",      "key": "pss.a_door_2_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:B_DOOR_1_CLOSED",      "key": "pss.b_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:B_DOOR_2_CLOSED",      "key": "pss.b_door_2_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:C_DOOR_1_CLOSED",      "key": "pss.c_door_1_b",              "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:D_DOOR_1_CLOSED",      "key": "pss.door_1_b",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:D_DOOR_2_CLOSED",      "key": "pss.door_2_b",                "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:FAULT_EXISTS",         "key": "pss.chain_b_fault",           "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PB:21ID:TRIP_EXISTS",          "key": "pss.chain_b_trip",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_BEAM_PRESENT",       "key": "pss.a_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_BEAM_PRESENT",       "key": "pss.b_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_BEAM_PRESENT",       "key": "pss.c_beam_present",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_BEAM_PRESENT",       "key": "pss.beam_present",            "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PC:21ID:FEEPS_FES_PERMIT",     "key": "pss.fe_eps",                  "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:A_CRASH_BUTTON_1",     "key": "pss.a_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_CRASH_BUTTON_1",     "key": "pss.b_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:B_CRASH_BUTTON_2",     "key": "pss.b_crash_button_2",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_CRASH_BUTTON_1",     "key": "pss.c_crash_button_1",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:C_CRASH_BUTTON_2",     "key": "pss.c_crash_button_2",        "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_CRASH_BUTTON_1",     "key": "pss.crash_button_1",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_CRASH_BUTTON_2",     "key": "pss.crash_button_2",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_CRASH_BUTTON_3",     "key": "pss.crash_button_3",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        { "inp": "PA:21ID:D_CRASH_BUTTON_4",     "key": "pss.crash_button_4",          "setter": "redis", "iscan": "Passive", "dtyp": "CA Source", "ri": "bi", "znam": "Off", "onam": "On"},
        #
        # MD2 originated PVs
        #  setter allows write access
        #
        { "pv": "phase",                    "key": "phase",                         "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "stringin"},
        { "pv": "phase_RBV",                "key": "phase",                                            "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "stringout"},
        { "pv": "md2_status_code",          "key": "md2_status_code",                                  "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "align:x",                  "key": "align.x.position",              "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "align:x_RBV",              "key": "align.x.position",                                 "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "align:y",                  "key": "align.y.position",              "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "align:y_RBV",              "key": "align.y.position",                                 "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "align:z",                  "key": "align.z.position",              "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "align:z_RBV",              "key": "align.z.position",                                 "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "appy",                     "key": "appy.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "appy_RBV",                 "key": "appy.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "appz",                     "key": "appz.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "appz_RBV",                 "key": "appz.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "backLight:factor",         "key": "backLight.factor.position",     "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "1"},
        { "pv": "backLight:factor_RBV",     "key": "backLight.factor.position",                        "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "backLight:intensity",      "key": "backLight.intensity.position",  "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "0"},
        { "pv": "backLight:intensity_RBV",  "key": "backLight.intensity.position",                     "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "backLight",                "key": "backLight.position",            "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Off", "onam": "On"},
        { "pv": "backLight_RBV",            "key": "backLight.position",                               "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Off", "onam": "On"},
        { "pv": "cam:zoom",                 "key": "cam.zoom.position",             "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "0"},
        { "pv": "cam:zoom_RBV",             "key": "cam.zoom.position",                                "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "capy",                     "key": "capy.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "capy_RBV",                 "key": "capy.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "capz",                     "key": "capz.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "capz_RBV",                 "key": "capz.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "centering:x",              "key": "centering.x.position",          "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "centering:x_RBV",          "key": "centering.x.position",                             "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "centering:y",              "key": "centering.y.position",          "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "centering:y_RBV",          "key": "centering.y.position",                             "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "cryo",                     "key": "cryo.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Out",    "onam": "In"},
        { "pv": "cryo_RBV",                 "key": "cryo.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Out",    "onam": "In"},
        { "pv": "dryer",                    "key": "dryer.position",                "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Out",    "onam": "In"},
        { "pv": "dryer_RBV",                "key": "dryer.position",                                   "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "fastShutter",              "key": "fastShutter.position",          "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Off",    "onam": "On"},
        { "pv": "fastShutter_RBV",          "key": "fastShutter.position",                             "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Closed", "onam": "Open"},
        { "pv": "fluo",                     "key": "fluo.position",                 "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Out",    "onam": "In"},
        { "pv": "fluo_RBV",                 "key": "fluo.position",                                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Out",    "onam": "In"},
        { "pv": "frontLight:factor",        "key": "frontLight.factor.position",    "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "1"},
        { "pv": "frontLight:factor_RBV",    "key": "frontLight.factor.position",                       "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "fzzntLight:intensity",     "key": "frontLight.intensity.position", "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "0"},
        { "pv": "frontLight:intensity_RBV", "key": "frontLight.intensity.position",                    "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "0"},
        { "pv": "frontLight",               "key": "frontLight.position",           "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Off",    "onam": "On"},
        { "pv": "frontLight_RBV",           "key": "frontLight.position",                              "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "kappa",                    "key": "kappa.position",                "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "kappa_RBV",                "key": "kappa.position",                                   "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "lightPolar",               "key": "lightPolar.position",           "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "lightPolar_RBV",           "key": "lightPolar.position",                              "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "omega",                    "key": "omega.position",                "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "omega_RBV",                "key": "omega.position",                                   "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "phi",                      "key": "phi.position",                  "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "phi_RBV",                  "key": "phi.position",                                     "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "scint:focus",              "key": "scint.focus.position",          "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "1"},
        { "pv": "scint:focus_RBV",          "key": "scint.focus.position",                             "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "1"},
        { "pv": "scint",                    "key": "scint.position",                "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "ai", "prec": "3"},
        { "pv": "scint_RBV",                "key": "scint.position",                                   "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "ao", "prec": "3"},
        { "pv": "smartMagnet",              "key": "smartMagnet.position",          "setter": "kvset", "iscan": "Passive",  "dtyp": "VAL Source",   "ri": "bi", "znam": "Off",    "onam": "On"},
        { "pv": "smartMagnet_RBV",          "key": "smartMagnet.position",                             "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "Off",    "onam": "On"},
        { "pv": "collection:running",       "key": "collection.running",                               "oscan": "I/O Intr", "dtyp": "Redis Source", "ro": "bo", "znam": "False",  "onam": "True"},
        #
        # Script sourced variables
        #
        { "pv": "esaf",                     "key": "esaf",               "ro": "longout",   "oscan": "I/O Intr", "dtyp": "Redis Source"},  # kvRedis.js
        { "pv": "robot:path",               "key": "robot.path",         "ro": "stringout", "oscan": "I/O Intr", "dtyp": "Redis Source"},  # CatsOk.py

        { "pv": "bnp:fb:offon",         "key": "bnp.fb.offon",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off",  "onam": "On"},
        { "pv": "bnp:fb:fullscan",      "key": "bnp.fb.fullscan",    "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off",  "onam": "On"},
        { "pv": "bnp:fb:partialscan",   "key": "bnp.fb.partialscan", "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off",  "onam": "On"},
        { "pv": "bnp:fb:peakx",         "key": "bnp.fb.peakx",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "bnp:fb:peaky",         "key": "bnp.fb.peaky",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "bnp:fb:detune",        "key": "bnp.fb.detune",      "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "bnp:fb:target",        "key": "bnp.fb.target",      "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "bnp:fb:fwhm",          "key": "bnp.fb.fwhm",        "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},

        { "pv": "saxs:start_RBV",       "key": "saxs.start",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.start",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:abort_RBV",       "key": "saxs.abort",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.abort",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:pause_RBV",       "key": "saxs.pause",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.pause",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:snap_RBV",        "key": "saxs.snap",        "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.snap",        "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:retake_RBV",      "key": "saxs.retake",      "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.retake",      "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:retakeframe_RBV", "key": "saxs.retakeframe", "ro": "longout",                      "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.retakeframe", "ri": "longin",    "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:directory_RBV",   "key": "saxs.directory",   "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.directory",   "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:prefix_RBV",      "key": "saxs.prefix",      "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.prefix",      "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:exptime_RBV",     "key": "saxs.exptime",     "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.exptime",     "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:delta_RBV",       "key": "saxs.delta",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.delta",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:ax0_RBV",       "key": "saxs.0.ax0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.ax0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:ay0_RBV",       "key": "saxs.0.ay0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.ay0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:az0_RBV",       "key": "saxs.0.az0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.az0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:cx0_RBV",       "key": "saxs.0.cx0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.cx0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:cy0_RBV",       "key": "saxs.0.cy0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.cy0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:x:dist_RBV",    "key": "saxs.0.x.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.x.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:x:npts_RBV",    "key": "saxs.0.x.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.x.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:y:dist_RBV",    "key": "saxs.0.y.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.y.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:y:npts_RBV",    "key": "saxs.0.y.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.0.y.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:0:set_RBV",       "key": "saxs.0.set",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.0.set",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:prefix1_RBV",     "key": "saxs.prefix1",     "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.prefix1",     "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:1:ax0_RBV",       "key": "saxs.1.ax0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.ax0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:ay0_RBV",       "key": "saxs.1.ay0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.ay0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:az0_RBV",       "key": "saxs.1.az0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.az0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:cx0_RBV",       "key": "saxs.1.cx0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.cx0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:cy0_RBV",       "key": "saxs.1.cy0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.cy0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:x:dist_RBV",    "key": "saxs.1.x.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.x.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:x:npts_RBV",    "key": "saxs.1.x.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.x.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:y:dist_RBV",    "key": "saxs.1.y.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.y.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:y:npts_RBV",    "key": "saxs.1.y.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.1.y.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:1:set_RBV",       "key": "saxs.1.set",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.1.set",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:prefix2_RBV",     "key": "saxs.prefix2",     "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.prefix2",     "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:2:ax0_RBV",       "key": "saxs.2.ax0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.ax0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:ay0_RBV",       "key": "saxs.2.ay0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.ay0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:az0_RBV",       "key": "saxs.2.az0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.az0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:cx0_RBV",       "key": "saxs.2.cx0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.cx0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:cy0_RBV",       "key": "saxs.2.cy0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.cy0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:x:dist_RBV",    "key": "saxs.2.x.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.x.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:x:npts_RBV",    "key": "saxs.2.x.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.x.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:y:dist_RBV",    "key": "saxs.2.y.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.y.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:y:npts_RBV",    "key": "saxs.2.y.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.2.y.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:2:set_RBV",       "key": "saxs.2.set",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.2.set",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:prefix3_RBV",     "key": "saxs.prefix3",     "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.prefix3",     "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        { "pv": "saxs:3:ax0_RBV",       "key": "saxs.3.ax0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.ax0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:ay0_RBV",       "key": "saxs.3.ay0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.ay0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:az0_RBV",       "key": "saxs.3.az0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.az0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:cx0_RBV",       "key": "saxs.3.cx0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.cx0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:cy0_RBV",       "key": "saxs.3.cy0",       "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.cy0",       "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:x:dist_RBV",    "key": "saxs.3.x.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.x.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:x:npts_RBV",    "key": "saxs.3.x.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.x.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:y:dist_RBV",    "key": "saxs.3.y.dist",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.y.dist",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:y:npts_RBV",    "key": "saxs.3.y.npts",    "ro": "ao",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "prec": "2"},    # medm for saxs.py
        {                               "key": "saxs.3.y.npts",    "ri": "ai",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "prec": "2"},     # medm for saxs.py
        { "pv": "saxs:3:set_RBV",       "key": "saxs.3.set",       "ro": "bo",                           "oscan": "I/O Intr", "dtyp": "Redis Source", "znam": "Off", "onam": "On"},
        {                               "key": "saxs.3.set",       "ri": "bi",        "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source",   "znam": "Off", "onam": "On"},
        { "pv": "saxs:prefix4_RBV",     "key": "saxs.prefix4",     "ro": "stringout",                    "oscan": "I/O Intr", "dtyp": "Redis Source"},
        {                               "key": "saxs.prefix4",     "ri": "stringin",  "setter": "redis", "iscan": "Passive",  "dtyp": "VAL Source"},
        
    ]
}

ndx    = "stns.1"
for obj in configList[ndx]:
    k = ndx+'.'+obj["key"]
    
    print 'HDEL %s DTYP OUT_RECORD INP ZNAM ONAM OUT_SCAN OUT_PV SETTER IN_RECORD OUT IN_PV OUT_DTYP IN_DTYP' % (k)

for obj in configList[ndx]:
    k = ndx+'.'+obj["key"]
    
    if obj.has_key( 'ro'):
        print 'HSET %s OUT_DTYP "%s"' % ( k, obj["dtyp"])
        print 'HSET %s OUT_RECORD %s' % ( k, obj["ro"])
        if obj.has_key( 'out'):
            print 'HSET %s OUT %s' % ( k, obj["out"])

        if obj.has_key( 'prec'):
            print 'HSET %s PREC %s' % ( k, obj["prec"])
            
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
    
        print 'HSET %s IN_DTYP "%s"' % ( k, obj["dtyp"])
        print 'HSET %s IN_RECORD %s' % ( k, obj["ri"])
        if obj.has_key( 'inp'):
            print 'HSET %s INP %s' % ( k, obj["inp"])
            
        if obj.has_key( 'prec'):
            print 'HSET %s PREC %s' % ( k, obj["prec"])

        if obj.has_key( 'znam'):
            print 'HSET %s ZNAM %s' %( k, obj["znam"])

        if obj.has_key( 'onam'):
            print 'HSET %s ONAM %s' %( k, obj["onam"])

        print 'HSET %s IN_SCAN "%s"' % ( k, obj["iscan"])

        if obj.has_key( 'pv'):
            print 'HSET %s IN_PV %s' % ( k, obj["pv"])
        else:
            print 'HSET %s IN_PV %s' % ( k, obj["key"].replace( '.', ':'))

    print 'HSETNX %s VALUE ""' % ( k)
