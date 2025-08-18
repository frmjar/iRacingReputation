/*
MIT License - iRacing Reputation System
Utilidades para manipulación de strings - Declaraciones
*/

#pragma once

#include <string>

namespace StringUtils
{

    // Elimina espacios al inicio y final de un string
    std::string TrimCopy(const std::string &s);

    // Normalización básica de nombres (elimina caracteres especiales)
    void BasicNormalize(std::string &s);

    // Conversión de CP1252 (Windows Latin-1) a UTF-8
    std::string CP1252ToUTF8(const std::string &s);

} // namespace StringUtils
