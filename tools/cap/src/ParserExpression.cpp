#include "ParserExpression.hpp"

#include <cassert>
#include <algorithm>

#include "ParserUtils.hpp"
#include "LeafType.hpp"
#include "ParserDeclarationType.hpp"
#include "ParserVariableDeclaration.hpp"
#include "ParserExpressionStmt.hpp"

struct ParserExprPayload
{
  VariableDeclarationContainer* container_;
  CodeElementType expression_type_;
  AbstractExpression* expression_;
  std::vector<CXCursor> parents_;
  CompositeStatementObject* enclosing_comp_stmt_;
  int depth_;
  int index_;
};

AbstractExpression* ParserExpression::parseNthChildExpression(CXCursor cursor, VariableDeclarationContainer* container,
  CodeElementType expression_type, int n)
{
  CXCursor child = ParserUtils::visitChild(cursor, n);

  if (clang_Cursor_isNull(child))
    return nullptr;

  return parseExpression(child, container, expression_type);
}

AbstractExpression* ParserExpression::parseExpression(CXCursor cursor, VariableDeclarationContainer* container,
  CompositeStatementObject* enclosing_comp_stmt, CodeElementType expression_type)
{
  if (enclosing_comp_stmt == nullptr)
    return parseExpression(cursor, container, expression_type);

  return parseExpression(cursor, container, enclosing_comp_stmt, expression_type,
    enclosing_comp_stmt->depth_, enclosing_comp_stmt->index_);
}

AbstractExpression* ParserExpression::parseExpression(CXCursor cursor, VariableDeclarationContainer* container,
  CompositeStatementObject* enclosing_comp_stmt, CodeElementType expression_type, int depth, int index)
{
  ParserExprPayload payload =
  {
    container,
    expression_type,
    nullptr,
    std::vector<CXCursor>(),
    enclosing_comp_stmt,
    depth,
    index
  };

  visitExpression(cursor, clang_getNullCursor(), &payload);

  sortLeafExpression(payload.expression_->variables_);

  return payload.expression_;
}

AbstractExpression* ParserExpression::parseExpression(CXCursor cursor, VariableDeclarationContainer* container, CodeElementType expression_type)
{
  return parseExpression(cursor, container, nullptr, expression_type, 0, 0);
}

