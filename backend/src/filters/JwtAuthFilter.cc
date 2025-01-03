#include "JwtAuthFilter.h"
#include <json/json.h>

const std::string JwtAuthFilter::JWT_SECRET = "your-secret-key"; // Должен совпадать с секретом в AuthController

void JwtAuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                           drogon::FilterCallback&& fcb,
                           drogon::FilterChainCallback&& fccb) {
    try {
        // Получаем токен из заголовка
        auto auth = req->getHeader("Authorization");
        if (auth.empty() || auth.find("Bearer ") != 0) {
            Json::Value result;
            result["message"] = "No token provided";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(result);
            resp->setStatusCode(drogon::k401Unauthorized);
            fcb(resp);
            return;
        }

        std::string token = auth.substr(7); // Пропускаем "Bearer "
        
        // Проверяем токен
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
            .with_issuer("auth0");
            
        auto decoded = jwt::decode(token);
        verifier.verify(decoded);
        
        // Добавляем данные пользователя в запрос для дальнейшего использования
        auto userId = decoded.get_payload_claim("user_id").as_string();
        auto username = decoded.get_payload_claim("username").as_string();
        
        req->setParameter("userId", userId);
        req->setParameter("username", username);
        
        // Продолжаем цепочку фильтров
        fccb();
    }
    catch (const std::exception& e) {
        Json::Value result;
        result["message"] = "Invalid token";
        result["error"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
    }
}
