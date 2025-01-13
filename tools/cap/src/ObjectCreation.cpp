#include "ObjectCreation.hpp"

ObjectCreation::ObjectCreation(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container)
  : AbstractCall(str, location_info, container)
{
  is_array_ = false;
}

ObjectCreation::~ObjectCreation()
{
  delete uml_type_;
}

json ObjectCreation::toJson()
{
  json j = AbstractCall::toJson();

  j["@type"] = "ObjectCreation";
  j["type"] = toJsonObj(uml_type_);
  j["isArray"] = is_array_;

  return j;
}