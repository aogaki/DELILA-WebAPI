
#include "Database.hpp"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/insert.hpp>

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

namespace db
{

Database::Database(const mongocxx::uri &uri, const std::string &dbName)
    : m_pool(std::make_shared<mongocxx::pool>(uri)), m_databaseName(dbName)
{
}

bsoncxx::document::value Database::createMongoDocument(
    const oatpp::Void &polymorph)
{
  // if you have huge docs, you may want to increase starting BufferOutputStream size.
  // Or you may want to use oatpp::data::stream::ChunkedBuffer instead - for no-copy growth.
  oatpp::data::stream::BufferOutputStream stream;
  m_objectMapper.write(&stream, polymorph);
  bsoncxx::document::view view(stream.getData(), stream.getCurrentPosition());
  return bsoncxx::document::value(view);
}

oatpp::Object<RunLogDto> Database::CreateNewRunRecord(
    oatpp::Object<RunLogDto> dto)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_dbName][m_collectionName];
  auto result = collection.insert_one(createMongoDocument(dto));

  return dto;
}

oatpp::List<oatpp::Object<RunLogDto>> Database::ReadRunList(std::string expName,
                                                            uint64_t listSize)
{
  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_dbName][m_collectionName];

  auto sortOpt = mongocxx::options::find{};
  if (listSize > 0) sortOpt.limit(listSize);
  auto order = bsoncxx::builder::stream::document{}
               << "start" << -1 << bsoncxx::builder::stream::finalize;
  sortOpt.sort(order.view());
  auto key = bsoncxx::builder::stream::document{}
             << "expName" << expName << bsoncxx::builder::stream::finalize;
  auto cursor = collection.find({key}, sortOpt);

  auto list = oatpp::List<oatpp::Object<RunLogDto>>::createShared();
  for (auto &&doc : cursor) {
    auto json = bsoncxx::to_json(doc);
    oatpp::parser::json::mapping::ObjectMapper objMapper;
    auto dto = objMapper.readFromString<oatpp::Object<RunLogDto>>(json.c_str());
    list->push_back(dto);
  }

  return list;
}

int32_t Database::UpdateRunRecord(oatpp::Object<RunLogDto> dto)
{
  // This is used for updating the stop time and comment of a run.
  // Filter is not id, just use runNumber, start time and experiment name.

  auto conn = m_pool->acquire();
  auto collection = (*conn)[m_dbName][m_collectionName];

  auto filter = bsoncxx::builder::stream::document{}
                << "runNumber" << dto->runNumber << "runNumber"
                << dto->runNumber << "start" << dto->start << "expName"
                << dto->expName->c_str() << bsoncxx::builder::stream::finalize;
  auto update = bsoncxx::builder::stream::document{}
                << "$set" << bsoncxx::builder::stream::open_document
                << "runNumber" << dto->runNumber << "start" << dto->start
                << "stop" << dto->stop << "expName" << dto->expName->c_str()
                << "source" << dto->source->c_str() << "distance"
                << dto->distance->c_str() << "comment" << dto->comment->c_str()
                << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::finalize;
  auto result = collection.update_one(filter.view(), update.view());

  return result->modified_count();
}

}  // namespace db