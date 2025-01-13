#include "AbstractCall.hpp"

AbstractCall::AbstractCall(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container)
  : LeafExpression(str, location_info, container)
{
  number_of_arguments_ = 0;
  coverage_ = StatementCoverageType::NONE;
}

json AbstractCall::toJson()
{
  json j = LeafExpression::toJson();

  j["numberOfArguments"] = number_of_arguments_;
  j["expression"] = expression_;
  j["arguments"] = arguments_;

  return j;
}