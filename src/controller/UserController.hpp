
#ifndef UserController_hpp
#define UserController_hpp 1

#include "controller/DELILAController.hpp"
#include "db/Database.hpp"
#include "dto/DTOs.hpp"
#include "oatpp-swagger/Types.hpp"
#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UserController : public oatpp::web::server::api::ApiController
{
 private:
  OATPP_COMPONENT(std::shared_ptr<db::Database>, m_database);

  DELILAController delila = DELILAController("172.18.4.77", 30000);

 public:
  UserController(const std::shared_ptr<ObjectMapper> &objectMapper)
      : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

 public:
  static std::shared_ptr<UserController> createShared(
      OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
  {
    return std::make_shared<UserController>(objectMapper);
  }

  ENDPOINT("GET", "/", root)
  {
    const String html =
        "<html lang='en'>"
        "<head>"
        "<meta charset=utf-8/>"
        "</head>"
        "<body>"
        "<p>Web API for DELILA</p>"
        "<a href='swagger/ui'>Checkout Swagger-UI page</a>"
        "</body>"
        "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }
  ADD_CORS(root)

  ENDPOINT_INFO(getStatus) { info->summary = "Get the status of DELILA"; }
  ADD_CORS(getStatus)
  ENDPOINT("GET", "/DELILA/get-status", getStatus)
  {
    auto status = delila.CheckStatus();
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(configure) { info->summary = "Configure DELILA"; }
  ADD_CORS(configure)
  ENDPOINT("POST", "/DELILA/configure", configure)
  {
    auto status = delila.Configure();
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(unconfigure) { info->summary = "Unconfigure DELILA"; }
  ADD_CORS(unconfigure)
  ENDPOINT("POST", "/DELILA/unconfigure", unconfigure)
  {
    auto status = delila.Unconfigure();
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(start)
  {
    info->summary = "Start DELILA";

    info->pathParams["runNo"].description = "Run number";
  }
  ADD_CORS(start)
  ENDPOINT("POST", "/DELILA/start/{runNo}", start, PATH(Int32, runNo))
  {
    auto status = delila.Start(runNo);
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(stop) { info->summary = "Stop DELILA"; }
  ADD_CORS(stop)
  ENDPOINT("POST", "/DELILA/stop", stop)
  {
    auto status = delila.Stop();
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(configureAndStart)
  {
    info->summary = "Configure and start DELILA";

    info->pathParams["runNo"].description = "Run number";
  }
  ADD_CORS(configureAndStart)
  ENDPOINT("POST", "/DELILA/configure-and-start/{runNo}", configureAndStart,
           PATH(Int32, runNo))
  {
    auto status = delila.ConfigureAndStart(runNo);
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(stopAndUnconfigure)
  {
    info->summary = "Stop and unconfigure DELILA";
  }
  ADD_CORS(stopAndUnconfigure)
  ENDPOINT("POST", "/DELILA/stop-and-unconfigure", stopAndUnconfigure)
  {
    auto status = delila.StopAndUnconfigure();
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }

  ENDPOINT_INFO(dryRun)
  {
    info->summary = "Dry run DELILA, configure and start without data writing.";
  }
  ADD_CORS(dryRun)
  ENDPOINT("POST", "/DELILA/dry-run", dryRun)
  {
    auto status = delila.ConfigureAndStart(-1);
    auto response = createResponse(Status::CODE_200, status);
    response->putHeader(Header::CONTENT_TYPE, "application/xml");

    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* UserController_hpp */
