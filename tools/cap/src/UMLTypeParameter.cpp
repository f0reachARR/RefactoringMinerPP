#include "UMLTypeParemter.hpp"

UMLTypeParameter::UMLTypeParameter(std::string name, LocationInfo* location_info)
  : name_(name),
    location_info_(location_info)
{

}

UMLTypeParameter::~UMLTypeParameter()
{
  for (auto t : type_bounds_)
    delete t;

  delete location_info_;
}

json UMLTypeParameter::toJson()
{
  json j;

  j["name"] = name_;
  j["typeBounds"] = toJsonArray<UMLType>(type_bounds_);
  j["locationInfo"] = toJsonObj(location_info_);

  return j;
}