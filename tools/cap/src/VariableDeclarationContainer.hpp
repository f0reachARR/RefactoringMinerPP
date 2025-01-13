#pragma once

#include "LocationInfoProvider.hpp"

class VariableDeclarationContainer : public LocationInfoProvider, public Serializable
{
  private:
    inline static int global_var_decl_container_counter_ = 0;

  public:
    const int id_;

    VariableDeclarationContainer()
      : id_(++global_var_decl_container_counter_)
    {

    }
    virtual ~VariableDeclarationContainer() = default;
};