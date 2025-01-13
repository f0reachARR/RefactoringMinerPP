#include "UMLParameter.hpp"

UMLParameter::UMLParameter(std::string name, UMLType* type, std::string kind)
  : name_(name),
    type_(type),
    kind_(kind)
{
  varargs_ = false;
  variable_declaration_ = nullptr;
}

UMLParameter::~UMLParameter()
{
  delete type_;
  delete variable_declaration_;
}

json UMLParameter::toJson()
{
  json j;

  j["name"] = name_;
  j["type"] = toJsonObj(type_);
  j["kind"] = kind_;
  j["varargs"] = varargs_;
  j["variableDeclaration"] = toJsonObj(variable_declaration_);

  return j;
}