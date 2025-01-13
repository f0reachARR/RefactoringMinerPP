#include "UMLAttribute.hpp"

UMLAttribute::UMLAttribute(std::string name, LocationInfo* location_info)
  : location_info_(location_info),
    name_(name)
{
  visibility_ = Visibility::PACKAGE;
}

UMLAttribute::~UMLAttribute()
{
  delete type_;

  delete location_info_;
}

LocationInfo* UMLAttribute::getLocationInfo()
{
  return location_info_;
}

json UMLAttribute::toJson()
{
  json j;

  j["id"] = id_;
  j["locationInfo"] = toJsonObj(location_info_);
  j["name"] = name_;
  j["type"] = toJsonObj(type_);
  j["visibility"] = visibility_;
  j["className"] = class_name_;
  j["isFinal"] = is_final_;
  j["isStatic"] = is_static_;
  j["isVolatile"] = is_volatile_;
  j["variableDeclaration"] = toJsonObj(variable_declaration_);

  return j;
}