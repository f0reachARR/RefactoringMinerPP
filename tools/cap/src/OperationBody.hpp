#pragma once

#include <string>
#include <vector>
#include <set>

#include "CompositeStatementObject.hpp"
#include "VariableDeclaration.hpp"
#include "VariableDeclarationContainer.hpp"


class OperationBody : public Serializable
{
  public:
    CompositeStatementObject* composite_statement_;
    VariableDeclarationContainer* container_;
    std::string stringified_;

    OperationBody(CompositeStatementObject* composite_statement_, VariableDeclarationContainer* container_,
      std::string stringified);
    ~OperationBody();

  json toJson() override;
};