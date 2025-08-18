/*
MIT License - iRacing Reputation System
Configuración centralizada de la aplicación
*/

#pragma once

#include <chrono>
#include <string>

/**
 * @brief Configuración centralizada de la aplicación
 *
 * Contiene todas las constantes y parámetros configurables del sistema
 */
class AppConfig
{
public:
    // Configuración de logging (niveles: 0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR, 4=CRITICAL)
    static constexpr const char *LOG_FILENAME = "iRacingReputation.log";
    static constexpr int LOG_LEVEL = 1; // Nivel INFO para mostrar logs de información

    // Configuración de actualización
    static constexpr auto DATA_UPDATE_INTERVAL = std::chrono::seconds(2);
    static constexpr auto FRAME_TIME = std::chrono::milliseconds(16); // ~60 FPS

    // Configuración de ventana
    static constexpr int DEFAULT_WINDOW_WIDTH = 800;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 600;

    // Configuración de iRacing SDK
    static constexpr int IRACING_TIMEOUT_MS = 16;
    static constexpr int MAX_CARS = 64;

    // Textos de la aplicación
    static constexpr const char *APP_NAME = "iRacing Reputation System";
    static constexpr const char *APP_VERSION = "v1.0.0";

    // Configuración de conexión
    static constexpr bool AUTO_FALLBACK_TO_MOCK = true;
    static constexpr bool PRESERVE_REAL_DATA = true;
};
