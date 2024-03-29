
#ifndef example_oatpp_mongo_AppComponent_hpp
#define example_oatpp_mongo_AppComponent_hpp

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include "Parameters.hpp"
#include "SwaggerComponent.hpp"
#include "db/Database.hpp"
#include "oatpp/core/base/CommandLineArguments.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

/**
 *  Class which creates and holds Application components and registers components in oatpp::base::Environment
 *  Order of components initialization is from top to bottom
 */
class AppComponent
{
 private:
  oatpp::base::CommandLineArguments m_cmdArgs;

 public:
  AppComponent(const oatpp::base::CommandLineArguments &cmdArgs)
      : m_cmdArgs(cmdArgs)
  {
  }

 public:
  /**
   *  Swagger component
   */
  SwaggerComponent swaggerComponent;

  /**
   *  Create ConnectionProvider component which listens on the port
   */
  OATPP_CREATE_COMPONENT(
      std::shared_ptr<oatpp::network::ServerConnectionProvider>,
      serverConnectionProvider)
  ([] {
    uint16_t port = std::stoi(Parameters::GetInstance()->Get("Server-Port"));
    return oatpp::network::tcp::server::ConnectionProvider::createShared(
        {"0.0.0.0", port, oatpp::network::Address::IP_4});
  }());

  /**
   *  Create Router component
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                         httpRouter)
  ([] { return oatpp::web::server::HttpRouter::createShared(); }());

  /**
   *  Create ConnectionHandler component which uses Router component to route requests
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                         serverConnectionHandler)
  ([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                    router);  // get Router component
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());

  /**
   *  Create ObjectMapper component to serialize/deserialize DTOs in Contoller's API
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>,
                         apiObjectMapper)
  ([] {
    auto objectMapper =
        oatpp::parser::json::mapping::ObjectMapper::createShared();
    objectMapper->getDeserializer()->getConfig()->allowUnknownFields = false;
    objectMapper->getSerializer()->getConfig()->useBeautifier = true;
    return objectMapper;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<db::Database>, database)
  ([this] {
    oatpp::String connectionString = std::getenv("MONGO_CONN_STR");
    if (!connectionString) {
      connectionString = Parameters::GetInstance()->Get("MongoDB-Address") +
                         "/" +
                         Parameters::GetInstance()->Get("MongoDB-Database");
    }

    mongocxx::uri uri(*connectionString);
    return std::make_shared<db::Database>(
        uri, Parameters::GetInstance()->Get("MongoDB-Database"));
  }());
};

#endif /* example_oatpp_mongo_AppComponent_hpp */
