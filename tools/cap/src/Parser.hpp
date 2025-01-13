#pragma once

#include <string>
#include <vector>

#include <clang-c/Index.h>

#include "UMLModel.hpp"
#include "LocationInfo.hpp"
#include "ParserToken.hpp"

class Parser
{
  public:
    static int parse(std::string input_file, std::string output_file);

    static CXChildVisitResult visitor(CXCursor cursor, CXCursor, CXClientData clientData);

    static UMLClass* parseClass(CXCursor cursor, CXClientData clientData);
    static CXChildVisitResult visitClassChildren(CXCursor cursor, CXCursor, CXClientData clientData);

    static void parseEnum(CXCursor cursor, CXClientData clientData);
    static CXChildVisitResult visitEnumChildren(CXCursor cursor, CXCursor, CXClientData clientData);


    static void parseClassField(CXCursor cursor, UMLClass* uml_class);
    static void parseClassParent(CXCursor cursor, UMLClass* uml_class, UMLModel* model);

    static void parseInclude(CXCursor cursor, CXClientData clientData);

    static void parseClassMethod(CXCursor cursor, UMLClass* uml_class);
};