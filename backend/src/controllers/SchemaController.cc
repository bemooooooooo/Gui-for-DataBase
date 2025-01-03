#include "SchemaController.h"
#include <json/json.h>

void SchemaController::createSchema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    auto userId = req->getAttributes()->get<int>("user_id");
    
    try {
        auto schema = models::Schema::create(
            userId,
            (*json)["name"].asString(),
            (*json)["tables"],
            (*json)["relations"]
        );
        
        if (!schema.validate()) {
            auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid schema"));
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }
        
        auto resp = HttpResponse::newHttpJsonResponse(schema.toJson());
        callback(resp);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void SchemaController::getSchemas(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    
    auto userId = req->getAttributes()->get<int>("user_id");
    
    try {
        auto schemas = models::Schema::findByUserId(userId);
        Json::Value result(Json::arrayValue);
        for (const auto& schema : schemas) {
            result.append(schema.toJson());
        }
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void SchemaController::generateSql(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    
    try {
        auto id = std::stoi(req->getParameter("id"));
        auto dbType = req->getParameter("type");
        
        auto schema = models::Schema::findById(id);
        auto sql = schema.generateSql(dbType);
        
        Json::Value result;
        result["sql"] = sql;
        
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

// Реализация остальных методов...