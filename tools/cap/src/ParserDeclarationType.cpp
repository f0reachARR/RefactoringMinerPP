#include "ParserDeclarationType.hpp"

#include <cassert>
#include <algorithm>

#include "LeafType.hpp"
#include "ParserUtils.hpp"

UMLType* ParserDeclarationType::getDeclarationType(CXCursor& cursor, std::vector<VariableDeclaration*>& prev_multi_var_decls,
  std::vector<UMLModifier*>& decl_modifiers)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);

  if (tokens.empty()) //libclang cannot parse some var decl e.g.int (*v2)()(nullptr); is valid syntax and compiles but libclang sucks sometimes
    return new LeafType();

  UMLType* type = getDeclarationType(cursor, prev_multi_var_decls, decl_modifiers, tokens);

  ParserUtils::freeTokens(tokens);

  return type;
}

UMLType* ParserDeclarationType::getDeclarationType(CXCursor& cursor, std::vector<VariableDeclaration*>& prev_multi_var_decls,
  std::vector<UMLModifier*>& decl_modifiers, std::vector<ParserToken*>& tokens)
{
  std::string var_name = ParserUtils::toString(clang_getCursorSpelling(cursor));

  filterPrevMultiVarDecls(prev_multi_var_decls, tokens[0]->location_info_);

  std::vector<ParserToken*> type_tokens;
  bool found_initializer = false;
  parseDeclarationType(cursor, tokens, var_name, prev_multi_var_decls, type_tokens, decl_modifiers, found_initializer);

  UMLType* type = parseTypeTokens(type_tokens);
  type->array_dimension_ = getArrayDimension(cursor);
  type->cust_initializer_detected_ = found_initializer;

  return type;
}

UMLType* ParserDeclarationType::getFunctionDeclType(CXCursor& cursor, std::vector<UMLModifier*>& decl_modifiers)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);
  std::vector<VariableDeclaration*> prev_multi_var_decls;

  UMLType* type = getDeclarationType(cursor, prev_multi_var_decls, decl_modifiers, tokens);

  ParserUtils::freeTokens(tokens);

  return type;
}

UMLType* ParserDeclarationType::getNewExprType(CXCursor cursor)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);
  std::vector<ParserToken*> stripped_tokens = getTypeTokensBetween(tokens, "new", "(", true);
  std::vector<ParserToken*> stripped_tokens_no_arr = ParserUtils::getTokensUntil(stripped_tokens, "[");

  UMLType* type = parseTypeTokens(stripped_tokens_no_arr);
  type->array_dimension_ = getArrayDimension(stripped_tokens);

  delete type->location_info_;
  type->location_info_ = ParserUtils::getTokenChainLocation(stripped_tokens, CodeElementType::TYPE);

  ParserUtils::freeTokens(tokens);

  return type;
}

UMLType* ParserDeclarationType::getStaticObjectType(CXCursor cursor)
{
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);
  std::vector<ParserToken*> stripped_tokens = getTypeTokensUntil(tokens, "(");

  UMLType* type = parseTypeTokens(stripped_tokens);
  type->array_dimension_ = getArrayDimension(stripped_tokens);

  ParserUtils::freeTokens(tokens);

  return type;
}

UMLType* ParserDeclarationType::parseTypeTokens(std::vector<ParserToken*>& type_tokens)
{
  size_t current_token = 0;
  std::vector<UMLType*> types = parseTypeTokens(type_tokens, current_token);

  if (types.size() != 1)
    assert(false);

  UMLType* type = types[0];
  type->location_info_ = ParserUtils::getTokenChainLocation(type_tokens, CodeElementType::TYPE);

  return type;
}

