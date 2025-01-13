#pragma once

enum class StatementCoverageType
{
  NONE,
  ONLY_CALL,
  RETURN_CALL,
  THROW_CALL,
  CAST_CALL,
  VARIABLE_DECLARATION_INITIALIZER_CALL
};