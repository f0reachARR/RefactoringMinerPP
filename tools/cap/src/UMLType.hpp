#pragma once

#include <vector>

#include "LocationInfo.hpp"
#include "Serializable.hpp"

class UMLType : public Serializable
{
  public:
    LocationInfo* location_info_;
    int array_dimension_;
    bool parameterized_;
    std::vector<UMLType*> type_arguments_;

    UMLType();
    UMLType(LocationInfo* location_info);
    virtual ~UMLType();

    std::string typeAgumentsToString();
    std::string typeArgumentsAndArrayDimensionToString();

    virtual std::string toQualifiedString() = 0;

    virtual UMLType* copy() = 0;

    virtual json toJson() override;

    //custom
    bool cust_initializer_detected_;
};