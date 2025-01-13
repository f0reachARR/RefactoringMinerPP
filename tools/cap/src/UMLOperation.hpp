#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "LocationInfo.hpp"
#include "Visibility.hpp"
#include "UMLParameter.hpp"
#include "OperationBody.hpp"
#include "AbstractExpression.hpp"
#include "UMLTypeParemter.hpp"
#include "UMLType.hpp"
#include "UMLComment.hpp"

class UMLClass;

class UMLOperation : public VariableDeclarationContainer
{
  public:
    LocationInfo* location_info_;
    std::string name_;
    Visibility visibility_;
    bool is_abstract_;
    std::vector<UMLParameter*> parameters_;
    std::string class_name_;
    bool is_constructor_;
    bool is_final_;
    bool is_static_;
    OperationBody* operation_body_;
    std::vector<UMLTypeParameter*> type_parameters_;
    std::vector<UMLModifier*> modifiers_;

    UMLOperation(std::string clss, std::string name, LocationInfo* location_info, UMLClass* container);
    ~UMLOperation();

    LocationInfo* getLocationInfo() override;

    //custom
    UMLClass* cust_container_;
    VariableDeclaration* custGetVarDecl(CXCursor& cursor);

    json toJson() override;
};