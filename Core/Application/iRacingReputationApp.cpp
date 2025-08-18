/*
MIT License - iRacing Reputation System
Implementación de la aplicación principal
*/

#include "iRacingReputationApp.h"
#include <iostream>
#include <thread>
#include <windows.h>

iRacingReputationApp::iRacingReputationApp()
{
    // Configurar consola para UTF-8 al inicio
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Logger::Initialize(AppConfig::LOG_FILENAME, AppConfig::LOG_LEVEL);
    Logger::Info("Iniciando " + std::string(AppConfig::APP_NAME) + " " + AppConfig::APP_VERSION + "...");

    m_driverTagWindow = std::make_unique<DriverTagWindow>();
    m_iracingConnection = std::make_unique<IRacingConnection>();
}

iRacingReputationApp::~iRacingReputationApp()
{
    m_running = false;

    // Cerrar conexión de iRacing
    if (m_iracingConnection)
    {
        m_iracingConnection->Shutdown();
    }

    Logger::Info("Cerrando iRacing Reputation System...");
}

bool iRacingReputationApp::Initialize()
{
    Logger::Info("Inicializando componentes...");

    // Inicializar conexión con iRacing
    if (!m_iracingConnection->Initialize())
    {
        Logger::Warning("No se pudo conectar con iRacing - usando datos mock");
    }
    else
    {
        Logger::Info("Conexión con iRacing establecida");
    }

    // Inicializar ventana de tagging de pilotos
    if (!m_driverTagWindow->Initialize())
    {
        Logger::Error("Error inicializando ventana de tagging");
        return false;
    }

    // Cargar datos iniciales y mostrar ventana
    UpdateDriverData();
    m_driverTagWindow->Show();

    Logger::Info("Componentes básicos inicializados correctamente");
    return true;
}

void iRacingReputationApp::UpdateDriverData()
{
    if (!m_iracingConnection)
    {
        Logger::Error("No hay conexión de iRacing inicializada - usando mock");
        if (ShouldUseMockData())
        {
            m_driverTagWindow->LoadMockData();
        }
        return;
    }

    ConnectionStatus status = m_iracingConnection->Update();
    HandleConnectionStatus(status);
}

bool iRacingReputationApp::ShouldUseMockData() const
{
    return !m_driverTagWindow->IsUsingRealData();
}

void iRacingReputationApp::HandleConnectionStatus(ConnectionStatus status)
{
    const auto &sessionDrivers = m_iracingConnection->GetSessionDrivers();
    LogConnectionInfo(status, sessionDrivers);

    switch (status)
    {
    case ConnectionStatus::IN_SESSION:
    {
        if (!sessionDrivers.empty())
        {
            UpdateWithRealData(sessionDrivers);
        }
        else
        {
            FallbackToMockIfNeeded();
        }
        break;
    }

    case ConnectionStatus::CONNECTED:
    {
        bool isInSession = m_iracingConnection->IsInSession();
        if (isInSession && !sessionDrivers.empty())
        {
            UpdateWithRealData(sessionDrivers);
        }
        else
        {
            FallbackToMockIfNeeded();
        }
        break;
    }

    case ConnectionStatus::DISCONNECTED:
    {
        FallbackToMockIfNeeded();
        break;
    }
    }
}

void iRacingReputationApp::UpdateWithRealData(const std::vector<DriverData> &drivers)
{
    m_driverTagWindow->UpdateSessionData(drivers);
}

void iRacingReputationApp::FallbackToMockIfNeeded()
{
    if (m_driverTagWindow->IsUsingRealData())
    {
        Logger::Warning("Conservando datos reales previos - sin nueva lectura válida");
    }
    else if (ShouldUseMockData())
    {
        Logger::Info("Cargando datos mock (no hay datos reales activos)");
        m_driverTagWindow->LoadMockData();
    }
}

void iRacingReputationApp::LogConnectionInfo(ConnectionStatus status, const std::vector<DriverData> &drivers) const
{
    const size_t driverCount = drivers.size();

    switch (status)
    {
    case ConnectionStatus::IN_SESSION:
        Logger::Info("🏁 SESIÓN ACTIVA - Usando datos REALES de iRacing - " +
                     std::to_string(driverCount) + " pilotos");
        break;

    case ConnectionStatus::CONNECTED:
    {
        bool isInSession = m_iracingConnection->IsInSession();
        Logger::Info("iRacing conectado - En sesión: " + std::string(isInSession ? "SÍ" : "NO") +
                     " - Pilotos: " + std::to_string(driverCount));
        break;
    }

    case ConnectionStatus::DISCONNECTED:
        Logger::Info("iRacing desconectado - usando datos mock si es necesario");
        break;
    }
}

void iRacingReputationApp::Run()
{
    if (!Initialize())
    {
        Logger::Error("Error en la inicialización");
        return;
    }

    m_running = true;
    Logger::Info("Sistema ejecutándose... Usa la ventana para interactuar o ciérrala para salir");

    auto lastDataUpdate = std::chrono::steady_clock::now();

    // Bucle principal optimizado
    while (m_running && m_driverTagWindow)
    {
        auto now = std::chrono::steady_clock::now();

        // Actualizar datos de iRacing periódicamente
        if (now - lastDataUpdate >= AppConfig::DATA_UPDATE_INTERVAL)
        {
            UpdateDriverData();
            lastDataUpdate = now;
        }

        // Actualizar y renderizar ventana
        m_driverTagWindow->Update();

        // Verificar si la ventana debe cerrarse
        if (m_driverTagWindow->ShouldClose())
        {
            Logger::Info("Ventana de tagging cerrada - terminando programa");
            m_running = false;
            break;
        }

        m_driverTagWindow->Render();
        std::this_thread::sleep_for(AppConfig::FRAME_TIME);
    }
}
