#pragma once
#include <drogon/orm/DbClient.h>
#include <memory>
#include <exception>

namespace models {

class Database {
public:
    static void initDb(const std::string& connStr); // TODO: #18 initialization database on PostgreSQL
    static std::shared_ptr<drogon::orm::DbClient> getDbClient(); // TODO: #19 ??? Разобраться надо

private:
    static std::shared_ptr<drogon::orm::DbClient> dbClient;
    static void createTables(); // TODO: #20 create Tables, static function...
};

} // namespace models
