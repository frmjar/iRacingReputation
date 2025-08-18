/*
MIT License - iRacing Reputation System
Aplicación principal del sistema de reputación de pilotos
*/

#pragma once

#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

#include "Utils/Logging/Logger.h"
#include "UI/DriverTagWindow.h"
#include "Core/IRacingSDK/IRacingConnection.h"
#include "AppConfig.h"

/**
 * @brief Clase principal de la aplicación iRacing Reputation System
 *
 * Maneja la lógica principal, actualización de datos y renderizado
 */
class iRacingReputationApp
{
public:
    iRacingReputationApp();
    ~iRacingReputationApp();

    // Métodos principales
    bool Initialize();
    void Run();

private:
    // Componentes principales
    std::unique_ptr<DriverTagWindow> m_driverTagWindow;
    std::unique_ptr<IRacingConnection> m_iracingConnection;

    // Control de ejecución
    std::atomic<bool> m_running{false};

    // Métodos privados
    void UpdateDriverData();
    bool ShouldUseMockData() const;
    void HandleConnectionStatus(ConnectionStatus status);
    void UpdateWithRealData(const std::vector<DriverData> &drivers);
    void FallbackToMockIfNeeded();
    void LogConnectionInfo(ConnectionStatus status, const std::vector<DriverData> &drivers) const;
};
