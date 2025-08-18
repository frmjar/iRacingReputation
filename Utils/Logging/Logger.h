/*
MIT License - iRacing Reputation System
Sistema de logging simple y eficiente
*/

#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdio>

// Definir constantes en lugar de enum class para evitar conflictos
#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define LOG_CRITICAL 4

class Logger
{
private:
    static std::mutex s_mutex;
    static std::ofstream s_logFile;
    static int s_minLevel;
    static bool s_initialized;

    static std::string GetCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static std::string LevelToString(int level)
    {
        switch (level)
        {
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_INFO:
            return "INFO";
        case LOG_WARNING:
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        case LOG_CRITICAL:
            return "CRIT";
        default:
            return "UNKNOWN";
        }
    }

    static void WriteLog(int level, const std::string &message)
    {
        if (level < s_minLevel)
            return;

        std::lock_guard<std::mutex> lock(s_mutex);

        // Evitar deadlock - no llamar Initialize desde WriteLog
        if (!s_initialized)
        {
            // Inicialización simple sin recursión
            s_logFile.open("iRacingReputation.log", std::ios::app);
            s_initialized = true;
        }

        std::string timestamp = GetCurrentTime();
        std::string levelStr = LevelToString(level);
        std::string logLine = "[" + timestamp + "] [" + levelStr + "] " + message;

        // Escribir a consola
        std::cout << logLine << std::endl;

        // Escribir a archivo
        if (s_logFile.is_open())
        {
            s_logFile << logLine << std::endl;
            s_logFile.flush();
        }
    }

public:
    static void Initialize(const std::string &filename = "iRacingReputation.log", int minLevel = LOG_INFO)
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        s_minLevel = minLevel;

        if (s_logFile.is_open())
            s_logFile.close();

        s_logFile.open(filename, std::ios::app);
        s_initialized = true;

        // Log inicial sin causar deadlock
        std::string timestamp = GetCurrentTime();
        std::string logLine = "[" + timestamp + "] [INFO] === iRacing Reputation System Started ===";
        std::cout << logLine << std::endl;
        if (s_logFile.is_open())
        {
            s_logFile << logLine << std::endl;
            s_logFile.flush();
        }
    }

    static void Shutdown()
    {
        std::lock_guard<std::mutex> lock(s_mutex);

        if (s_logFile.is_open())
        {
            // Log final sin causar deadlock
            std::string timestamp = GetCurrentTime();
            std::string logLine = "[" + timestamp + "] [INFO] === iRacing Reputation System Shutdown ===";
            std::cout << logLine << std::endl;
            s_logFile << logLine << std::endl;
            s_logFile.close();
        }

        s_initialized = false;
    }

    static void SetLevel(int level)
    {
        s_minLevel = level;
    }

    // Métodos públicos de logging
    static void Debug(const std::string &message)
    {
        WriteLog(LOG_DEBUG, message);
    }

    static void Info(const std::string &message)
    {
        WriteLog(LOG_INFO, message);
    }

    static void Warning(const std::string &message)
    {
        WriteLog(LOG_WARNING, message);
    }

    static void Error(const std::string &message)
    {
        WriteLog(LOG_ERROR, message);
    }

    static void Critical(const std::string &message)
    {
        WriteLog(LOG_CRITICAL, message);
    }

    // Métodos con formato
    template <typename... Args>
    static void DebugF(const std::string &format, Args... args)
    {
        char buffer[1024];
        sprintf(buffer, format.c_str(), args...);
        Debug(std::string(buffer));
    }

    template <typename... Args>
    static void InfoF(const std::string &format, Args... args)
    {
        char buffer[1024];
        sprintf(buffer, format.c_str(), args...);
        Info(std::string(buffer));
    }

    template <typename... Args>
    static void WarningF(const std::string &format, Args... args)
    {
        char buffer[1024];
        sprintf(buffer, format.c_str(), args...);
        Warning(std::string(buffer));
    }

    template <typename... Args>
    static void ErrorF(const std::string &format, Args... args)
    {
        char buffer[1024];
        sprintf(buffer, format.c_str(), args...);
        Error(std::string(buffer));
    }
};
