#pragma once

#include <string>

#include "UMLType.hpp"
#include "LocationInfo.hpp"

class LeafType : public UMLType
{
  private:
    std::string class_type_;

  public:
    std::string non_qualified_class_type_;

    LeafType();
    LeafType(LocationInfo* location_info, std::string class_type);

    std::string getClassType();

    void setClassType(std::string str);
    std::string get_simple_name(std::string str);

    std::string toString();
    virtual std::string toQualifiedString() override;

    UMLType* copy() override;

    json toJson() override;
};