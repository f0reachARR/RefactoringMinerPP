#include "TryStatementObject.hpp"

TryStatementObject::TryStatementObject(CompositeStatementObject& c)
  : CompositeStatementObject(c)
{
  c.statement_list_.clear();
  c.expression_list_.clear();
  c.variable_declarations_.clear();
  c.try_container_ = nullptr;
  c.location_info_ = nullptr;
}

TryStatementObject::TryStatementObject(CompositeStatementObject& c, LocationInfo* location_info)
  : TryStatementObject(c)
{
  delete location_info_;
  location_info_ = location_info;
}

json TryStatementObject::toJson()
{
  json j = CompositeStatementObject::toJson();

  j["@type"] = "TryStatementObject";

  std::vector<int> c(catch_clauses_.size());
  std::transform(catch_clauses_.begin(), catch_clauses_.end(), c.begin(), [](AbstractCodeFragment* s) { return s->id_; });

  j["catchClauses"] = c;

  return j;
}