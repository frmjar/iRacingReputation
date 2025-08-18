/*
MIT License - iRacing Reputation System
Proveedor de información de sesión de iRacing - Implementaciones
*/

#include "SessionInfoProvider.h"
#include "../../Core/IRacingSDK/irsdk_client.h"
#include "../../Core/IRacingSDK/irsdk_defines.h"

// Variables externas del SDK de iRacing que necesitamos
extern irsdkCVar ir_SessionState;

namespace SessionInfoProvider
{

    std::string GetSessionType()
    {
        try
        {
            int sessionState = ir_SessionState.getInt();

            switch (sessionState)
            {
            case irsdk_StateInvalid:
                return "Invalid";
            case irsdk_StateGetInCar:
                return "GetInCar";
            case irsdk_StateWarmup:
                return "Warmup";
            case irsdk_StateParadeLaps:
                return "ParadeLaps";
            case irsdk_StateRacing:
                return "Racing";
            case irsdk_StateCheckered:
                return "Checkered";
            case irsdk_StateCoolDown:
                return "CoolDown";
            default:
                return "Unknown";
            }
        }
        catch (...)
        {
            return "Unknown";
        }
    }

    std::string GetCurrentSessionInfo(const std::string &sessionInfoString)
    {
        if (sessionInfoString.empty())
            return "";

        // Parse basic session info like iRon does
        std::string result = "Session: " + GetSessionType();

        // Add track info if available from session string
        if (sessionInfoString.find("TrackDisplayName:") != std::string::npos)
        {
            size_t start = sessionInfoString.find("TrackDisplayName:") + 17;
            size_t end = sessionInfoString.find('\n', start);
            if (end != std::string::npos)
            {
                std::string trackName = sessionInfoString.substr(start, end - start);
                // Clean up the track name
                if (!trackName.empty())
                {
                    result += " | Track: " + trackName;
                }
            }
        }

        return result;
    }

    float GetStrengthOfField(const std::vector<DriverData> &drivers)
    {
        float totalIR = 0.0f;
        int count = 0;

        for (const auto &driver : drivers)
        {
            if (driver.iRating > 0)
            {
                totalIR += driver.iRating;
                count++;
            }
        }

        return count > 0 ? totalIR / count : 0.0f;
    }

    SessionType DetermineSessionType(const std::string &sessionInfo)
    {
        if (sessionInfo.find("Practice") != std::string::npos)
            return SessionType::PRACTICE;
        else if (sessionInfo.find("Qualify") != std::string::npos)
            return SessionType::QUALIFY;
        else if (sessionInfo.find("Race") != std::string::npos)
            return SessionType::RACE;

        return SessionType::UNKNOWN;
    }

} // namespace SessionInfoProvider
