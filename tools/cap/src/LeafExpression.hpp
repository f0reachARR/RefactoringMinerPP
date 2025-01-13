#pragma once

#include <string>

#include "AbstractCodeFragment.hpp"
#include "LocationInfo.hpp"
#include "VariableDeclarationContainer.hpp"

class LeafExpression : public AbstractCodeFragment
{
  public:
    std::string string_;
    LocationInfo* location_info_;
    VariableDeclarationContainer* container_;

    LeafExpression(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container);
    ~LeafExpression();

    virtual json toJson() override;
};