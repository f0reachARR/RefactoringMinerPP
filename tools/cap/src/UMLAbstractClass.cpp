#include "UMLAbstractClass.hpp"

UMLAbstractClass::UMLAbstractClass(std::string namesp, std::string name, LocationInfo* location_info)
  : id_(++global_class_counter_),
    location_info_(location_info),
    package_name_(namesp),
    name_(name)
{
  superclass_ = nullptr;
}

UMLAbstractClass::~UMLAbstractClass()
{
  delete location_info_;

  for (auto o : operations_)
    delete o;

  for (auto a : attributes_)
    delete a;

  delete superclass_;

  for (auto m : modifiers_)
    delete m;

  for (auto e : enum_constants_)
    delete e;
}

json UMLAbstractClass::toJson()
{
  json j;

  j["id"] = id_;
  j["locationInfo"] = toJsonObj(location_info_);
  j["packageName"] = package_name_;
  j["name"] = name_;
  j["operations"] = toJsonArray<UMLOperation>(operations_);
  j["attributes"] = toJsonArray<UMLAttribute>(attributes_);
  j["superclass"] = toJsonObj(superclass_);
  j["implementedInterfaces"] = toJsonArray<UMLType>(implemented_interfaces_);
  j["importedTypes"] = toJsonArray<UMLImport>(imported_types_);
  j["modifiers"] = toJsonArray<UMLModifier>(modifiers_);
  j["enumConstants"] = toJsonArray<UMLEnumConstant>(enum_constants_);

  return j;
}