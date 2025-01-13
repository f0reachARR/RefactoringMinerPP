#pragma once

#include <vector>

#include <clang-c/Index.h>

#include "UMLType.hpp"
#include "ParserToken.hpp"
#include "UMLModifier.hpp"
#include "VariableDeclaration.hpp"

class ParserDeclarationType
{
  public:
    inline static const std::string decl_type_ = "decltype";

    inline static const std::vector<std::string> modifier_kw_ = { "inline", "virtual", "explicit", "friend",
      "constexpr", "register", "static", "thread_local", "extern", "mutable", "const", "volatile" };

    inline static const std::vector<std::string> type_kw_ = { "char", "char16_t", "char32_t", "wchar_t", "bool", "short",
      "int", "long", "signed", "unsigned", "float", "double", "void", "auto" };

    inline static const std::vector<std::string> declarator_start_ = { "...", "*", "&", "&&", "(", "::", "=" }; //include '=' for unnamed params with default initializer

    static UMLType* getDeclarationType(CXCursor& cursor, std::vector<VariableDeclaration*>& prev_multi_var_decls,
      std::vector<UMLModifier*>& decl_modifiers);
    static UMLType* getDeclarationType(CXCursor& cursor, std::vector<VariableDeclaration*>& prev_multi_var_decls,
      std::vector<UMLModifier*>& decl_modifiers, std::vector<ParserToken*>& tokens);

    static UMLType* getFunctionDeclType(CXCursor& cursor, std::vector<UMLModifier*>& decl_modifiers);

    static UMLType* parseTypeTokens(std::vector<ParserToken*>& type_tokens);
    static UMLType* getNewExprType(CXCursor cursor);
    static UMLType* getStaticObjectType(CXCursor cursor);

    static std::vector<ParserToken*> getTypeTokensBetween(std::vector<ParserToken*>& tokens,
      std::string begin, std::string end, bool allow_open_end = false);
    static std::vector<ParserToken*> getTypeTokensUntil(std::vector<ParserToken*>& tokens, std::string end);

  private:
    static std::vector<UMLType*> parseTypeTokens(std::vector<ParserToken*>& type_tokens, size_t& current_token);

    static void addDeclTypeTokens(std::vector<ParserToken*>& decl_stmt_tokens,
      std::vector<ParserToken*>& decl_type_tokens, size_t& i);

    static int getArrayDimension(CXCursor cursor);
    static int getArrayDimension(std::vector<ParserToken*> tokens);

    static void parseDeclarationType(CXCursor& cursor, std::vector<ParserToken*>& tokens, std::string& var_name,
      std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*>& type_tokens,
      std::vector<UMLModifier*>& modifiers, bool& found_initializer);

    static bool isModifierKeyword(std::string& str);
    static bool isTypeKeyword(std::string& str);

    static bool isBeginningOfDeclarator(std::vector<ParserToken*>& tokens, std::string& var_name, size_t i);
    static std::vector<UMLModifier*> parseDeclarator(std::vector<ParserToken*>& tokens, std::string& var_name,
      bool& var_name_found, size_t& i);

    static bool checkQualifiedVarName(std::vector<ParserToken*>& tokens, std::string& var_name, size_t& i,
      std::vector<ParserToken*>& qualified_var_name);

    static void addGenericTypeTokens(std::vector<ParserToken*>& tokens,
      std::vector<ParserToken*>& type_tokens, size_t& i);

    static void skipPrevMultiVarDecls(std::vector<ParserToken*>& tokens,
      std::vector<VariableDeclaration*>& prev_multi_var_decls, size_t& i);
    static void filterPrevMultiVarDecls(std::vector<VariableDeclaration*>& prev_multi_var_decls,
      LocationInfo* first_decl_token_loc);

    static std::vector<UMLModifier*> addPostFunctionModifiers(std::vector<ParserToken*>& tokens,
      std::string var_name, bool parse_params, size_t& i);
    static std::vector<UMLModifier*> parsePostFunctionModifiers(std::vector<ParserToken*>& tokens,
      size_t& i, int early_exit_index = -1);
    static std::tuple<size_t, size_t> findFuncParamParentheses(std::vector<ParserToken*>& tokens, std::string var_name);

    //use this function to parse var decls
    static std::vector<UMLModifier*> addPostFunctionModifiers2(CXCursor& cursor,
      std::vector<ParserToken*>& tokens, bool& found_initializer, size_t& i);
    static int determineLastTypeTokenIndex(CXCursor& cursor, std::vector<ParserToken*>& tokens,
      bool& found_initializer, size_t i);
    static std::vector<std::pair<size_t, size_t>> getParenPairs(std::vector<ParserToken*>& tokens, size_t i);
    static bool hasParamChild(CXCursor& cursor);
};