#pragma once

#include <string>

#include "LocationInfo.hpp"

class ParserToken
{
  public:
    std::string token_;
    LocationInfo* location_info_;

    ParserToken(std::string token_, LocationInfo* location_info);
    ParserToken(ParserToken& token);
    ~ParserToken();
};