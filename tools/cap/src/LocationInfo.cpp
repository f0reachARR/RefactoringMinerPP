#include "LocationInfo.hpp"

LocationInfo::LocationInfo(std::string file_path, int start_offset, int start_line, int start_column, int end_offset,
  int end_line, int end_column, CodeElementType code_element_type)
  : file_path_(file_path),
    start_offset_(start_offset),
    end_offset_(end_offset),
    start_line_(start_line),
    start_column_(start_column),
    end_line_(end_line),
    end_column_(end_column),
    code_element_type_(code_element_type)
{
  length_ = end_offset_ - start_offset_;
}

LocationInfo::LocationInfo(const LocationInfo& l)
  : LocationInfo(l.file_path_, l.start_offset_, l.start_line_, l.start_column_, l.end_offset_,
      l.end_line_, l.end_column_, l.code_element_type_)
{

}

LocationInfo::LocationInfo(LocationInfo& l, CodeElementType code_element_type)
  : LocationInfo(l)
{
  code_element_type_ = code_element_type;
}

LocationInfo& LocationInfo::operator=(const LocationInfo& l)
{
  file_path_ = l.file_path_;
  start_offset_ = l.start_offset_;
  start_line_ = l.start_line_;
  start_column_ = l.start_column_;
  end_offset_ = l.end_offset_;
  end_line_ = l.end_line_;
  end_column_ = l.end_column_;
  code_element_type_ = l.code_element_type_;
  length_ = l.length_;

  return *this;
}


json LocationInfo::toJson()
{
  json j;

  j["filePath"] = file_path_;
  j["startOffset"] = start_offset_;
  j["endOffset"] = end_offset_;
  j["length"] = length_;
  j["startLine"] = start_line_;
  j["startColumn"] = start_column_;
  j["endLine"] = end_line_;
  j["endColumn"] = end_column_;
  j["codeElementType"] = code_element_type_;

  return j;
}