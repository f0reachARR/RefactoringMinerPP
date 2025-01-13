#include <iostream>
#include <algorithm>
#include <cassert>
#include <fstream>

#include "Parser.hpp"
#include "LeafType.hpp"
#include "ParserUtils.hpp"
#include "ParserDeclarationType.hpp"
#include "ParserVariableDeclaration.hpp"
#include "ParserMethod.hpp"

int main(int argc, char* argv[])
{
  if (argc != 3)
    return 1;

  std::string input_file = argv[1];
  std::string output_file = argv[2];

  Parser::parse(input_file, output_file);

  return 0;
}

int Parser::parse(std::string input_file, std::string output_file)
{
  CXIndex index = clang_createIndex(0, 0);
  ParserUtils::unit = clang_parseTranslationUnit(
    index,
    input_file.c_str(),
    nullptr,
    0,
    nullptr,
    0,
    CXTranslationUnit_DetailedPreprocessingRecord
  );

  if (ParserUtils::unit == nullptr)
  {
    std::cout << "err" << std::endl;
    return -1;
  }

  UMLModel model;

  Payload payload;
  payload.model_ = &model;

  CXCursor cursor = clang_getTranslationUnitCursor(ParserUtils::unit);

  clang_visitChildren(
    cursor,
    visitor,
    &payload
  );

  clang_disposeTranslationUnit(ParserUtils::unit);
  clang_disposeIndex(index);

  std::ofstream out(output_file);
  out << model.toJson();

  return 0;
}

CXChildVisitResult Parser::visitor(CXCursor cursor, CXCursor, CXClientData clientData)
{
  CXSourceLocation location = clang_getCursorLocation(cursor);

  if (!clang_Location_isFromMainFile(location))
    return CXChildVisit_Continue;

  if (cursor.kind == CXCursor_Namespace)
  {
    ((Payload*)clientData)->namespace_ = ParserUtils::toString(clang_getCursorSpelling(cursor));
    return CXChildVisit_Recurse;
  }
  else if (cursor.kind == CXCursor_ClassDecl || cursor.kind == CXCursor_ClassTemplate)
  {
    parseClass(cursor, clientData);
  }
  else if (cursor.kind == CXCursor_InclusionDirective)
  {
    parseInclude(cursor, clientData);
  }
  else if (cursor.kind == CXCursor_EnumDecl)
  {
    parseEnum(cursor, clientData);
  }

  return CXChildVisit_Continue;
}

UMLClass* Parser::parseClass(CXCursor cursor, CXClientData clientData)
{
  Payload payload = ParserUtils::getPayload(clientData);

  std::string class_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* location_info = ParserUtils::createLocationInfo(cursor, CodeElementType::TYPE_DECLARATION);

  UMLClass* uml_class = new UMLClass(payload.namespace_, class_name, location_info);
  uml_class->is_abstract_ = clang_CXXRecord_isAbstract(cursor) != 0;

  if (!payload.includes_.empty())
    uml_class->imported_types_.insert(uml_class->imported_types_.end(), payload.includes_.begin(), payload.includes_.end());

  payload.model_->class_list_.push_back(uml_class);
  payload.class_ = uml_class;

  clang_visitChildren(cursor,
                      visitClassChildren,
                      &payload);

  return uml_class;
}

CXChildVisitResult Parser::visitClassChildren(CXCursor cursor, CXCursor, CXClientData clientData)
{
  Payload payload = ParserUtils::getPayload(clientData);

  if (cursor.kind == CXCursor_CXXFinalAttr)
  {
    payload.class_->is_final_ = true;
    payload.class_->modifiers_.push_back(new UMLModifier("final", ParserUtils::createLocationInfo(cursor, CodeElementType::MODIFIER)));
  }
  else if (cursor.kind == CXCursor_TemplateTypeParameter)
  {
    std::string name = ParserUtils::toString(clang_getCursorSpelling(cursor));
    LocationInfo* location_info = ParserUtils::createLocationInfo(cursor, CodeElementType::TYPE_DECLARATION);

    payload.class_->type_parameters_.push_back(new UMLTypeParameter(name, location_info));
  }
  else if (cursor.kind == CXCursor_FieldDecl
    || cursor.kind == CXCursor_VarDecl)
  {
    parseClassField(cursor, payload.class_);
  }
  else if (cursor.kind == CXCursor_CXXBaseSpecifier)
  {
    parseClassParent(cursor, payload.class_, payload.model_);
  }
  else if (cursor.kind == CXCursor_CXXMethod
    || cursor.kind == CXCursor_Constructor
    || cursor.kind == CXCursor_FunctionTemplate
    || cursor.kind == CXCursor_Destructor)
  {
    parseClassMethod(cursor, payload.class_);
  }

  return CXChildVisit_Continue;
}

