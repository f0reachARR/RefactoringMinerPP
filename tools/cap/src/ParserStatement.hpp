#pragma once

#include <clang-c/Index.h>

#include "AbstractStatement.hpp"
#include "UMLOperation.hpp"
#include "StatementObject.hpp"

class ParserStatement
{
  public:
    static AbstractStatement* parse(CXCursor cursor, UMLOperation* container);
    static AbstractStatement* parse(CXCursor cursor, UMLOperation* container, CompositeStatementObject* parent,
      int depth, int index);

    //Use this function to insert statements into the statement list of a composite statement.
    //It returns the abstract statement index of the last inserted statement
    static int insertStmt(AbstractStatement* stmt, std::vector<AbstractStatement*>& dest);

  private:
    static CompositeStatementObject* parseCompositeStatement(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index, CodeElementType cet);
    static CXChildVisitResult visitCompositeStmtChildren(CXCursor cursor, CXCursor, CXClientData clientData);

    static AbstractStatement* parseStatement(CXCursor cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);

    static CodeElementType getStmtType(CXCursor& cursor);

    static void registerExprVarRefs(CompositeStatementObject* cs);

    static TryStatementObject* parseTryStatement(CXCursor& cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);
    static CompositeStatementObject* parseCatchStmt(CXCursor& cursor, UMLOperation* container,
      CompositeStatementObject* parent, int depth, int index);
};