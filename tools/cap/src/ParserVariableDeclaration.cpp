#include "ParserVariableDeclaration.hpp"

#include <algorithm>
#include <cassert>

#include "ParserDeclarationType.hpp"
#include "ParserUtils.hpp"
#include "ParserExpression.hpp"
#include "LeafType.hpp"

VariableDeclaration* ParserVariableDeclaration::parseFieldDecl(CXCursor cursor, VariableDeclarationContainer* container,
  std::vector<VariableDeclaration*>& prev_multi_var_decls)
{
  VariableDeclaration* var_decl = parseVariableDeclaration(cursor, CodeElementType::FIELD_DECLARATION, container,
    nullptr, prev_multi_var_decls);
  var_decl->is_attribute_ = true;

  return var_decl;
}

VariableDeclaration* ParserVariableDeclaration::parseParameterDeclaration(CXCursor cursor, VariableDeclarationContainer* container)
{
  VariableDeclaration* var_decl = parseVariableDeclaration(cursor, CodeElementType::SINGLE_VARIABLE_DECLARATION, container,
    nullptr);
  var_decl->is_parameter_ = true;

  return var_decl;
}

VariableDeclaration* ParserVariableDeclaration::parseDeclStmtVar(CXCursor cur_var_decl, VariableDeclarationContainer* container,
  CompositeStatementObject* enclosing_comp_stmt, bool isStatement,
  std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*> decl_stmt_tokens)
{
  CodeElementType cet = isStatement
    ? CodeElementType::VARIABLE_DECLARATION_STATEMENT
    : CodeElementType::VARIABLE_DECLARATION_EXPRESSION;

  return parseVariableDeclaration(cur_var_decl, cet, container, enclosing_comp_stmt,
    prev_multi_var_decls, decl_stmt_tokens);
}

VariableDeclaration* ParserVariableDeclaration::parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
  VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt)
{
  std::vector<VariableDeclaration*> prev_multi_var_decls;

  return parseVariableDeclaration(cursor, cet, container, enclosing_comp_stmt, prev_multi_var_decls);
}

VariableDeclaration* ParserVariableDeclaration::parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
  VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt,
  std::vector<VariableDeclaration*>& prev_multi_var_decls)
{
  std::vector<ParserToken*> tokens;

  return parseVariableDeclaration(cursor, cet, container, enclosing_comp_stmt, prev_multi_var_decls, tokens);
}

VariableDeclaration* ParserVariableDeclaration::parseVariableDeclaration(CXCursor cursor, CodeElementType cet,
  VariableDeclarationContainer* container, CompositeStatementObject* enclosing_comp_stmt,
  std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*>& tokens)
{
  std::string var_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* var_location = ParserUtils::createLocationInfo(cursor, cet);

  std::vector<UMLModifier*> modifiers;

  UMLType* var_type = tokens.empty()
    ? ParserDeclarationType::getDeclarationType(cursor, prev_multi_var_decls, modifiers)
    : ParserDeclarationType::getDeclarationType(cursor, prev_multi_var_decls, modifiers, tokens);

  VariableDeclaration* var_dec = new VariableDeclaration(var_name, var_type, var_location, cursor);
  var_dec->modifiers_.insert(var_dec->modifiers_.end(), modifiers.begin(), modifiers.end());
  var_dec->initializer_ = getInitializer(cursor, container, enclosing_comp_stmt, var_type);
  var_dec->is_final_ = clang_isConstQualifiedType(clang_getCursorType(cursor));

  ScopeLocation scope_loc(container->getLocationInfo());
  setVariableScope(var_dec, &scope_loc);

  return var_dec;
}

VariableDeclaration* ParserVariableDeclaration::parseEnumConstantDeclaration(CXCursor cursor, UMLClass* enum_class)
{
  std::string var_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* var_location = ParserUtils::createLocationInfo(cursor, CodeElementType::ENUM_CONSTANT_DECLARATION);
  UMLType* var_type = new LeafType(nullptr, enum_class->name_);

  VariableDeclaration* var_dec = new VariableDeclaration(var_name, var_type, var_location, cursor);
  var_dec->is_enum_constant_ = true;

  ScopeLocation scope(enum_class->location_info_);
  setVariableScope(var_dec, &scope);

  return var_dec;
}

AbstractExpression* ParserVariableDeclaration::getInitializer(CXCursor cursor, VariableDeclarationContainer* container,
  CompositeStatementObject* enclosing_comp_stmt, UMLType* var_type)
{
  if (var_type->cust_initializer_detected_)
  {
    CXCursor last_child = ParserUtils::visitLastChild(cursor);

    if (clang_isExpression(last_child.kind) && last_child.kind != CXCursor_ParmDecl)
      return ParserExpression::parseExpression(last_child, container, enclosing_comp_stmt, CodeElementType::VARIABLE_DECLARATION_INITIALIZER);
  }

  return nullptr;
}

void ParserVariableDeclaration::setVariableScope(VariableDeclaration* var_dec, ScopeLocation* scope_loc)
{
  if (var_dec->location_info_->code_element_type_ == CodeElementType::FIELD_DECLARATION)
  {
    var_dec->scope_ = new VariableScope();
    var_dec->scope_->file_path_ = scope_loc->location_info_->file_path_;
    var_dec->scope_->start_line_ = scope_loc->location_info_->start_line_;
    var_dec->scope_->start_column_ = scope_loc->location_info_->start_column_;
    var_dec->scope_->start_offset_ = scope_loc->location_info_->start_offset_;
    var_dec->scope_->end_line_ = scope_loc->location_info_->end_line_;
    var_dec->scope_->end_column_ = scope_loc->location_info_->end_column_;
    var_dec->scope_->end_offset_ = scope_loc->location_info_->end_offset_;
  }
  else
  {
    var_dec->scope_ = new VariableScope();
    var_dec->scope_->file_path_ = scope_loc->location_info_->file_path_;
    var_dec->scope_->start_line_ = var_dec->location_info_->start_line_;
    var_dec->scope_->start_column_ = var_dec->location_info_->start_column_;
    var_dec->scope_->start_offset_ = var_dec->location_info_->start_offset_;
    var_dec->scope_->end_line_ = scope_loc->location_info_->end_line_;
    var_dec->scope_->end_column_ = scope_loc->location_info_->end_column_;
    var_dec->scope_->end_offset_ = scope_loc->location_info_->end_offset_;
  }
}
