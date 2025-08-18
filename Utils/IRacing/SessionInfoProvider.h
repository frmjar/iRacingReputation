/*
MIT License - iRacing Reputation System
Proveedor de información de sesión de iRacing - Declaraciones
*/

#pragma once

#include "../Common/Types.h"
#include <string>
#include <vector>

namespace SessionInfoProvider
{

    // Función para obtener tipo de sesión (como en iRon)
    std::string GetSessionType();

    // Función para obtener información de la sesión actual (como en iRon)
    std::string GetCurrentSessionInfo(const std::string &sessionInfoString);

    // Función para obtener SOF (Strength of Field) calculado (como en iRon)
    float GetStrengthOfField(const std::vector<DriverData> &drivers);

    // Función para determinar el tipo de sesión desde string de información
    SessionType DetermineSessionType(const std::string &sessionInfo);

} // namespace SessionInfoProvider
