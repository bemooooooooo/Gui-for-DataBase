#include "Database.h"
#include <drogon/drogon.h>

namespace models {

std::shared_ptr<drogon::orm::DbClient> Database::dbClient;

void Database::initDb(const std::string& connStr) {
    dbClient = drogon::orm::DbClient::newPgClient(connStr, 1);
    createTables();
}

std::shared_ptr<drogon::orm::DbClient> Database::getDbClient() {
    return dbClient;
}

void Database::createTables() {
    // Создание таблицы пользователей
    dbClient->execSqlSync(
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY,"
        "username VARCHAR(50) UNIQUE NOT NULL,"
        "email VARCHAR(100) UNIQUE NOT NULL,"
        "password_hash VARCHAR(255) NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
    );

    // Создание таблицы конфигураций баз данных
    dbClient->execSqlSync(
        "CREATE TABLE IF NOT EXISTS database_configs ("
        "id SERIAL PRIMARY KEY,"
        "user_id INTEGER REFERENCES users(id),"
        "name VARCHAR(100) NOT NULL,"
        "config JSONB NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
    );

    // Создание таблицы для хранения учетных данных серверов
    dbClient->execSqlSync(
        "CREATE TABLE IF NOT EXISTS server_credentials ("
        "id SERIAL PRIMARY KEY,"
        "user_id INTEGER REFERENCES users(id),"
        "host VARCHAR(255) NOT NULL,"
        "username VARCHAR(100) NOT NULL,"
        "encrypted_password VARCHAR(1000) NOT NULL,"
        "db_type VARCHAR(50) NOT NULL,"
        "port INTEGER,"
        "additional_config JSONB,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");"
    );
}

} // namespace models