void Parser::parseClassField(CXCursor cursor, UMLClass* uml_class)
{
  CXType cursor_type = clang_getCursorType(cursor);

  LocationInfo* field_location = ParserUtils::createLocationInfo(cursor, CodeElementType::FIELD_DECLARATION);

  std::string field_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  UMLAttribute* field = new UMLAttribute(field_name, field_location);

  field->visibility_ = ParserUtils::getCursorVisiblity(cursor);

  field->class_name_ = uml_class->qualified_name_;

  field->is_final_ = clang_isConstQualifiedType(cursor_type) != 0;
  field->is_static_ = clang_getCursorKind(cursor) == CXCursor_VarDecl;
  field->is_volatile_ = clang_isVolatileQualifiedType(cursor_type) != 0;

  //put all field decls in a list libclang does not distinguish between multi field decls and single field decls
  //pretent all field decls are one multi decl. This list will be filtered in var decl parser
  std::vector<VariableDeclaration*> all_fields;
  for (auto f : uml_class->attributes_)
    all_fields.push_back(f->variable_declaration_);

  field->variable_declaration_ = ParserVariableDeclaration::parseFieldDecl(cursor, field, all_fields);

  field->type_ = field->variable_declaration_->type_->copy();

  uml_class->attributes_.push_back(field);
}

void Parser::parseClassParent(CXCursor cursor, UMLClass* uml_class, UMLModel* model)
{
  std::string parent_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  std::vector<ParserToken*> tokens = ParserUtils::tokenize(cursor);

  assert(tokens[0]->token_ == "public" || tokens[0]->token_ == "protected" || tokens[0]->token_ == "private");

  std::vector<ParserToken*> tokens_no_acc_spec; //skip inheritance access specifier, public, protected, private
  tokens_no_acc_spec.insert(tokens_no_acc_spec.end(), tokens.begin() + 1, tokens.end());

  UMLType* type = ParserDeclarationType::parseTypeTokens(tokens_no_acc_spec);

  //java does not support multi inheritance
  //c++ does not support interfaces
  //therefore treat the first parent class as super class and all other as interfaces
  //until you figure out how to detect if a parent class is an abstract class
  if (uml_class->superclass_ == nullptr)
  {
    uml_class->superclass_ = type;
    model->generalization_list_.push_back(new UMLGeneralization(uml_class, parent_name));
  }
  else
  {
    uml_class->implemented_interfaces_.push_back(type);
    model->realization_list_.push_back(new UMLRealization(uml_class, parent_name));
  }

  ParserUtils::freeTokens(tokens);
}

void Parser::parseEnum(CXCursor cursor, CXClientData clientData)
{
  Payload payload = ParserUtils::getPayload(clientData);

  std::string enum_name = ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* location_info = ParserUtils::createLocationInfo(cursor, CodeElementType::ENUM_DECLARATION);

  UMLClass* uml_class = new UMLClass(payload.namespace_, enum_name, location_info);
  uml_class->is_enum_ = true;

  payload.model_->class_list_.push_back(uml_class);
  payload.class_ = uml_class;

  clang_visitChildren(cursor,
                      visitEnumChildren,
                      &payload);
}

CXChildVisitResult Parser::visitEnumChildren(CXCursor cursor, CXCursor, CXClientData clientData)
{
  Payload payload = ParserUtils::getPayload(clientData);

  if (cursor.kind == CXCursor_EnumConstantDecl)
  {
    std::string name = ParserUtils::toString(clang_getCursorSpelling(cursor));
    LeafType* type = new LeafType(nullptr, payload.class_->qualified_name_);
    LocationInfo* loc = ParserUtils::createLocationInfo(cursor, CodeElementType::ENUM_CONSTANT_DECLARATION);

    UMLEnumConstant* uml_enum = new UMLEnumConstant(name, loc);
    uml_enum->type_ = type;
    uml_enum->visibility_ = Visibility::PUBLIC;
    uml_enum->class_name_ = payload.class_->qualified_name_;
    uml_enum->is_final_ = true;
    uml_enum->is_static_ = true;
    uml_enum->variable_declaration_ = ParserVariableDeclaration::parseEnumConstantDeclaration(cursor, payload.class_);

    CXCursor first_child = ParserUtils::visitChild(cursor, 1);

    if (!clang_Cursor_isNull(first_child))
      uml_enum->arguments_.push_back(ParserUtils::toStringTokens(first_child));

    payload.class_->enum_constants_.push_back(uml_enum);
  }

  return CXChildVisit_Continue;
}

void Parser::parseInclude(CXCursor cursor, CXClientData clientData)
{
  std::string name =  ParserUtils::toString(clang_getCursorSpelling(cursor));
  LocationInfo* loc = ParserUtils::createLocationInfo(cursor, CodeElementType::IMPORT_DECLARATION);

  UMLImport* include = new UMLImport(name, loc);

  ((Payload*)clientData)->includes_.push_back(include);
}

void Parser::parseClassMethod(CXCursor cursor, UMLClass* uml_class)
{
  UMLOperation* op = ParserMethod::parse(cursor, uml_class);
  uml_class->operations_.push_back(op);
}