#pragma once
#include <drogon/HttpController.h>
#include <jwt-cpp/jwt.h>
#include <string>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::login, "/api/auth/login", Post);
        ADD_METHOD_TO(AuthController::register, "/api/auth/register", Post);
    METHOD_LIST_END

    void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void reg(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

private:
    std::string generateToken(const std::string& userId, const std::string& username);
    static const std::string JWT_SECRET;
};
