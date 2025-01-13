#pragma once

class LocationInfoProvider
{
  public:
    virtual LocationInfo* getLocationInfo() = 0;
    virtual ~LocationInfoProvider() = default;
};