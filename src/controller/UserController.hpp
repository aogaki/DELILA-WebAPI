
#ifndef UserController_hpp
#define UserController_hpp 1

#include "Parameters.hpp"
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

  DELILA::DELILAController delila = DELILA::DELILAController(
      Parameters::GetInstance()->Get("DELILA-IP"),
      std::stoi(Parameters::GetInstance()->Get("DELILA-Port")));

  std::shared_ptr<ApiController::OutgoingResponse> genResponse(
      std::string status)
  {
    std::shared_ptr<ApiController::OutgoingResponse> response;
    if (status == DELILA::Failed) {
      response = createResponse(
          Status::CODE_500,
          status +
              ": Socket error between API server and DELILA, maybe DELILA is "
              "not running or wrong IP address.");
      response->putHeader(Header::CONTENT_TYPE, "text/plain");
    } else {
      response = createResponse(Status::CODE_200, status);
      response->putHeader(Header::CONTENT_TYPE, "application/xml");
    }

    return response;
  }

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

  // To control the DELILA, we need to send commands to DAQ-Middleware
  ENDPOINT_INFO(getStatus) { info->summary = "Get the status of DELILA"; }
  ADD_CORS(getStatus)
  ENDPOINT("GET", "/DELILA/get-status", getStatus)
  {
    auto status = delila.CheckStatus();
    auto response = genResponse(status);
    return response;
  }

  ENDPOINT_INFO(configure) { info->summary = "Configure DELILA"; }
  ADD_CORS(configure)
  ENDPOINT("POST", "/DELILA/configure", configure)
  {
    auto status = delila.Configure();
    auto response = genResponse(status);
    return response;
  }

  ENDPOINT_INFO(unconfigure) { info->summary = "Unconfigure DELILA"; }
  ADD_CORS(unconfigure)
  ENDPOINT("POST", "/DELILA/unconfigure", unconfigure)
  {
    auto status = delila.Unconfigure();
    auto response = genResponse(status);
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
    auto response = genResponse(status);
    return response;
  }

  ENDPOINT_INFO(stop) { info->summary = "Stop DELILA"; }
  ADD_CORS(stop)
  ENDPOINT("POST", "/DELILA/stop", stop)
  {
    auto status = delila.Stop();
    auto response = genResponse(status);
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
    auto response = genResponse(status);
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
    auto response = genResponse(status);
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
    auto response = genResponse(status);
    return response;
  }

  ENDPOINT_INFO(createNeweRunRecord)
  {
    info->summary =
        "Post start time with run number.  Creating document in the DB.";
    info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "application/json");
  }
  ADD_CORS(createNeweRunRecord)
  ENDPOINT("POST", "/DELILA-DB/create-new-run-record", createNeweRunRecord,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto echo = m_database->CreateNewRunRecord(dto);
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(readRunList)
  {
    info->summary = "Read run list from DB.";
    info->addResponse<List<Object<RunLogDto>>>(Status::CODE_200,
                                               "application/json");
    info->pathParams["expName"].description = "Experiment name";
  }
  ADD_CORS(readRunList)
  ENDPOINT("GET", "/DELILA-DB/read-run-list/{expName}", readRunList,
           PATH(String, expName),
           QUERY(UInt64, listSize, "listSize", uint64_t(0)))
  {
    auto runList = m_database->ReadRunList(expName, listSize);
    auto response = createDtoResponse(Status::CODE_200, runList);
    return response;
  }

  ENDPOINT_INFO(updateRunRecord)
  {
    info->summary =
        "Update run record in DB by POST.  Return the nummber of "
        "documents updated.";
    // info->addConsumes<Object<RunLogDto>>("application/json");
    info->addResponse<Object<RunLogDto>>(Status::CODE_200, "text/plain");
    // info->addResponse<String>(Status::CODE_404, "text/plain");
  }
  ADD_CORS(updateRunRecord)
  ENDPOINT("POST", "/DELILA-DB/update-run-record", updateRunRecord,
           BODY_DTO(Object<RunLogDto>, dto))
  {
    auto count = m_database->UpdateRunRecord(dto);
    auto response = createResponse(Status::CODE_200, std::to_string(count));
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* UserController_hpp */
