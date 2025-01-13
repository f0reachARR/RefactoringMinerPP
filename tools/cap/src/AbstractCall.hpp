#pragma once

#include <string>
#include <vector>

#include "LeafExpression.hpp"
#include "StatementCoverageType.hpp"

class AbstractCall : public LeafExpression
{
  public:
    int number_of_arguments_;
    std::string expression_;
    std::vector<std::string> arguments_;
    StatementCoverageType coverage_;

    AbstractCall(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container);
    virtual ~AbstractCall() = default;

    virtual json toJson() override;
};