std::vector<UMLType*> ParserDeclarationType::parseTypeTokens(std::vector<ParserToken*>& type_tokens, size_t& current_token)
{
  std::vector<ParserToken*> current_tokens;

  LeafType* current_leaf_type = new LeafType();
  std::vector<UMLType*> leaf_types = { current_leaf_type };

  for (; current_token < type_tokens.size(); current_token++)
  {
    std::string token_str = type_tokens[current_token]->token_;

    if (token_str == decl_type_)
    {
      addDeclTypeTokens(type_tokens, current_tokens, current_token);
      continue;
    }

    if (token_str == "<")
    {
      current_token++;

      std::vector<UMLType*> type_args = parseTypeTokens(type_tokens, current_token);
      current_leaf_type->type_arguments_.insert(current_leaf_type->type_arguments_.end(), type_args.begin(), type_args.end());
      current_leaf_type->parameterized_ = true;

      if (type_tokens[current_token]->token_.rfind(">", 0) == 0)
      { //multiple templates closed with this token
        type_tokens[current_token]->token_ = token_str.substr(1, token_str.length() - 1);
        break;
      }

      continue;
    }

    if (token_str == ",")
    {
      current_leaf_type->setClassType(ParserUtils::toStringTokens(current_tokens));

      current_tokens.clear();
      current_leaf_type = new LeafType();
      leaf_types.push_back(current_leaf_type);

      continue;
    }

    if (token_str.rfind(">", 0) == 0) //starts with <
    {
      type_tokens[current_token]->token_ = token_str.substr(1, token_str.length() - 1);
      break;
    }

    current_tokens.push_back(type_tokens[current_token]);
  }

  current_leaf_type->setClassType(ParserUtils::toStringTokens(current_tokens));

  return leaf_types;
}

void ParserDeclarationType::addDeclTypeTokens(std::vector<ParserToken*>& decl_stmt_tokens,
  std::vector<ParserToken*>& decl_type_tokens, size_t& i)
{
  decl_type_tokens.push_back(decl_stmt_tokens[i]); //decltype token
  i++;

  assert(decl_stmt_tokens[i]->token_ == "(");
  decl_type_tokens.push_back(decl_stmt_tokens[i]);
  i++;

  int paren_open = 1;

  for (; i < decl_stmt_tokens.size(); i++)
  {
    ParserToken* t = decl_stmt_tokens[i];

    decl_type_tokens.push_back(t);

    if (t->token_ == "(")
    {
      paren_open++;
    }
    else if (t->token_ == ")")
    {
      paren_open--;

      if (paren_open == 0)
        break;
    }
  }
}

int ParserDeclarationType::getArrayDimension(CXCursor cursor)
{
  CXType type = clang_getCursorType(cursor);

  if (type.kind != CXType_ConstantArray && type.kind != CXType_VariableArray)
    return 0;

  std::string spelling = ParserUtils::toString(clang_getTypeSpelling(type));

  size_t count_brackets = std::count_if(spelling.begin(), spelling.end(), [](char c){ return c == '['; });

  return count_brackets;
}

int ParserDeclarationType::getArrayDimension(std::vector<ParserToken*> tokens)
{
  std::string str = ParserUtils::toStringTokens(tokens);

  size_t count_brackets = std::count_if(str.begin(), str.end(), [](char c){ return c == '['; });

  return count_brackets;
}

std::vector<ParserToken*> ParserDeclarationType::getTypeTokensBetween(std::vector<ParserToken*>& tokens,
  std::string begin, std::string end, bool allow_open_end)
{
  std::vector<ParserToken*> type_tokens;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i]->token_ == begin)
    {
      for (i = i + 1; i < tokens.size(); i++)
      {
        if (tokens[i]->token_ == decl_type_)
        {
          addDeclTypeTokens(tokens, type_tokens, i);
          continue;
        }

        if (tokens[i]->token_ == end)
          return type_tokens;

        type_tokens.push_back(tokens[i]);
      }
    }
  }

  if (!allow_open_end)
    type_tokens.clear();

  return type_tokens;
}

std::vector<ParserToken*> ParserDeclarationType::getTypeTokensUntil(std::vector<ParserToken*>& tokens, std::string end)
{
  std::vector<ParserToken*> type_tokens;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i]->token_ == decl_type_)
    {
      addDeclTypeTokens(tokens, type_tokens, i);
      continue;
    }

    if (tokens[i]->token_ == end)
      break;

    type_tokens.push_back(tokens[i]);
  }

  return type_tokens;
}

