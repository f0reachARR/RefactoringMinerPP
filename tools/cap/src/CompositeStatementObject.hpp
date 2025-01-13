#pragma once

#include <vector>

#include "AbstractStatement.hpp"
#include "AbstractExpression.hpp"
#include "VarDeclCollection.hpp"

class TryStatementObject;
class VariableDeclaration;
class UMLOperation;

class CompositeStatementObject : public AbstractStatement
{
  public:
    std::vector<AbstractStatement*> statement_list_;
    std::vector<AbstractExpression*> expression_list_;
    std::vector<VariableDeclaration*> variable_declarations_;
    TryStatementObject* try_container_;
    LocationInfo* location_info_;

    //custom
    VarDeclCollection cust_var_decls_;
    UMLOperation* cust_container_;

    CompositeStatementObject(LocationInfo* location_info, CompositeStatementObject* parent, UMLOperation* container, int depth, int index);
    CompositeStatementObject(const CompositeStatementObject& c);
    virtual ~CompositeStatementObject();

    //custom
    VariableDeclaration* custGetVarDecl(CXCursor& cursor);

    virtual json toJson() override;
};