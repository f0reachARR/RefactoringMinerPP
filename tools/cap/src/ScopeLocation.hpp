#pragma once

#include "LocationInfo.hpp"

class ScopeLocation
{
  public:
    LocationInfo* location_info_;

    ScopeLocation(LocationInfo* location_info)
      : location_info_(location_info)
    {

    }
};