void ParserDeclarationType::parseDeclarationType(CXCursor& cursor, std::vector<ParserToken*>& tokens, std::string& var_name,
  std::vector<VariableDeclaration*>& prev_multi_var_decls, std::vector<ParserToken*>& type_tokens,
  std::vector<UMLModifier*>& modifiers, bool& found_initializer)
{
  bool is_multi_var_decl = !prev_multi_var_decls.empty();
  std::string first_var_name = is_multi_var_decl ? prev_multi_var_decls[0]->variable_name_ : var_name;

  bool var_name_found = false;

  size_t i;
  for (i = 0; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->token_ == var_name)
    {
      var_name_found = true;
      break;
    }

    if (t->token_ == decl_type_)
    {
      addDeclTypeTokens(tokens, type_tokens, i);
      continue;
    }

    if (t->token_ == "<")
    {
      addGenericTypeTokens(tokens, type_tokens, i);
      continue;
    }

    if (isModifierKeyword(t->token_))
    {
      modifiers.push_back(new UMLModifier(t->token_, new LocationInfo(*t->location_info_, CodeElementType::MODIFIER)));
      continue;
    }

    if (isTypeKeyword(t->token_))
    {
      type_tokens.push_back(t);
      continue;
    }

    //if this section of the for loop is reached that means the current token is not recognized

    if (is_multi_var_decl && t->token_ == prev_multi_var_decls[0]->variable_name_)
    {
      skipPrevMultiVarDecls(tokens, prev_multi_var_decls, i);
      is_multi_var_decl = false; //reset flag and pretend there never was a multi decl
      first_var_name = var_name;
      continue;
    }

    if (isBeginningOfDeclarator(tokens, first_var_name, i))
    {
      if (is_multi_var_decl)
      {
        skipPrevMultiVarDecls(tokens, prev_multi_var_decls, i);
        is_multi_var_decl = false;
        first_var_name = var_name;
        continue;
      }

      std::vector<UMLModifier*> vec = parseDeclarator(tokens, var_name, var_name_found, i);
      modifiers.insert(modifiers.end(), vec.begin(), vec.end());

      //var name is always found unless its a unnamed param
      if (var_name_found)
        break;

      continue;
    }

    //TODO test this once static var defs can be parsed
    if (std::vector<ParserToken*> qualified_var_name; checkQualifiedVarName(tokens, var_name, i, qualified_var_name))
    {
      modifiers.push_back(new UMLModifier(ParserUtils::toStringTokens(qualified_var_name),
        ParserUtils::getTokenChainLocation(qualified_var_name, CodeElementType::MODIFIER)));

      continue;
    }

    //assume token is part of the type
    type_tokens.push_back(t);
  }

  assert(var_name_found || (cursor.kind == CXCursor_ParmDecl && var_name == "")); //var name always found except for unnamed params

  if (i >= tokens.size())
    return;

  if (cursor.kind == CXCursor_CXXMethod
    || cursor.kind == CXCursor_FunctionDecl
    || cursor.kind == CXCursor_ParmDecl) //if a params last tokens are neither its name nor an initializer, assume its a function passed as a param
                                         //params cannot have an (...) initializer so they can be passed the same way as functions
  {
    bool parse_params = cursor.kind == CXCursor_ParmDecl; //otherwise params are skipped

    std::vector<UMLModifier*> vec = addPostFunctionModifiers(tokens, var_name, parse_params, i);
      modifiers.insert(modifiers.end(), vec.begin(), vec.end());
  }
  else if (cursor.kind == CXCursor_VarDecl || cursor.kind == CXCursor_FieldDecl)
  {
    if (tokens[i]->token_ == var_name)
      i++;

    std::vector<UMLModifier*> vec = addPostFunctionModifiers2(cursor, tokens, found_initializer, i);
      modifiers.insert(modifiers.end(), vec.begin(), vec.end());
  }

  found_initializer = i < tokens.size()
    && (tokens[i]->token_ == "="
      || tokens[i]->token_ == "("
      || tokens[i]->token_ == "{");
}

