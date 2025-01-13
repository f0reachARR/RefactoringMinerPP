#pragma once

#include "AbstractCodeFragment.hpp"

class CompositeStatementObject;

class AbstractStatement : public AbstractCodeFragment
{
  public:
    CompositeStatementObject* parent_;

    AbstractStatement(CompositeStatementObject* parent, int depth, int index);
    virtual ~AbstractStatement() = default;

    virtual json toJson() override;
};