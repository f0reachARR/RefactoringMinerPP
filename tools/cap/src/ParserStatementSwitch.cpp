#include "ParserStatementSwitch.hpp"

#include <cassert>

#include "ParserStatementCtrlStr.hpp"
#include "ParserUtils.hpp"
#include "ParserStatement.hpp"

CompositeStatementObject* ParserStatementSwitch::parseSwitchStmt(CXCursor& cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  CompositeStatementObject* switch_stmt =
    ParserStatementCtrlStr::createCtrlStr(cursor, container, parent, depth, index, CodeElementType::SWITCH_STATEMENT);

  std::vector<CXCursor> exprs;
  CXCursor stmt = clang_getNullCursor();

  ParserStatementCtrlStr::getCtrlStrExprsAndStmt(cursor, exprs, stmt, true);

  ParserStatementCtrlStr::fillCtrlStr(switch_stmt, exprs, CodeElementType::SWITCH_STATEMENT_CONDITION);

  //NOTE: TODO: libclang does not support init stmts in switch stmts
  parseSwitchStmtCases(stmt, switch_stmt, depth + 1);

  return switch_stmt;
}

void ParserStatementSwitch::parseSwitchStmtCases(CXCursor& cursor, CompositeStatementObject* switch_stmt, int depth)
{
  //contains case stmts (expr and stmt combined) and break stmts
  std::vector<CXCursor> switch_body_children;

  if (cursor.kind == CXCursor_CompoundStmt)
    switch_body_children = ParserUtils::visitAllFirstLevelChildren(cursor);
  else
    switch_body_children.push_back(cursor);

  //contains case exprs, case stmts and break stmts
  std::vector<CXCursor> all_switch_body_stmts = getAllSwitchBodyStmts(switch_body_children);

  int index = 0;

  for (CXCursor& cur : all_switch_body_stmts)
  {
    AbstractStatement* stmt = ParserStatement::parse(cur, switch_stmt->cust_container_, switch_stmt, depth, index);
    index = ParserStatement::insertStmt(stmt, switch_stmt->statement_list_);

    index++;
  }
}

std::vector<CXCursor> ParserStatementSwitch::getAllSwitchBodyStmts(std::vector<CXCursor>& switch_body_children)
{
  std::vector<CXCursor> all_switch_body_stmts;

  for (CXCursor& cursor : switch_body_children)
  {
    if (cursor.kind == CXCursor_CaseStmt || cursor.kind == CXCursor_DefaultStmt)
      addSwitchStmtCaseStmt(cursor, all_switch_body_stmts);
    else if (cursor.kind == CXCursor_BreakStmt)
      all_switch_body_stmts.push_back(cursor);
    else
      assert(false);
  }

  return all_switch_body_stmts;
}

void ParserStatementSwitch::addSwitchStmtCaseStmt(CXCursor& cursor, std::vector<CXCursor>& all_switch_body_stmts)
{
  if (cursor.kind == CXCursor_CaseStmt || cursor.kind == CXCursor_DefaultStmt)
  {
    all_switch_body_stmts.push_back(cursor);
    CXCursor case_stmt = ParserUtils::visitLastChild(cursor);

    addSwitchStmtCaseStmt(case_stmt, all_switch_body_stmts);
  }
  else
  {
    all_switch_body_stmts.push_back(cursor);
  }
}