#pragma once
#include <drogon/orm/DbClient.h>
#include <memory>

namespace models {

class Database {
public:
    static void initDb(const std::string& connStr);
    static std::shared_ptr<drogon::orm::DbClient> getDbClient();

private:
    static std::shared_ptr<drogon::orm::DbClient> dbClient;
    static void createTables();
};

} // namespace models
