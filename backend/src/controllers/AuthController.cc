#include "AuthController.h"
#include "../models/User.h"
#include <bcrypt/BCrypt.hpp>
#include <chrono>

const std::string AuthController::JWT_SECRET = "your-secret-key"; // В production использовать безопасный ключ

void AuthController::login(const HttpRequestPtr& req, 
                         std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    auto user = models::User::findByUsername(username);
    if (!user || !models::User::validatePassword(password, user->getPasswordHash())) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid credentials"));
        resp->setStatusCode(k401Unauthorized);
        callback(resp);
        return;
    }

    std::string token = generateToken(std::to_string(user->getId()), user->getUsername());
    
    Json::Value result;
    result["token"] = token;
    result["user"] = user->toJson();
    
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void AuthController::reg(const HttpRequestPtr& req,
                            std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();
    std::string email = (*json)["email"].asString();

    // Проверяем существование пользователя
    if (models::User::findByUsername(username)) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Username already exists"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Хэшируем пароль
    std::string hashedPassword = BCrypt::generateHash(password);
    
    // Создаем пользователя
    auto user = models::User::create(username, email, hashedPassword);
    
    // Генерируем токен
    std::string token = generateToken(std::to_string(user.getId()), user.getUsername());
    
    Json::Value result;
    result["token"] = token;
    result["user"] = user.toJson();
    
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

std::string AuthController::generateToken(const std::string& userId, const std::string& username) {
    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
        .set_payload_claim("user_id", jwt::claim(userId))
        .set_payload_claim("username", jwt::claim(username))
        .sign(jwt::algorithm::hs256{JWT_SECRET});
    
    return token;
}
