#include "LeafExpression.hpp"

LeafExpression::LeafExpression(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container)
  : AbstractCodeFragment(),
    string_(str),
    location_info_(location_info),
    container_(container)
{

}

LeafExpression::~LeafExpression()
{
  delete location_info_;
}

json LeafExpression::toJson()
{
  json j = AbstractCodeFragment::toJson();

  j["@type"] = "LeafExpression";
  j["string"] = string_;
  j["locationInfo"] = toJsonObj(location_info_);
  j["container"] = container_ != nullptr ? container_->id_ : -1;

  return j;
}