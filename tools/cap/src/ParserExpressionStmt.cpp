#include "ParserExpressionStmt.hpp"

#include <cassert>

#include "ParserVariableDeclaration.hpp"

struct ParserExprStmtPayload
{
  std::vector<VariableDeclaration*> var_decls_;
  CompositeStatementObject* enclosing_comp_stmt_;
  std::vector<ParserToken*>& decl_stmt_tokens_;
};

std::vector<VariableDeclaration*> ParserExpressionStmt::parseDeclStmt(CXCursor cursor,
  CompositeStatementObject* enclosing_comp_stmt, std::vector<ParserToken*>& tokens)
{
  ParserExprStmtPayload payload = { std::vector<VariableDeclaration*>(), enclosing_comp_stmt, tokens };

  clang_visitChildren(cursor,
                      visitVarDecl,
                      &payload);

  return payload.var_decls_;
}

CXChildVisitResult ParserExpressionStmt::visitVarDecl(CXCursor cursor, CXCursor, CXClientData clientData)
{
  assert(cursor.kind == CXCursor_VarDecl);

  ParserExprStmtPayload* payload = (ParserExprStmtPayload*)clientData;

  VariableDeclaration* var_decl = parseVariableDeclaration(cursor, payload->enclosing_comp_stmt_,
    payload->var_decls_, payload->decl_stmt_tokens_);

  payload->var_decls_.push_back(var_decl);

  return CXChildVisit_Continue;
}

VariableDeclaration* ParserExpressionStmt::parseVariableDeclaration(CXCursor cursor,
  CompositeStatementObject* enclosing_comp_stmt, std::vector<VariableDeclaration*> prev_var_decls,
  std::vector<ParserToken*>& decl_stmt_tokens_)
{
  bool isExpression = isExprDecl(enclosing_comp_stmt->location_info_->code_element_type_);

  VariableDeclaration* var_decl = ParserVariableDeclaration::parseDeclStmtVar(cursor,
    enclosing_comp_stmt->cust_container_, enclosing_comp_stmt, !isExpression, prev_var_decls, decl_stmt_tokens_);

  return var_decl;
}

LocationInfo* ParserExpressionStmt::getVarDeclLocation(CXCursor cursor)
{
  std::string var_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);

  //if the decl name is empty, the variable is unnamed -> take the end of last token as location
  if (var_name.empty())
  {
    LocationInfo* last_token_loc = tokens[tokens.size() - 1]->location_info_;
    LocationInfo* var_decl_loc = new LocationInfo(last_token_loc->file_path_, last_token_loc->end_offset_,
      last_token_loc->end_line_, last_token_loc->end_column_, last_token_loc->end_offset_, last_token_loc->end_line_,
      last_token_loc->end_column_, CodeElementType::SIMPLE_NAME
    );

    ParserUtils::freeTokens(tokens);

    return var_decl_loc;
  }

  //otherwise search for var name token
  ParserToken* var_name_token = nullptr;

  for (ParserToken* t : tokens)
  {
    if (t->token_ == var_name)
    {
      var_name_token = t;
      break;
    }
  }

  assert(var_name_token != nullptr);

  LocationInfo* var_decl_loc = new LocationInfo(*var_name_token->location_info_, CodeElementType::SIMPLE_NAME);

  ParserUtils::freeTokens(tokens);

  return var_decl_loc;
}

bool ParserExpressionStmt::isExprDecl(CodeElementType enclosing_comp_stmt_type)
{
  return false
    || enclosing_comp_stmt_type == CodeElementType::FOR_STATEMENT
    || enclosing_comp_stmt_type == CodeElementType::ENHANCED_FOR_STATEMENT
    || enclosing_comp_stmt_type == CodeElementType::IF_STATEMENT
    || enclosing_comp_stmt_type == CodeElementType::WHILE_STATEMENT
    || enclosing_comp_stmt_type == CodeElementType::DO_STATEMENT;
}