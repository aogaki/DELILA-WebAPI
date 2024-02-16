
#ifndef example_oatpp_mongo_SwaggerComponent_hpp
#define example_oatpp_mongo_SwaggerComponent_hpp

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/core/macro/component.hpp"

/**
 *  Swagger ui is served at
 *  http://host:port/swagger/ui
 */
class SwaggerComponent
{
 public:
  /**
   *  General API docs info
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>,
                         swaggerDocumentInfo)
  ([] {
    oatpp::swagger::DocumentInfo::Builder builder;

    auto server = "http://" + Parameters::GetInstance()->Get("Server-IP") +
                  ":" + Parameters::GetInstance()->Get("Server-Port");

    builder.setTitle("DELILA Web API - Oat++ and MongoDB")
        .setDescription("Controlling and Save parameters of DELILA")
        .setVersion("1.0")
        .setContactName("Soiciro Aogaki")
        .setContactUrl("https://github.com/aogaki")

        .addServer(server.c_str(), "Office server");

    return builder.build();
  }());

  /**
   *  Swagger-Ui Resources (<oatpp-examples>/lib/oatpp-swagger/res)
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>,
                         swaggerResources)
  ([] {
    // Make sure to specify correct full path to oatpp-swagger/res folder !!!
    return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
  }());
};

#endif /* example_oatpp_mongo_SwaggerComponent_hpp */
