#include "UMLModel.hpp"

UMLModel::UMLModel()
{
  partial_ = false;
}

UMLModel::~UMLModel()
{
  for (auto c : class_list_)
    delete c;

  for (auto g : generalization_list_)
    delete g;

  for (auto r : realization_list_)
    delete r;

  for (auto& t : tree_map_)
    delete t.second;
}

json UMLModel::toJson()
{
  json j;

  j["repositoryDirectories"] = repository_directories_;
  j["classList"] = toJsonArray<UMLClass>(class_list_);
  j["generalizationList"] = toJsonArray<UMLGeneralization>(generalization_list_);
  j["realizationList"] = toJsonArray<UMLRealization>(realization_list_);
  j["partial"] = partial_;

  return j;
}