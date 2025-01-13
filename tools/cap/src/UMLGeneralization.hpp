#pragma once

#include <string>

#include "UMLClass.hpp"

class UMLGeneralization : public Serializable
{
  public:
    UMLClass* child_;
    std::string parent_;

    UMLGeneralization(UMLClass* child, std::string parent)
      : child_(child),
        parent_(parent)
    {
      
    }

    json toJson() override
    {
      json j;

      j["child"] = child_->id_;
      j["parent"] = parent_;

      return j;
    }
};