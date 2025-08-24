#include "DriverTagWindow.h"

DriverReputation &DriverTagWindow::GetOrCreateReputation(int customerId, const std::string &userName)
{
    auto it = m_driverReputations.find(customerId);
    if (it == m_driverReputations.end())
    {
        DriverReputation newRep;
        newRep.customerId = customerId;
        newRep.userName = userName;
        newRep.behaviorFlags = static_cast<uint32_t>(DriverFlags::UNKNOWN);
        newRep.trustLevel = DriverTrustLevel::NEUTRAL;
        newRep.notes = "";
        newRep.lastUpdated = std::time(nullptr);
        newRep.lastSeen = "";
        m_driverReputations[customerId] = newRep;
        return m_driverReputations[customerId];
    }
    return it->second;
}

bool DriverTagWindow::InitPersistence()
{
    if (m_persistenceInitialized)
        return true;
    try
    {
        char modulePath[MAX_PATH];
        if (GetModuleFileNameA(nullptr, modulePath, MAX_PATH) == 0)
            return false;
        std::filesystem::path exePath(modulePath);
        auto baseDir = exePath.parent_path();
        auto dbPath = baseDir / "reputation.db";
        if (!m_db.Open(dbPath.string()))
        {
            Logger::Error("No se pudo abrir/crear la base de datos: " + dbPath.string());
            return false;
        }
        if (!m_repo.Init(m_db))
        {
            Logger::Error("No se pudo inicializar el repositorio de reputaciones");
            return false;
        }
        if (!m_repo.LoadAll(m_db, m_driverReputations))
        {
            Logger::Warning("No se pudieron cargar reputaciones existentes (continuando vacío)");
        }
        m_persistenceInitialized = true;
        m_lastFlush = std::time(nullptr);
        return true;
    }
    catch (const std::exception &ex)
    {
        Logger::Error(std::string("Excepción inicializando persistencia: ") + ex.what());
        return false;
    }
}

void DriverTagWindow::MarkDirty(int customerId)
{
    if (!m_persistenceInitialized)
        return;
    if (std::find(m_dirtyIds.begin(), m_dirtyIds.end(), customerId) == m_dirtyIds.end())
    {
        m_dirtyIds.push_back(customerId);
    }
    m_reputationsDirty = true;
}

void DriverTagWindow::FlushDirty(bool force)
{
    if (!m_persistenceInitialized || !m_reputationsDirty)
        return;
    const int debounceSeconds = 3;
    std::time_t now = std::time(nullptr);
    if (!force && (now - m_lastFlush) < debounceSeconds)
        return;
    int flushed = 0;
    for (int id : m_dirtyIds)
    {
        auto it = m_driverReputations.find(id);
        if (it == m_driverReputations.end())
            continue;
        it->second.lastUpdated = now;
        if (it->second.lastSeen.empty())
        {
            std::time_t t = now;
            std::tm tmStruct;
#if defined(_WIN32)
            localtime_s(&tmStruct, &t);
#else
            localtime_r(&t, &tmStruct);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tmStruct);
            it->second.lastSeen = buf;
        }
        if (!m_repo.Upsert(m_db, it->second))
        {
            Logger::Warning("Fallo guardando reputación para id=" + std::to_string(id));
        }
        else
        {
            flushed++;
        }
    }
    m_dirtyIds.clear();
    m_reputationsDirty = false;
    m_lastFlush = now;
    if (flushed > 0)
        Logger::Info("Reputaciones guardadas: " + std::to_string(flushed));
}
