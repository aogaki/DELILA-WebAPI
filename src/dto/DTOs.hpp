
#ifndef DTOs_hpp
#define DTOs_hpp 1

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class RunLogDto : public oatpp::DTO
{
  DTO_INIT(RunLogDto, DTO)

  DTO_FIELD(Int32, runNumber);
  DTO_FIELD(Int64, start);
  DTO_FIELD(Int64, stop);
  DTO_FIELD(String, expName);
  DTO_FIELD(String, comment);
  DTO_FIELD(String, source);
  DTO_FIELD(String, distance);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
