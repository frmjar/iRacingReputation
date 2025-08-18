/*
MIT License - iRacing Reputation System
Parser YAML para información de pilotos de iRacing - Implementaciones
*/

#include "YAMLDriverParser.h"
#include "StringUtils.h"
#include "../../Core/IRacingSDK/yaml_parser.h"
#include "../../Core/IRacingSDK/irsdk_client.h"
#include "../Logging/Logger.h"
#include <algorithm>

// Variables externas del SDK de iRacing que necesitamos
extern irsdkCVar ir_CarIdxPosition;

namespace YAMLDriverParser
{

    bool ParseYamlInt(const char *yamlStr, const char *path, int *dest)
    {
        int count = 0;
        const char *s = nullptr;
        if (parseYaml(yamlStr, path, &s, &count))
        {
            *dest = atoi(s);
            return true;
        }
        return false;
    }

    bool ParseYamlStr(const char *yamlStr, const char *path, std::string &dest)
    {
        int count = 0;
        const char *s = nullptr;
        if (parseYaml(yamlStr, path, &s, &count))
        {
            // strip leading quotes
            if (*s == '"')
            {
                s++;
                count--;
            }
            dest.assign(s, count);
            // strip trailing quotes
            if (!dest.empty() && dest[dest.length() - 1] == '"')
                dest.pop_back();
            return true;
        }
        return false;
    }

    std::vector<DriverData> ParseDriverInfoFromYAML(const std::string &yaml, int playerCarIdx)
    {
        std::vector<DriverData> parsed;

        if (yaml.empty())
            return parsed;

        const char *sessionYaml = yaml.c_str();
        char path[256];
        const int MAX_CARS = 64;

        parsed.reserve(MAX_CARS);

        // Parsear drivers directamente usando el parser de YAML de iRon (más preciso)
        for (int carIdx = 0; carIdx < MAX_CARS; ++carIdx)
        {
            std::string userName;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}UserName:", carIdx);
            if (!ParseYamlStr(sessionYaml, path, userName))
                continue; // No driver at this index

            // Clean special characters from username (similar a iRon)
            for (char &c : userName)
            {
                if (c == '\n' || c == '\r')
                    c = ' ';
                else if ((unsigned char)c < 0x20)
                    c = ' ';
            }

            DriverData d;
            d.carIdx = carIdx;
            d.displayName = StringUtils::CP1252ToUTF8(userName);
            StringUtils::BasicNormalize(d.displayName);
            d.userName = d.displayName;

            // Car number (string version)
            std::string carNumStr;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CarNumber:", carIdx);
            ParseYamlStr(sessionYaml, path, carNumStr);
            d.carNumber = carNumStr.empty() ? std::to_string(carIdx + 1) : carNumStr;

            // iRating
            int irating = 0;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}IRating:", carIdx);
            ParseYamlInt(sessionYaml, path, &irating);
            d.iRating = irating;

            // License information
            std::string licenseStr;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}LicString:", carIdx);
            ParseYamlStr(sessionYaml, path, licenseStr);
            d.licenseString = licenseStr;
            if (!licenseStr.empty())
            {
                d.licenseLevel = std::string(1, licenseStr[0]);
                try
                {
                    d.safetyRating = std::stof(licenseStr.substr(1));
                }
                catch (...)
                {
                    d.safetyRating = 0.0f;
                }
            }

            // License color (como en iRon)
            std::string licenseColStr;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}LicColor:", carIdx);
            if (ParseYamlStr(sessionYaml, path, licenseColStr))
            {
                // Parse hex color like iRon does: "0xFFFFFF"
                unsigned licColHex = 0;
                if (sscanf(licenseColStr.c_str(), "0x%x", &licColHex) == 1)
                {
                    // Store as normalized RGB values (0.0-1.0) in unused fields
                    // Using bestLapTime as temp storage for color data
                    float r = float((licColHex >> 16) & 0xff) / 255.0f;
                    float g = float((licColHex >> 8) & 0xff) / 255.0f;
                    float b = float((licColHex >> 0) & 0xff) / 255.0f;
                    // Store in custom format for UI use (could extend DriverData later)
                }
            }

            // Car class color (como en iRon)
            std::string classColStr;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CarClassColor:", carIdx);
            ParseYamlStr(sessionYaml, path, classColStr);

            // Car class ID
            int classId = 0;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CarClassID:", carIdx);
            ParseYamlInt(sessionYaml, path, &classId);

            // User ID
            int userID = -1;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}UserID:", carIdx);
            ParseYamlInt(sessionYaml, path, &userID);
            d.customerId = (userID > 0) ? userID : (carIdx + 1000);

            // Incident count (como en iRon)
            int incidentCount = 0;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CurDriverIncidentCount:", carIdx);
            ParseYamlInt(sessionYaml, path, &incidentCount);
            // Store in gapToPlayer as temp (could extend DriverData)
            d.gapToPlayer = static_cast<float>(incidentCount);

            // Car name (como en iRon)
            std::string carName;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CarScreenName:", carIdx);
            ParseYamlStr(sessionYaml, path, carName);

            // Check if pace car or spectator (como en iRon)
            int isPaceCar = 0, isSpectator = 0;
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}CarIsPaceCar:", carIdx);
            ParseYamlInt(sessionYaml, path, &isPaceCar);
            sprintf(path, "DriverInfo:Drivers:CarIdx:{%d}IsSpectator:", carIdx);
            ParseYamlInt(sessionYaml, path, &isSpectator);

            // Skip pace car and spectators
            if (isPaceCar || isSpectator)
            {
                continue;
            }

            // Position from telemetry
            try
            {
                d.position = ir_CarIdxPosition.getInt(carIdx);
            }
            catch (...)
            {
                d.position = 0;
            }

            d.isPlayer = (carIdx == playerCarIdx);
            d.isValid = true;

            parsed.push_back(d);
        }

        // Filter out pace car
        parsed.erase(std::remove_if(parsed.begin(), parsed.end(),
                                    [](const DriverData &d)
                                    { return d.displayName == "Pace Car"; }),
                     parsed.end());

        // Filter placeholder drivers that weren't actually parsed
        size_t before = parsed.size();
        parsed.erase(std::remove_if(parsed.begin(), parsed.end(),
                                    [](const DriverData &d)
                                    {
                                        if (d.displayName.rfind("Piloto #", 0) != 0)
                                            return false;
                                        return d.customerId == d.carIdx + 1000;
                                    }),
                     parsed.end());

        size_t removed = before - parsed.size();
        if (removed > 0)
            Logger::Info("Placeholders filtrados: " + std::to_string(removed));

        Logger::Info("ParseDriverInfoYAML (estilo iRon): " + std::to_string(parsed.size()) + " pilotos");

        return parsed;
    }

} // namespace YAMLDriverParser
