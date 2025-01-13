#pragma once

#include <clang-c/Index.h>

#include "AbstractStatement.hpp"
#include "UMLOperation.hpp"

class ParserStatementCtrlStr
{
  public:
    static CompositeStatementObject* parseIfStmt(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);
    static CompositeStatementObject* parseWhileStmt(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);
    static CompositeStatementObject* parseDoStmt(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);

    static void getCtrlStrExprsAndStmt(CXCursor cursor, std::vector<CXCursor>& exprs, CXCursor& stmt, bool expr_first);

    static CompositeStatementObject* createCtrlStr(CXCursor& cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index, CodeElementType ctrl_str_type);

    static void fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs, CXCursor& stmt,
      CodeElementType expr_type);
    static void fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs,
      std::vector<CXCursor>& stmts, CodeElementType expr_type);
     static void fillCtrlStr(CompositeStatementObject* ctrl_str, std::vector<CXCursor>& exprs,
      CodeElementType expr_type);

    static std::vector<CXCursor> splitCommaSeparatedExpression(CXCursor cur_expr);

    static void addExpr(CompositeStatementObject* ctrl_str, CXCursor& expr, CodeElementType expr_type);

  private:
    static CXChildVisitResult visitBinaryForStmtExpression(CXCursor cursor, CXCursor, CXClientData clientData);

    static int getNumberOfIfExprs(CXCursor cur_if);
    static void getIfCtrlStrExprsAndStmts(CXCursor& cur_if, std::vector<CXCursor>& children,
      std::vector<CXCursor>& exprs, std::vector<CXCursor>& stmts);

    static void sortChildren(std::vector<CXCursor>& children);
};