#include "OperationBody.hpp"

OperationBody::OperationBody(CompositeStatementObject* composite_statement, VariableDeclarationContainer* container,
  std::string stringified)
  : composite_statement_(composite_statement),
    container_(container),
    stringified_(stringified)
{

}

OperationBody::~OperationBody()
{
  delete composite_statement_;
}

json OperationBody::toJson()
{
  json j;

  j["compositeStatement"] = toJsonObj(composite_statement_);
  j["container"] = container_ != nullptr ? container_->id_ : -1;
  j["stringified"] = stringified_;

  return j;
}