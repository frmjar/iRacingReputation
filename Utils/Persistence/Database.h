#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

// Forward declare sqlite3 (amalgamación se añadirá como sqlite3.c / sqlite3.h)
struct sqlite3;
struct sqlite3_stmt;

class Database
{
public:
    Database() = default;
    ~Database();
    bool Open(const std::string &path);
    void Close();
    bool Exec(const std::string &sql);
    bool Prepare(const std::string &sql, sqlite3_stmt **stmt);
    bool Step(sqlite3_stmt *stmt);
    void Finalize(sqlite3_stmt *stmt);
    long long LastInsertId() const;
    int Changes() const;

private:
    sqlite3 *m_db = nullptr;
    std::mutex m_mutex; // simple serialización
};
