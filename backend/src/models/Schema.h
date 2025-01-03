#pragma once
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <json/json.h>
#include <string>
#include <vector>

namespace models {

class Schema {
public:
    int id;
    int userId;
    std::string name;
    std::string description;
    Json::Value tables;
    Json::Value relations;
    std::string version;
    std::string createdAt;
    std::string updatedAt;

    static Schema fromJson(const Json::Value& json);
    Json::Value toJson() const;
    
    static Schema create(int userId, const std::string& name, 
                        const Json::Value& tables, 
                        const Json::Value& relations);
    static Schema findById(int id);
    static std::vector<Schema> findByUserId(int userId);
    void update();
    void remove();
    bool validate() const;
    std::string generateSql(const std::string& dbType) const;
    
private:
    std::string generatePostgresSql() const;
    std::string generateMySqlSql() const;
    bool validateTable(const Json::Value& table) const;
    bool validateColumn(const Json::Value& column) const;
    bool validateRelation(const Json::Value& relation) const;
};

} // namespace models