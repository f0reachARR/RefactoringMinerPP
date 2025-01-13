#pragma once

#include <string>

#include "AbstractTree.hpp"

class DefaultTree : public AbstractTree
{
  public:
    std::string type_;
    std::string label_;
    int pos_;
    int length_;
};