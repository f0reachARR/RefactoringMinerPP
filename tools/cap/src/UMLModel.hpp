#pragma once

#include <set>
#include <vector>
#include <map>

#include "UMLClass.hpp"
#include "UMLGeneralization.hpp"
#include "UMLRealization.hpp"
#include "Tree.hpp"
#include "UMLComment.hpp"
#include "Serializable.hpp"

class UMLModel : public Serializable
{
  public:
    std::set<std::string> repository_directories_;
    std::vector<UMLClass*> class_list_;
    std::vector<UMLGeneralization*> generalization_list_;
    std::vector<UMLRealization*> realization_list_;
    bool partial_;
    std::map<std::string, Tree*> tree_map_;

    UMLModel();
    ~UMLModel();

    json toJson() override;
};