bool ParserDeclarationType::isModifierKeyword(std::string& str)
{
  return std::find(modifier_kw_.begin(), modifier_kw_.end(), str) != modifier_kw_.end();
}

bool ParserDeclarationType::isTypeKeyword(std::string& str)
{
  return std::find(type_kw_.begin(), type_kw_.end(), str) != type_kw_.end();
}

bool ParserDeclarationType::isBeginningOfDeclarator(std::vector<ParserToken*>& tokens, std::string& var_name, size_t i)
{
  if (std::find(declarator_start_.begin(), declarator_start_.end(), tokens[i]->token_) != declarator_start_.end())
    return true;

  //check if nested-name-specifier
  bool is_scope = false;

  for (i++; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->token_ == var_name)
      return false;

    if (t->token_ == "::")
      is_scope = true;
    else
    {
      if (!is_scope) //make sure there is only one token between ::
        return false;

      if (t->token_ == "*" && is_scope)
        return true;

      is_scope = false;
    }
  }

  assert(false); //var name should be reached in for loop
}

std::vector<UMLModifier*> ParserDeclarationType::parseDeclarator(std::vector<ParserToken*>& tokens, std::string& var_name,
  bool& var_name_found, size_t& i)
{
  std::vector<UMLModifier*> modifiers;
  std::vector<ParserToken*> current_modifier;

  int paren_open = 0;

  for (; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->token_ == var_name)
    {
      var_name_found = true;
      continue;
    }

    current_modifier.push_back(t);

    if (t->token_ == "=" && !var_name_found) //this is probably an unnamed param with default initializer
    {
      var_name_found = true;

      if (paren_open == 0)
      {
        i = tokens.size(); //ignore all remaining tokens, there is nothing after an = that needs to be parsed now
        break;
      }
    }

    if (var_name_found && paren_open == 0)
      break;

    if (t->token_ == "(" || t->token_ == "[")
    {
      paren_open++;
      continue;
    }

    if (t->token_ == ")" || t->token_ == "]")
      paren_open--;

    if (t->token_ == "noexcept" && i + 1 < tokens.size() && tokens[i + 1]->token_ == "(")
      continue;

    if (paren_open == 0
      && t->token_ != "::" //combine everything joined with :: into one modifier
      && current_modifier[current_modifier.size() - 1]->token_ != "::"
      && t->token_ != decl_type_)
    {
      modifiers.push_back(new UMLModifier(ParserUtils::toStringTokens(current_modifier),
        ParserUtils::getTokenChainLocation(current_modifier, CodeElementType::MODIFIER)));

      current_modifier.clear();
    }
  }

  return modifiers;
}

bool ParserDeclarationType::checkQualifiedVarName(std::vector<ParserToken*>& tokens, std::string& var_name, size_t& i,
  std::vector<ParserToken*>& qualified_var_name)
{
  bool is_scope = true;

  for (size_t j = i; j < tokens.size(); j++)
  {
    ParserToken* t = tokens[j];

    qualified_var_name.push_back(t);

    if (t->token_ == "::")
      is_scope = true;
    else
    {
      if (!is_scope)
        return false;

      if (t->token_ == var_name)
      {
        qualified_var_name.pop_back();
        i = j - 1;

        return true;
      }

      is_scope = false;
    }
  }

  return false;
}

void ParserDeclarationType::addGenericTypeTokens(std::vector<ParserToken*>& tokens,
  std::vector<ParserToken*>& type_tokens, size_t& i)
{
  int generic_open = 0;

  for (; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->token_ == decl_type_)
    {
      addDeclTypeTokens(tokens, type_tokens, i); //skips all tokens in decltype(...)
      continue;
    }

    type_tokens.push_back(t);

    if (t->token_ == "<")
    {
      generic_open++;
      continue;
    }

    size_t generic_closed = std::count_if( t->token_.begin(), t->token_.end(), [](char c){ return c == '>'; });
    generic_open -= generic_closed;

    if (generic_open == 0)
      break;
  }
}

