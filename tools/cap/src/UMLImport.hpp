#pragma once

#include <string>

#include "LocationInfo.hpp"

class UMLImport : public Serializable
{
  public:
    std::string name_;
    LocationInfo* location_info_;

    UMLImport(std::string name, LocationInfo* location_info)
      : name_(name), location_info_(location_info)
    {
      
    }

    json toJson() override
    {
      json j;

      j["name"] = name_;
      j["locationInfo"] = toJsonObj(location_info_);

      return j;
    }
};