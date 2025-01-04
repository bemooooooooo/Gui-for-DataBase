#pragma once
#include <drogon/HttpController.h>
#include "../models/Schema.h"
#include <json/json.h>

using namespace drogon;

class SchemaController : public drogon::HttpController<SchemaController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(SchemaController::createSchema, "/api/protected/schemas", Post);
        ADD_METHOD_TO(SchemaController::getSchemas, "/api/protected/schemas", Get);
        ADD_METHOD_TO(SchemaController::getSchema, "/api/protected/schemas/{id}", Get);
        ADD_METHOD_TO(SchemaController::updateSchema, "/api/protected/schemas/{id}", Put);
        ADD_METHOD_TO(SchemaController::deleteSchema, "/api/protected/schemas/{id}", Delete);
        ADD_METHOD_TO(SchemaController::validateSchema, "/api/protected/schemas/validate", Post);
        ADD_METHOD_TO(SchemaController::generateSql, "/api/protected/schemas/{id}/sql", Get);
    METHOD_LIST_END

    void createSchema(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void getSchemas(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void getSchema(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void updateSchema(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void deleteSchema(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
    void validateSchema(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback); // TODO #1
    void generateSql(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
};