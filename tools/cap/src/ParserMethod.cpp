#include "ParserMethod.hpp"

#include <algorithm>
#include <cassert>
#include <regex>

#include "ParserUtils.hpp"
#include "ParserDeclarationType.hpp"
#include "ParserVariableDeclaration.hpp"
#include "ParserStatement.hpp"

struct ParserMethodPayload
{
  UMLClass* uml_class_;
  UMLOperation* uml_op_;
};

UMLOperation* ParserMethod::parse(CXCursor cursor, UMLClass* uml_class)
{
  std::string method_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* location_info = ParserUtils::createLocationInfo(cursor, CodeElementType::METHOD_DECLARATION);

  UMLOperation* method = new UMLOperation(uml_class->qualified_name_, method_name, location_info, uml_class);

  method->visibility_ = ParserUtils::getCursorVisiblity(cursor);
  method->is_abstract_ = clang_CXXMethod_isPureVirtual(cursor) > 0;
  method->is_constructor_ = cursor.kind == CXCursor_Constructor;
  method->is_static_ = clang_CXXMethod_isStatic(cursor) > 0;

  if (!method->is_constructor_)
  {
    std::vector<UMLModifier*> modifiers;
    UMLType* ret_type = ParserDeclarationType::getFunctionDeclType(cursor, modifiers);

    method->parameters_.push_back(new UMLParameter("return", ret_type, "return"));

    method->modifiers_.insert(method->modifiers_.end(), modifiers.begin(), modifiers.end());
  }

  ParserMethodPayload payload = { uml_class, method };

  parseMethodParams(cursor, method);

  clang_visitChildren(cursor, visitMethodChildren, &payload);

  return method;
}

void ParserMethod::parseMethodParams(CXCursor& cur_method, UMLOperation* op)
{
  int num_params = clang_Cursor_getNumArguments(cur_method);

  for (int i = 0; i < num_params; i++)
  {
    CXCursor cur_param = clang_Cursor_getArgument(cur_method, i);

    std::string name = ParserUtils::toString(clang_getCursorSpelling(cur_param));

    VariableDeclaration* var_decl = ParserVariableDeclaration::parseParameterDeclaration(cur_param, op);
    UMLType* type = var_decl->type_->copy();

    UMLParameter* param = new UMLParameter(name, type, "in");
    param->variable_declaration_ = var_decl;

    op->parameters_.push_back(param);
  }
}

CXChildVisitResult ParserMethod::visitMethodChildren(CXCursor cursor, CXCursor, CXClientData clientData)
{
  ParserMethodPayload* payload = (ParserMethodPayload*)clientData;

  if (cursor.kind == CXCursor_CXXFinalAttr)
  {
    payload->uml_op_->is_final_ = true;
    payload->uml_op_->modifiers_.push_back(new UMLModifier("final", ParserUtils::createLocationInfo(cursor, CodeElementType::MODIFIER)));
  }
  else if (cursor.kind == CXCursor_TemplateTypeParameter)
  {
    std::string name = ParserUtils::toString(clang_getCursorSpelling(cursor));
    LocationInfo* location_info = ParserUtils::createLocationInfo(cursor, CodeElementType::TYPE_PARAMETER);

    payload->uml_op_->type_parameters_.push_back(new UMLTypeParameter(name, location_info));
  }
  else if (cursor.kind == CXCursor_CompoundStmt)
  {
    payload->uml_op_->operation_body_ = parseMethodBody(cursor, payload->uml_op_);
  }
  else if (cursor.kind == CXCursor_CXXTryStmt)
  {
    payload->uml_op_->operation_body_ = parseFunctionTryBlock(cursor, payload->uml_op_);
  }

  return CXChildVisit_Continue;
}

OperationBody* ParserMethod::parseMethodBody(CXCursor cursor, UMLOperation* container)
{
  CompositeStatementObject* composite_stmt = (CompositeStatementObject*)ParserStatement::parse(cursor, container);

  OperationBody* op_body = new OperationBody(composite_stmt, container, ParserUtils::toStringTokens(cursor));

  return op_body;
}

OperationBody* ParserMethod::parseFunctionTryBlock(CXCursor cursor, UMLOperation* container)
{
  //create artifical composite stmt surrounding function try block
  LocationInfo* loc = ParserUtils::createLocationInfo(cursor, CodeElementType::BLOCK);
  CompositeStatementObject* composite_stmt = new CompositeStatementObject(loc, nullptr, container, 0, 0);

  AbstractStatement* try_stmt = ParserStatement::parse(cursor, container, composite_stmt, 1, 0);
  ParserStatement::insertStmt(try_stmt, composite_stmt->statement_list_);

  OperationBody* op_body = new OperationBody(composite_stmt, container, ParserUtils::toStringTokens(cursor));

  return op_body;
}