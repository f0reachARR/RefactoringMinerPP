#include "UMLType.hpp"

UMLType::UMLType()
{
  location_info_ = nullptr;
  array_dimension_ = 0;
  parameterized_ = false;

  cust_initializer_detected_ = false;
}

UMLType::UMLType(LocationInfo* location_info)
  : UMLType()
{
  location_info_ = location_info;
}

UMLType::~UMLType()
{
  delete location_info_;
}

std::string UMLType::typeAgumentsToString()
{
  std::string str = "";

  if (type_arguments_.empty())
  {
    if (parameterized_)
      str += "<>";
  }
  else
  {
    str += "<";

    for(size_t i = 0; i < type_arguments_.size(); i++)
    {
      str += type_arguments_[i]->toQualifiedString();

      if (i < type_arguments_.size() - 1)
        str += ",";
    }

    str += ">";
  }

  return str;
}

std::string UMLType::typeArgumentsAndArrayDimensionToString()
{
  std::string str = "";

  if (type_arguments_.size() > 0 || parameterized_)
    str += typeAgumentsToString();

  for (int i = 0; i < array_dimension_; i++)
    str += "[]";

  return str;
}

json UMLType::toJson()
{
  json j;

  j["locationInfo"] = toJsonObj(location_info_);
  j["arrayDimension"] = array_dimension_;
  j["parameterized"] = parameterized_;
  j["typeArguments"] = toJsonArray<UMLType>(type_arguments_);

  return j;
}
