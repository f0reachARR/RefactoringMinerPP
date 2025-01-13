#pragma once

#include <vector>
#include <iostream>
#include <clang-c/Index.h>

#include "LocationInfo.hpp"
#include "ParserToken.hpp"
#include "UMLModel.hpp"
#include "UMLClass.hpp"

class Payload
{
  public:
    UMLModel* model_;
    std::string namespace_;
    UMLClass* class_;
    std::vector<UMLImport*> includes_;
};

class ParserUtils
{
  public:
    inline static CXTranslationUnit unit = NULL;

    static std::string toString(CXString str);
    static std::string toStringTokens(std::vector<ParserToken*>& tokens);
    static std::string toStringTokens(CXCursor cursor);

    static LocationInfo* createLocationInfo(CXCursor cursor, CodeElementType cet);

    static std::vector<ParserToken*> tokenize(CXCursor cursor);
    static LocationInfo* getTokenLocation(CXToken token);
    static LocationInfo* getTokenChainLocation(std::vector<ParserToken*>& tokens, CodeElementType cde);
    static void freeTokens(std::vector<ParserToken*>& tokens);

    static Payload getPayload(CXClientData clientData);

    static CXCursor visitChild(CXCursor cursor, int n);
    static CXCursor visitLastChild(CXCursor cursor);
    static std::vector<CXCursor> visitAllFirstLevelChildren(CXCursor cursor);
    static CXChildVisitResult visitChild(CXCursor cursor, CXCursor, CXClientData clientData);

    static void printLocation(CXCursor cursor);
    static void printLocation(LocationInfo* location_info);

    static std::vector<ParserToken*> getFirstTokensBetween(std::vector<ParserToken*>& tokens, std::string begin, std::string end, bool allow_open_end);
    static std::vector<ParserToken*> getTokensUntil(std::vector<ParserToken*>& tokens, std::string end);
    static ParserToken* findToken(std::vector<ParserToken*>& tokens, std::string search, int* index = nullptr);
    static CXCursor getRecursiveNthChildOfType(CXCursor cursor, int n, CXCursorKind kind);

    static Visibility getCursorVisiblity(CXCursor cursor);

    static std::string combineTokens(std::vector<ParserToken*>& tokens, int from, int to);
};