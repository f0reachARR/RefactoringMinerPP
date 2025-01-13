#pragma once

#include <vector>

#include "Tree.hpp"
#include "TreeMetrics.hpp"

class AbstractTree : public Tree
{
  public:
    Tree tree_;
    std::vector<Tree> children_;
    TreeMetrics metrics_;
};