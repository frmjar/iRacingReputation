#pragma once

#include <vector>
#include <string>
#include <chrono>

#include "irsdk_defines.h"
#include "irsdk_client.h"
#include "yaml_parser.h"
#include "IRacingVariables.h"
#include "../../Utils/Common/Types.h"
#include "../../Utils/Logging/Logger.h"
#include "../../Utils/IRacing/StringUtils.h"
#include "../../Utils/IRacing/YAMLDriverParser.h"
#include "../../Utils/IRacing/SessionInfoProvider.h"

#define IR_MAX_CARS 64

/**
 * @brief Clase principal para gestionar la conexión con iRacing
 *
 * Esta clase integra las mejores prácticas de iRon para una conexión
 * más limpia y eficiente con el SDK de iRacing.
 */
class IRacingConnection
{
public:
    // Constructor y destructor
    IRacingConnection() = default;
    ~IRacingConnection();

    // Gestión de conexión
    bool Initialize();
    void Shutdown();
    ConnectionStatus Update();

    // Estado de conexión
    bool IsConnected() const { return m_connected; }
    bool IsInSession() const { return m_inSession; }
    bool IsInitialized() const { return m_initialized; }

    // Datos de sesión
    const std::vector<DriverData> &GetSessionDrivers() const { return m_sessionDrivers; }
    int GetPlayerCarIdx() const { return m_carIdx; }
    const DriverData &GetPlayerData() const { return m_playerData; }
    SessionType GetCurrentSessionType() const { return m_currentSessionType; }

    // Búsqueda de pilotos
    DriverData GetDriverByCarIdx(int carIdx) const;
    DriverData GetDriverByCustomerId(int customerId) const;

    // Métodos de información
    bool HasNewData() const;
    void LogSessionInfo() const;
    std::string GetSessionInfoString() const;

    // Funciones para compatibilidad con iRon
    std::string GetSessionType() const;
    std::string GetCurrentSessionInfo() const;
    float GetStrengthOfField() const;
    std::vector<DriverData> GetExtendedDriverInfo() const;

private:
    // Estado de conexión
    bool m_initialized = false;
    bool m_connected = false;
    bool m_inSession = false;

    // Control de actualizaciones
    int m_lastStatusID = -1;
    std::chrono::steady_clock::time_point m_lastUpdateTime;

    // Datos de sesión
    std::vector<DriverData> m_sessionDrivers;
    DriverData m_playerData;
    int m_carIdx = -1;
    SessionType m_currentSessionType = SessionType::UNKNOWN;

    // Métodos privados
    void ParseSessionInfo();
    void UpdateDriverData();
    void CalculateGapsToPlayer();
    void ParseDriverInfoYAML(const std::string &yaml);
    SessionType DetermineSessionType(const std::string &sessionInfo);
};
