#include "ParserUtils.hpp"

#include <cassert>

struct VistiChildPlayload
{
  int n;
  CXCursor child_cursor;
  CXCursorKind kind_type;
  std::vector<CXCursor> history;
};

std::vector<ParserToken*> ParserUtils::tokenize(CXCursor cursor)
{
  std::vector<ParserToken*> vec_tokens;

  CXSourceRange range = clang_getCursorExtent(cursor);

  CXToken* tokens = 0;
  unsigned int num_tokens;
  clang_tokenize(unit, range, &tokens, &num_tokens);

  for (unsigned int i = 0; i < num_tokens; ++i)
  {
    std::string token = toString(clang_getTokenSpelling(unit, tokens[i]));
    LocationInfo* loc = getTokenLocation(tokens[i]);

    vec_tokens.push_back(new ParserToken(token, loc));
  }

  clang_disposeTokens(unit, tokens, num_tokens);

  return vec_tokens;
}

LocationInfo* ParserUtils::getTokenLocation(CXToken token)
{
  CXFile file;
  unsigned int line;
  unsigned int column;
  unsigned int offset;

  CXSourceLocation loc = clang_getTokenLocation(unit, token);

  clang_getFileLocation(loc, &file, &line, &column, &offset);

  unsigned int line2;
  unsigned int column2;
  unsigned int offset2;

  CXSourceLocation loc2 = clang_getRangeEnd(clang_getTokenExtent(unit, token));

  clang_getFileLocation(loc2, NULL, &line2, &column2, &offset2);

  return new LocationInfo(toString(clang_getFileName(file)), offset, line, column, offset2, line2, column2, CodeElementType::ANNOTATION);
}

std::string ParserUtils::toStringTokens(std::vector<ParserToken*>& tokens)
{
  std::string tokens_str = tokens[0]->token_;

  for (size_t i = 1; i < tokens.size(); i++)
  {
    int spaces_num = tokens[i]->location_info_->start_line_ == tokens[i - 1]->location_info_->start_line_
      ? tokens[i]->location_info_->start_offset_ - tokens[i - 1]->location_info_->end_offset_ //same line
      : 1; //different lines

    std::string spaces(spaces_num, ' ');

    tokens_str += spaces + tokens[i]->token_;
  }

  return tokens_str;
}

LocationInfo* ParserUtils::createLocationInfo(CXCursor cursor, CodeElementType cet)
{
  CXSourceLocation loc = clang_getCursorLocation(cursor);

  CXFile file;
  unsigned int start_line;
  unsigned int start_column;
  unsigned int start_offset;

  clang_getFileLocation(loc, &file, &start_line, &start_column, &start_offset);

  unsigned int end_line;
  unsigned int end_column;
  unsigned int end_offset;

  CXSourceLocation loc2 = clang_getRangeEnd(clang_getCursorExtent(cursor));

  clang_getFileLocation(loc2, NULL, &end_line, &end_column, &end_offset);

  return new LocationInfo(ParserUtils::toString(clang_getFileName(file)), start_offset, start_line, start_column,
    end_offset, end_line, end_column, cet);
}

std::string ParserUtils::toString(CXString str)
{
  std::string result = clang_getCString(str);

  clang_disposeString(str);

  return result;
}

std::string ParserUtils::toStringTokens(CXCursor cursor)
{
  std::vector<ParserToken*> tokens = tokenize(cursor);

  std::string str = toStringTokens(tokens);

  freeTokens(tokens);

  return str;
}

Payload ParserUtils::getPayload(CXClientData clientData)
{
  Payload p = *((Payload*)clientData);
  return p;
}

LocationInfo* ParserUtils::getTokenChainLocation(std::vector<ParserToken*>& tokens, CodeElementType cde)
{
  if (tokens.size() == 1)
  {
    LocationInfo* loc = new LocationInfo(*tokens[0]->location_info_);
    loc->code_element_type_ = cde;
    return loc;
  }

  LocationInfo* first_token_loc = tokens[0]->location_info_;
  LocationInfo* last_token_loc = tokens[tokens.size() - 1]->location_info_;

  return new LocationInfo(first_token_loc->file_path_,
    first_token_loc->start_offset_, first_token_loc->start_line_, first_token_loc->start_column_,
    last_token_loc->end_offset_, last_token_loc->end_line_, last_token_loc->end_column_,
    cde);
}

void ParserUtils::freeTokens(std::vector<ParserToken*>& tokens)
{
  for (auto a : tokens)
    delete a;
}

CXCursor ParserUtils::visitChild(CXCursor cursor, int n)
{
  VistiChildPlayload payload = { n, clang_getNullCursor(), (CXCursorKind)-1, {} };

  clang_visitChildren(cursor,
                      ParserUtils::visitChild,
                      &payload);

  return payload.child_cursor;
}

