#include "ParserStatement.hpp"

#include "ParserUtils.hpp"
#include "ParserExpression.hpp"
#include "ConverterExpression.hpp"
#include "ParserVariableDeclaration.hpp"
#include "ParserStatementFor.hpp"
#include "ParserStatementCtrlStr.hpp"
#include "ParserStatementSwitch.hpp"
#include "TryStatementObject.hpp"

#include <cassert>

struct ParserStmtPayload
{
  UMLOperation* container_;
  CompositeStatementObject* parent_;
  int depth_;
  int index_;
};

AbstractStatement* ParserStatement::parse(CXCursor cursor, UMLOperation* container)
{
  return parse(cursor, container, nullptr, 0, 0);
}

AbstractStatement* ParserStatement::parse(CXCursor cursor, UMLOperation* container, CompositeStatementObject* parent,
  int depth, int index)
{
  if (cursor.kind == CXCursor_CompoundStmt)
    return parseCompositeStatement(cursor, container, parent, depth, index, CodeElementType::BLOCK);
  else if (cursor.kind == CXCursor_LabelStmt)
    return parseCompositeStatement(cursor, container, parent, depth, index, CodeElementType::LABELED_STATEMENT);
  else if (cursor.kind == CXCursor_CXXTryStmt)
    return parseTryStatement(cursor, container, parent, depth, index);
  else
    return parseStatement(cursor, container, parent, depth, index);
}

CompositeStatementObject* ParserStatement::parseCompositeStatement(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index, CodeElementType cet)
{
  LocationInfo* loc_info = ParserUtils::createLocationInfo(cursor, cet);

  CompositeStatementObject* composite_stmt = new CompositeStatementObject(loc_info, parent, container, depth, index);

  ParserStmtPayload payload = { container, composite_stmt, depth + 1, 0 };

  clang_visitChildren(cursor,
                      visitCompositeStmtChildren,
                      &payload);

  return composite_stmt;
}

CXChildVisitResult ParserStatement::visitCompositeStmtChildren(CXCursor cursor, CXCursor, CXClientData clientData)
{
  ParserStmtPayload* payload = (ParserStmtPayload*)clientData;

  AbstractStatement* stmt = parse(cursor, payload->container_, payload->parent_, payload->depth_, payload->index_);

  payload->index_ = insertStmt(stmt, payload->parent_->statement_list_);
  payload->index_++;

  return CXChildVisit_Continue;
}

AbstractStatement* ParserStatement::parseStatement(CXCursor cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  AbstractStatement* stmt;
  bool is_comp_stmt = true;

  if (cursor.kind == CXCursor_ForStmt)
    stmt = ParserStatementFor::parseForStmt(cursor, container, parent, depth, index);
  else if (cursor.kind == CXCursor_CXXForRangeStmt)
    stmt = ParserStatementFor::parseEnhancedForStmt(cursor, container, parent, depth, index);
  else if (cursor.kind == CXCursor_IfStmt)
    stmt = ParserStatementCtrlStr::parseIfStmt(cursor, container, parent, depth, index);
  else if (cursor.kind == CXCursor_WhileStmt)
    stmt = ParserStatementCtrlStr::parseWhileStmt(cursor, container, parent, depth, index);
  else if (cursor.kind == CXCursor_DoStmt)
    stmt = ParserStatementCtrlStr::parseDoStmt(cursor, container, parent, depth, index);
  else if (cursor.kind == CXCursor_SwitchStmt)
    stmt = ParserStatementSwitch::parseSwitchStmt(cursor, container, parent, depth, index);
  else
  {
    AbstractExpression* expr = ParserExpression::parseExpression(cursor, container, parent, getStmtType(cursor), depth, index);
    stmt = ConverterExpression::toStatementObject(expr);

    is_comp_stmt = false;
  }

  if (is_comp_stmt)
    registerExprVarRefs((CompositeStatementObject*)stmt);

  return stmt;
}

CodeElementType ParserStatement::getStmtType(CXCursor& cursor)
{
  switch (cursor.kind)
  {
    case CXCursor_DeclStmt:
      return CodeElementType::VARIABLE_DECLARATION_STATEMENT;

    case CXCursor_CaseStmt:
    case CXCursor_DefaultStmt:
      return CodeElementType::SWITCH_CASE;

    case CXCursor_BreakStmt:
      return CodeElementType::BREAK_STATEMENT;

    case CXCursor_ContinueStmt:
      return CodeElementType::CONTINUE_STATEMENT;

    case CXCursor_ReturnStmt:
      return CodeElementType::RETURN_STATEMENT;

    case CXCursor_NullStmt:
      return CodeElementType::EMPTY_STATEMENT;

    default:
      return CodeElementType::EXPRESSION_STATEMENT;
  }
}

void ParserStatement::registerExprVarRefs(CompositeStatementObject* cs)
{
  for (AbstractExpression* expr : cs->expression_list_)
  {
    for (VariableDeclaration* var : expr->cust_referenced_vars_)
    {
      if (var->location_info_->code_element_type_ != CodeElementType::VARIABLE_DECLARATION_EXPRESSION)
        var->scope_->statements_in_scope_using_variable_.insert(cs);
    }
  }
}

TryStatementObject* ParserStatement::parseTryStatement(CXCursor& cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cursor);
  assert(children.size() >= 2);

  CompositeStatementObject* try_body = parseCompositeStatement(children[0], container, parent, depth, index,
    CodeElementType::BLOCK);

  LocationInfo* try_loc = ParserUtils::createLocationInfo(cursor, CodeElementType::TRY_STATEMENT);
  TryStatementObject* try_stmt = new TryStatementObject(*try_body, try_loc);
  delete try_body;

  for (size_t i = 1; i < children.size(); i++)
  {
    index++;
    CompositeStatementObject* catch_stmt = parseCatchStmt(children[i], container, parent, depth, index);

    try_stmt->catch_clauses_.push_back(catch_stmt);
    catch_stmt->try_container_ = try_stmt;
  }

  return try_stmt;
}

CompositeStatementObject* ParserStatement::parseCatchStmt(CXCursor& cursor, UMLOperation* container,
  CompositeStatementObject* parent, int depth, int index)
{
  std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cursor);
  assert(children.size() <= 2);

  int comp_stmt_index = children.size() == 2 ? 1 : 0;

  CompositeStatementObject* catch_stmt = parseCompositeStatement(children[comp_stmt_index], container, parent, depth,
    index, CodeElementType::CATCH_CLAUSE);
  delete catch_stmt->location_info_;
  catch_stmt->location_info_ = ParserUtils::createLocationInfo(cursor, CodeElementType::CATCH_CLAUSE);

  //parse catch expression
  if (children.size() == 2)
  {
    AbstractExpression* expr = ParserExpression::parseExpression(children[0], container, catch_stmt,
      CodeElementType::CATCH_CLAUSE_EXCEPTION_NAME, depth, index);

    catch_stmt->expression_list_.push_back(expr);
  }

  return catch_stmt;
}

int ParserStatement::insertStmt(AbstractStatement* stmt, std::vector<AbstractStatement*>& dest)
{
  int index = stmt->index_;

  dest.push_back(stmt);

  TryStatementObject* try_stmt = dynamic_cast<TryStatementObject*>(stmt);

  if (try_stmt == nullptr)
    return index;

  dest.insert(dest.end(), try_stmt->catch_clauses_.begin(), try_stmt->catch_clauses_.end());

  return index + try_stmt->catch_clauses_.size();
}
