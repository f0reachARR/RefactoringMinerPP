#pragma once

#include <string>
#include <vector>
#include <map>

#include "LocationInfo.hpp" 
#include "UMLOperation.hpp"
#include "UMLAttribute.hpp"
#include "UMLComment.hpp"
#include "VariableDeclaration.hpp"
#include "UMLType.hpp"
#include "UMLImport.hpp"
#include "UMLModifier.hpp"
#include "UMLEnumConstant.hpp"
#include "Serializable.hpp"

class UMLAbstractClass : public Serializable
{
  private:
    inline static int global_class_counter_ = 0;

  public:
    const int id_;

    LocationInfo* location_info_;
    std::string package_name_;
    std::string name_;
    std::vector<UMLOperation*> operations_;
    std::vector<UMLAttribute*> attributes_;
    UMLType* superclass_;
    std::vector<UMLType*> implemented_interfaces_;
    std::vector<UMLImport*> imported_types_;
    std::vector<UMLModifier*> modifiers_;
    std::vector<UMLEnumConstant*> enum_constants_;

    UMLAbstractClass(std::string namesp, std::string name, LocationInfo* location_info);
    virtual ~UMLAbstractClass();

    virtual json toJson() override;
};