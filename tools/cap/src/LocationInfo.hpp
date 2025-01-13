#pragma once

#include <string>

#include "CodeElementType.hpp"
#include "Serializable.hpp"

class LocationInfo : public Serializable
{
  public:
    std::string file_path_;
    int start_offset_;
    int end_offset_;
    int length_;
    int start_line_;
    int start_column_;
    int end_line_;
    int end_column_;
    CodeElementType code_element_type_;

    LocationInfo(std::string file_path, int start_offset, int start_line, int start_column, int end_offset,
      int end_line, int end_column, CodeElementType code_element_type);
    LocationInfo(const LocationInfo& l);
    LocationInfo(LocationInfo& l, CodeElementType code_element_type);
    LocationInfo& operator=(const LocationInfo& l);

    json toJson() override;
};