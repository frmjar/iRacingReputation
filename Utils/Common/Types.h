/*
MIT License - iRacing Reputation System
Tipos comunes y enumeraciones utilizadas en todo el sistema
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ctime>

// Forward declarations
struct ID3D11ShaderResourceView;

// Include ImGui for ImVec4
#include "../../External/ImGui/imgui.h"

// Enumeraciones principales
enum class ConnectionStatus
{
    DISCONNECTED = 0,
    CONNECTED,
    IN_SESSION
};

enum class SessionType
{
    UNKNOWN = 0,
    PRACTICE,
    QUALIFY,
    RACE
};

enum class DriverFlags
{
    UNKNOWN = 0,
    CLEAN_DRIVER = 1,   // Piloto limpio
    AGGRESSIVE = 2,     // Agresivo pero justo
    DIRTY_DRIVER = 4,   // Piloto sucio
    RAMMER = 8,         // Intencional contact
    BLOCKING = 16,      // Bloqueo excesivo
    UNSAFE_REJOIN = 32, // Reentradas peligrosas
    GOOD_RACER = 64,    // Buen piloto, recomendado
    NEWBIE = 128        // Piloto novato
};

enum class DriverTrustLevel
{
    AVOID = 0,   // Evitar este piloto
    CAUTION = 1, // Precaución
    NEUTRAL = 2, // Neutral
    TRUSTED = 3  // Confiable
};

// Estructura de datos de piloto
struct DriverData
{
    int carIdx = -1;
    int customerId = -1;
    std::string userName;
    std::string displayName;
    std::string carNumber;
    int iRating = 0;
    std::string licenseString;
    std::string licenseLevel = "D"; // Licencia como string
    float safetyRating = 0.0f;      // Safety Rating
    int position = 0;
    float lapDistPct = 0.0f;
    bool isPlayer = false;
    bool isValid = false;

    // Datos de proximidad
    float gapToPlayer = 999.0f;    // Gap en segundos
    float distanceToPlayer = 0.0f; // Distancia en metros
    bool isAhead = false;
};

// Estructura de reputación de piloto
struct DriverReputation
{
    int customerId = -1;
    std::string userName;
    DriverTrustLevel trustLevel = DriverTrustLevel::NEUTRAL;
    std::string notes; // Notas personales
    std::time_t lastUpdated = 0;
    uint32_t behaviorFlags = 0; // Combinación de DriverFlags flags (para compatibilidad)
    int encounterCount = 0;     // Número de veces que nos hemos encontrado
    std::string lastSeen;       // Última vez visto (ISO date)
    float trustScore = 0.5f;    // Puntuación de confianza (0.0 - 1.0)

    // Métodos helper
    bool HasBehavior(DriverFlags behavior) const
    {
        return (behaviorFlags & static_cast<uint32_t>(behavior)) != 0;
    }

    void AddBehavior(DriverFlags behavior)
    {
        behaviorFlags |= static_cast<uint32_t>(behavior);
    }

    void RemoveBehavior(DriverFlags behavior)
    {
        behaviorFlags &= ~static_cast<uint32_t>(behavior);
    }

    bool HasWarningFlags() const
    {
        uint32_t warningFlags = static_cast<uint32_t>(DriverFlags::DIRTY_DRIVER) |
                                static_cast<uint32_t>(DriverFlags::RAMMER) |
                                static_cast<uint32_t>(DriverFlags::BLOCKING) |
                                static_cast<uint32_t>(DriverFlags::UNSAFE_REJOIN);
        return (behaviorFlags & warningFlags) != 0;
    }

    bool IsPositive() const
    {
        return HasBehavior(DriverFlags::CLEAN_DRIVER) ||
               HasBehavior(DriverFlags::GOOD_RACER);
    }
};

// Estructura para warnings del overlay
struct ProximityWarning
{
    DriverData driver;
    DriverReputation reputation;
    float timeNearby = 0.0f; // Tiempo que lleva cerca (segundos)
    bool isActive = true;

    std::string GetWarningText() const
    {
        std::string text = driver.displayName + " (#" + driver.carNumber + ")";

        if (reputation.HasBehavior(DriverFlags::DIRTY_DRIVER))
            text += " - DIRTY DRIVER";
        else if (reputation.HasBehavior(DriverFlags::RAMMER))
            text += " - RAMMER";
        else if (reputation.HasBehavior(DriverFlags::AGGRESSIVE))
            text += " - AGGRESSIVE";
        else if (reputation.HasBehavior(DriverFlags::BLOCKING))
            text += " - BLOCKER";
        else if (reputation.HasBehavior(DriverFlags::UNSAFE_REJOIN))
            text += " - UNSAFE REJOINS";
        else if (reputation.HasBehavior(DriverFlags::NEWBIE))
            text += " - ROOKIE";

        return text;
    }

    uint32_t GetWarningColor() const
    {
        if (reputation.HasBehavior(DriverFlags::RAMMER))
            return 0xFF0000FF; // Rojo intenso
        else if (reputation.HasBehavior(DriverFlags::DIRTY_DRIVER))
            return 0xFF4444FF; // Rojo
        else if (reputation.HasBehavior(DriverFlags::AGGRESSIVE))
            return 0xFFAA00FF; // Naranja
        else if (reputation.HasBehavior(DriverFlags::BLOCKING))
            return 0xFFFF00FF; // Amarillo
        else if (reputation.HasBehavior(DriverFlags::NEWBIE))
            return 0x00FFFFFF; // Cyan

        return 0xFFFFFFFF; // Blanco por defecto
    }
};

// Utilidades
inline std::string DriverFlagsToString(DriverFlags behavior)
{
    switch (behavior)
    {
    case DriverFlags::CLEAN_DRIVER:
        return "Clean Driver";
    case DriverFlags::AGGRESSIVE:
        return "Aggressive";
    case DriverFlags::DIRTY_DRIVER:
        return "Dirty Driver";
    case DriverFlags::RAMMER:
        return "Rammer";
    case DriverFlags::BLOCKING:
        return "Blocking";
    case DriverFlags::UNSAFE_REJOIN:
        return "Unsafe Rejoin";
    case DriverFlags::GOOD_RACER:
        return "Good Racer";
    case DriverFlags::NEWBIE:
        return "Newbie";
    default:
        return "Unknown";
    }
}

// Forward declarations para evitar dependencias circulares
struct ID3D11ShaderResourceView;
namespace ImGui
{
    struct ImVec4;
}

// Estructura para información de tags de comportamiento
struct TagInfo
{
    DriverFlags behavior;
    const char *name;
    const char *iconPath;
    ID3D11ShaderResourceView *iconTexture = nullptr;
    ImVec4 color;
    const char *description;
};

using DriverList = std::vector<DriverData>;
using ProximityWarningList = std::vector<ProximityWarning>;
