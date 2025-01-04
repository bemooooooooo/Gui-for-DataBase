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
    
    // TODO #2:
    static Schema fromJson(const Json::Value& json);
    // TODO #3:
    Json::Value toJson() const;
    
    static Schema create(int userId, const std::string& name, 
                        const Json::Value& tables, 
                        const Json::Value& relations); // TODO #4
    static Schema findById(int id); // TODO #5
    static std::vector<Schema> findByUserId(int userId); // TODO #6
    void update(const Json::Value& json); // TODO: #7 Обновление повходящему JSON
    void remove(); // TODO: #8 Удаление из таблицы, а не объекта.
    bool validate() const; // TODO #9:
    std::string generateSql(const std::string& dbType) const; // TODO #10
    
private:
    // <TODO #11
    std::string generatePostgresSql() const;
    std::string generateMySqlSql() const;
    bool validateTable(const Json::Value& table) const;
    bool validateColumn(const Json::Value& column) const;
    bool validateRelation(const Json::Value& relation) const;
    // !TODO>
};

} // namespace models