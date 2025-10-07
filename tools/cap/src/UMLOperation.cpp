#include "UMLOperation.hpp"

#include "cassert"

#include "UMLClass.hpp"

UMLOperation::UMLOperation(std::string clss, std::string name, LocationInfo* location_info, UMLClass* container)
  : location_info_(location_info),
    name_(name),
    class_name_(clss),
    cust_container_(container),
    operation_body_(nullptr)
{
  is_final_ = false;
}

UMLOperation::~UMLOperation()
{
  delete location_info_;

  for (auto p : parameters_)
    delete p;

  delete operation_body_;

  for (auto t : type_parameters_)
    delete t;

  for (auto m : modifiers_)
    delete m;
}

VariableDeclaration* UMLOperation::custGetVarDecl(CXCursor& cursor)
{
  for (UMLParameter* parm : parameters_)
  {
    if (parm->variable_declaration_ != nullptr && clang_equalCursors(parm->variable_declaration_->cust_decl_cursor_, cursor))
      return parm->variable_declaration_;
  }

  if (cust_container_ != nullptr)
    return cust_container_->custGetVarDecl(cursor);

  assert(false);
}

LocationInfo* UMLOperation::getLocationInfo()
{
  return location_info_;
}

json UMLOperation::toJson()
{
  json j;

  j["id"] = id_;
  j["locationInfo"] = toJsonObj(location_info_);
  j["name"] = name_;
  j["visibility"] = visibility_;
  j["isAbstract"] = is_abstract_;
  j["parameters"] = toJsonArray<UMLParameter>(parameters_);
  j["className"] = class_name_;
  j["isConstructor"] = is_constructor_;
  j["isFinal"] = is_final_;
  j["isStatic"] = is_static_;
  j["operationBody"] = toJsonObj(operation_body_);
  j["typeParameters"] = toJsonArray<UMLTypeParameter>(type_parameters_);
  j["modifiers"] = toJsonArray<UMLModifier>(modifiers_);

  return j;
}