CXChildVisitResult ParserExpression::visitExpression(CXCursor cursor, CXCursor parent, CXClientData clientData)
{
  ParserExprPayload* payload = (ParserExprPayload*)clientData;

  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);

  if (tokens.empty())
    return CXChildVisit_Continue;

  std::string expr_str = ParserUtils::toStringTokens(tokens);
  LocationInfo* expr_loc = ParserUtils::createLocationInfo(cursor, CodeElementType::ANNOTATION); //change loc cet

  LeafExpression* leaf = new LeafExpression(expr_str, new LocationInfo(*expr_loc), payload->container_);

  if (payload->expression_ == nullptr)
  {
    LocationInfo* expr_loc = ParserUtils::createLocationInfo(cursor, payload->expression_type_);

    payload->expression_ = new AbstractExpression(expr_str, expr_loc, payload->enclosing_comp_stmt_,
      payload->depth_, payload->index_);
  }

  if (cursor.kind == CXCursor_BinaryOperator)
  {
    std::string op = getBinaryOperator(cursor, tokens);

    if (op == "=")
      delete leaf;
    else
    {
      leaf->location_info_->code_element_type_ = CodeElementType::INFIX_EXPRESSION;
      payload->expression_->infix_expressions_.push_back(leaf);
      payload->expression_->infix_operators_.push_back(op);
    }
  }
  else if (cursor.kind == CXCursor_IntegerLiteral || cursor.kind == CXCursor_FloatingLiteral)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::NUMBER_LITERAL;
    payload->expression_->number_literals_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_StringLiteral)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::STRING_LITERAL;
    payload->expression_->string_literals_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_CharacterLiteral)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::CHAR_LITERAL;
    payload->expression_->char_literals_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_CXXBoolLiteralExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::BOOLEAN_LITERAL;
    payload->expression_->boolean_literals_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_CXXNullPtrLiteralExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::NULL_LITERAL;
    payload->expression_->null_literals_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_ParenExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::PARENTHESIZED_EXPRESSION;
    payload->expression_->parenthesized_expressions_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_UnaryOperator)
  {
    if (isUnaryOperatorPrefix(cursor, tokens))
    {
      leaf->location_info_->code_element_type_ = CodeElementType::PREFIX_EXPRESSION;
      payload->expression_->prefix_expressions_.push_back(leaf);
    }
    else
    {
      leaf->location_info_->code_element_type_ = CodeElementType::POSTFIX_EXPRESSION;
      payload->expression_->postfix_expressions_.push_back(leaf);
    }
  }
  else if (cursor.kind == CXCursor_CStyleCastExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::CAST_EXPRESSION;
    payload->expression_->cast_expressions_.push_back(leaf);

    std::string type = getCastType(tokens, "(", ")");

    if (type != "")
      payload->expression_->types_.push_back(type);
  }
  else if (cursor.kind == CXCursor_CXXStaticCastExpr
    || cursor.kind == CXCursor_CXXDynamicCastExpr
    || cursor.kind == CXCursor_CXXConstCastExpr
    || cursor.kind == CXCursor_CXXReinterpretCastExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::CAST_EXPRESSION;
    payload->expression_->cast_expressions_.push_back(leaf);

    std::string type = getCastType(tokens, "<", ">");

    if (type != "")
      payload->expression_->types_.push_back(type);
  }
  else if (cursor.kind == CXCursor_ConditionalOperator)
  {
    payload->expression_->ternary_operator_expressions_.push_back(parseTernaryOperator(cursor, expr_str, new LocationInfo(*expr_loc), payload->container_));
    delete leaf;
  }
  else if (cursor.kind == CXCursor_ArraySubscriptExpr)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::ARRAY_ACCESS;
    payload->expression_->array_accesses_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_CXXNewExpr)
  {
    ObjectCreation* obj = new ObjectCreation(expr_str, new LocationInfo(*expr_loc), payload->container_);
    parseObjectCreation(cursor, obj, payload->expression_);

    payload->expression_->creations_.push_back(obj);
  }
  else if (cursor.kind == CXCursor_TypeRef)
  {
    //if the parent cursor is a new expr and the first child of the parent is a template type,
    // the current cursor is probably a type used in the generic. Do not add such types to the type list
    bool is_generic = ((parent.kind == CXCursor_CXXNewExpr || parent.kind == CXCursor_CallExpr || parent.kind == CXCursor_CXXFunctionalCastExpr)
      && ParserUtils::visitChild(parent, 1).kind == CXCursor_TemplateRef);

    if (!is_generic)
      payload->expression_->types_.push_back(ParserUtils::toStringTokens(cursor));

    delete leaf;
  }
  else if (cursor.kind == CXCursor_TemplateRef)
  {
    CXCursor parent_first_child = ParserUtils::visitChild(parent, 1);

    //if the parent cursor is a new expr (or static obj) and the first child of the parent is a template ref but not the current cursor,
    // that means that the current cursor is probably a type used in the generic. Do not add such types to the type list
    bool is_generic_type = ((parent.kind == CXCursor_CXXNewExpr || parent.kind == CXCursor_CallExpr || parent.kind == CXCursor_CXXFunctionalCastExpr)
      && parent_first_child.kind == CXCursor_TemplateRef
      && !clang_equalCursors(cursor, parent_first_child));

    if (!is_generic_type)
    {
      UMLType* uml_type = parent.kind == CXCursor_CXXNewExpr ? ParserDeclarationType::getNewExprType(parent) : ParserDeclarationType::getStaticObjectType(parent);
      LeafType* type = dynamic_cast<LeafType*>(uml_type);

      payload->expression_->types_.push_back(type->toString());
      delete type;
    }

    delete leaf;
  }
  else if (cursor.kind == CXCursor_CallExpr)
  {
    ObjectCreation* obj = parseStaticObjectCreation(cursor, payload->container_);

    if (obj != nullptr)
      payload->expression_->creations_.push_back(obj);
    else
    {
      OperationInvocation* op = parseOperationCall(cursor, new LocationInfo(*expr_loc), payload->container_);

      if (op != nullptr)
        payload->expression_->method_invocations_.push_back(op);
    }

    delete leaf;
  }
  else if (cursor.kind == CXCursor_CXXThisExpr)
  {
    payload->expression_->this_expressions_.push_back(leaf);
  }
  else if (cursor.kind == CXCursor_MemberRefExpr || cursor.kind == CXCursor_DeclRefExpr)
  {
    LeafExpression* var = parseVariable(cursor, payload->parents_, payload->container_);

    if (var != nullptr)
      payload->expression_->variables_.push_back(var);

    addReferencedVar(payload->expression_, cursor, payload->enclosing_comp_stmt_);

    delete leaf;
  }
  else if (cursor.kind == CXCursor_LabelRef)
  {
    leaf->location_info_->code_element_type_ = CodeElementType::SIMPLE_NAME;
    payload->expression_->variables_.push_back(leaf);
  }

  ////////////////////////////////////////////////////////////////////////////////
  //statement cursor types
  ////////////////////////////////////////////////////////////////////////////////
  else if (cursor.kind == CXCursor_DeclStmt)
  {
    std::vector<VariableDeclaration*> var_decls = ParserExpressionStmt::parseDeclStmt(cursor,
      payload->enclosing_comp_stmt_, tokens);
    assert(!var_decls.empty());

    payload->expression_->variable_declarations_.insert(payload->expression_->variable_declarations_.end(),
      var_decls.begin(), var_decls.end());

    payload->expression_->types_.push_back(var_decls[0]->type_->toQualifiedString());

    for (VariableDeclaration* var_decl : var_decls)
      payload->enclosing_comp_stmt_->cust_var_decls_.addVarDecl(var_decl);

    delete leaf;
  }
  else if (cursor.kind == CXCursor_VarDecl)
  {
    LocationInfo* loc = ParserExpressionStmt::getVarDeclLocation(cursor);
    std::string var_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
    LeafExpression* l = new LeafExpression(var_name, loc, payload->container_);

    payload->expression_->variables_.push_back(l);

    delete leaf;

    //simple declarations have no decl stmt parent but still a var decl needs to be registered
    if (parent.kind != CXCursor_DeclStmt)
    {
      VariableDeclaration* var_decl = ParserVariableDeclaration::parseVariableDeclaration(cursor,
        CodeElementType::VARIABLE_DECLARATION_EXPRESSION, payload->container_, payload->enclosing_comp_stmt_);

      payload->enclosing_comp_stmt_->cust_var_decls_.addVarDecl(var_decl);

      if (payload->expression_type_ == CodeElementType::CATCH_CLAUSE_EXCEPTION_NAME)
      {
        var_decl->location_info_->code_element_type_ = CodeElementType::SINGLE_VARIABLE_DECLARATION;
        payload->enclosing_comp_stmt_->variable_declarations_.push_back(var_decl);
      }
      else
        payload->expression_->variable_declarations_.push_back(var_decl);
    }

    //var decls in for each loops need special treatment
    if (payload->expression_type_ == CodeElementType::ENHANCED_FOR_STATEMENT_PARAMETER_NAME)
    {
      //param in for each loop does not include type -> remove from expr string and location info
      payload->expression_->expression_ = var_name;

      LocationInfo* new_expr_loc = new LocationInfo(*loc, CodeElementType::ENHANCED_FOR_STATEMENT_PARAMETER_NAME);

      delete payload->expression_->location_info_;
      payload->expression_->location_info_ = new_expr_loc;

      return CXChildVisit_Break;
    }
    else if (payload->expression_type_ == CodeElementType::CATCH_CLAUSE_EXCEPTION_NAME)
    {
      delete l;

      payload->expression_->expression_ = var_name;
      payload->expression_->variables_.clear();
    }
  }
  else if (cursor.kind == CXCursor_CaseStmt || cursor.kind == CXCursor_DefaultStmt)
  {
    delete leaf;

    if (parent.kind == CXCursor_CaseStmt || parent.kind == CXCursor_DefaultStmt)
      return CXChildVisit_Break;

    parseCaseStmt(payload->expression_, tokens);
  }
  else
  {
    delete leaf;
  }

  delete expr_loc;

  ParserUtils::freeTokens(tokens);

  std::vector<CXCursor> parents(payload->parents_);
  parents.insert(parents.begin(), cursor);

  ParserExprPayload new_payload =
  {
    payload->container_,
    payload->expression_type_,
    payload->expression_,
    std::vector<CXCursor>(payload->parents_),
    payload->enclosing_comp_stmt_,
    payload->depth_,
    payload->index_
  };

  new_payload.parents_.insert(new_payload.parents_.begin(), cursor);

  clang_visitChildren(cursor, visitExpression, &new_payload);

  return CXChildVisit_Continue;
}

