#pragma once

#include <string>

#include "UMLType.hpp"
#include "VariableDeclaration.hpp"

class UMLParameter : public Serializable
{
  public:
    std::string name_;
    UMLType* type_;
    std::string kind_;
    bool varargs_;
    VariableDeclaration* variable_declaration_;

    UMLParameter(std::string name, UMLType* type, std::string kind);
    ~UMLParameter();

    json toJson() override;
};