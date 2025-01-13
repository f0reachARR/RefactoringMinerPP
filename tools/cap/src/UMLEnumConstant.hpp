#pragma once

#include <vector>
#include <string>

#include "UMLAttribute.hpp"

class UMLEnumConstant : public UMLAttribute
{
  public:
    std::vector<std::string> arguments_;

    UMLEnumConstant(std::string name, LocationInfo* location_info)
      : UMLAttribute(name, location_info)
    {

    }

    json toJson() override
    {
      json j = UMLAttribute::toJson();

      j["arguments"] = arguments_;

      return j;
    }
};