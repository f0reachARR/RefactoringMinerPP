#include "VarDeclCollection.hpp"

VarDeclCollection& VarDeclCollection::operator=(const VarDeclCollection& v)
{
  var_decls_.insert(var_decls_.end(), v.var_decls_.begin(), v.var_decls_.end());
  return *this;
}

void VarDeclCollection::addVarDecl(VariableDeclaration* var_decl)
{
  var_decls_.push_back(var_decl);
}

VariableDeclaration* VarDeclCollection::findVarDecl(CXCursor& cur_var_decl)
{
  for (VariableDeclaration* var_decl : var_decls_)
  {
    if (clang_equalCursors(var_decl->cust_decl_cursor_, cur_var_decl))
      return var_decl;
  }

  return nullptr;
}

void VarDeclCollection::dropLast()
{
  var_decls_.pop_back();
}