#pragma once

#include <clang-c/Index.h>

#include "UMLClass.hpp"
#include "ParserToken.hpp"

class ParserMethod
{
  public:
    static UMLOperation* parse(CXCursor cursor, UMLClass* uml_class);

  private:
    static void parseMethodParams(CXCursor& cur_method, UMLOperation* op);
    static CXChildVisitResult visitMethodChildren(CXCursor cursor, CXCursor, CXClientData clientData);

    static OperationBody* parseMethodBody(CXCursor cursor, UMLOperation* container);
    static OperationBody* parseFunctionTryBlock(CXCursor cursor, UMLOperation* container);
};