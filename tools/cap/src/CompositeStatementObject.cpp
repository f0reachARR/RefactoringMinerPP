#include "CompositeStatementObject.hpp"

#include <cassert>

#include "TryStatementObject.hpp"
#include "UMLOperation.hpp"

CompositeStatementObject::CompositeStatementObject(LocationInfo* location_info, CompositeStatementObject* parent,
  UMLOperation* container, int depth, int index)
  : AbstractStatement(parent, depth, index),
    location_info_(location_info),
    cust_container_(container)
{
  try_container_ = nullptr;
}

CompositeStatementObject::CompositeStatementObject(const CompositeStatementObject& c)
  : CompositeStatementObject(c.location_info_, c.parent_, c.cust_container_, c.depth_, c.index_)
{
  statement_list_.insert(statement_list_.end(), c.statement_list_.begin(), c.statement_list_.end());
  expression_list_.insert(expression_list_.end(), c.expression_list_.begin(), c.expression_list_.end());
  variable_declarations_.insert(variable_declarations_.end(), c.variable_declarations_.begin(), c.variable_declarations_.end());
  try_container_ = c.try_container_;
  cust_var_decls_ = c.cust_var_decls_;
}

CompositeStatementObject::~CompositeStatementObject()
{
  for (auto& s : statement_list_)
    delete s;

  for (auto& e : expression_list_)
    delete e;

  for (auto& v : variable_declarations_)
    delete v;

  delete location_info_;
}

VariableDeclaration* CompositeStatementObject::custGetVarDecl(CXCursor& cursor)
{
  VariableDeclaration* var_decl = cust_var_decls_.findVarDecl(cursor);

  if (var_decl != nullptr)
    return var_decl;

  if (parent_ != nullptr)
    return parent_->custGetVarDecl(cursor);

  if (cust_container_ != nullptr)
    return cust_container_->custGetVarDecl(cursor);

  assert(false);
}

json CompositeStatementObject::toJson()
{
  json j = AbstractStatement::toJson();

  j["@type"] = "CompositeStatementObject";
  j["statementList"] = toJsonArrayRev<AbstractStatement>(statement_list_);
  j["expressionList"] = toJsonArrayRev<AbstractExpression>(expression_list_);
  j["variableDeclarations"] = toJsonArrayRev<VariableDeclaration>(variable_declarations_);
  j["tryContainer"] = try_container_ != nullptr ? try_container_->id_ : -1;
  j["locationInfo"] = toJsonObj(location_info_);

  return j;
}