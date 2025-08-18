#include "ReputationRepository.h"
#include "../../Utils/Logging/Logger.h"
#include <sqlite3.h>

bool ReputationRepository::Init(Database &db)
{
    return EnsureSchema(db);
}

bool ReputationRepository::EnsureSchema(Database &db)
{
    const char *sql = R"(CREATE TABLE IF NOT EXISTS driver_reputation (
        customer_id INTEGER PRIMARY KEY,
        user_name TEXT,
        behavior_flags INTEGER NOT NULL DEFAULT 0,
        trust_level INTEGER NOT NULL DEFAULT 2,
        notes TEXT,
        encounter_count INTEGER NOT NULL DEFAULT 0,
        last_seen TEXT,
        last_updated INTEGER,
        trust_score REAL NOT NULL DEFAULT 0.5
    );)";
    return db.Exec(sql);
}

bool ReputationRepository::LoadAll(Database &db, std::map<int, DriverReputation> &out)
{
    sqlite3_stmt *stmt = nullptr;
    if (!db.Prepare("SELECT customer_id,user_name,behavior_flags,trust_level,notes,encounter_count,last_seen,last_updated,trust_score FROM driver_reputation", &stmt))
        return false;
    while (true)
    {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            DriverReputation rep;
            rep.customerId = sqlite3_column_int(stmt, 0);
            rep.userName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            rep.behaviorFlags = (uint32_t)sqlite3_column_int(stmt, 2);
            rep.trustLevel = (DriverTrustLevel)sqlite3_column_int(stmt, 3);
            const unsigned char *notesTxt = sqlite3_column_text(stmt, 4);
            if (notesTxt)
                rep.notes = reinterpret_cast<const char *>(notesTxt);
            rep.encounterCount = sqlite3_column_int(stmt, 5);
            const unsigned char *lastSeenTxt = sqlite3_column_text(stmt, 6);
            if (lastSeenTxt)
                rep.lastSeen = reinterpret_cast<const char *>(lastSeenTxt);
            rep.lastUpdated = (std::time_t)sqlite3_column_int64(stmt, 7);
            rep.trustScore = (float)sqlite3_column_double(stmt, 8);
            // reconstruir flags convenience
            if (rep.behaviorFlags & (uint32_t)DriverFlags::DIRTY_DRIVER || rep.behaviorFlags & (uint32_t)DriverFlags::RAMMER)
                rep.trustLevel = DriverTrustLevel::AVOID;
            else if (rep.behaviorFlags & (uint32_t)DriverFlags::AGGRESSIVE || rep.behaviorFlags & (uint32_t)DriverFlags::BLOCKING || rep.behaviorFlags & (uint32_t)DriverFlags::UNSAFE_REJOIN)
                rep.trustLevel = DriverTrustLevel::CAUTION;
            else if (rep.behaviorFlags & (uint32_t)DriverFlags::CLEAN_DRIVER || rep.behaviorFlags & (uint32_t)DriverFlags::GOOD_RACER)
                rep.trustLevel = DriverTrustLevel::TRUSTED;
            out[rep.customerId] = rep;
        }
        else if (rc == SQLITE_DONE)
            break;
        else
        {
            Logger::Error("SQLite step error load all");
            sqlite3_finalize(stmt);
            return false;
        }
    }
    sqlite3_finalize(stmt);
    Logger::Info("Reputaciones cargadas desde SQLite: " + std::to_string(out.size()));
    return true;
}

bool ReputationRepository::Upsert(Database &db, const DriverReputation &rep)
{
    const char *sql = R"(INSERT INTO driver_reputation (customer_id,user_name,behavior_flags,trust_level,notes,encounter_count,last_seen,last_updated,trust_score)
        VALUES (?,?,?,?,?,?,?,?,?)
        ON CONFLICT(customer_id) DO UPDATE SET
          user_name=excluded.user_name,
          behavior_flags=excluded.behavior_flags,
          trust_level=excluded.trust_level,
          notes=excluded.notes,
          encounter_count=excluded.encounter_count,
          last_seen=excluded.last_seen,
          last_updated=excluded.last_updated,
          trust_score=excluded.trust_score; )";
    sqlite3_stmt *stmt = nullptr;
    if (!db.Prepare(sql, &stmt))
        return false;
    sqlite3_bind_int(stmt, 0 + 1, rep.customerId);
    sqlite3_bind_text(stmt, 1 + 1, rep.userName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2 + 1, (int)rep.behaviorFlags);
    sqlite3_bind_int(stmt, 3 + 1, (int)rep.trustLevel);
    sqlite3_bind_text(stmt, 4 + 1, rep.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5 + 1, rep.encounterCount);
    sqlite3_bind_text(stmt, 6 + 1, rep.lastSeen.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 7 + 1, (sqlite3_int64)rep.lastUpdated);
    sqlite3_bind_double(stmt, 8 + 1, (double)rep.trustScore);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        Logger::Error("SQLite upsert error: " + std::to_string(rc));
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}
