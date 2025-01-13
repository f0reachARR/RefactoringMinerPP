#pragma once

#include <string>

#include "AbstractCall.hpp"
#include "UMLType.hpp"

class ObjectCreation : public AbstractCall
{
  public:
    UMLType* uml_type_;
    bool is_array_;
    ObjectCreation(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container);
    ~ObjectCreation();

    json toJson() override;
};