void ParserDeclarationType::skipPrevMultiVarDecls(std::vector<ParserToken*>& tokens,
  std::vector<VariableDeclaration*>& prev_multi_var_decls, size_t& i)
{
  int jump_past_offset = prev_multi_var_decls[prev_multi_var_decls.size() - 1]->location_info_->end_offset_;

  for (; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (t->location_info_->start_offset_ >= jump_past_offset)
    {
      assert(t->token_ == ",");
      return;
    }
  }

  assert(false);
}

void ParserDeclarationType::filterPrevMultiVarDecls(std::vector<VariableDeclaration*>& prev_multi_var_decls,
  LocationInfo* first_decl_token_loc)
{
  for (size_t i = 0; i < prev_multi_var_decls.size(); i++)
  {
    VariableDeclaration* v = prev_multi_var_decls[i];

    //do not remove var decl if its type or first modifier have the same start location
    if (v->type_->location_info_->start_offset_ == first_decl_token_loc->start_offset_
      || (!v->modifiers_.empty() && v->modifiers_[0]->location_info_->start_offset_ == first_decl_token_loc->start_offset_))
    {
      continue;
    }

    prev_multi_var_decls.erase(prev_multi_var_decls.begin() + i);
    i--;
  }
}

std::vector<UMLModifier*> ParserDeclarationType::addPostFunctionModifiers(std::vector<ParserToken*>& tokens,
  std::string var_name, bool parse_params, size_t& i)
{
  if (parse_params)
  {
    if (tokens[i]->token_ == var_name)
      i++;

    //at this point i is pointing to the first token after the var name
  }
  else //skip params
  {
    auto [param_start, param_end] = findFuncParamParentheses(tokens, var_name);

    if (param_end > i)
      i = param_end + 1;

    //at this point i is pointing to the first token after the param list

    //there might be closing ). skip those
    while (i < tokens.size() && tokens[i]->token_ == ")")
      i++;
  }

  return parsePostFunctionModifiers(tokens, i);
}

std::vector<UMLModifier*> ParserDeclarationType::parsePostFunctionModifiers(std::vector<ParserToken*>& tokens,
  size_t& i, int early_exit_index)
{
  static const std::vector<std::string> stop = { "{", ";", "=" }; //= for pure virtual functions

  std::vector<UMLModifier*> modifiers;
  std::vector<ParserToken*> current_modifier;

  int paren_open = 0;

  size_t last_token_index = early_exit_index == -1
    ? tokens.size() - 1
    : early_exit_index;

  for (; i <= last_token_index; i++)
  {
    ParserToken* t = tokens[i];

    current_modifier.push_back(t);

    if (paren_open == 0 && std::find(stop.begin(), stop.end(), t->token_) != stop.end())
      break;

    if (t->token_ == "noexcept" && i + 1 < tokens.size() && tokens[i + 1]->token_ == "(")
      continue;

    if (t->token_ == decl_type_)
      continue;

    if (t->token_ == "(" || t->token_ == "[")
    {
      paren_open++;
      continue;
    }

    if (t->token_ == ")" || t->token_ == "]")
      paren_open--;

    if (paren_open == 0)
    {
      modifiers.push_back(new UMLModifier(ParserUtils::toStringTokens(current_modifier),
        ParserUtils::getTokenChainLocation(current_modifier, CodeElementType::MODIFIER)));

      current_modifier.clear();
    }
  }

  return modifiers;
}

