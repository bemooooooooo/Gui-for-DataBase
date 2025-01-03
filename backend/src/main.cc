#include <drogon/drogon.h>
#include "controllers/AuthController.h"
#include "filters/JwtAuthFilter.h"

int main() {
    // Настройка CORS
    drogon::app().registerHandler(
        "/api/{*}",
        [](const drogon::HttpRequestPtr& req,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type,Authorization");
            callback(resp);
        },
        {drogon::Options}
    );

    // Применяем JWT фильтр ко всем защищенным маршрутам
    drogon::app().registerFilter(std::make_shared<JwtAuthFilter>("/api/protected/*"));

    // Настройка сервера
    drogon::app().addListener("0.0.0.0", 8080)
        .setThreadNum(16)
        .enableRunAsDaemon()
        .run();
    
    return 0;
}
