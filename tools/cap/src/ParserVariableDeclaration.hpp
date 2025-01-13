#pragma once

#include <clang-c/Index.h>

#include "VariableDeclaration.hpp"
#include "ParserToken.hpp"
#include "VariableDeclarationContainer.hpp"
#include "ScopeLocation.hpp"
#include "UMLClass.hpp"

class ParserVariableDeclaration
{
  public:
    static VariableDeclaration* parseFieldDecl(CXCursor cursor, VariableDeclarationContainer* container,
      std::vector<VariableDeclaration*>& prev_multi_var_decls);

    static VariableDeclaration* parseParameterDeclaration(CXCursor cursor, VariableDeclarationContainer* container);

    static VariableDeclaration* parseDeclStmtVar(CXCursor cur_var_decl, VariableDeclarationContainer* container,
      CompositeStatementObject* enclosing_comp_stmt, bool isStatement,
      std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*> decl_stmt_tokens);

    static VariableDeclaration* parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
      VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt);

    static VariableDeclaration* parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
      VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt,
      std::vector<VariableDeclaration*>& prev_multi_var_decls);

    static VariableDeclaration* parseEnumConstantDeclaration(CXCursor cursor, UMLClass* enum_class);

  private:
    static VariableDeclaration* parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
      VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt,
      std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*>& tokens);

    static AbstractExpression* getInitializer(CXCursor cursor, VariableDeclarationContainer* container,
      CompositeStatementObject* enclosing_comp_stmt, UMLType* var_type);

    static void setVariableScope(VariableDeclaration* var_dec, ScopeLocation* scope_loc);
};