std::string ParserExpression::getBinaryOperator(CXCursor cursor, std::vector<ParserToken*>& tokens)
{
  CXCursor lhs_cursor = ParserUtils::visitChild(cursor, 1);
  assert(!clang_Cursor_isNull(lhs_cursor));

  std::vector<ParserToken*> lhs_tokens = ParserUtils::tokenize(lhs_cursor);
  ParserUtils::freeTokens(lhs_tokens);

  return tokens[lhs_tokens.size()]->token_;
}

bool ParserExpression::isUnaryOperatorPrefix(CXCursor cursor, std::vector<ParserToken*>& tokens)
{
  CXCursor expr = ParserUtils::visitChild(cursor, 1);
  assert(!clang_Cursor_isNull(expr));

  std::vector<ParserToken*> expr_tokens = ParserUtils::tokenize(expr);

  bool isPrefix = expr_tokens[0]->token_ != tokens[0]->token_;

  ParserUtils::freeTokens(expr_tokens);

  return isPrefix;
}

std::string ParserExpression::getCastType(std::vector<ParserToken*>& tokens, std::string begin, std::string end)
{
  std::vector<ParserToken*> type_tokens = ParserDeclarationType::getTypeTokensBetween(tokens, begin, end);

  if (type_tokens.size() >= 1)
    return ParserUtils::toStringTokens(type_tokens);

  return "";
}

