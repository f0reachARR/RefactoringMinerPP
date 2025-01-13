#pragma once

#include <clang-c/Index.h>

#include "StatementObject.hpp"
#include "UMLOperation.hpp"

class ParserStatementFor
{
  public:
    static CompositeStatementObject* parseForStmt(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);
    static CompositeStatementObject* parseEnhancedForStmt(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);

  private:
    static void parseForStmtExpressions(CXCursor cur_for, CompositeStatementObject* for_stmt, std::vector<CXCursor>& cur_for_exprs);

    static CodeElementType getForStmtExpressionType(CXCursor cur_expr, LocationInfo* semicolon1, LocationInfo* semicolon2);
    static void getForStmtSemicolonLocations(CXCursor& cur_for, LocationInfo*& loc1, LocationInfo*& loc2);

    static void parseEnhancedForParam(CXCursor& cur_expr, CompositeStatementObject* for_stmt,
      AbstractExpression* for_stmt_param);

    static VariableDeclaration* getEnhancedForParamVarDecl(CXCursor& cursor, CompositeStatementObject* for_stmt);
};