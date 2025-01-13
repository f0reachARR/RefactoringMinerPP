#pragma once

#include <string>

#include <clang-c/Index.h>

#include "UMLType.hpp"
#include "LocationInfo.hpp"
#include "VariableScope.hpp"
#include "UMLModifier.hpp"

class AbstractExpression;

class VariableDeclaration : public Serializable
{
  public:
    std::string variable_name_;
    AbstractExpression* initializer_;
    UMLType* type_;
    bool varargs_parameter_;
    LocationInfo* location_info_;
    bool is_parameter_;
    bool is_attribute_;
    bool is_enum_constant_;
    VariableScope* scope_;
    bool is_final_; 
    std::vector<UMLModifier*> modifiers_;

    //custom
    CXCursor cust_decl_cursor_;

    VariableDeclaration(std::string name, UMLType* type, LocationInfo* location_info, CXCursor cursor);
    ~VariableDeclaration();

    json toJson() override;
};