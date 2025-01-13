#pragma once

#include <string>
#include <set>

#include "AbstractCodeFragment.hpp"

class VariableScope : public Serializable
{
  public:
    std::string file_path_;
    int start_offset_;
    int end_offset_;
    int start_line_;
    int start_column_;
    int end_line_;
    int end_column_;
    std::set<AbstractCodeFragment*> statements_in_scope_using_variable_;

    json toJson() override
    {
      json j;

      j["filePath"] = file_path_;
      j["startOffset"] = start_offset_;
      j["endOffset"] = end_offset_;
      j["startLine"] = start_line_;
      j["startColumn"] = start_column_;
      j["endLine"] = end_line_;
      j["endColumn"] = end_column_;

      std::vector<int> stmts(statements_in_scope_using_variable_.size());

      std::transform(statements_in_scope_using_variable_.begin(), statements_in_scope_using_variable_.end(),
        stmts.begin(), [](AbstractCodeFragment* s) { return s->id_; });

      j["statementsInScopeUsingVariable"] = stmts;

      return j;
    }
};