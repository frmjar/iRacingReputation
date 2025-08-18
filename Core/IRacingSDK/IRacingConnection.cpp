/*
MIT License - iRacing Reputation System
Clase para manejar la conexión y datos del SDK de iRacing - Implementaciones
*/

#include "IRacingConnection.h"
#include "IRacingVariables.h"
#include "../../Utils/IRacing/YAMLDriverParser.h"
#include "../../Utils/IRacing/SessionInfoProvider.h"
#include "../../Utils/Logging/Logger.h"
#include <chrono>

IRacingConnection::~IRacingConnection()
{
    Shutdown();
}

bool IRacingConnection::Initialize()
{
    if (m_initialized)
    {
        Logger::Warning("IRacingConnection ya está inicializado");
        return true;
    }

    Logger::Info("Inicializando conexión con iRacing SDK...");

    m_initialized = true;
    m_lastUpdateTime = std::chrono::steady_clock::now();

    Logger::Info("iRacing SDK inicializado correctamente");
    return true;
}

void IRacingConnection::Shutdown()
{
    if (!m_initialized)
        return;

    Logger::Info("Cerrando conexión con iRacing SDK");

    m_initialized = false;
    m_connected = false;
    m_inSession = false;
    m_sessionDrivers.clear();
}

ConnectionStatus IRacingConnection::Update()
{
    if (!m_initialized)
        return ConnectionStatus::DISCONNECTED;

    // Esperar por datos del SDK usando el singleton
    irsdkClient &client = irsdkClient::instance();
    bool hasData = client.waitForData(16); // 16ms timeout

    if (!hasData || !client.isConnected())
    {
        if (m_connected)
        {
            Logger::Info("Conexión con iRacing perdida");
            m_connected = false;
            m_inSession = false;
            m_sessionDrivers.clear();
        }
        return ConnectionStatus::DISCONNECTED;
    }

    // Verificar si hay cambios
    int currentStatusID = client.getStatusID();
    if (currentStatusID == m_lastStatusID)
    {
        return m_connected ? (m_inSession ? ConnectionStatus::IN_SESSION : ConnectionStatus::CONNECTED)
                           : ConnectionStatus::DISCONNECTED;
    }

    m_lastStatusID = currentStatusID;

    // Marcar como conectado si no lo estaba
    if (!m_connected)
    {
        Logger::Info("Conectado a iRacing");
        m_connected = true;
    }

    // Actualizar información de sesión
    ParseSessionInfo();

    return m_inSession ? ConnectionStatus::IN_SESSION : ConnectionStatus::CONNECTED;
}

void IRacingConnection::UpdateDriverData()
{
    Logger::Info("Actualizando datos de pilotos (aceptando position=0) ...");

    try
    {
        // Solo procesamos si tenemos un playerCarIdx válido
        if (m_carIdx >= 0)
        {
            // Obtener datos del jugador desde las variables globales
            m_playerData.carIdx = m_carIdx;
            m_playerData.isPlayer = true;

            try
            {
                m_playerData.position = ir_CarIdxPosition.getInt(m_carIdx);
            }
            catch (...)
            {
                m_playerData.position = 0; // Default cuando no hay datos
            }

            Logger::Info("✓ Datos del jugador actualizados - Posición: " + std::to_string(m_playerData.position));
        }

        int driverCount = 0;
        for (int i = 0; i < 64; ++i)
        {
            try
            {
                int position = ir_CarIdxPosition.getInt(i);
                if (position >= 0)
                { // Aceptar position=0 también
                    driverCount++;
                }
            }
            catch (...)
            {
                // Skip invalid entries
            }
        }

        Logger::Info("✓ Capturados " + std::to_string(driverCount) + " pilotos (posiciones pueden ser 0)");
    }
    catch (...)
    {
        Logger::Warning("Error al actualizar datos de pilotos");
    }
}

void IRacingConnection::CalculateGapsToPlayer()
{
    if (m_sessionDrivers.empty() || m_carIdx < 0)
        return;

    // Por ahora, simplificamos el cálculo de gaps
    // En una implementación completa, usaríamos las variables irsdkCVar correspondientes

    for (auto &driver : m_sessionDrivers)
    {
        if (driver.carIdx == m_carIdx)
            continue;

        // Cálculo simplificado basado en posición
        int gapPositions = driver.position - m_playerData.position;
        driver.gapToPlayer = gapPositions * 5.0f; // Estimación: 5 segundos por posición
        driver.isAhead = gapPositions < 0;        // Posición menor = adelante
    }
}

void IRacingConnection::ParseDriverInfoYAML(const std::string &yaml)
{
    // Usar el parser YAML extraído para mejor organización del código
    m_sessionDrivers = YAMLDriverParser::ParseDriverInfoFromYAML(yaml, m_carIdx);
}

