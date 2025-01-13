#include "UMLClass.hpp"

#include <cassert>

UMLClass::UMLClass(std::string namesp, std::string name)
  : UMLAbstractClass(namesp, name, nullptr)
{
  is_final_ = false;
  is_abstract_ = false;
  is_enum_ = false;

  qualified_name_ = getQualifiedName(namesp, name);
}

UMLClass::UMLClass(std::string namesp, std::string name, LocationInfo* location_info)
  : UMLClass(namesp, name)
{
  location_info_ = location_info;

  source_file_ = location_info_->file_path_;

  size_t path_slash_index = source_file_.find_last_of('/');

  if (path_slash_index == std::string::npos)
    source_folder_ = "";
  else
    source_folder_ = source_file_.substr(0, path_slash_index);
}

UMLClass::~UMLClass()
{
  for (auto t : type_parameters_)
    delete t;
}

std::string UMLClass::getQualifiedName(std::string namesp, std::string name)
{
  return namesp == ""
    ? name
    : namesp + "." + name;
}

VariableDeclaration* UMLClass::custGetVarDecl(CXCursor& cursor)
{
  for (UMLAttribute* attr : attributes_)
  {
    if (clang_equalCursors(attr->variable_declaration_->cust_decl_cursor_, cursor))
      return attr->variable_declaration_;
  }

  return nullptr; //TODO: fix this. allow for now but declaration should be found somewhere

  //assert(false);
}

json UMLClass::toJson()
{
  json j = UMLAbstractClass::toJson();

  j["qualifiedName"] = qualified_name_;
  j["sourceFile"] = source_file_;
  j["sourceFolder"] = source_folder_;
  j["isFinal"] = is_final_;
  j["isAbstract"] = is_abstract_;
  j["isEnum"] = is_enum_;
  j["typeParameters"] = toJsonArray<UMLTypeParameter>(type_parameters_);

  return j;
}