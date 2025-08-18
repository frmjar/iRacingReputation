/*
MIT License - iRacing Reputation System
Variables globales del SDK de iRacing
*/

#include "irsdk_client.h"

irsdkCVar ir_IsOnTrack("IsOnTrack");                     // bool[1] 1=Car on track physics running with player in car
irsdkCVar ir_IsOnTrackCar("IsOnTrackCar");               // bool[1] 1=Car on track physics running
irsdkCVar ir_PlayerCarIdx("PlayerCarIdx");               // int[1] Players carIdx
irsdkCVar ir_SessionNum("SessionNum");                   // int[1] Session number
irsdkCVar ir_SessionState("SessionState");               // int[1] Session state (irsdk_SessionState)
irsdkCVar ir_CarIdxLap("CarIdxLap");                     // int[64] Laps started by car index
irsdkCVar ir_CarIdxPosition("CarIdxPosition");           // int[64] Cars position in race by car index
irsdkCVar ir_CarIdxClassPosition("CarIdxClassPosition"); // int[64] Cars class position in race by car index
