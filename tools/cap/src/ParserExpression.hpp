#pragma once

#include <clang-c/Index.h>
#include <string>
#include <vector>

#include "AbstractExpression.hpp"
#include "ParserToken.hpp"
#include "ObjectCreation.hpp"
#include "OperationInvocation.hpp"

class ParserExpression
{
  public:
    static AbstractExpression* parseNthChildExpression(CXCursor cursor, VariableDeclarationContainer* container,
      CodeElementType expression_type, int n);

    static AbstractExpression* parseExpression(CXCursor cursor, VariableDeclarationContainer* container,
      CompositeStatementObject* enclosing_comp_stmt, CodeElementType expression_type, int depth, int index);
    static AbstractExpression* parseExpression(CXCursor cursor, VariableDeclarationContainer* container,
      CompositeStatementObject* enclosing_comp_stmt, CodeElementType expression_type);
    static AbstractExpression* parseExpression(CXCursor cursor, VariableDeclarationContainer* container, CodeElementType expression_type);
    
    static std::string getBinaryOperator(CXCursor cursor, std::vector<ParserToken*>& tokens);
    static CXChildVisitResult visitExpression(CXCursor cursor, CXCursor parent, CXClientData clientData);
    static bool isUnaryOperatorPrefix(CXCursor cursor, std::vector<ParserToken*>& tokens);

    static std::string getCastType(std::vector<ParserToken*>& tokens, std::string begin, std::string end);
    static TernaryOperatorExpression* parseTernaryOperator(CXCursor cursor, std::string expr, LocationInfo* loc, VariableDeclarationContainer* container);

    static void parseObjectCreation(CXCursor cursor, ObjectCreation* obj, AbstractExpression* ab_expr);
    static std::vector<std::string> getObjectCreationParameters(CXCursor call_expr);

    static ObjectCreation* parseStaticObjectCreation(CXCursor cursor, VariableDeclarationContainer* container);
    static std::vector<std::string> getStaticObjectCreationParameters(CXCursor call_expr);

    static bool isPossibleOperationCall(CXCursor call_expr);
    static OperationInvocation* parseOperationCall(CXCursor call_expr, LocationInfo* loc, VariableDeclarationContainer* container);
    static void parseFunctionCall(CXCursor call_expr, OperationInvocation* op);
    static void setMethodCallExpressions(CXCursor mem_ref_expr, OperationInvocation* op);
    static void setOperationSubExpressions(std::vector<ParserToken*>& op_expression_tokens, OperationInvocation* op);
    static void setFunctionCallExpressions(CXCursor unexp_expr, OperationInvocation* op);
    static int skipTokensBetweenBrackets(std::vector<ParserToken*>& tokens, int start_index, std::string begin_bracket,
      std::string end_bracket, std::vector<ParserToken*>& skipped_tokens);

    static LeafExpression* parseVariable(CXCursor cursor, std::vector<CXCursor>& parents, VariableDeclarationContainer* container);
    static void sortLeafExpression(std::vector<LeafExpression*>& vec);

    static void addReferencedVar(AbstractExpression* expr, CXCursor cursor_ref_var, CompositeStatementObject* enclosing_cmp_stmt);

    static void parseCaseStmt(AbstractExpression* expr, std::vector<ParserToken*>& tokens);
};