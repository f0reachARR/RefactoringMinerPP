#include "ParserStatementFor.hpp"

#include <cassert>

#include "ParserUtils.hpp"
#include "ParserStatement.hpp"
#include "ParserExpression.hpp"
#include "ParserVariableDeclaration.hpp"
#include "ParserStatementCtrlStr.hpp"

CompositeStatementObject* ParserStatementFor::parseForStmt(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* for_stmt =
    ParserStatementCtrlStr::createCtrlStr(cursor, container, parent, depth, index, CodeElementType::FOR_STATEMENT);

  CXCursor stmt = clang_getNullCursor();
  std::vector<CXCursor> exprs;

  ParserStatementCtrlStr::getCtrlStrExprsAndStmt(cursor, exprs, stmt, true);

  parseForStmtExpressions(cursor, for_stmt, exprs);
  ParserStatement::insertStmt(ParserStatement::parse(stmt, container, for_stmt, depth + 1, 0), for_stmt->statement_list_);

  return for_stmt;
}

void ParserStatementFor::parseForStmtExpressions(CXCursor cur_for, CompositeStatementObject* for_stmt, std::vector<CXCursor>& cur_for_exprs)
{
  LocationInfo *semicolon1, *semicolon2;
  getForStmtSemicolonLocations(cur_for, semicolon1, semicolon2);

  for (CXCursor cur : cur_for_exprs)
  {
    CodeElementType expr_type = getForStmtExpressionType(cur, semicolon1, semicolon2);

    ParserStatementCtrlStr::addExpr(for_stmt, cur, expr_type);
  }

  delete semicolon1;
  delete semicolon2;
}

CodeElementType ParserStatementFor::getForStmtExpressionType(CXCursor cur_expr, LocationInfo* semicolon1, LocationInfo* semicolon2)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cur_expr);

  LocationInfo expr_loc = LocationInfo(*tokens[0]->location_info_);

  ParserUtils::freeTokens(tokens);

  if (expr_loc.start_offset_ < semicolon1->start_offset_)
    return CodeElementType::FOR_STATEMENT_INITIALIZER;

  if (expr_loc.start_offset_ < semicolon2->start_offset_)
    return CodeElementType::FOR_STATEMENT_CONDITION;

  return CodeElementType::FOR_STATEMENT_UPDATER;
}

void ParserStatementFor::getForStmtSemicolonLocations(CXCursor& cur_for, LocationInfo*& loc1, LocationInfo*& loc2)
{
  loc1 = nullptr;
  loc2 = nullptr;

  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cur_for);

  LocationInfo** cur_loc = &loc1;

  for (ParserToken* tok : tokens)
  {
    if (tok->token_ == ";")
    {
      *cur_loc = new LocationInfo(*tok->location_info_);

      if (*cur_loc == loc1)
        cur_loc = &loc2;
      else
        break;
    }
  }

  assert(loc1 != nullptr && loc2 != nullptr);

  ParserUtils::freeTokens(tokens);
}

CompositeStatementObject* ParserStatementFor::parseEnhancedForStmt(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* for_stmt =
    ParserStatementCtrlStr::createCtrlStr(cursor, container, parent, depth, index, CodeElementType::ENHANCED_FOR_STATEMENT);

  CXCursor stmt = clang_getNullCursor();
  std::vector<CXCursor> exprs;

  ParserStatementCtrlStr::getCtrlStrExprsAndStmt(cursor, exprs, stmt, true);
  assert(exprs.size() == 2);

  for (CXCursor cur : exprs)
  {
    //adjust cet later on, but keep it at ENHANCED_FOR_STATEMENT_PARAMETER_NAME
    ParserStatementCtrlStr::addExpr(for_stmt, cur, CodeElementType::ENHANCED_FOR_STATEMENT_PARAMETER_NAME);
  }

  for_stmt->expression_list_[1]->location_info_->code_element_type_ = CodeElementType::ENHANCED_FOR_STATEMENT_EXPRESSION;

  parseEnhancedForParam(exprs[0], for_stmt, for_stmt->expression_list_[0]);
  ParserStatement::insertStmt(ParserStatement::parse(stmt, container, for_stmt, depth + 1, 0), for_stmt->statement_list_);

  return for_stmt;
}

void ParserStatementFor::parseEnhancedForParam(CXCursor& cur_expr, CompositeStatementObject* for_stmt,
  AbstractExpression* for_stmt_param)
{
  //the variable decl of the param must be removed because it is incorrect and ref min expects the var decl in the comp stmt
  assert(for_stmt_param->variable_declarations_.size() == 1);
  delete for_stmt_param->variable_declarations_[0];
  for_stmt_param->variable_declarations_.clear();
  for_stmt->cust_var_decls_.dropLast();

  VariableDeclaration* var_decl_expr = getEnhancedForParamVarDecl(cur_expr, for_stmt);

  for_stmt->variable_declarations_.push_back(var_decl_expr);
  for_stmt->cust_var_decls_.addVarDecl(var_decl_expr);
}

VariableDeclaration* ParserStatementFor::getEnhancedForParamVarDecl(CXCursor& cursor, CompositeStatementObject* for_stmt)
{
  VariableDeclaration* var_decl_expr = ParserVariableDeclaration::parseVariableDeclaration(cursor,
    CodeElementType::SINGLE_VARIABLE_DECLARATION, for_stmt->cust_container_, for_stmt);

  //recreate location info because the location info in the ast does not include the type
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);
  std::vector<ParserToken*> sub_tokens(tokens.begin(), tokens.end() - 1); //drop last token ':'

  LocationInfo* new_loc = ParserUtils::getTokenChainLocation(sub_tokens, CodeElementType::SINGLE_VARIABLE_DECLARATION);

  ParserUtils::freeTokens(tokens);

  delete var_decl_expr->location_info_;
  var_decl_expr->location_info_ = new_loc;

  return var_decl_expr;
}
