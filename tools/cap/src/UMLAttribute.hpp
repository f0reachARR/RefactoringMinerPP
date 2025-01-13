#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "LocationInfo.hpp"
#include "UMLType.hpp"
#include "Visibility.hpp"
#include "VariableDeclaration.hpp"
#include "UMLComment.hpp"
#include "VariableDeclarationContainer.hpp"

class UMLAttribute : public VariableDeclarationContainer
{
  public:
    LocationInfo* location_info_;
    std::string name_;
    UMLType* type_;
    Visibility visibility_;
    std::string class_name_;
    bool is_final_;
    bool is_static_;
    bool is_volatile_;
    VariableDeclaration* variable_declaration_;

    UMLAttribute(std::string name, LocationInfo* location_info);
    virtual ~UMLAttribute();

    LocationInfo* getLocationInfo() override;
    virtual json toJson() override;
};