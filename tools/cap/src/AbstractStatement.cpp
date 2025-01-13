#include "AbstractStatement.hpp"

#include "CompositeStatementObject.hpp"

AbstractStatement::AbstractStatement(CompositeStatementObject* parent, int depth, int index)
  : AbstractCodeFragment(depth, index),
    parent_(parent)
{

}

json AbstractStatement::toJson()
{
  json j = AbstractCodeFragment::toJson();

  j["parent"] = parent_ != nullptr ? parent_->id_ : -1;

  return j;
}
