#include "ParserToken.hpp"

ParserToken::ParserToken(std::string token, LocationInfo* location_info)
  : token_(token),
    location_info_(location_info)
{

}

ParserToken::ParserToken(ParserToken& token)
  : ParserToken(token.token_, new LocationInfo(*token.location_info_))
{

}

ParserToken::~ParserToken()
{
  delete location_info_;
}