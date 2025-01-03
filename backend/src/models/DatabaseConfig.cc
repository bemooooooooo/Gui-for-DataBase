#include "DatabaseConfig.h"
#include "Database.h"

namespace models {

DatabaseConfig::DatabaseConfig(const drogon::orm::Row& row) {
    id = row["id"].as<int>();
    userId = row["user_id"].as<int>();
    name = row["name"].as<std::string>();
    config = Json::parse(row["config"].as<std::string>());
    createdAt = row["created_at"].as<std::string>();
    updatedAt = row["updated_at"].as<std::string>();
}

std::optional<DatabaseConfig> DatabaseConfig::findById(int id) {
    auto db = Database::getDbClient();
    try {
        auto result = db->execSqlSync(
            "SELECT * FROM database_configs WHERE id = $1",
            id
        );
        if (result.size() > 0) {
            return DatabaseConfig(result[0]);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Error finding database config by id: " << e.what();
    }
    return std::nullopt;
}

std::vector<DatabaseConfig> DatabaseConfig::findByUserId(int userId) {
    auto db = Database::getDbClient();
    std::vector<DatabaseConfig> configs;
    try {
        auto result = db->execSqlSync(
            "SELECT * FROM database_configs WHERE user_id = $1 ORDER BY created_at DESC",
            userId
        );
        for (const auto& row : result) {
            configs.emplace_back(row);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Error finding database configs by user id: " << e.what();
    }
    return configs;
}

DatabaseConfig DatabaseConfig::create(int userId, const std::string& name, const Json::Value& config) {
    auto db = Database::getDbClient();
    Json::FastWriter writer;
    std::string configStr = writer.write(config);
    
    auto result = db->execSqlSync(
        "INSERT INTO database_configs (user_id, name, config) VALUES ($1, $2, $3::jsonb) RETURNING *",
        userId,
        name,
        configStr
    );
    return DatabaseConfig(result[0]);
}

bool DatabaseConfig::update(int id, const std::string& name, const Json::Value& config) {
    auto db = Database::getDbClient();
    try {
        Json::FastWriter writer;
        std::string configStr = writer.write(config);
        
        auto result = db->execSqlSync(
            "UPDATE database_configs SET name = $1, config = $2::jsonb, updated_at = CURRENT_TIMESTAMP "
            "WHERE id = $3",
            name,
            configStr,
            id
        );
        return result.affectedRows() > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "Error updating database config: " << e.what();
        return false;
    }
}

bool DatabaseConfig::remove(int id) {
    auto db = Database::getDbClient();
    try {
        auto result = db->execSqlSync(
            "DELETE FROM database_configs WHERE id = $1",
            id
        );
        return result.affectedRows() > 0;
    } catch (const std::exception& e) {
        LOG_ERROR << "Error removing database config: " << e.what();
        return false;
    }
}

Json::Value DatabaseConfig::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["user_id"] = userId;
    json["name"] = name;
    json["config"] = config;
    json["created_at"] = createdAt;
    json["updated_at"] = updatedAt;
    return json;
}

} // namespace models
