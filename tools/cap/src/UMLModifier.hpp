#pragma once

#include <string>

#include "LocationInfo.hpp"

class UMLModifier : public Serializable
{
  public:
    std::string keyword_; //done
    LocationInfo* location_info_; //done

    UMLModifier(std::string keyword, LocationInfo* location_info);
    ~UMLModifier();

    json toJson() override;
};