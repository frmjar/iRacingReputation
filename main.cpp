/*
MIT License

Copyright (c) 2025 iRacing Reputation System

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <exception>
#include <windows.h>

#include "Utils/Logging/Logger.h"
#include "Core/Application/iRacingReputationApp.h"

/**
 * @brief Punto de entrada principal de la aplicación
 *
 * Inicializa y ejecuta el sistema de reputación de iRacing
 */
int main()
{
    try
    {
        // Activar DPI awareness para evitar escalado de sistema que deforma tamaños.
#if defined(_WIN32)
        // Windows 10 per-monitor v2 si disponible
        HMODULE shcore = LoadLibraryA("Shcore.dll");
        if (shcore)
        {
            typedef HRESULT(WINAPI * SetProcessDpiAwarenessFunc)(int);
            auto setAwareness = (SetProcessDpiAwarenessFunc)GetProcAddress(shcore, "SetProcessDpiAwareness");
            if (setAwareness)
            {
                setAwareness(2 /*PROCESS_PER_MONITOR_DPI_AWARE*/);
            }
            FreeLibrary(shcore);
        }
        else
        {
            SetProcessDPIAware();
        }
#endif
        iRacingReputationApp app;
        app.Run();
        return 0;
    }
    catch (const std::exception &e)
    {
        Logger::Error("Error crítico: " + std::string(e.what()));
        std::cout << "Error crítico en la aplicación: " << e.what() << std::endl;
        std::cout << "Presiona Enter para salir..." << std::endl;
        std::cin.get();
        return -1;
    }
    catch (...)
    {
        Logger::Error("Error crítico desconocido");
        std::cout << "Error crítico desconocido en la aplicación" << std::endl;
        std::cout << "Presiona Enter para salir..." << std::endl;
        std::cin.get();
        return -1;
    }
}
