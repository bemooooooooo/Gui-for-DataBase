#include "User.h"
#include "Database.h"
#include <bcrypt/BCrypt.hpp>

namespace models {

User::User(const drogon::orm::Row& row) {
    id = row["id"].as<int>();
    username = row["username"].as<std::string>();
    email = row["email"].as<std::string>();
    passwordHash = row["password_hash"].as<std::string>();
    createdAt = row["created_at"].as<std::string>();
}

std::optional<User> User::findByUsername(const std::string& username) {
    auto db = Database::getDbClient();
    try {
        auto result = db->execSqlSync(
            "SELECT * FROM users WHERE username = $1",
            username
        );
        if (result.size() > 0) {
            return User(result[0]);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Error finding user by username: " << e.what();
    }
    return std::nullopt;
}

std::optional<User> User::findById(int id) {
    auto db = Database::getDbClient();
    try {
        auto result = db->execSqlSync(
            "SELECT * FROM users WHERE id = $1",
            id
        );
        if (result.size() > 0) {
            return User(result[0]);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Error finding user by id: " << e.what();
    }
    return std::nullopt;
}

User User::create(const std::string& username, const std::string& email, const std::string& passwordHash) {
    auto db = Database::getDbClient();
    auto result = db->execSqlSync(
        "INSERT INTO users (username, email, password_hash) VALUES ($1, $2, $3) RETURNING *",
        username,
        email,
        passwordHash
    );
    return User(result[0]);
}

bool User::validatePassword(const std::string& password, const std::string& hash) {
    return BCrypt::validatePassword(password, hash);
}

Json::Value User::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["username"] = username;
    json["email"] = email;
    json["created_at"] = createdAt;
    return json;
}

} // namespace models
