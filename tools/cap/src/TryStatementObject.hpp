#pragma once

#include <vector>

#include "CompositeStatementObject.hpp"

class CompositeStatementObject;

class TryStatementObject : public CompositeStatementObject
{
  public:
    std::vector<CompositeStatementObject*> catch_clauses_;

    TryStatementObject(CompositeStatementObject& c);
    TryStatementObject(CompositeStatementObject& c, LocationInfo* location_info);

    json toJson() override;
};