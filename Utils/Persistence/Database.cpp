#include "Database.h"
#include "../../Utils/Logging/Logger.h"
#include <sqlite3.h>

Database::~Database()
{
    Close();
}

bool Database::Open(const std::string &path)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db)
        return true;
    int rc = sqlite3_open(path.c_str(), &m_db);
    if (rc != SQLITE_OK)
    {
        Logger::Error("SQLite open failed: " + std::string(sqlite3_errmsg(m_db)));
        return false;
    }
    Exec("PRAGMA journal_mode=WAL;");
    Exec("PRAGMA synchronous=NORMAL;");
    return true;
}

void Database::Close()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool Database::Exec(const std::string &sql)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        if (errMsg)
        {
            Logger::Error("SQLite exec error: " + std::string(errMsg));
            sqlite3_free(errMsg);
        }
        return false;
    }
    return true;
}

bool Database::Prepare(const std::string &sql, sqlite3_stmt **stmt)
{
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        Logger::Error("SQLite prepare failed: " + std::to_string(rc));
        return false;
    }
    return true;
}

bool Database::Step(sqlite3_stmt *stmt)
{
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
        return true; // caller reads columns
    if (rc == SQLITE_DONE)
        return false; // finished
    Logger::Error("SQLite step error: " + std::to_string(rc));
    return false;
}

void Database::Finalize(sqlite3_stmt *stmt)
{
    if (stmt)
        sqlite3_finalize(stmt);
}

long long Database::LastInsertId() const
{
    return sqlite3_last_insert_rowid(m_db);
}

int Database::Changes() const
{
    return sqlite3_changes(m_db);
}
