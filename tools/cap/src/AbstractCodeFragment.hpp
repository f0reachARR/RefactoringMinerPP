#pragma once

#include <string>

#include "Serializable.hpp"

class AbstractCodeFragment : public Serializable
{
  private:
    inline static int global_code_fragment_counter_ = 0;

  public:
    const int id_;

    int depth_;
    int index_;

    AbstractCodeFragment(int depth, int index);
    AbstractCodeFragment();
    virtual ~AbstractCodeFragment() = default;

    virtual json toJson() override;
};