#pragma once

#include "AbstractExpression.hpp"
#include "StatementObject.hpp"


class ConverterExpression
{
  public:
    static StatementObject* toStatementObject(AbstractExpression* expr);
};
