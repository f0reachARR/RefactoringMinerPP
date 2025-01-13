#pragma once

#include <clang-c/Index.h>

#include "CompositeStatementObject.hpp"


class ParserStatementSwitch
{
  public:
    static CompositeStatementObject* parseSwitchStmt(CXCursor& cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);

  private:
    static void parseSwitchStmtCases(CXCursor& cursor, CompositeStatementObject* switch_stmt, int depth);
    static std::vector<CXCursor> getAllSwitchBodyStmts(std::vector<CXCursor>& switch_body_children);
    static void addSwitchStmtCaseStmt(CXCursor& cursor, std::vector<CXCursor>& all_switch_body_stmts);
};