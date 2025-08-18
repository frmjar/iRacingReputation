#pragma once

#include "irsdk_client.h"

// Declaración de variables externas del SDK de iRacing
extern irsdkCVar ir_IsOnTrack;           // bool[1] 1=Car on track physics running with player in car
extern irsdkCVar ir_IsOnTrackCar;        // bool[1] 1=Car on track physics running
extern irsdkCVar ir_PlayerCarIdx;        // int[1] Players carIdx
extern irsdkCVar ir_SessionNum;          // int[1] Session number
extern irsdkCVar ir_SessionState;        // int[1] Session state (irsdk_SessionState)
extern irsdkCVar ir_CarIdxLap;           // int[64] Laps started by car index
extern irsdkCVar ir_CarIdxPosition;      // int[64] Cars position in race by car index
extern irsdkCVar ir_CarIdxClassPosition; // int[64] Cars class position in race by car index
