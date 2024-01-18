
#ifndef example_oatpp_mongo_Database_hpp
#define example_oatpp_mongo_Database_hpp

#include <bsoncxx/document/value.hpp>
#include <mongocxx/pool.hpp>

#include "dto/DTOs.hpp"
#include "oatpp-mongo/bson/mapping/ObjectMapper.hpp"

namespace db
{

class Database
{
 private:
  std::shared_ptr<mongocxx::pool> m_pool;
  std::string m_databaseName;
  oatpp::mongo::bson::mapping::ObjectMapper m_objectMapper;
  const std::string m_dbName = "DELILA";
  const std::string m_collectionName = "run_information";

  bsoncxx::document::value createMongoDocument(const oatpp::Void &polymorph);

 public:
  Database(const mongocxx::uri &uri, const std::string &dbName);

  oatpp::Object<RunLogDto> CreateNewRunRecord(oatpp::Object<RunLogDto> dto);
  oatpp::List<oatpp::Object<RunLogDto>> ReadRunList(std::string expName,
                                                    uint64_t listSize = 0);
  int32_t UpdateRunRecord(oatpp::Object<RunLogDto> dto);
};

}  // namespace db

#endif  //example_oatpp_mongo_Database_hpp