TernaryOperatorExpression* ParserExpression::parseTernaryOperator(CXCursor cursor, std::string expr, LocationInfo* loc,
  VariableDeclarationContainer* container)
{
  AbstractExpression* cond_expr = parseNthChildExpression(cursor, container, CodeElementType::TERNARY_OPERATOR_CONDITION, 1);
  AbstractExpression* then_expr = parseNthChildExpression(cursor, container, CodeElementType::TERNARY_OPERATOR_THEN_EXPRESSION, 2);
  AbstractExpression* else_expr = parseNthChildExpression(cursor, container, CodeElementType::TERNARY_OPERATOR_ELSE_EXPRESSION, 3);

  return new TernaryOperatorExpression(expr, loc, container, cond_expr, then_expr, else_expr);
}

void ParserExpression::parseObjectCreation(CXCursor cursor, ObjectCreation* obj, AbstractExpression* ab_expr)
{
  CXCursor call_expr = ParserUtils::getRecursiveNthChildOfType(cursor, 1, CXCursor_CallExpr);

  LeafType* type = dynamic_cast<LeafType*>(ParserDeclarationType::getNewExprType(cursor));

  obj->uml_type_ = type;
  obj->is_array_ = type->array_dimension_ > 0;

  if (clang_Cursor_isNull(call_expr)) //cursor has no call expr, probably primitive type ??
  {
    if (obj->is_array_)
      obj->location_info_->code_element_type_ = CodeElementType::ARRAY_CREATION;
    else
      obj->location_info_->code_element_type_ = CodeElementType::CREATION_REFERENCE; //??

    ab_expr->types_.push_back(type->toString()); //manually add this to abstract expr, the primitive type is not an AST node, it will not be visited

    for (CXCursor child : ParserUtils::visitAllFirstLevelChildren(cursor))
      obj->arguments_.push_back(ParserUtils::toStringTokens(child));
  }
  else
  {
    obj->location_info_->code_element_type_ = CodeElementType::CLASS_INSTANCE_CREATION;

    std::vector<std::string> parameters = getObjectCreationParameters(call_expr);

    obj->arguments_.insert(obj->arguments_.end(), parameters.begin(), parameters.end());
    obj->number_of_arguments_ = parameters.size();
    // ab_expr->arguments_.insert(ab_expr->arguments_.end(), parameters.begin(), parameters.end());
    //not sure if this needs to be added to the abstract expr as well. Refactoring miner is inconsistent in this regard
  }
}