std::tuple<size_t, size_t> ParserDeclarationType::findFuncParamParentheses(std::vector<ParserToken*>& tokens,
  std::string var_name)
{
  int var_name_index = -1;

  //find var name index
  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i]->token_ == var_name)
    {
      var_name_index = i;
      break;
    }
  }

  assert(var_name_index > -1);

  size_t param_start_index = var_name_index + 1;

  //skip all closing paren ). idk maybe the function name can be put in parentheses but not the params
  while (param_start_index < tokens.size() && tokens[param_start_index]->token_ == ")")
    param_start_index++;

  //params open right after func name (ignoring closing ))
  assert(param_start_index < tokens.size() && tokens[param_start_index]->token_ == "(");

  size_t param_end_index = param_start_index + 1;
  int paren_open = 1;

  //skip all closing paren )
  while (param_end_index < tokens.size())
  {
    ParserToken* t = tokens[param_end_index];

    if (t->token_ == ")")
      paren_open--;
    else if (t->token_ == "(")
      paren_open++;

    if (paren_open == 0)
      break;

    param_end_index++;
  }

  assert(param_end_index < tokens.size() && tokens[param_end_index]->token_ == ")");

  return { param_start_index, param_end_index };
}

std::vector<UMLModifier*> ParserDeclarationType::addPostFunctionModifiers2(CXCursor& cursor,
  std::vector<ParserToken*>& tokens, bool& found_initializer, size_t& i)
{
  //there might be a closing ) -> skip those
  while (i < tokens.size() && tokens[i]->token_ == ")")
    i++;

  int last_index = determineLastTypeTokenIndex(cursor, tokens, found_initializer, i);

  return parsePostFunctionModifiers(tokens, i, last_index);
}

int ParserDeclarationType::determineLastTypeTokenIndex(CXCursor& cursor, std::vector<ParserToken*>& tokens,
  bool& found_initializer, size_t i)
{
  //check how many parentheses are opened after the var name token
  std::vector<std::pair<size_t, size_t>> paren_pairs = getParenPairs(tokens, i);
  assert(paren_pairs.size() <= 2); //at most two I think

  if (paren_pairs.size() == 1)
  {
    auto [start, end] = paren_pairs[0]; //this might be a param list or an initializer

    if (start + 1 == end //empty parentheses -> this is a param and can be parsed
      || end != tokens.size() - 1 //parentheses are not at the end -> parse as param
      || hasParamChild(cursor)) //if the type has a param child assume its a param
    {
      return tokens.size() - 1; //parse until the end
    }
    else
    {
      found_initializer = true;
      return start - 1; //this parentheses are probably an initializer -> do not parse
    }
  }
  else if (paren_pairs.size() == 2)
  {
    auto [second_start, _] = paren_pairs[1];

    found_initializer = true;
    return second_start - 1; //first parentheses have to be params, second parentheses have to be initializer
  }
  else
    return -1;
}

std::vector<std::pair<size_t, size_t>> ParserDeclarationType::getParenPairs(std::vector<ParserToken*>& tokens, size_t i)
{
  std::vector<std::string> stop = { "=", "{" };

  std::vector<std::pair<size_t, size_t>> paren_pairs;

  int paren_open = 0;

  size_t paren_start_index = 0;

  for (; i < tokens.size(); i++)
  {
    ParserToken* t = tokens[i];

    if (paren_open > 0)
    {
      if (t->token_ == "(")
        paren_open++;
      else if (t->token_ == ")")
      {
        paren_open--;

        if (paren_open == 0)
        {
          paren_pairs.push_back({ paren_start_index, i });
          paren_start_index = 0;
        }
      }

      continue;
    }

    if (std::find(stop.begin(), stop.end(), t->token_) != stop.end())
      break;

    if (t->token_ == "("
      && tokens[i - 1]->token_ != "noexcept"
      && tokens[i - 1]->token_ != "decltype")
    {
      paren_start_index = i;
      paren_open++;

      continue;
    }
  }

  assert(paren_start_index == 0);

  return paren_pairs;
}

bool ParserDeclarationType::hasParamChild(CXCursor& cursor)
{
  std::vector<CXCursor> children = ParserUtils::visitAllFirstLevelChildren(cursor);

  for (CXCursor& c : children)
  {
    if (c.kind == CXCursor_ParmDecl)
      return true;
  }

  return false;
}