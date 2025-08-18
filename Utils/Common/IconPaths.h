/*
MIT License - iRacing Reputation System
Definiciones de rutas de iconos para comportamientos
*/

#pragma once

// Namespace para rutas de iconos PNG
namespace IconPaths
{
    constexpr const char *CLEAN_RACING = "Assets/Icons/clean_driver.png";
    constexpr const char *FAIR_PLAY = "Assets/Icons/good_racer.png";
    constexpr const char *AGGRESSIVE = "Assets/Icons/aggressive.png";
    constexpr const char *RECKLESS = "Assets/Icons/dirty_driver.png";
    constexpr const char *ROOKIE_FRIENDLY = "Assets/Icons/newbie.png";
    constexpr const char *PROFESSIONAL = "Assets/Icons/professional.png";
    constexpr const char *HELPFUL = "Assets/Icons/helpful.png";
    constexpr const char *FAST = "Assets/Icons/fast.png";
    constexpr const char *TEAM_PLAYER = "Assets/Icons/team_player.png";
    constexpr const char *INCONSISTENT = "Assets/Icons/inconsistent.png";
}

// Legacy defines para compatibilidad con código existente
#define ICON_CLEAN_DRIVER IconPaths::CLEAN_RACING
#define ICON_GOOD_RACER IconPaths::FAIR_PLAY
#define ICON_AGGRESSIVE IconPaths::AGGRESSIVE
#define ICON_DIRTY_DRIVER IconPaths::RECKLESS
#define ICON_RAMMER IconPaths::RECKLESS
#define ICON_BLOCKING IconPaths::AGGRESSIVE
#define ICON_UNSAFE_REJOIN IconPaths::RECKLESS
#define ICON_NEWBIE IconPaths::ROOKIE_FRIENDLY