std::vector<std::string> ParserExpression::getObjectCreationParameters(CXCursor call_expr)
{
  std::vector<CXCursor> parameters = ParserUtils::visitAllFirstLevelChildren(call_expr);
  std::vector<std::string> parameters_str;

  for (CXCursor param : parameters)
  {
    std::vector<ParserToken*> tokens = ParserUtils::tokenize(param);

    parameters_str.push_back(ParserUtils::toStringTokens(tokens));

    ParserUtils::freeTokens(tokens);
  }

  return parameters_str;
}

ObjectCreation* ParserExpression::parseStaticObjectCreation(CXCursor cursor, VariableDeclarationContainer* container)
{
  CXCursor first_child_l1 = ParserUtils::visitChild(cursor, 1);

  if (clang_Cursor_isNull(first_child_l1) || first_child_l1.kind != CXCursor_UnexposedExpr)
    return nullptr;

  CXCursor first_child_l2 = ParserUtils::visitChild(first_child_l1, 1);

  if (clang_Cursor_isNull(first_child_l2))
    return nullptr;

  CXCursor call_expr = first_child_l2;

  if (first_child_l2.kind == CXCursor_CXXFunctionalCastExpr) //this is probably a static object creation with exactly one param
  {
    CXCursor call_expr_l3 = ParserUtils::visitLastChild(first_child_l2);

    if (!clang_Cursor_isNull(call_expr_l3) && call_expr_l3.kind == CXCursor_CallExpr)
      call_expr = call_expr_l3;
  }

  if (call_expr.kind == CXCursor_CallExpr) //this is probably a static object creation with none or more than one params
  {
    std::vector<ParserToken*> tokens = ParserUtils::tokenize(call_expr);

    std::string expr_str = ParserUtils::toStringTokens(tokens);
    LocationInfo* expr_loc = ParserUtils::getTokenChainLocation(tokens, CodeElementType::CLASS_INSTANCE_CREATION);

    ObjectCreation* obj = new ObjectCreation(expr_str, expr_loc, container);
    obj->uml_type_ = ParserDeclarationType::getStaticObjectType(call_expr);

    std::vector<std::string> parameters = getStaticObjectCreationParameters(call_expr);

    obj->arguments_.insert(obj->arguments_.end(), parameters.begin(), parameters.end());
    obj->number_of_arguments_ = parameters.size();

    ParserUtils::freeTokens(tokens);

    return obj;
  }

  return nullptr;
}

std::vector<std::string> ParserExpression::getStaticObjectCreationParameters(CXCursor call_expr)
{
  std::vector<CXCursor> parameters = ParserUtils::visitAllFirstLevelChildren(call_expr);
  std::vector<std::string> parameters_str;

  for (CXCursor param : parameters)
  {
    if (param.kind == CXCursor_TypeRef || param.kind == CXCursor_TemplateRef)
      continue;

    std::vector<ParserToken*> tokens = ParserUtils::tokenize(param);

    parameters_str.push_back(ParserUtils::toStringTokens(tokens));

    ParserUtils::freeTokens(tokens);
  }

  return parameters_str;
}

bool ParserExpression::isPossibleOperationCall(CXCursor call_expr)
{
  std::string op_name = ParserUtils::toString(clang_getCursorSpelling(call_expr));

  if (op_name == "")
    return false;

  std::vector<ParserToken*> tokens = ParserUtils::tokenize(call_expr);

  bool hasParams = tokens.size() > 0 && tokens[tokens.size() - 1]->token_ == ")";

  ParserUtils::freeTokens(tokens);

  return hasParams;
}

