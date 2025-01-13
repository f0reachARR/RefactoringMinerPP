#include "ParserStatementCtrlStr.hpp"

#include <cassert>
#include <algorithm>

#include "ParserUtils.hpp"
#include "ParserStatement.hpp"
#include "ParserExpression.hpp"
#include "ConverterExpression.hpp"

CompositeStatementObject* ParserStatementCtrlStr::parseIfStmt(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* if_stmt = createCtrlStr(cursor, container, parent, depth, index, CodeElementType::IF_STATEMENT);

  std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cursor);
  std::vector<CXCursor> exprs, stmts;

  getIfCtrlStrExprsAndStmts(cursor, children, exprs, stmts);

  fillCtrlStr(if_stmt, exprs, stmts, CodeElementType::IF_STATEMENT_CONDITION);

  return if_stmt;
}

CompositeStatementObject* ParserStatementCtrlStr::parseWhileStmt(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* while_stmt = createCtrlStr(cursor, container, parent, depth, index, CodeElementType::WHILE_STATEMENT);

  std::vector<CXCursor> exprs;
  CXCursor stmt = clang_getNullCursor();

  getCtrlStrExprsAndStmt(cursor, exprs, stmt, true);

  fillCtrlStr(while_stmt, exprs, stmt, CodeElementType::WHILE_STATEMENT_CONDITION);

  return while_stmt;
}

CompositeStatementObject* ParserStatementCtrlStr::parseDoStmt(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* do_stmt = createCtrlStr(cursor, container, parent, depth, index, CodeElementType::DO_STATEMENT);

  std::vector<CXCursor> exprs;
  CXCursor stmt = clang_getNullCursor();

  getCtrlStrExprsAndStmt(cursor, exprs, stmt, false);

  fillCtrlStr(do_stmt, exprs, stmt, CodeElementType::DO_STATEMENT_CONDITION);

  return do_stmt;
}

CompositeStatementObject* ParserStatementCtrlStr::createCtrlStr(CXCursor& cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index, CodeElementType ctrl_str_type)
{
  LocationInfo* loc_info = ParserUtils::createLocationInfo(cursor, ctrl_str_type);
  return new CompositeStatementObject(loc_info, parent, container, depth, index);
}

void ParserStatementCtrlStr::fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs, CXCursor& stmt,
  CodeElementType expr_type)
{
  std::vector<CXCursor> stmts = { stmt };
  fillCtrlStr(ctrl_str, exprs, stmts, expr_type);
}

void ParserStatementCtrlStr::fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs,
  CodeElementType expr_type)
{
  std::vector<CXCursor> stmts;
  fillCtrlStr(ctrl_str, exprs, stmts, expr_type);
}

void ParserStatementCtrlStr::fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs,
  std::vector<CXCursor>& stmts, CodeElementType expr_type)
{
  for (CXCursor& expr : exprs)
  {
    addExpr(ctrl_str, expr, expr_type);
  }

  int depth = ctrl_str->depth_ + 1;
  int index = 0;

  for (CXCursor& stmt : stmts)
  {
    index = ParserStatement::insertStmt(ParserStatement::parse(stmt, ctrl_str->cust_container_, ctrl_str, depth, index),
      ctrl_str->statement_list_);
    index++;
  }
}

void ParserStatementCtrlStr::addExpr(CompositeStatementObject* ctrl_str, CXCursor& expr, CodeElementType expr_type)
{
  std::vector<CXCursor> split_exprs = splitCommaSeparatedExpression(expr);

  for (CXCursor split_expr : split_exprs)
  {
    AbstractExpression* expr = ParserExpression::parseExpression(split_expr, ctrl_str->cust_container_, ctrl_str, expr_type);
    ctrl_str->expression_list_.push_back(expr);
  }
}

std::vector<CXCursor> ParserStatementCtrlStr::splitCommaSeparatedExpression(CXCursor cur_expr)
{
  std::vector<CXCursor> exprs;

  visitBinaryForStmtExpression(cur_expr, clang_getNullCursor(), &exprs);

  return exprs;
}

