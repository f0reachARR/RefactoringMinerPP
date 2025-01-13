#include "UMLModifier.hpp"

UMLModifier::UMLModifier(std::string keyword, LocationInfo* location_info)
  : keyword_(keyword),
    location_info_(location_info)
{

}

UMLModifier::~UMLModifier()
{
  delete location_info_;
}

json UMLModifier::toJson()
{
  json j;

  j["keyword"] = keyword_;
  j["locationInfo"] = toJsonObj(location_info_);

  return j;
}