#pragma once
#include <string>
#include <json/json.h>
#include <drogon/orm/Result.h>
#include <optional>

namespace models {

class User {
public:
    User() = default;
    User(const drogon::orm::Row& row);

    int getId() const { return id; }
    const std::string& getUsername() const { return username; }
    const std::string& getEmail() const { return email; }
    const std::string& getPasswordHash() const { return passwordHash; }

    static std::optional<User> findByUsername(const std::string& username); // TODO #12
    static std::optional<User> findById(int id); // TODO #13
    static User create(const std::string& username, const std::string& email, const std::string& passwordHash); // TODO: #14 Create user in table
    static bool validatePassword(const std::string& password, const std::string& hash); // TODO: #15 validation of password in table

    Json::Value toJson() const; // TODO: #16 Convert user to JSON

private:
    int id;
    std::string username;
    std::string email;
    std::string passwordHash;
    std::string createdAt;
};

} // namespace models
