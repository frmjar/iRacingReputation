/*
MIT License - iRacing Reputation System
Implementación del sistema de logging
*/

#include "Logger.h"

// Definición de variables estáticas
std::mutex Logger::s_mutex;
std::ofstream Logger::s_logFile;
int Logger::s_minLevel = LOG_INFO;
bool Logger::s_initialized = false;
