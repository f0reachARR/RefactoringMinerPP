#pragma once

#include <string>
#include <vector>

#include "UMLType.hpp"
#include "LocationInfo.hpp"

class UMLTypeParameter : public Serializable
{
  public:
    std::string name_;
    std::vector<UMLType*> type_bounds_;
    LocationInfo* location_info_;

    UMLTypeParameter(std::string name, LocationInfo* location_info);
    ~UMLTypeParameter();

  json toJson() override;
};