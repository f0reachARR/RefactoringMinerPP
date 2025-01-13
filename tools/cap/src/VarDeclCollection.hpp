#pragma once

#include <vector>

#include "LocationInfo.hpp"
#include "VariableDeclaration.hpp"

class VarDeclCollection
{
  private:
    std::vector<VariableDeclaration*> var_decls_;

  public:
    VarDeclCollection() = default;
    VarDeclCollection& operator=(const VarDeclCollection& v);

    void addVarDecl(VariableDeclaration* var_decl);
    VariableDeclaration* findVarDecl(CXCursor& cur_var_decl);

    void dropLast();
};