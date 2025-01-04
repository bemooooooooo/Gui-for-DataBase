#include "SchemaController.h"

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

  int userId = req->getAttributes()->get<int>("user_id");

  try {
    models::Schema schema =
        models::Schema::create(userId, (*json)["name"].asString(),
                               (*json)["tables"], (*json)["relations"]);

    if (!schema.validate()) {
      drogon::HttpResponsePtr resp =
          HttpResponse::newHttpJsonResponse(Json::Value("Invalid schema"));
      resp->setStatusCode(k400BadRequest);
      callback(resp);
      return;
    }
    callback(HttpResponse::newHttpJsonResponse(schema.toJson()));
  } catch (const std::exception& e) {
    drogon::HttpResponsePtr resp =
        HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
  }
}

void SchemaController::getSchemas(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  int userId = req->getAttributes()->get<int>("user_id");

  try {
    std::vector<models::Schema> schemas = models::Schema::findByUserId(userId);
    Json::Value result(Json::arrayValue);
    for (const auto& schema : schemas) {
      result.append(schema.toJson());
    }
    callback(HttpResponse::newHttpJsonResponse(result));
  } catch (const std::exception& e) {
    drogon::HttpResponsePtr resp =
        HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
  }
}

void SchemaController::getSchema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  int schemaId = req->getAttributes()->get<int>("id");
  int userId = req->getAttributes()->get<int>("user_id");
  try {
    models::Schema schema = models::Schema::findById(schemaId);
    if (schema.userId != userId) {
      throw(std::exception("Schema doesn't belong to user"));
    }
    callback(HttpResponse::newHttpJsonResponse(schema.toJson()));
  } catch (const std::exception& e) {
    drogon::HttpResponsePtr resp =
        HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
  }
}

void SchemaController::updateSchema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  std::shared_ptr<Json::Value> json = req->getJsonObject();
  if (!json) {
    drogon::HttpResponsePtr resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid JSON"));
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }
  int schemaId = req->getAttributes()->get<int>("id");
  int userId = req->getAttributes()->get<int>("user_id");
  try {
    models::Schema schema = models::Schema::findById(schemaId);
    if (schema.userId != userId) {
      throw(std::exception("Schema doesn't belong to user"));
    }
    schema.update(*json);
  } catch(const std::exception& e){
    drogon::HttpResponsePtr resp =
        HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
  }
}

void SchemaController::deleteSchema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  int schemaId = req->getAttributes()->get<int>("id");
  int userId = req->getAttributes()->get<int>("user_id");
  try {
    models::Schema schema = models::Schema::findById(schemaId);
    if (schema.userId != userId) {
      throw(std::exception("Schema doesn't belong to user"));
    }
    schema.remove();
    callback(HttpResponse::newHttpJsonResponse(schema.toJson()));
  } catch (const std::exception& e) {
    drogon::HttpResponsePtr resp =
        HttpResponse::newHttpJsonResponse(Json::Value(e.what()));
    resp->setStatusCode(k500InternalServerError);
    callback(resp);
  }
}

void SchemaController::validateSchema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {}

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