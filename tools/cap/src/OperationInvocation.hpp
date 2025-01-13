#pragma once

#include <string>
#include <vector>

#include "AbstractCall.hpp"

class OperationInvocation : public AbstractCall
{
  public:
    std::string method_name_;
    std::vector<std::string> sub_expressions_;

    OperationInvocation(std::string str, LocationInfo* location_info, VariableDeclarationContainer* container)
      : AbstractCall(str, location_info, container)
    {
      
    }

    json toJson() override
    {
      json j = AbstractCall::toJson();

      j["@type"] = "OperationInvocation";
      j["methodName"] = method_name_;
      j["subExpressions"] = sub_expressions_;

      return j;
    }
};