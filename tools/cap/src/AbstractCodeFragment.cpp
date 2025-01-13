#include "AbstractCodeFragment.hpp"

AbstractCodeFragment::AbstractCodeFragment(int depth, int index)
  : id_(++global_code_fragment_counter_),
    depth_(depth),
    index_(index)
{

}

AbstractCodeFragment::AbstractCodeFragment()
  : AbstractCodeFragment(0, 0)
{

}

json AbstractCodeFragment::toJson()
{
  json j;

  j["id"] = id_;
  j["depth"] = depth_;
  j["index"] = index_;

  return j;
}