CXCursor ParserUtils::visitLastChild(CXCursor cursor)
{
  VistiChildPlayload payload = { -1, clang_getNullCursor(), (CXCursorKind)-1, {} };

  clang_visitChildren(cursor,
                      ParserUtils::visitChild,
                      &payload);

  return payload.child_cursor;
}

std::vector<CXCursor> ParserUtils::visitAllFirstLevelChildren(CXCursor cursor)
{
  VistiChildPlayload payload = { -2, clang_getNullCursor(), (CXCursorKind)-1, {} };

  clang_visitChildren(cursor,
                      ParserUtils::visitChild,
                      &payload);

  return payload.history;
}

CXChildVisitResult ParserUtils::visitChild(CXCursor cursor, CXCursor, CXClientData clientData)
{
  VistiChildPlayload* payload = (VistiChildPlayload*)clientData;

  if (payload->kind_type != (CXCursorKind)-1)
  {
    if (cursor.kind == payload->kind_type)
    {
      payload->child_cursor = cursor;
      return CXChildVisit_Break;
    }

    return CXChildVisit_Recurse;
  }

  if (payload->n == -2)
  {
    payload->history.push_back(cursor);
    return CXChildVisit_Continue;
  }

  if (payload->n == -1)
  {
    payload->child_cursor = cursor;
    return CXChildVisit_Continue;
  }

  if (payload->n <= 1)
  {
    payload->child_cursor = cursor;
    return CXChildVisit_Break;
  }

  payload->n--;

  return CXChildVisit_Continue;
}

void ParserUtils::printLocation(CXCursor cursor)
{
  CXSourceLocation loc = clang_getCursorLocation(cursor);

  CXFile file;
  unsigned int line;
  unsigned int column;
  unsigned int offset;

  clang_getFileLocation(loc, &file, &line, &column, &offset);

  std::cout << toString(clang_getFileName(file)) << std::endl;

  CXFile file2;
  unsigned int line2;
  unsigned int column2;
  unsigned int offset2;

  CXSourceLocation loc2 = clang_getRangeEnd(clang_getCursorExtent(cursor));

  clang_getFileLocation(loc2, &file2, &line2, &column2, &offset2);

  std::cout << "\t1 l" << line << " c" << column << " o" << offset << std::endl;
  std::cout << "\t2 l" << line2 << " c" << column2 << " o" << offset2 << std::endl;
}

void ParserUtils::printLocation(LocationInfo* location_info)
{
  std::cout
    << "\t1 l" << location_info->start_line_
    << " c" << location_info->start_column_
    << " o" << location_info->start_offset_
    << std::endl;

  std::cout
    << "\t2 l" << location_info->end_line_
    << " c" << location_info->end_column_
    << " o" << location_info->end_offset_
    << std::endl;
}

std::vector<ParserToken*> ParserUtils::getFirstTokensBetween(std::vector<ParserToken*>& tokens, std::string begin, std::string end, bool allow_open_end)
{
  std::vector<ParserToken*> found;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i]->token_ == begin)
    {
      for (i = i + 1; i < tokens.size(); i++)
      {
        if (tokens[i]->token_ == end)
          return found;

        found.push_back(tokens[i]);
      }
    }
  }

  if (!allow_open_end)
    found.clear();

  return found;
}

std::vector<ParserToken*> ParserUtils::getTokensUntil(std::vector<ParserToken*>& tokens, std::string end)
{
  std::vector<ParserToken*> found;

  for (size_t i = 0; i < tokens.size(); i++)
  {
    if (tokens[i]->token_ == end)
      break;

    found.push_back(tokens[i]);
  }

  return found;
}

ParserToken* ParserUtils::findToken(std::vector<ParserToken*>& tokens, std::string search, int* index)
{
  for (size_t i = 0; i < tokens.size(); i++)
  {
    ParserToken* token = tokens[i];

    if (token->token_ == search)
    {
      if (index != nullptr)
        *index = i;

      return token;
    }
  }

  return nullptr;
}

CXCursor ParserUtils::getRecursiveNthChildOfType(CXCursor cursor, int n, CXCursorKind kind)
{
  VistiChildPlayload payload = { n, clang_getNullCursor(), kind, {} };

  clang_visitChildren(cursor,
                      ParserUtils::visitChild,
                      &payload);

  return payload.child_cursor;
}

Visibility ParserUtils::getCursorVisiblity(CXCursor cursor)
{
  CX_CXXAccessSpecifier access_specifier = clang_getCXXAccessSpecifier(cursor);

  if (access_specifier == CX_CXXPublic)
    return Visibility::PUBLIC;

  if (access_specifier == CX_CXXPrivate)
    return Visibility::PRIVATE;

  if (access_specifier == CX_CXXProtected)
    return Visibility::PROTECTED;

    assert(false);
}

std::string ParserUtils::combineTokens(std::vector<ParserToken*>& tokens, int from, int to)
{
  std::string str = "";

  for (int i = from; i < (int)tokens.size() && i <= to; i++) 
  {
    str += tokens[i]->token_;
  }

  return str;
}