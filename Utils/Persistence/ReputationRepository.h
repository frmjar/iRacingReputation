#pragma once
#include <string>
#include <map>
#include "../../Utils/Common/Types.h"
#include "Database.h"

class ReputationRepository
{
public:
    bool Init(Database &db);
    bool LoadAll(Database &db, std::map<int, DriverReputation> &out);
    bool Upsert(Database &db, const DriverReputation &rep);

private:
    bool EnsureSchema(Database &db);
};
