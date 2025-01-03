#include "DeploymentController.h"
#include "../models/DatabaseConfig.h"
#include "../models/User.h"
#include <libssh/libssh.h>
#include <sstream>
#include <memory>

void DeploymentController::saveConfig(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    // Получаем ID пользователя из JWT токена
    auto userId = req->getAttributes()->get<int>("user_id");
    
    std::string name = (*json)["name"].asString();
    Json::Value config = (*json)["config"];

    try {
        auto dbConfig = models::DatabaseConfig::create(userId, name, config);
        auto resp = HttpResponse::newHttpJsonResponse(dbConfig.toJson());
        callback(resp);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

void DeploymentController::getConfigs(const HttpRequestPtr& req,
                                    std::function<void(const HttpResponsePtr&)>&& callback) {
    auto userId = req->getAttributes()->get<int>("user_id");
    
    auto configs = models::DatabaseConfig::findByUserId(userId);
    
    Json::Value result(Json::arrayValue);
    for (const auto& config : configs) {
        result.append(config.toJson());
    }
    
    auto resp = HttpResponse::newHttpJsonResponse(result);
    callback(resp);
}

void DeploymentController::deployDatabase(const HttpRequestPtr& req,
                                        std::function<void(const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    int configId = (*json)["config_id"].asInt();
    std::string host = (*json)["host"].asString();
    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();
    int port = (*json)["port"].asInt();

    auto userId = req->getAttributes()->get<int>("user_id");
    auto config = models::DatabaseConfig::findById(configId);
    
    if (!config) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Configuration not found"));
        resp->setStatusCode(k404NotFound);
        callback(resp);
        return;
    }

    if (config->getUserId() != userId) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Access denied"));
        resp->setStatusCode(k403Forbidden);
        callback(resp);
        return;
    }

    try {
        // Создаем SSH сессию
        ssh_session ssh = ssh_new();
        if (ssh == nullptr) {
            throw std::runtime_error("Failed to create SSH session");
        }

        std::unique_ptr<ssh_session_struct, decltype(&ssh_free)> session(ssh, ssh_free);

        ssh_options_set(ssh, SSH_OPTIONS_HOST, host.c_str());
        ssh_options_set(ssh, SSH_OPTIONS_PORT, &port);
        ssh_options_set(ssh, SSH_OPTIONS_USER, username.c_str());

        int rc = ssh_connect(ssh);
        if (rc != SSH_OK) {
            throw std::runtime_error("Failed to connect to server");
        }

        rc = ssh_userauth_password(ssh, nullptr, password.c_str());
        if (rc != SSH_AUTH_SUCCESS) {
            throw std::runtime_error("Failed to authenticate");
        }

        // Генерируем и выполняем команды для развертывания базы данных
        std::string dbType = config->getConfig()["type"].asString();
        std::vector<std::string> commands = generateDeploymentCommands(dbType, config->getConfig());

        for (const auto& cmd : commands) {
            ssh_channel channel = ssh_channel_new(ssh);
            if (channel == nullptr) {
                throw std::runtime_error("Failed to create SSH channel");
            }

            rc = ssh_channel_open_session(channel);
            if (rc != SSH_OK) {
                ssh_channel_free(channel);
                throw std::runtime_error("Failed to open SSH channel");
            }

            rc = ssh_channel_request_exec(channel, cmd.c_str());
            if (rc != SSH_OK) {
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                throw std::runtime_error("Failed to execute command");
            }

            ssh_channel_close(channel);
            ssh_channel_free(channel);
        }

        Json::Value result;
        result["status"] = "success";
        result["message"] = "Database deployed successfully";
        
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
    } catch (const std::exception& e) {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

std::vector<std::string> DeploymentController::generateDeploymentCommands(
    const std::string& dbType,
    const Json::Value& config) {
    
    std::vector<std::string> commands;
    
    if (dbType == "postgresql") {
        commands = generatePostgresCommands(config);
    } else if (dbType == "mysql") {
        commands = generateMysqlCommands(config);
    } else if (dbType == "redis") {
        commands = generateRedisCommands(config);
    } else {
        throw std::runtime_error("Unsupported database type");
    }
    
    return commands;
}

std::vector<std::string> DeploymentController::generatePostgresCommands(const Json::Value& config) {
    std::vector<std::string> commands;
    
    // Установка PostgreSQL
    commands.push_back("sudo apt-get update");
    commands.push_back("sudo apt-get install -y postgresql postgresql-contrib");
    
    // Создание базы данных и пользователя
    std::string dbName = config["name"].asString();
    std::string dbUser = config["user"].asString();
    std::string dbPassword = config["password"].asString();
    
    commands.push_back("sudo -u postgres psql -c 'CREATE DATABASE " + dbName + ";'");
    commands.push_back("sudo -u postgres psql -c \"CREATE USER " + dbUser + " WITH PASSWORD '" + dbPassword + "';\"");
    commands.push_back("sudo -u postgres psql -c 'GRANT ALL PRIVILEGES ON DATABASE " + dbName + " TO " + dbUser + ";'");
    
    // Создание таблиц
    const Json::Value& tables = config["tables"];
    std::stringstream sqlStream;
    
    for (const auto& table : tables) {
        sqlStream << "CREATE TABLE " << table["name"].asString() << " (";
        
        const Json::Value& columns = table["columns"];
        bool first = true;
        
        for (const auto& column : columns) {
            if (!first) {
                sqlStream << ", ";
            }
            sqlStream << column["name"].asString() << " " << column["type"].asString();
            
            if (column["primary_key"].asBool()) {
                sqlStream << " PRIMARY KEY";
            }
            if (column["not_null"].asBool()) {
                sqlStream << " NOT NULL";
            }
            if (column.isMember("default")) {
                sqlStream << " DEFAULT " << column["default"].asString();
            }
            
            first = false;
        }
        
        sqlStream << ");";
    }
    
    commands.push_back("sudo -u postgres psql -d " + dbName + " -c '" + sqlStream.str() + "'");
    
    return commands;
}

std::vector<std::string> DeploymentController::generateMysqlCommands(const Json::Value& config) {
    std::vector<std::string> commands;
    
    // Установка MySQL
    commands.push_back("sudo apt-get update");
    commands.push_back("sudo apt-get install -y mysql-server");
    
    // Создание базы данных и пользователя
    std::string dbName = config["name"].asString();
    std::string dbUser = config["user"].asString();
    std::string dbPassword = config["password"].asString();
    
    commands.push_back("sudo mysql -e 'CREATE DATABASE " + dbName + ";'");
    commands.push_back("sudo mysql -e \"CREATE USER '" + dbUser + "'@'localhost' IDENTIFIED BY '" + dbPassword + "';\"");
    commands.push_back("sudo mysql -e 'GRANT ALL PRIVILEGES ON " + dbName + ".* TO '" + dbUser + "'@'localhost';'");
    commands.push_back("sudo mysql -e 'FLUSH PRIVILEGES;'");
    
    // Создание таблиц
    const Json::Value& tables = config["tables"];
    std::stringstream sqlStream;
    
    for (const auto& table : tables) {
        sqlStream << "CREATE TABLE " << table["name"].asString() << " (";
        
        const Json::Value& columns = table["columns"];
        bool first = true;
        
        for (const auto& column : columns) {
            if (!first) {
                sqlStream << ", ";
            }
            sqlStream << column["name"].asString() << " " << column["type"].asString();
            
            if (column["primary_key"].asBool()) {
                sqlStream << " PRIMARY KEY";
            }
            if (column["not_null"].asBool()) {
                sqlStream << " NOT NULL";
            }
            if (column.isMember("default")) {
                sqlStream << " DEFAULT " << column["default"].asString();
            }
            
            first = false;
        }
        
        sqlStream << ");";
    }
    
    commands.push_back("sudo mysql " + dbName + " -e '" + sqlStream.str() + "'");
    
    return commands;
}

std::vector<std::string> DeploymentController::generateRedisCommands(const Json::Value& config) {
    std::vector<std::string> commands;
    
    // Установка Redis
    commands.push_back("sudo apt-get update");
    commands.push_back("sudo apt-get install -y redis-server");
    
    // Настройка Redis
    std::string password = config["password"].asString();
    std::string port = config["port"].asString();
    std::string maxMemory = config["max_memory"].asString();
    
    commands.push_back("sudo sed -i 's/# requirepass foobared/requirepass " + password + "/g' /etc/redis/redis.conf");
    commands.push_back("sudo sed -i 's/port 6379/port " + port + "/g' /etc/redis/redis.conf");
    commands.push_back("sudo sed -i 's/# maxmemory <bytes>/maxmemory " + maxMemory + "mb/g' /etc/redis/redis.conf");
    
    // Перезапуск Redis
    commands.push_back("sudo systemctl restart redis-server");
    
    return commands;
}