void IRacingConnection::ParseSessionInfo()
{
    // Implementación usando variables irsdkCVar (patrón de iRon)
    // Detecta correctamente sesiones activas incluso desde el garage

    try
    {
        // Verificar estado básico de conexión
        bool isOnTrack = ir_IsOnTrack.getBool();
        bool isOnTrackCar = ir_IsOnTrackCar.getBool();
        int playerCarIdx = ir_PlayerCarIdx.getInt();
        int sessionState = ir_SessionState.getInt();

        Logger::Info("Estado SDK - IsOnTrack: " + std::string(isOnTrack ? "SI" : "NO") +
                     " | IsOnTrackCar: " + std::string(isOnTrackCar ? "SI" : "NO") +
                     " | PlayerCarIdx: " + std::to_string(playerCarIdx) +
                     " | SessionState: " + std::to_string(sessionState));

        // Determinar si estamos en una sesión válida
        m_carIdx = playerCarIdx;

        // Consideramos que hay sesión activa si:
        // 1. Tenemos un PlayerCarIdx válido (>= 0) Y
        // 2. El SessionState no es inválido (0)
        bool inValidSession = (playerCarIdx >= 0 && sessionState > irsdk_StateInvalid);

        if (inValidSession)
        {
            m_inSession = true;

            // Log del estado de la sesión
            std::string stateText;
            switch (sessionState)
            {
            case irsdk_StateGetInCar:
                stateText = "ENTRAR_COCHE";
                break;
            case irsdk_StateWarmup:
                stateText = "CALENTAMIENTO";
                break;
            case irsdk_StateParadeLaps:
                stateText = "FORMACION";
                break;
            case irsdk_StateRacing:
                stateText = "CARRERA";
                break;
            case irsdk_StateCheckered:
                stateText = "TERMINADA";
                break;
            case irsdk_StateCoolDown:
                stateText = "ENFRIAMIENTO";
                break;
            default:
                stateText = "SESION_ACTIVA";
                break;
            }

            Logger::Info("✓ SESIÓN ACTIVA DETECTADA - Estado: " + stateText + " | PlayerCarIdx: " + std::to_string(m_carIdx));
            UpdateDriverData();
            // Parsear YAML para nombres reales
            irsdkClient &client = irsdkClient::instance();
            ParseDriverInfoYAML(client.getSessionStr());
        }
        else
        {
            Logger::Info("⚠ No en sesión activa - usando datos mock (CarIdx=" + std::to_string(playerCarIdx) +
                         ", SessionState=" + std::to_string(sessionState) + ")");
            m_sessionDrivers.clear();
        }
    }
    catch (...)
    {
        m_inSession = false;
        m_sessionDrivers.clear();
        Logger::Warning("Error accediendo a variables SDK - fallback a mock data");
    }

    // Determinar tipo de sesión (por ahora, simplificado)
    m_currentSessionType = SessionType::PRACTICE; // Default
}

SessionType IRacingConnection::DetermineSessionType(const std::string &sessionInfo)
{
    return SessionInfoProvider::DetermineSessionType(sessionInfo);
}

DriverData IRacingConnection::GetDriverByCarIdx(int carIdx) const
{
    for (const auto &driver : m_sessionDrivers)
    {
        if (driver.carIdx == carIdx)
            return driver;
    }
    return DriverData{}; // Datos vacíos si no se encuentra
}

DriverData IRacingConnection::GetDriverByCustomerId(int customerId) const
{
    for (const auto &driver : m_sessionDrivers)
    {
        if (driver.customerId == customerId)
            return driver;
    }
    return DriverData{}; // Datos vacíos si no se encuentra
}

bool IRacingConnection::HasNewData() const
{
    irsdkClient &client = irsdkClient::instance();
    return client.isConnected();
}

void IRacingConnection::LogSessionInfo() const
{
    Logger::InfoF("Estado de sesión - Conectado: %s, En sesión: %s, Tipo: %s",
                  m_connected ? "Sí" : "No",
                  m_inSession ? "Sí" : "No",
                  m_currentSessionType == SessionType::PRACTICE ? "Practice" : m_currentSessionType == SessionType::QUALIFY ? "Qualify"
                                                                           : m_currentSessionType == SessionType::RACE      ? "Race"
                                                                                                                            : "Unknown");

    Logger::InfoF("Pilotos en sesión: %d, Car Idx jugador: %d",
                  static_cast<int>(m_sessionDrivers.size()), m_carIdx);
}

std::string IRacingConnection::GetSessionInfoString() const
{
    // Obtener el string de SessionInfo del SDK
    irsdkClient &client = irsdkClient::instance();
    return client.getSessionStr();
}

// Funciones estilo iRon para información extendida
std::string IRacingConnection::GetSessionType() const
{
    return SessionInfoProvider::GetSessionType();
}

std::string IRacingConnection::GetCurrentSessionInfo() const
{
    return SessionInfoProvider::GetCurrentSessionInfo(GetSessionInfoString());
}

float IRacingConnection::GetStrengthOfField() const
{
    return SessionInfoProvider::GetStrengthOfField(m_sessionDrivers);
}

std::vector<DriverData> IRacingConnection::GetExtendedDriverInfo() const
{
    // Return current drivers with all the extended info we now parse
    return m_sessionDrivers;
}
