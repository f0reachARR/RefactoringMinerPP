#include "StatementObject.hpp"

StatementObject::StatementObject(CompositeStatementObject* parent, int depth, int index)
  : AbstractStatement(parent, depth, index)
{

}

StatementObject::~StatementObject()
{
  delete location_info_;

  for (auto a : variables_)
    delete a;

  for (auto a : method_invocations_)
    delete a;

  for (auto a : string_literals_)
    delete a;

  for (auto a : char_literals_)
    delete a;

  for (auto a : number_literals_)
    delete a;

  for (auto a : null_literals_)
    delete a;

  for (auto a : boolean_literals_)
    delete a;

  for (auto a : type_literals_)
    delete a;

  for (auto a : creations_)
    delete a;

  for (auto a : infix_expressions_)
    delete a;

  for (auto a : array_accesses_)
    delete a;

  for (auto a : prefix_expressions_)
    delete a;

  for (auto a : postfix_expressions_)
    delete a;

  for (auto a : this_expressions_)
    delete a;

  for (auto a : arguments_)
    delete a;

  for (auto a : parenthesized_expressions_)
    delete a;

  for (auto a : cast_expressions_)
    delete a;

  for (auto a : ternary_operator_expressions_)
    delete a;

  for (auto a : lambdas_)
    delete a;
}

json StatementObject::toJson()
{
  json j = AbstractStatement::toJson();

  j["@type"] = "StatementObject";
  j["statement"] = statement_;
  j["locationInfo"] = toJsonObj(location_info_);
  j["variables"] = toJsonArray<LeafExpression>(variables_);
  j["types"] = types_;
  j["variableDeclarations"] = toJsonArray<VariableDeclaration>(variable_declarations_);
  j["methodInvocations"] = toJsonArray<AbstractCall>(method_invocations_);
  j["stringLiterals"] = toJsonArray<LeafExpression>(string_literals_);
  j["charLiterals"] = toJsonArray<LeafExpression>(char_literals_);
  j["numberLiterals"] = toJsonArray<LeafExpression>(number_literals_);
  j["nullLiterals"] = toJsonArray<LeafExpression>(null_literals_);
  j["booleanLiterals"] = toJsonArray<LeafExpression>(boolean_literals_);
  j["typeLiterals"] = toJsonArray<LeafExpression>(type_literals_);
  j["creations"] = toJsonArray<AbstractCall>(creations_);
  j["infixExpressions"] = toJsonArray<LeafExpression>(infix_expressions_);
  j["infixOperators"] = infix_operators_;
  j["arrayAccesses"] = toJsonArray<LeafExpression>(array_accesses_);
  j["prefixExpressions"] = toJsonArray<LeafExpression>(prefix_expressions_);
  j["postfixExpressions"] = toJsonArray<LeafExpression>(postfix_expressions_);
  j["thisExpressions"] = toJsonArray<LeafExpression>(this_expressions_);
  j["arguments"] = toJsonArray<LeafExpression>(arguments_);
  j["parenthesizedExpressions"] = toJsonArray<LeafExpression>(parenthesized_expressions_);
  j["castExpressions"] = toJsonArray<LeafExpression>(cast_expressions_);
  j["ternaryOperatorExpressions"] = toJsonArray<TernaryOperatorExpression>(ternary_operator_expressions_);

  return j;
}