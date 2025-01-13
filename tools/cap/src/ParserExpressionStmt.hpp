#pragma once

#include <clang-c/Index.h>
#include <vector>

#include "VariableDeclaration.hpp"
#include "CompositeStatementObject.hpp"
#include "ParserUtils.hpp"

class ParserExpressionStmt
{
  public:
    static std::vector<VariableDeclaration*> parseDeclStmt(CXCursor cursor,
      CompositeStatementObject* enclosing_comp_stmt, std::vector<ParserToken*>& tokens);
    static LocationInfo* getVarDeclLocation(CXCursor cursor);

  private:
    static CXChildVisitResult visitVarDecl(CXCursor cursor, CXCursor, CXClientData clientData);
    static bool isExprDecl(CodeElementType enclosing_comp_stmt_type);
    static VariableDeclaration* parseVariableDeclaration(CXCursor cursor,
      CompositeStatementObject* enclosing_comp_stmt, std::vector<VariableDeclaration*> prev_var_decls,
      std::vector<ParserToken*>& decl_stmt_tokens_);
};