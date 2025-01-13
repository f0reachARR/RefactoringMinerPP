#pragma once

#include <string>

#include "UMLClass.hpp"

class UMLRealization : public Serializable
{
  public:
    UMLClass* client_;
    std::string supplier_;

    UMLRealization(UMLClass* client, std::string supplier)
      : client_(client),
        supplier_(supplier)
    {
    
    }

    json toJson() override
    {
      json j;

      j["client"] = client_->id_;
      j["supplier"] = supplier_;

      return j;
    }
};