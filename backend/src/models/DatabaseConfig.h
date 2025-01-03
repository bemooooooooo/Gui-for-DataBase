#pragma once
#include <string>
#include <json/json.h>
#include <drogon/orm/Result.h>
#include <optional>

namespace models {

class DatabaseConfig {
public:
    DatabaseConfig() = default;
    DatabaseConfig(const drogon::orm::Row& row);

    int getId() const { return id; }
    int getUserId() const { return userId; }
    const std::string& getName() const { return name; }
    const Json::Value& getConfig() const { return config; }

    static std::optional<DatabaseConfig> findById(int id);
    static std::vector<DatabaseConfig> findByUserId(int userId);
    static DatabaseConfig create(int userId, const std::string& name, const Json::Value& config);
    static bool update(int id, const std::string& name, const Json::Value& config);
    static bool remove(int id);

    Json::Value toJson() const;

private:
    int id;
    int userId;
    std::string name;
    Json::Value config;
    std::string createdAt;
    std::string updatedAt;
};

} // namespace models
