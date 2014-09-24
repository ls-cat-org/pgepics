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
            { "epics": "S:SRcurrentAI",      "redis": "stns.1.current",       "prec": 0},
            { "epics": "21:D1:DT:Z0:ActPos", "redis": "stns.1.detector.dist", "prec": 3},
            { "epics": "EPS:21:ID:BLEPS:SPER",       "prec": 0, "redis": "stns.1.pss.bleps_fe_shutter_permit"},
            { "epics": "EPS:21:ID:BLEPS:VACPER",     "prec": 0, "redis": "stns.1.pss.bleps_fe_vacuum_permit"},
            { "epics": "PC:21ID:SDS_BLEPS_PERMIT",   "prec": 0, "redis": "stns.1.pss.bleps_station_permit"},
            { "epics": "PA:21ID:ACIS_FES_PERMIT",    "prec": 0, "redis": "stns.1.pss.acis_permit"},
            { "epics": "PA:21ID:ACIS_GLOBAL_ONLINE", "prec": 0, "redis": "stns.1.pss.global_online"},
            { "epics": "PA:21ID:A_APS_KEY",          "prec": 0, "redis": "stns.1.pss.a_enabled"},
            { "epics": "PA:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.a_door_1_a"},
            { "epics": "PA:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.a_door_2_a"},
            { "epics": "PA:21ID:A_SEARCHED",         "prec": 0, "redis": "stns.1.pss.a_searched"},
            { "epics": "PA:21ID:A_USER_KEY",         "prec": 0, "redis": "stns.1.pss.a_user_key"},
            { "epics": "PA:21ID:A_VOICE_SEARCHING",  "prec": 0, "redis": "stns.1.pss.a_searching"},
            { "epics": "PA:21ID:B_APS_KEY",          "prec": 0, "redis": "stns.1.pss.b_enabled"},
            { "epics": "PA:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.b_door_1_a"},
            { "epics": "PA:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.b_door_2_a"},
            { "epics": "PA:21ID:B_SEARCHED",         "prec": 0, "redis": "stns.1.pss.b_searched"},
            { "epics": "PA:21ID:B_USER_KEY",         "prec": 0, "redis": "stns.1.pss.b_user_key"},
            { "epics": "PA:21ID:B_VOICE_SEARCHING",  "prec": 0, "redis": "stns.1.pss.b_searching"},
            { "epics": "PA:21ID:C_APS_KEY",          "prec": 0, "redis": "stns.1.pss.c_enabled"},
            { "epics": "PA:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.c_door_1_a"},
            { "epics": "PA:21ID:C_SEARCHED",         "prec": 0, "redis": "stns.1.pss.c_searched"},
            { "epics": "PA:21ID:C_USER_KEY",         "prec": 0, "redis": "stns.1.pss.c_user_key"},
            { "epics": "PA:21ID:C_VOICE_SEARCHING",  "prec": 0, "redis": "stns.1.pss.c_searching"},
            { "epics": "PA:21ID:DIW_FLOW_1_LOW_SPT", "prec": 0, "redis": "stns.1.pss.di_water_flow1_trip"},
            { "epics": "PA:21ID:DIW_FLOW_2_LOW_SPT", "prec": 0, "redis": "stns.1.pss.di_water_flow3_trip"},
            { "epics": "PA:21ID:DIW_FLOW_3_LOW_SPT", "prec": 0, "redis": "stns.1.pss.di_water_flow2_trip"},
            { "epics": "PA:21ID:DIW_FLOW_4_LOW_SPT", "prec": 0, "redis": "stns.1.pss.di_water_flow4_trip"},
            { "epics": "PA:21ID:D_APS_KEY",          "prec": 0, "redis": "stns.1.pss.enabled"},
            { "epics": "PA:21ID:D_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.door_1_a"},
            { "epics": "PA:21ID:D_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.door_2_a"},
            { "epics": "PA:21ID:D_SEARCHED",         "prec": 0, "redis": "stns.1.pss.searched"},
            { "epics": "PA:21ID:D_USER_KEY",         "prec": 0, "redis": "stns.1.pss.user_key"},
            { "epics": "PA:21ID:D_VOICE_SEARCHING",  "prec": 0, "redis": "stns.1.pss.searching"},
            { "epics": "PA:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.1.pss.chain_a_fault"},
            { "epics": "PA:21ID:FES_BLOCKING_BEAM",  "prec": 0, "redis": "stns.1.pss.fe_shutter"},
            { "epics": "PA:21ID:SDS_BLOCKING_BEAM",  "prec": 0, "redis": "stns.1.pss.stn_shutter"},
            { "epics": "PA:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.1.pss.chain_a_trip"},
            { "epics": "PB:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.a_door_1_b"},
            { "epics": "PB:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.a_door_2_b"},
            { "epics": "PB:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.b_door_1_b"},
            { "epics": "PB:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.b_door_2_b"},
            { "epics": "PB:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.c_door_1_b"},
            { "epics": "PB:21ID:D_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.1.pss.door_1_b"},
            { "epics": "PB:21ID:D_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.1.pss.door_2_b"},
            { "epics": "PB:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.1.pss.chain_b_fault"},
            { "epics": "PB:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.1.pss.chain_b_trip"},
            { "epics": "PC:21ID:A_BEAM_PRESENT",     "prec": 0, "redis": "stns.1.pss.a_beam_present"},
            { "epics": "PC:21ID:B_BEAM_PRESENT",     "prec": 0, "redis": "stns.1.pss.b_beam_present"},
            { "epics": "PC:21ID:C_BEAM_PRESENT",     "prec": 0, "redis": "stns.1.pss.c_beam_present"},
            { "epics": "PC:21ID:D_BEAM_PRESENT",     "prec": 0, "redis": "stns.1.pss.beam_present"},
            { "epics": "PC:21ID:FEEPS_FES_PERMIT",   "prec": 0, "redis": "stns.1.pss.fe_eps"},

            { "epics": "PA:21ID:A_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.1.pss.a_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.1.pss.b_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.1.pss.b_crash_button_2"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.1.pss.c_crash_button_1"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.1.pss.c_crash_button_2"},

            { "epics": "PA:21ID:D_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.1.pss.crash_button_1"},
            { "epics": "PA:21ID:D_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.1.pss.crash_button_2"},
            { "epics": "PA:21ID:D_CRASH_BUTTON_3",   "prec": 0, "redis": "stns.1.pss.crash_button_3"},
            { "epics": "PA:21ID:D_CRASH_BUTTON_4",   "prec": 0, "redis": "stns.1.pss.crash_button_4"}            ]
        },
    {
        "stn": 2,
        "rconnect": {
            "host": "venison.ls-cat.org",
            "port": 6379,
            "db":   0
            },
        "rpvs": [
            { "epics": "S:SRcurrentAI",      "redis": "stns.2.current",       "prec": 0},
            { "epics": "21:E1:DT:Z0:ActPos", "redis": "stns.2.detector.dist", "prec": 3},
            { "epics": "EPS:21:ID:BLEPS:SPER",       "prec": 0, "redis": "stns.2.pss.bleps_fe_shutter_permit"},
            { "epics": "EPS:21:ID:BLEPS:VACPER",     "prec": 0, "redis": "stns.2.pss.bleps_fe_vacuum_permit"},
            { "epics": "PC:21ID:SES_BLEPS_PERMIT",   "prec": 0, "redis": "stns.2.pss.bleps_station_permit"},
            { "epics": "PA:21ID:ACIS_FES_PERMIT",    "prec": 0, "redis": "stns.2.pss.acis_permit"},
            { "epics": "PA:21ID:ACIS_GLOBAL_ONLINE", "prec": 0, "redis": "stns.2.pss.global_online"},
            { "epics": "PA:21ID:A_APS_KEY",          "prec": 0, "redis": "stns.2.pss.a_enabled"},
            { "epics": "PA:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.a_door_1_a"},
            { "epics": "PA:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.a_door_2_a"},
            { "epics": "PA:21ID:A_SEARCHED",         "prec": 0, "redis": "stns.2.pss.a_searched"},
            { "epics": "PA:21ID:A_USER_KEY",         "prec": 0, "redis": "stns.2.pss.a_user_key"},
            { "epics": "PA:21ID:A_VOICE_SEARCHING",  "prec": 0, "redis": "stns.2.pss.a_searching"},
            { "epics": "PA:21ID:B_APS_KEY",          "prec": 0, "redis": "stns.2.pss.b_enabled"},
            { "epics": "PA:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.b_door_1_a"},
            { "epics": "PA:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.b_door_2_a"},
            { "epics": "PA:21ID:B_SEARCHED",         "prec": 0, "redis": "stns.2.pss.b_searched"},
            { "epics": "PA:21ID:B_USER_KEY",         "prec": 0, "redis": "stns.2.pss.b_enabled"},
            { "epics": "PA:21ID:B_VOICE_SEARCHING",  "prec": 0, "redis": "stns.2.pss.b_searching"},
            { "epics": "PA:21ID:C_APS_KEY",          "prec": 0, "redis": "stns.2.pss.c_enabled"},
            { "epics": "PA:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.c_door_1_a"},
            { "epics": "PA:21ID:C_SEARCHED",         "prec": 0, "redis": "stns.2.pss.c_searched"},
            { "epics": "PA:21ID:C_USER_KEY",         "prec": 0, "redis": "stns.2.pss.c_user_key"},
            { "epics": "PA:21ID:C_VOICE_SEARCHING",  "prec": 0, "redis": "stns.2.pss.c_searching"},
            { "epics": "PA:21ID:DIW_FLOW_1_LOW_SPT", "prec": 0, "redis": "stns.2.pss.di_water_flow1_trip"},
            { "epics": "PA:21ID:DIW_FLOW_2_LOW_SPT", "prec": 0, "redis": "stns.2.pss.di_water_flow2_trip"},
            { "epics": "PA:21ID:DIW_FLOW_3_LOW_SPT", "prec": 0, "redis": "stns.2.pss.di_water_flow3_trip"},
            { "epics": "PA:21ID:DIW_FLOW_4_LOW_SPT", "prec": 0, "redis": "stns.2.pss.di_water_flow4_trip"},
            { "epics": "PA:21ID:E_APS_KEY",          "prec": 0, "redis": "stns.2.pss.e_enabled"},
            { "epics": "PA:21ID:E_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.e_door_1_a"},
            { "epics": "PA:21ID:E_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.e_door_2_a"},
            { "epics": "PA:21ID:E_SEARCHED",         "prec": 0, "redis": "stns.2.pss.e_searched"},
            { "epics": "PA:21ID:E_USER_KEY",         "prec": 0, "redis": "stns.2.pss.e_user_key"},
            { "epics": "PA:21ID:E_VOICE_SEARCHING",  "prec": 0, "redis": "stns.2.pss.e_searching"},
            { "epics": "PA:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.2.pss.chain_a_fault"},
            { "epics": "PA:21ID:FES_BLOCKING_BEAM",  "prec": 0, "redis": "stns.2.pss.fe_shutter"},
            { "epics": "PA:21ID:SES_BLOCKING_BEAM",  "prec": 0, "redis": "stns.2.pss.stn_shutter"},
            { "epics": "PA:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.2.pss.chain_a_trip"},
            { "epics": "PB:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.a_door_1_b"},
            { "epics": "PB:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.a_door_2_b"},
            { "epics": "PB:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.b_door_1_b"},
            { "epics": "PB:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.b_door_2_b"},
            { "epics": "PB:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.c_door_1_b"},
            { "epics": "PB:21ID:E_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.2.pss.door_1_b"},
            { "epics": "PB:21ID:E_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.2.pss.door_2_b"},
            { "epics": "PB:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.2.pss.chain_b_fault"},
            { "epics": "PB:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.2.pss.chain_b_trip"},
            { "epics": "PC:21ID:A_BEAM_PRESENT",     "prec": 0, "redis": "stns.2.pss.a_beam_present"},
            { "epics": "PC:21ID:B_BEAM_PRESENT",     "prec": 0, "redis": "stns.2.pss.b_beam_present"},
            { "epics": "PC:21ID:C_BEAM_PRESENT",     "prec": 0, "redis": "stns.2.pss.c_beam_present"},
            { "epics": "PC:21ID:E_BEAM_PRESENT",     "prec": 0, "redis": "stns.2.pss.beam_present"},
            { "epics": "PC:21ID:FEEPS_FES_PERMIT",   "prec": 0, "redis": "stns.2.pss.fe_eps"},
            { "epics": "PA:21ID:A_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.2.pss.a_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.2.pss.b_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.2.pss.b_crash_button_2"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.2.pss.c_crash_button_1"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.2.pss.c_crash_button_2"},
            { "epics": "PA:21ID:E_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.2.pss.crash_button_1"},
            { "epics": "PA:21ID:E_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.2.pss.crash_button_2"},
            { "epics": "PA:21ID:E_CRASH_BUTTON_3",   "prec": 0, "redis": "stns.2.pss.crash_button_3"},
            { "default": "1",                                   "redis": "stns.2.pss.crash_button_4"}
            ]
        },
    {
        "stn": 3,
        "rconnect": {
            "host": "vanilla.ls-cat.org",
            "port": 6379,
            "db":   0
            },
        "rpvs": [
            { "epics": "S:SRcurrentAI",      "redis": "stns.3.current",       "prec": 0},
            { "epics": "21:F1:DT:D:ActPos", "redis": "stns.3.detector.dist", "prec": 3},
            { "epics": "EPS:21:ID:BLEPS:SPER",       "prec": 0, "redis": "stns.3.pss.bleps_fe_shutter_permit"},
            { "epics": "EPS:21:ID:BLEPS:VACPER",     "prec": 0, "redis": "stns.3.pss.bleps_fe_vacuum_permit"},
            { "epics": "PC:21ID:SFS_BLEPS_PERMIT",   "prec": 0, "redis": "stns.3.pss.bleps_station_permit"},
            { "epics": "PA:21ID:ACIS_FES_PERMIT",    "prec": 0, "redis": "stns.3.pss.acis_permit"},
            { "epics": "PA:21ID:ACIS_GLOBAL_ONLINE", "prec": 0, "redis": "stns.3.pss.global_onlin"},
            { "epics": "PA:21ID:A_APS_KEY",          "prec": 0, "redis": "stns.3.pss.a_enabled"},
            { "epics": "PA:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.a_door_1_a"},
            { "epics": "PA:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.a_door_2_a"},
            { "epics": "PA:21ID:A_SEARCHED",         "prec": 0, "redis": "stns.3.pss.a_searched"},
            { "epics": "PA:21ID:A_USER_KEY",         "prec": 0, "redis": "stns.3.pss.a_user_key"},
            { "epics": "PA:21ID:A_VOICE_SEARCHING",  "prec": 0, "redis": "stns.3.pss.a_searching"},
            { "epics": "PA:21ID:B_APS_KEY",          "prec": 0, "redis": "stns.3.pss.b_enabled"},
            { "epics": "PA:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.b_door_1_a"},
            { "epics": "PA:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.b_door2_a"},
            { "epics": "PA:21ID:B_SEARCHED",         "prec": 0, "redis": "stns.3.pss.b_searched"},
            { "epics": "PA:21ID:B_USER_KEY",         "prec": 0, "redis": "stns.3.pss.b_user_key"},
            { "epics": "PA:21ID:B_VOICE_SEARCHING",  "prec": 0, "redis": "stns.3.pss.b_searching"},
            { "epics": "PA:21ID:C_APS_KEY",          "prec": 0, "redis": "stns.3.pss.c_enabled"},
            { "epics": "PA:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.c_door_1_a"},
            { "epics": "PA:21ID:C_SEARCHED",         "prec": 0, "redis": "stns.3.pss.c_searched"},
            { "epics": "PA:21ID:C_USER_KEY",         "prec": 0, "redis": "stns.3.pss.c_user_key"},
            { "epics": "PA:21ID:C_VOICE_SEARCHING",  "prec": 0, "redis": "stns.3.pss.c_searching"},
            { "epics": "PA:21ID:DIW_FLOW_1_LOW_SPT", "prec": 0, "redis": "stns.3.pss.di_water_flow1_trip"},
            { "epics": "PA:21ID:DIW_FLOW_2_LOW_SPT", "prec": 0, "redis": "stns.3.pss.di_water_flow2_trip"},
            { "epics": "PA:21ID:DIW_FLOW_3_LOW_SPT", "prec": 0, "redis": "stns.3.pss.di_water_flow3_trip"},
            { "epics": "PA:21ID:DIW_FLOW_4_LOW_SPT", "prec": 0, "redis": "stns.3.pss.di_water_flow4_trip"},
            { "epics": "PA:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.3.pss.chain_1_fault"},
            { "epics": "PA:21ID:FES_BLOCKING_BEAM",  "prec": 0, "redis": "stns.3.pss.fe_shutter"},
            { "epics": "PA:21ID:F_APS_KEY",          "prec": 0, "redis": "stns.3.pss.enabled"},
            { "epics": "PA:21ID:F_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.door_1_a"},
            { "epics": "PA:21ID:F_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.door_2_a"},
            { "epics": "PA:21ID:F_SEARCHED",         "prec": 0, "redis": "stns.3.pss.searched"},
            { "epics": "PA:21ID:F_USER_KEY",         "prec": 0, "redis": "stns.3.pss.user_key"},
            { "epics": "PA:21ID:F_VOICE_SEARCHING",  "prec": 0, "redis": "stns.3.pss.searching"},
            { "epics": "PA:21ID:SFS_BLOCKING_BEAM",  "prec": 0, "redis": "stns.3.pss.stn_shutter"},
            { "epics": "PA:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.3.pss.chain_a_trip"},
            { "epics": "PB:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.a_door_1_b"},
            { "epics": "PB:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.a_door_2_b"},
            { "epics": "PB:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.b_door_1_b"},
            { "epics": "PB:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.b_door-2_b"},
            { "epics": "PB:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.c_door_1_b"},
            { "epics": "PB:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.3.pss.chain_b_fault"},
            { "epics": "PB:21ID:F_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.3.pss.door_1_b"},
            { "epics": "PB:21ID:F_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.3.pss.door_2_b"},
            { "epics": "PB:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.3.pss.chain_b_trip"},
            { "epics": "PC:21ID:A_BEAM_PRESENT",     "prec": 0, "redis": "stns.3.pss.a_beam_present"},
            { "epics": "PC:21ID:B_BEAM_PRESENT",     "prec": 0, "redis": "stns.3.pss.b_beam_present"},
            { "epics": "PC:21ID:C_BEAM_PRESENT",     "prec": 0, "redis": "stns.3.pss.c_beam_present"},
            { "epics": "PC:21ID:FEEPS_FES_PERMIT",   "prec": 0, "redis": "stns.3.pss.fe_eps"},
            { "epics": "PC:21ID:F_BEAM_PRESENT",     "prec": 0, "redis": "stns.3.pss.beam_present"},
            { "epics": "PA:21ID:A_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.3.pss.a_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.3.pss.b_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.3.pss.b_crash_button_2"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.3.pss.c_crash_button_1"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.3.pss.c_crash_button_2"},
            { "epics": "PA:21ID:F_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.3.pss.crash_button_1"},
            { "default": "1",                                   "redis": "stns.3.pss.crash_button_2"},
            { "default": "1",                                   "redis": "stns.3.pss.crash_button_3"},
            { "default": "1",                                   "redis": "stns.3.pss.crash_button_4"}            

            ]
        },
    {
        "stns": 4,
        "rconnect": {
            "host": "vinegar.ls-cat.org",
            "port": 6379,
            "db":   0
            },
        "rpvs": [
            { "epics": "S:SRcurrentAI",      "redis": "stns.4.current",       "prec": 0},
            { "epics": "21:G1:DT:D:ActPos", "redis": "stns.4.detector.dist", "prec": 3},
            { "epics": "EPS:21:ID:BLEPS:SPER",       "prec": 0, "redis": "stns.4.pss.bleps_fe_shutter_permit"},
            { "epics": "EPS:21:ID:BLEPS:VACPER",     "prec": 0, "redis": "stns.4.pss.bleps_fe_vacuum_permit"},
            { "epics": "PC:21ID:SGS_BLEPS_PERMIT",   "prec": 0, "redis": "stns.4.pss.bleps_station_permit"},
            { "epics": "PA:21ID:ACIS_FES_PERMIT",    "prec": 0, "redis": "stns.4.pss.acis_permit"},
            { "epics": "PA:21ID:ACIS_GLOBAL_ONLINE", "prec": 0, "redis": "stns.4.pss.global_online"},
            { "epics": "PA:21ID:A_APS_KEY",          "prec": 0, "redis": "stns.4.pss.a_enabled"},
            { "epics": "PA:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.a_door_1_a"},
            { "epics": "PA:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.a_door_2_a"},
            { "epics": "PA:21ID:A_SEARCHED",         "prec": 0, "redis": "stns.4.pss.a_searched"},
            { "epics": "PA:21ID:A_USER_KEY",         "prec": 0, "redis": "stns.4.pss.a_user_key"},
            { "epics": "PA:21ID:A_VOICE_SEARCHING",  "prec": 0, "redis": "stns.4.pss.a_searching"},
            { "epics": "PA:21ID:B_APS_KEY",          "prec": 0, "redis": "stns.4.pss.b_enabled"},
            { "epics": "PA:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.b_door_1_a"},
            { "epics": "PA:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.b_door_2_a"},
            { "epics": "PA:21ID:B_SEARCHED",         "prec": 0, "redis": "stns.4.pss.b_searched"},
            { "epics": "PA:21ID:B_USER_KEY",         "prec": 0, "redis": "stns.4.pss.b_user_key"},
            { "epics": "PA:21ID:B_VOICE_SEARCHING",  "prec": 0, "redis": "stns.4.pss.b_searching"},
            { "epics": "PA:21ID:C_APS_KEY",          "prec": 0, "redis": "stns.4.pss.c_enabled"},
            { "epics": "PA:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.c_door_1_a"},
            { "epics": "PA:21ID:C_SEARCHED",         "prec": 0, "redis": "stns.4.pss.c_searched"},
            { "epics": "PA:21ID:C_USER_KEY",         "prec": 0, "redis": "stns.4.pss.c_user_key"},
            { "epics": "PA:21ID:C_VOICE_SEARCHING",  "prec": 0, "redis": "stns.4.pss.c_searching"},
            { "epics": "PA:21ID:DIW_FLOW_1_LOW_SPT", "prec": 0, "redis": "stns.4.pss.di_water_flow1_trip"},
            { "epics": "PA:21ID:DIW_FLOW_2_LOW_SPT", "prec": 0, "redis": "stns.4.pss.di_water_flow2_trip"},
            { "epics": "PA:21ID:DIW_FLOW_3_LOW_SPT", "prec": 0, "redis": "stns.4.pss.di_water_flow3_trip"},
            { "epics": "PA:21ID:DIW_FLOW_4_LOW_SPT", "prec": 0, "redis": "stns.4.pss.di_water_flow4_trip"},
            { "epics": "PA:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.4.pss.chain_a_fault"},
            { "epics": "PA:21ID:FES_BLOCKING_BEAM",  "prec": 0, "redis": "stns.4.pss.fe_shutter"},
            { "epics": "PA:21ID:G_APS_KEY",          "prec": 0, "redis": "stns.4.pss.enabled"},
            { "epics": "PA:21ID:G_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.door_1_a"},
            { "epics": "PA:21ID:G_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.door_2_a"},
            { "epics": "PA:21ID:G_SEARCHED",         "prec": 0, "redis": "stns.4.pss.searched"},
            { "epics": "PA:21ID:G_USER_KEY",         "prec": 0, "redis": "stns.4.pss.enabled"},
            { "epics": "PA:21ID:G_VOICE_SEARCHING",  "prec": 0, "redis": "stns.4.pss.searching"},
            { "epics": "PA:21ID:SGS_BLOCKING_BEAM",  "prec": 0, "redis": "stns.4.pss.stn_shutter"},
            { "epics": "PA:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.4.pss.chain_a_trip"},
            { "epics": "PB:21ID:A_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.a_door_1_b"},
            { "epics": "PB:21ID:A_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.a_door_2_b"},
            { "epics": "PB:21ID:B_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.b_door_1_b"},
            { "epics": "PB:21ID:B_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.b_door_2_b"},
            { "epics": "PB:21ID:C_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.c_door_1_b"},
            { "epics": "PB:21ID:FAULT_EXISTS",       "prec": 0, "redis": "stns.4.pss.chain_b_fault"},
            { "epics": "PB:21ID:G_DOOR_1_CLOSED",    "prec": 0, "redis": "stns.4.pss.door_1_b"},
            { "epics": "PB:21ID:G_DOOR_2_CLOSED",    "prec": 0, "redis": "stns.4.pss.door_2_b"},
            { "epics": "PB:21ID:TRIP_EXISTS",        "prec": 0, "redis": "stns.4.pss.chain_b_trip"},
            { "epics": "PC:21ID:A_BEAM_PRESENT",     "prec": 0, "redis": "stns.4.pss.a_beam_present"},
            { "epics": "PC:21ID:B_BEAM_PRESENT",     "prec": 0, "redis": "stns.4.pss.b_beam_present"},
            { "epics": "PC:21ID:C_BEAM_PRESENT",     "prec": 0, "redis": "stns.4.pss.c_beam_present"},
            { "epics": "PC:21ID:FEEPS_FES_PERMIT",   "prec": 0, "redis": "stns.4.pss.fe_eps"},
            { "epics": "PC:21ID:G_BEAM_PRESENT",     "prec": 0, "redis": "stns.4.pss.beam_present"},
            { "epics": "PA:21ID:A_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.4.pss.a_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.4.pss.b_crash_button_1"},
            { "epics": "PA:21ID:B_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.4.pss.b_crash_button_2"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.4.pss.c_crash_button_1"},
            { "epics": "PA:21ID:C_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.4.pss.c_crash_button_2"},
            { "epics": "PA:21ID:G_CRASH_BUTTON_1",   "prec": 0, "redis": "stns.4.pss.crash_button_1"},
            { "epics": "PA:21ID:G_CRASH_BUTTON_2",   "prec": 0, "redis": "stns.4.pss.crash_button_2"},
            { "default": "1",                                   "redis": "stns.4.pss.crash_button_3"},
            { "default": "1",                                   "redis": "stns.4.pss.crash_button_4"}

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


        if  not self.pvList[pv.pvname].has_key('lastValue') or self.pvList[pv.pvname]["lastValue"] != tmp:
            print "here i am", k, err, tmp
            self.r.hset( k, "VALUE", tmp)
            self.r.publish( "REDIS_PV_CONNECTOR", k)
            self.pvList[pv.pvname]["lastValue"] = tmp;

    def __init__( self, rconnect, rpvs):
        self.r = redis.Redis( host=rconnect["host"], port=rconnect["port"], db=rconnect["db"])

        self.pvList = {}
        for rpv in rpvs:
            if rpv.has_key( "epics"):
                self.pvList[rpv["epics"]] = { "rkey": rpv["redis"], "pv": EpicsCA.PV( rpv["epics"]), "prec": rpv["prec"]}

            #
            # Sometimes we need a redis variable even when there is not a corresponding PV.
            # For example, the stations do not all have the same number of crash buttons and the simplist way to deal
            # with this is to assume they all have the maximum number (4 as of this writing) so we need
            # some dummy variables to simplify the UI logic.
            #
            if rpv.has_key( "default"):
                self.r.hset( rpv["redis"], "VALUE", rpv["default"])
                self.r.publish( "REDIS_PV_CONNECTOR", rpv["redis"])
            

        #
        # Initialize PV's
        #
        for p in self.pvList.keys():
            try:
                ftmp = float(self.pvList[p]["pv"].get())
                tmp  = "%.*f" % (self.pvList[p]["prec"], ftmp)
                self.pvList[p]["lastValue"] = tmp;
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


