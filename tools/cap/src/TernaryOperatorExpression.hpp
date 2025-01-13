#pragma once

#include <string>

#include "LeafExpression.hpp"
#include "AbstractCodeFragment.hpp"

class AbstractExpression;

class TernaryOperatorExpression : public LeafExpression
{
  public:
    AbstractExpression* condition_;
    AbstractExpression* then_expression_;
    AbstractCodeFragment* else_expression_;

    TernaryOperatorExpression(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container,
      AbstractExpression* condition, AbstractExpression* then_expression, AbstractCodeFragment* else_expression);

    json toJson() override;
};