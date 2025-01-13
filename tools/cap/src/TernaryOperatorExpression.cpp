#include "TernaryOperatorExpression.hpp"

#include "AbstractExpression.hpp"

TernaryOperatorExpression::TernaryOperatorExpression(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container,
  AbstractExpression* condition, AbstractExpression* then_expression, AbstractCodeFragment* else_expression)
  : LeafExpression(str, location_info, container),
    condition_(condition),
    then_expression_(then_expression),
    else_expression_(else_expression)
{

}

json TernaryOperatorExpression::toJson()
{
  json j = LeafExpression::toJson();

  j["@type"] = "TernaryOperatorExpression";
  j["condition"] = toJsonObj(condition_);
  j["thenExpression"] = toJsonObj(then_expression_);
  j["elseExpression"] = toJsonObj(else_expression_);

  return j;
}