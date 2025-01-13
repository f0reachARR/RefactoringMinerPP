#include "ConverterExpression.hpp"

StatementObject* ConverterExpression::toStatementObject(AbstractExpression* expr)
{
  StatementObject* stmt = new StatementObject(expr->owner_, expr->depth_, expr->index_);

  stmt->statement_ = expr->expression_;
  stmt->location_info_ = expr->location_info_;
  stmt->variables_ = expr->variables_;
  stmt->types_ = expr->types_;
  stmt->variable_declarations_ = expr->variable_declarations_;
  stmt->method_invocations_ = expr->method_invocations_;
  stmt->string_literals_ = expr->string_literals_;
  stmt->char_literals_ = expr->char_literals_;
  stmt->number_literals_ = expr->number_literals_;
  stmt->null_literals_ = expr->null_literals_;
  stmt->boolean_literals_ = expr->boolean_literals_;
  stmt->type_literals_ = expr->type_literals_;
  stmt->creations_ = expr->creations_;
  stmt->infix_expressions_ = expr->infix_expressions_;
  stmt->infix_operators_ = expr->infix_operators_;
  stmt->array_accesses_ = expr->array_accesses_;
  stmt->prefix_expressions_ = expr->prefix_expressions_;
  stmt->postfix_expressions_ = expr->postfix_expressions_;
  stmt->this_expressions_ = expr->this_expressions_;
  stmt->arguments_ = expr->arguments_;
  stmt->parenthesized_expressions_ = expr->parenthesized_expressions_;
  stmt->cast_expressions_ = expr->cast_expressions_;
  stmt->ternary_operator_expressions_ = expr->ternary_operator_expressions_;
  stmt->lambdas_ = expr->lambdas_;

  for (VariableDeclaration* ref_var : expr->cust_referenced_vars_)
  {
    ref_var->scope_->statements_in_scope_using_variable_.insert(stmt);
  }

  if (stmt->statement_[stmt->statement_.size() - 1] != ';')
  {
    CompositeStatementObject* parent = stmt->parent_;

    while (parent != nullptr)
    {
      if (parent->location_info_->end_offset_ == stmt->location_info_->end_offset_)
      {
        parent->location_info_->end_column_ += 1;
        parent->location_info_->end_offset_ += 1;

        parent = parent->parent_;
      }
      else
        break;
    }

    stmt->statement_ += ";";
    stmt->location_info_->end_column_ += 1;
    stmt->location_info_->end_offset_ += 1;
  }

  stmt->statement_ += "\n";

  expr->is_converted_ = true;
  delete expr;

  return stmt;
}