CXChildVisitResult ParserStatementCtrlStr::visitBinaryForStmtExpression(CXCursor cursor, CXCursor, CXClientData clientData)
{
  if (cursor.kind == CXCursor_UnexposedExpr) //TODO: idk if this is right
  {
    clang_visitChildren(cursor, visitBinaryForStmtExpression, clientData);
    return CXChildVisit_Break;
  }

  std::vector<CXCursor>* exprs = (std::vector<CXCursor>*)clientData;

  if (cursor.kind != CXCursor_BinaryOperator)
  {
    exprs->push_back(cursor);
    return CXChildVisit_Continue;
  }

  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);

  std::string op = ParserExpression::getBinaryOperator(cursor, tokens);

  ParserUtils::freeTokens(tokens);

  if (op != ",")
  {
    exprs->push_back(cursor);
    return CXChildVisit_Continue;
  }

  clang_visitChildren(cursor, visitBinaryForStmtExpression, clientData);

  return CXChildVisit_Continue;
}


void ParserStatementCtrlStr::getCtrlStrExprsAndStmt(CXCursor cursor, std::vector<CXCursor>& exprs,
  CXCursor& stmt, bool expr_first)
{
  std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cursor);

  if (expr_first)
  {
    exprs = std::vector<CXCursor>(children.begin(), children.end() - 1);
    stmt = children[children.size() - 1];
  }
  else
  {
    exprs = std::vector<CXCursor>(children.begin() + 1, children.end());
    stmt = children[0];
  }
}

int ParserStatementCtrlStr::getNumberOfIfExprs(CXCursor cur_if)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cur_if);

  int if_index = 0;
  ParserUtils::findToken(tokens, "if", &if_index);
  assert(tokens[if_index + 1]->token_ == "(");

  int expr_start_index = if_index + 2;
  int paren_open = 1;
  int semicolons_found = 0;

  //find end of if condition
  for (size_t i = expr_start_index; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->token_ == "(")
      paren_open++;
    else if (t->token_ == ")")
      paren_open--;
    else if (t->token_ == ";")
      semicolons_found++;

    if (paren_open == 0)
      break;
  }

  ParserUtils::freeTokens(tokens);

  assert(semicolons_found <= 1);

  //check if there is a simple declaration
  if (semicolons_found == 1)
  {
    std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cur_if);

    for (CXCursor& cur : children)
    {
      if (cur.kind == CXCursor_VarDecl)
        return 3; //init stmt + var decl + decision var
    }

    return 2; //init stmt + some condition
  }

  return 1; //condition
}

void ParserStatementCtrlStr::getIfCtrlStrExprsAndStmts(CXCursor& cur_if, std::vector<CXCursor>& children,
  std::vector<CXCursor>& exprs, std::vector<CXCursor>& stmts)
{
  //if children must be sorted because a simple decl shows up as the first child for some reason
  sortChildren(children);

  int num_exprs = getNumberOfIfExprs(cur_if);

  exprs = std::vector<CXCursor>(children.begin(), children.begin() + num_exprs);
  stmts = std::vector<CXCursor>(children.begin() + num_exprs, children.end());
}

void ParserStatementCtrlStr::sortChildren(std::vector<CXCursor>& children)
{
  struct child
  {
    CXCursor cur_;
    LocationInfo* loc_;
  };

  std::vector<child> children2;

  for (CXCursor cur : children)
  {
    children2.push_back({ cur, ParserUtils::createLocationInfo(cur, CodeElementType::ANNOTATION) });
  }

  std::sort(children2.begin(), children2.end(),
    [] (child const& a, child const& b) { return a.loc_->start_offset_< b.loc_->start_offset_; } );

  children.clear();

  for (child c : children2)
  {
    children.push_back(c.cur_);
    delete c.loc_;
  }
}