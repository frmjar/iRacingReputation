/*
MIT License - iRacing Reputation System
Detector de proximidad entre pilotos
*/

#pragma once

#include "../IRacingSDK/IRacingConnection.h"
#include "../../Utils/Common/Types.h"
#include "../../Utils/Logging/Logger.h"
#include <vector>
#include <algorithm>
#include <cmath>

class ProximityDetector
{
private:
    IRacingConnection *m_iracingConnection;
    float m_proximityThreshold; // Gap en segundos
    float m_distanceThreshold;  // Distancia en metros (opcional)

    // Calcular distancia aproximada basada en el gap de tiempo
    float CalculateApproximateDistance(float timeGap, float avgSpeed = 50.0f) const
    {
        // Estimación simple: distancia = tiempo * velocidad promedio
        // avgSpeed en m/s (50 m/s ≈ 180 km/h, velocidad típica en circuito)
        return std::abs(timeGap) * avgSpeed;
    }

    // Verificar si un piloto está dentro del rango de proximidad
    bool IsDriverNearby(const DriverData &driver, const DriverData &player) const
    {
        if (!driver.isValid || !player.isValid || driver.isPlayer)
            return false;

        // Verificar gap de tiempo
        float absGap = std::abs(driver.gapToPlayer);
        if (absGap > m_proximityThreshold)
            return false;

        // Verificación adicional: estar en vueltas similares o misma vuelta
        // (para evitar que pilotos doblados aparezcan como "cercanos")
        // Esto se puede implementar cuando tengamos datos de vuelta

        return true;
    }

public:
    ProximityDetector(IRacingConnection *iracingConnection, float proximityThreshold = 2.0f)
        : m_iracingConnection(iracingConnection), m_proximityThreshold(proximityThreshold), m_distanceThreshold(100.0f) // 100 metros por defecto
    {
        Logger::InfoF("ProximityDetector inicializado con threshold: %.1f segundos", proximityThreshold);
    }

    void SetProximityThreshold(float seconds)
    {
        m_proximityThreshold = seconds;
        Logger::InfoF("Threshold de proximidad actualizado a: %.1f segundos", seconds);
    }

    void SetDistanceThreshold(float meters)
    {
        m_distanceThreshold = meters;
        Logger::InfoF("Threshold de distancia actualizado a: %.1f metros", meters);
    }

    float GetProximityThreshold() const { return m_proximityThreshold; }
    float GetDistanceThreshold() const { return m_distanceThreshold; }

    // Obtener todos los pilotos cerca del jugador
    std::vector<DriverData> GetNearbyDrivers() const
    {
        std::vector<DriverData> nearbyDrivers;

        if (!m_iracingConnection || !m_iracingConnection->IsConnected())
            return nearbyDrivers;

        const auto &allDrivers = m_iracingConnection->GetSessionDrivers();
        const auto &playerData = m_iracingConnection->GetPlayerData();

        if (!playerData.isValid)
            return nearbyDrivers;

        for (const auto &driver : allDrivers)
        {
            if (IsDriverNearby(driver, playerData))
            {
                nearbyDrivers.push_back(driver);
            }
        }

        // Ordenar por proximidad (más cerca primero)
        std::sort(nearbyDrivers.begin(), nearbyDrivers.end(),
                  [](const DriverData &a, const DriverData &b)
                  {
                      return std::abs(a.gapToPlayer) < std::abs(b.gapToPlayer);
                  });

        return nearbyDrivers;
    }

    // Obtener el piloto más cercano
    DriverData GetClosestDriver() const
    {
        auto nearbyDrivers = GetNearbyDrivers();
        return nearbyDrivers.empty() ? DriverData{} : nearbyDrivers[0];
    }

    // Obtener pilotos cerca por delante y por detrás por separado
    std::pair<std::vector<DriverData>, std::vector<DriverData>> GetNearbyDriversSeparated() const
    {
        std::vector<DriverData> driversAhead;
        std::vector<DriverData> driversBehind;

        auto nearbyDrivers = GetNearbyDrivers();

        for (const auto &driver : nearbyDrivers)
        {
            if (driver.isAhead)
                driversAhead.push_back(driver);
            else
                driversBehind.push_back(driver);
        }

        return {driversAhead, driversBehind};
    }

    // Verificar si un piloto específico está cerca
    bool IsDriverNearby(int customerId) const
    {
        if (!m_iracingConnection || !m_iracingConnection->IsConnected())
            return false;

        const DriverData driver = m_iracingConnection->GetDriverByCustomerId(customerId);
        if (!driver.isValid)
            return false;

        const auto &playerData = m_iracingConnection->GetPlayerData();
        return IsDriverNearby(driver, playerData);
    }

    // Obtener información detallada de proximidad
    struct ProximityInfo
    {
        bool isNearby = false;
        float timeGap = 999.0f;
        float estimatedDistance = 999.0f;
        bool isAhead = false;
        std::string description;
    };

    ProximityInfo GetProximityInfo(int customerId) const
    {
        ProximityInfo info;

        if (!m_iracingConnection || !m_iracingConnection->IsConnected())
        {
            info.description = "No conectado a iRacing";
            return info;
        }

        const DriverData driver = m_iracingConnection->GetDriverByCustomerId(customerId);
        if (!driver.isValid)
        {
            info.description = "Piloto no encontrado en sesión";
            return info;
        }

        const auto &playerData = m_iracingConnection->GetPlayerData();
        if (!playerData.isValid)
        {
            info.description = "Datos del jugador no válidos";
            return info;
        }

        info.timeGap = driver.gapToPlayer;
        info.estimatedDistance = CalculateApproximateDistance(info.timeGap);
        info.isAhead = driver.isAhead;
        info.isNearby = IsDriverNearby(driver, playerData);

        // Crear descripción
        char desc[256];
        if (info.isNearby)
        {
            snprintf(desc, sizeof(desc), "%.1fs %s (≈%.0fm)",
                     std::abs(info.timeGap),
                     info.isAhead ? "adelante" : "atrás",
                     info.estimatedDistance);
        }
        else
        {
            snprintf(desc, sizeof(desc), "%.1fs %s - fuera de rango",
                     std::abs(info.timeGap),
                     info.isAhead ? "adelante" : "atrás");
        }
        info.description = desc;

        return info;
    }

    // Estadísticas de proximidad
    struct ProximityStats
    {
        int totalNearbyDrivers = 0;
        int driversAhead = 0;
        int driversBehind = 0;
        float closestGap = 999.0f;
        int closestDriverId = -1;
    };

    ProximityStats GetProximityStats() const
    {
        ProximityStats stats;
        auto nearbyDrivers = GetNearbyDrivers();

        stats.totalNearbyDrivers = static_cast<int>(nearbyDrivers.size());

        for (const auto &driver : nearbyDrivers)
        {
            if (driver.isAhead)
                stats.driversAhead++;
            else
                stats.driversBehind++;

            float absGap = std::abs(driver.gapToPlayer);
            if (absGap < stats.closestGap)
            {
                stats.closestGap = absGap;
                stats.closestDriverId = driver.customerId;
            }
        }

        return stats;
    }
};
