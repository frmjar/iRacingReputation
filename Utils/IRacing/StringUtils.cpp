/*
MIT License - iRacing Reputation System
Utilidades para manipulación de strings - Implementaciones
*/

#include "StringUtils.h"
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

namespace StringUtils
{

    std::string TrimCopy(const std::string &s)
    {
        auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            return "";
        auto end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    void BasicNormalize(std::string &s)
    {
        std::string out;
        out.reserve(s.size());

        for (char c : s)
        {
            if (c == '\0')
                break;

            // Permitir caracteres alfanuméricos, espacios y algunos especiales
            if (std::isalnum(static_cast<unsigned char>(c)) ||
                c == ' ' || c == '-' || c == '_' || c == '.' ||
                c == '(' || c == ')' || c == '[' || c == ']')
            {
                out += c;
            }
            else if (c == '\t' || c == '\r' || c == '\n')
            {
                out += ' '; // Convertir tabs/newlines a espacios
            }
            // Otros caracteres se ignoran
        }

        // Trim final
        s = TrimCopy(out);
    }

    std::string CP1252ToUTF8(const std::string &s)
    {
#ifdef _WIN32
        if (s.empty())
            return s;

        int wlen = MultiByteToWideChar(1252, 0, s.c_str(), -1, nullptr, 0);
        if (wlen <= 0)
            return s;

        std::wstring wbuf(wlen, L'\0');
        MultiByteToWideChar(1252, 0, s.c_str(), -1, &wbuf[0], wlen);

        int u8len = WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (u8len <= 0)
            return s;

        std::string out(u8len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), -1, &out[0], u8len, nullptr, nullptr);

        if (!out.empty() && out.back() == '\0')
            out.pop_back();

        return out;
#else
        return s; // En sistemas no-Windows, asumir que ya es UTF-8
#endif
    }

} // namespace StringUtils
