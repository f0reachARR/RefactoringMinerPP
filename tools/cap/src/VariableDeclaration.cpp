#include "VariableDeclaration.hpp"

#include <cassert>

#include "AbstractExpression.hpp"

VariableDeclaration::VariableDeclaration(std::string name, UMLType* type, LocationInfo* location_info, CXCursor cursor)
  : variable_name_(name),
    type_(type),
    location_info_(location_info),
    cust_decl_cursor_(cursor)
{
  assert(cust_decl_cursor_.kind == CXCursor_VarDecl
    || cust_decl_cursor_.kind == CXCursor_FieldDecl
    || cust_decl_cursor_.kind == CXCursor_EnumConstantDecl
    || cust_decl_cursor_.kind == CXCursor_ParmDecl);

  varargs_parameter_ = false;

  is_parameter_ = false;
  is_attribute_ = false;
  is_enum_constant_ = false;

  is_final_ = false;

  scope_ = nullptr;
}

VariableDeclaration::~VariableDeclaration()
{
  delete initializer_;
  delete type_;
  delete location_info_;
  delete scope_;

  for (auto m : modifiers_)
    delete m;
}

json VariableDeclaration::toJson()
{
  json j;

  j["variableName"] = variable_name_;
  j["initializer"] = toJsonObj(initializer_);
  j["type"] = toJsonObj(type_);
  j["locationInfo"] = toJsonObj(location_info_);
  j["isParameter"] = is_parameter_;
  j["isAttribute"] = is_attribute_;
  j["isEnumConstant"] = is_enum_constant_;
  j["scope"] = toJsonObj(scope_);
  j["isFinal"] = is_final_;
  j["modifiers"] = toJsonArray<UMLModifier>(modifiers_);

  return j;
}