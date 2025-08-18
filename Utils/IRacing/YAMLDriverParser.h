/*
MIT License - iRacing Reputation System
Parser YAML para información de pilotos de iRacing - Declaraciones
*/

#pragma once

#include "../Common/Types.h"
#include <vector>
#include <string>

namespace YAMLDriverParser
{

    // Función principal para parsear información de pilotos desde YAML
    std::vector<DriverData> ParseDriverInfoFromYAML(const std::string &yaml, int playerCarIdx);

    // Funciones helper para parsing YAML estilo iRon
    bool ParseYamlInt(const char *yamlStr, const char *path, int *dest);
    bool ParseYamlStr(const char *yamlStr, const char *path, std::string &dest);

} // namespace YAMLDriverParser
