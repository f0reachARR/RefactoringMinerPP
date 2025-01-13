#pragma once

#include <string>
#include <vector>

#include "UMLAbstractClass.hpp"
#include "UMLTypeParemter.hpp"
#include "UMLComment.hpp"

class UMLClass : public UMLAbstractClass
{
  public:
    std::string qualified_name_;
    std::string source_file_;
    std::string source_folder_;
    bool is_final_;
    bool is_abstract_;
    bool is_enum_;
    std::vector<UMLTypeParameter*> type_parameters_;

    UMLClass(std::string namesp, std::string name);
    UMLClass(std::string namesp, std::string name, LocationInfo* location_info);
    ~UMLClass();

    static std::string getQualifiedName(std::string namesp, std::string name);

    //custom
    VariableDeclaration* custGetVarDecl(CXCursor& cursor);

    json toJson() override;
};