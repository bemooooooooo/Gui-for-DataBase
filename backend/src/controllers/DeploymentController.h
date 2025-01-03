#pragma once
#include <drogon/HttpController.h>
#include <json/json.h>
#include <vector>

using namespace drogon;

class DeploymentController : public drogon::HttpController<DeploymentController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(DeploymentController::saveConfig, "/api/protected/config", Post);
    ADD_METHOD_TO(DeploymentController::getConfigs, "/api/protected/configs", Get);
    ADD_METHOD_TO(DeploymentController::deployDatabase, "/api/protected/deploy", Post);
    METHOD_LIST_END

    void saveConfig(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);
    void getConfigs(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);
    void deployDatabase(const HttpRequestPtr& req,
                       std::function<void(const HttpResponsePtr&)>&& callback);

private:
    std::vector<std::string> generateDeploymentCommands(const std::string& dbType,
                                                       const Json::Value& config);
    std::vector<std::string> generatePostgresCommands(const Json::Value& config);
    std::vector<std::string> generateMysqlCommands(const Json::Value& config);
    std::vector<std::string> generateRedisCommands(const Json::Value& config);
};
