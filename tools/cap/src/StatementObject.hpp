#pragma once

#include <string>

#include "AbstractCodeFragment.hpp"
#include "LocationInfo.hpp"
#include "LeafExpression.hpp"
#include "VariableDeclaration.hpp"
#include "AbstractCall.hpp"
#include "TernaryOperatorExpression.hpp"
#include "CompositeStatementObject.hpp"

class StatementObject : public AbstractStatement
{
  public:
    std::string statement_;
    LocationInfo* location_info_;
    std::vector<LeafExpression*> variables_;
    std::vector<std::string> types_;
    std::vector<VariableDeclaration*> variable_declarations_;
    std::vector<AbstractCall*> method_invocations_;
    std::vector<LeafExpression*> string_literals_;
    std::vector<LeafExpression*> char_literals_;
    std::vector<LeafExpression*> number_literals_;
    std::vector<LeafExpression*> null_literals_;
    std::vector<LeafExpression*> boolean_literals_;
    std::vector<LeafExpression*> type_literals_;
    std::vector<AbstractCall*> creations_;
    std::vector<LeafExpression*> infix_expressions_;
    std::vector<std::string> infix_operators_;
    std::vector<LeafExpression*> array_accesses_;
    std::vector<LeafExpression*> prefix_expressions_;
    std::vector<LeafExpression*> postfix_expressions_;
    std::vector<LeafExpression*> this_expressions_;
    std::vector<LeafExpression*> arguments_;
    std::vector<LeafExpression*> parenthesized_expressions_;
    std::vector<LeafExpression*> cast_expressions_;
    std::vector<TernaryOperatorExpression*> ternary_operator_expressions_;
    std::vector<LambdaExpressionObject*> lambdas_;

    StatementObject(CompositeStatementObject* parent, int depth, int index);
    ~StatementObject();

    json toJson() override;
};