OperationInvocation* ParserExpression::parseOperationCall(CXCursor call_expr, LocationInfo* loc, VariableDeclarationContainer* container)
{
  if (!isPossibleOperationCall(call_expr))
    return nullptr;

  CXCursor first_child = ParserUtils::visitChild(call_expr, 1);

  if (clang_Cursor_isNull(first_child))
    return nullptr;

  std::string str_expr = ParserUtils::toStringTokens(call_expr);

  OperationInvocation* op = new OperationInvocation(str_expr, loc, container);
  op->method_name_ = ParserUtils::toString(clang_getCursorSpelling(call_expr));

  if (first_child.kind == CXCursor_MemberRefExpr) // this is probably a method invocation of some object
  {
    parseFunctionCall(call_expr, op);
    setMethodCallExpressions(first_child, op);

    return op;
  }
  else if (first_child.kind == CXCursor_UnexposedExpr) //static or globabl function invocation
  {
    CXCursor unexp_first_child = ParserUtils::visitChild(first_child, 1);

    //if this cursor is a call expr, then this is probably some call in a call chain and not relevant
    if (unexp_first_child.kind != CXCursor_CallExpr)
    {
      parseFunctionCall(call_expr, op);
      setFunctionCallExpressions(first_child, op);

      return op;
    }
  }

  delete op;

  return nullptr;
}

void ParserExpression::parseFunctionCall(CXCursor call_expr, OperationInvocation* op)
{
  std::vector<CXCursor> call_expr_children = ParserUtils::visitAllFirstLevelChildren(call_expr);

  //get function params
  for (size_t i = 1; i < call_expr_children.size(); i++) //skip mem ref or unexposed expr
  {
    CXCursor param = call_expr_children[i];

    std::vector<ParserToken*> tokens = ParserUtils::tokenize(param);

    op->arguments_.push_back(ParserUtils::toStringTokens(tokens));

    ParserUtils::freeTokens(tokens);
  }

  op->number_of_arguments_ = op->arguments_.size();
}

void ParserExpression::setMethodCallExpressions(CXCursor mem_ref_expr, OperationInvocation* op)
{
  CXCursor first_child = ParserUtils::visitChild(mem_ref_expr, 1);

  if (clang_Cursor_isNull(first_child))
    return;

  if (first_child.kind == CXCursor_TypeRef) //this is probably a generic type arg
    return;

  std::vector<ParserToken*> tokens = ParserUtils::tokenize(first_child);

  op->expression_ = ParserUtils::toStringTokens(tokens);
  setOperationSubExpressions(tokens, op);

  ParserUtils::freeTokens(tokens);
}

void ParserExpression::setOperationSubExpressions(std::vector<ParserToken*>& op_expression_tokens, OperationInvocation* op)
{
  if (op_expression_tokens.empty())
    return;

  for (int i = op_expression_tokens.size() - 1; i >= 0; i--)
  {
    //if the sub expression does not end with ")" it is not an operation call
    if (op_expression_tokens[i]->token_ != ")")
      break;

    std::vector<ParserToken*> sub_expr;

    i = skipTokensBetweenBrackets(op_expression_tokens, i, "(", ")", sub_expr);

    if (i >= 0 && op_expression_tokens[i]->token_ == ">")
      i = skipTokensBetweenBrackets(op_expression_tokens, i, "<", ">", sub_expr);

    //there is one more token before the opening "(", usually the operation name
    if (i >= 0)
    {
      sub_expr.push_back(op_expression_tokens[i]); //add operation name
      std::reverse(sub_expr.begin(), sub_expr.end());

      op->sub_expressions_.push_back(ParserUtils::toStringTokens(sub_expr));

      i--; //skip token such as ".", "::", "->"
    }
  }
}

int ParserExpression::skipTokensBetweenBrackets(std::vector<ParserToken*>& tokens, int start_index, std::string begin_bracket,
  std::string end_bracket, std::vector<ParserToken*>& skipped_tokens)
{
  int i = start_index;

  for (; i >= 0; i--)
  {
    ParserToken* tok = tokens[i];

    if (tok->token_ == end_bracket)
      break;
  }

  if (i < 0)
    return i;

  ParserToken* tok = tokens[i];

  skipped_tokens.push_back(tok); //add end bracket

  int bracket_closed = 1;
  i--;

  //skip to opening bracket
  while (i >= 0 && bracket_closed > 0)
  {
    tok = tokens[i];

    if (begin_bracket == "<" && end_bracket == ">")
    {
      if (tok->token_ == ")") //skip parentheses, because it might be a decltype expr
      {
        i = skipTokensBetweenBrackets(tokens, i, "(", ")", skipped_tokens);
        continue;
      }
    }

    skipped_tokens.push_back(tok);

    if (tok->token_ == end_bracket)
      bracket_closed++;
    else if (tok->token_ == begin_bracket)
      bracket_closed--;

    i--;
  }

  return i;
}

