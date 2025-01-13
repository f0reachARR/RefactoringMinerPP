#include "LeafType.hpp"

#include <cassert>

LeafType::LeafType()
  : UMLType(nullptr)
{

}

LeafType::LeafType(LocationInfo* location_info, std::string class_type)
  : UMLType(location_info),
    class_type_(class_type)
{
  non_qualified_class_type_ = get_simple_name(class_type);
}

void LeafType::setClassType(std::string str)
{
  class_type_ = str;
  non_qualified_class_type_ = get_simple_name(str);
}

std::string LeafType::get_simple_name(std::string str)
{
  size_t pos = str.find_last_of("::");

  if (pos == std::string::npos)
    return str;

  return str.substr(pos + 1);
}

std::string LeafType::getClassType()
{
  return class_type_;
}

std::string LeafType::toString()
{
  std::string str = "";

  str += non_qualified_class_type_;
  str += typeArgumentsAndArrayDimensionToString();

  return str;
}

std::string LeafType::toQualifiedString()
{
  std::string str = "";

  str += class_type_;
  str += typeArgumentsAndArrayDimensionToString();

  return str;
}

UMLType* LeafType::copy()
{
  LocationInfo* l = location_info_ == nullptr ? nullptr : new LocationInfo(*location_info_);
  LeafType* t = new LeafType(l, class_type_);

  t->array_dimension_ = array_dimension_;
  t->parameterized_ = parameterized_;

  for (UMLType* a : type_arguments_)
    t->type_arguments_.push_back(a->copy());

  return t;
}

json LeafType::toJson()
{
  json j = UMLType::toJson();

  j["@type"] = "LeafType";
  j["classType"] = class_type_;
  j["nonQualifiedClassType"] = non_qualified_class_type_;

  return j;
}