void ParserExpression::setFunctionCallExpressions(CXCursor unexp_expr, OperationInvocation* op)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(unexp_expr);

  int index_expression_end = 0;

  for (int i = tokens.size() - 1; i >= 0; i--)
  {
    ParserToken* tok = tokens[i];

    if (tok->token_ == op->method_name_)
    {
      index_expression_end = i - 2; //-1 function name, -1 (:: . ->)
      break;
    }
  }

  if (index_expression_end >= 0)
  {
    std::vector<ParserToken*> expression_tokens;
    expression_tokens.insert(expression_tokens.begin(), tokens.begin(), tokens.begin() + index_expression_end + 1);

    op->expression_ = ParserUtils::toStringTokens(expression_tokens);
    setOperationSubExpressions(expression_tokens, op);
  }

  ParserUtils::freeTokens(tokens);
}

LeafExpression* ParserExpression::parseVariable(CXCursor cursor, std::vector<CXCursor>& parents, VariableDeclarationContainer* container)
{
  //TODO: include "this" in variable name. Also check if static vars referenced with this are added twice (this.v & v)
  bool isVariable = true;
  std::string cursor_spelling = ParserUtils::toString(clang_getCursorSpelling(cursor));

  for (CXCursor parent : parents)
  {
    if (parent.kind == CXCursor_UnexposedExpr)
      continue;

    if (parent.kind == CXCursor_CallExpr)
      isVariable = cursor_spelling != ParserUtils::toString(clang_getCursorSpelling(parent));

    break;
  }

  CodeElementType cet = cursor.kind == CXCursor_MemberRefExpr
    ? CodeElementType::FIELD_ACCESS
    : CodeElementType::SIMPLE_NAME;

  return isVariable
    ? new LeafExpression(cursor_spelling, ParserUtils::createLocationInfo(cursor, cet), container)
    : nullptr;
}

void ParserExpression::sortLeafExpression(std::vector<LeafExpression*>& vec)
{
  std::sort(vec.begin(),vec.end(),[](LeafExpression* &a, LeafExpression* &b)
    { return a->location_info_->start_column_< b->location_info_->start_column_; });
}

void ParserExpression::addReferencedVar(AbstractExpression* expr, CXCursor cursor_ref_var, CompositeStatementObject* enclosing_cmp_stmt)
{
  //if the expr is not surrounded by a composite stmt, it is not part of an stmt.
  //Therefore, it will not be registered as a variable reference
  if (enclosing_cmp_stmt == nullptr)
    return;

  CXCursor cursor_decl_var = clang_getCursorReferenced(cursor_ref_var);

  assert(!clang_Cursor_isNull(cursor_decl_var));

  //cannot keep track of external var references
  if (clang_Location_isInSystemHeader(clang_getCursorLocation(cursor_decl_var)))
    return;

  //only references to variables can be recorded
  if (cursor_decl_var.kind == CXCursor_CXXMethod || cursor_decl_var.kind == CXCursor_FunctionDecl)
    return;

  VariableDeclaration* ref_var_decl = enclosing_cmp_stmt->custGetVarDecl(cursor_decl_var);

  if (ref_var_decl != nullptr)
    expr->cust_referenced_vars_.insert(ref_var_decl);
}

void ParserExpression::parseCaseStmt(AbstractExpression* expr, std::vector<ParserToken*>& tokens)
{
  std::vector<ParserToken*> case_tokens;

  for (ParserToken* t : tokens)
  {
    case_tokens.push_back(t);

    if (t->token_ == ":")
      break;
  }

  expr->expression_ = ParserUtils::toStringTokens(case_tokens);
  expr->location_info_ = ParserUtils::getTokenChainLocation(case_tokens, CodeElementType::SWITCH_CASE);
}