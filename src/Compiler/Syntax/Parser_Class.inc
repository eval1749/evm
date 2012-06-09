// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Class
//
//
//  class-declaration ::=
//      attribute*
//      class_modifier*
//      "class" identifier
//      ("<" identifier ("," identifier)* ">")?
//      (":" name ("," name)*)?
//      "{" class-member-decl* "}"
//
case State_Class:
  if (auto const name_ref = GetNameRef(token)) {
    StartClass(*name_ref);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Class_Member:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ == class_def_);

  method_def_ = nullptr;
  name_ref_ = nullptr;

  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    DEBUG_FORMAT("End class %s", class_def_);
    outer_ns_member_ = outer_ns_member_->owner().StaticCast<ClassOrNamespaceDef>();
    class_def_ = outer_ns_member_->DynamicCast<ClassDef>();
    PopState();
    PopNameScope();
    return;
  }

  if (token.DynamicCast<KeywordToken>()) {
    CallState(State_Modifier, State_Class_Member_Modifier);
    continue;
  }

  ContinueState(State_Class_Member_Modifier);
  continue;

case State_Class_Member_Modifier:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ == class_def_);

  if (token.Is(*K_class)) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Class_BadModifier,
        Modifier_Mask_Class);

    CallState(State_Class, State_Class_Member);
    return;
  }

  if (token.Is(*K_enum)) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Enum_BadModifier,
        Modifier_Mask_Enum);

    CallState(State_Enum, State_Class_Member);
    return;
  }

  if (token.Is(*K_interface)) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Interface_BadModifier,
        Modifier_Mask_Interface);

    modifiers_ |= Modifier_ClassVariation_Interface;
    CallState(State_Class, State_Class_Member);
    return;
  }

  if (token.Is(*K_struct)) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Struct_BadModifier,
        Modifier_Mask_Struct);

    modifiers_ |= Modifier_ClassVariation_Struct;
    CallState(State_Class, State_Class_Member);
    return;
  }

  CallState(State_Type, State_Class_Member_Type);
  continue;

case State_Class_Member_Type:
  ASSERT(!!class_def_);
  ASSERT(name_ref_ == nullptr);
  ASSERT(outer_ns_member_ == class_def_);
  ASSERT(current_type_ != nullptr);

  // Constructor - T T(...)
  if (token.Is(*Op__OpenParen) && current_type_ == &class_def_->GetClass()) {
    if (class_def_->IsInterface()) {
      AddError(CompileError_Parse_Interface_Ctor, token);

    } else if (class_def_->IsStatic()) {
      AddError(CompileError_Parse_Class_Ctor_Static, token);
    }

    current_type_ = Ty_Void;
    name_ref_ = new NameRef(
        *QD_ctor, 
        token.source_info());
    ContinueState(State_Class_Member_Type_Name);
    continue;
  }

  // Operator overloading - T "operator"
  if (token.Is(*K_operator)) {
    GotoState(State_Class_Member_Type_Operator);
    return;
  }

  // Indexer - T this "[" type name ("," type name)* "]"
  if (token.Is(*K_this)) {
    StartProperty(
        IsIndexer,
        *current_type_,
        *new NameRef(*Q_Item, token.source_info()));
    GotoState(State_Indexer);
    return;
  }

  // Method - T Name
  if (auto const name_ref = GetNameRef(token)) {
    if (auto const simple_name = name_ref->GetSimpleName()) {
      if (simple_name->name() == class_def_->name()) {
        AddError(CompileError_Parse_Class_Ctor_HasType, token);
      }
    }
    name_ref_ = name_ref;
    GotoState(State_Class_Member_Type_Name);
    return;
  }

  AddError(CompileError_Parse_Class_ExpectMemberName, token);
  return;

// ConstFieldDef ::= attribute* modifier* "const" type?
//  Name "=" expr ";"
//
// FieldDef ::= attribute* modifier* "var" Name ("=" expr)? ";"
//          | attribute* modifier* type Name ("=" expr)? ";"
//
// MethodDef ::=
//      attribute* modifier* type
//      InterfaceType? Name
//      TypeParamDefs?
//      ParamDefs
//      MethodBody
//
//  PropertyDef ::=
//      attribute* modifier* type
//      (Name ".")? Name "{" (AccessorName AccessorBody)+ "}"
//
// ParamDefs ::= "(" (type Name ("," Name)*)? ")"
//
case State_Class_Member_Type_Name: {
  ASSERT(!!class_def_);
  ASSERT(!!name_ref_);
  ASSERT(outer_ns_member_ == class_def_);

  auto& source_info = name_ref_->source_info();

  // Field with initializeion - T Name "=" Expr
  if (token.Is(*Op__Assign)) {
    auto& name = GetSimpleName(
        *name_ref_,
        CompileError_Parse_Class_BadMember);

    if (current_type_ == Ty_Void) {
      AddError(source_info, CompileError_Parse_Field_Value_Void, name);
      current_type_ = Ty_Object;
    }

    auto& field_def = *new(memory_zone_) FieldDef(
        *class_def_,
        ValidateField(source_info, name),
        *current_type_,
        name,
        source_info);
    class_def_->Add(field_def);
    GotoState(State_Field_Name_Eq);
    return;
  }

  // Property - T Name "{" ... "}"
  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    StartProperty(IsProperty, *current_type_, *name_ref_);
    GotoState(State_Property_Members);
    return;
  }

  // Method - T Name "(" MethodParam* ")"
  if (token.Is(*Op__OpenParen)) {
    PushParenthesis(token);
    StartMethod(*name_ref_);
    GotoState(State_Method_Param);
    return;
  }

  // Field- T Name ";"
  if (token.Is(*Op__SemiColon)) {
    auto& name = GetSimpleName(
        *name_ref_,
        CompileError_Parse_Class_BadMember);

    auto const modifiers = ValidateField(source_info, name);
    if (modifiers & Modifier_Const) {
      AddError(source_info, CompileError_Parse_Field_Const_NoValue, name);
      return;
    }

    if (current_type_ == Ty_Void) {
      AddError(source_info, CompileError_Parse_Field_Value_Void, name);
      current_type_ = Ty_Object;
    }

    auto& field_def = *new(memory_zone_) FieldDef(
        *class_def_,
        modifiers,
        *current_type_,
        name,
        source_info);
    class_def_->Add(field_def);
    GotoState(State_Class_Member);
    return;
  }

  AddError(CompileError_Parse_Class_BadMember, token);
  return;
}

// "operator" Unary | Binary | Conversion
//  Unary ::= "+" | "-" | "!" | "~" | "++" | "--" | "true" | "false"
//  Binary ::= + - * / % & | ^ << >> == != < > >= <=
//  Conversin ::= ("explicit" | "implicit") type
case State_Class_Member_Type_Operator:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ == class_def_);

  if (token.Is(*K_false)) {
    name_ref_ = new NameRef(*Q_false, token.source_info());
    GotoState(State_Class_Member_Type_Name);
    return;
  }

  if (token.Is(*K_true)) {
    name_ref_ = new NameRef(*Q_true, token.source_info());
    GotoState(State_Class_Member_Type_Name);
    return;
  }

  if (auto const op_token = token.DynamicCast<OperatorToken>()) {
    name_ref_ = new NameRef(
        op_token->value().name(), token.source_info());
    GotoState(State_Class_Member_Type_Name);
    return;
  }

  AddError(CompileError_Parse_Class_ExpectOperator, token);
  return;

case State_Class_Name:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ != class_def_);

  if (token.Is(*Op__Colon)) {
    GotoState(State_Class_Name_Colon);
    return;
  }

  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    outer_ns_member_ = class_def_;
    GotoState(State_Class_Member);
    return;
  }

  AddError(CompileError_Parse_Class_ExpectBaseOrBody, token);
  return;

case State_Class_Name_Colon:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ != class_def_);

  if (auto const name_ref = GetNameRef(token)) {
    auto& type = ResolveTypeName(*name_ref);
    class_def_->StaticCast<ClassDef>()->AddBaseSpec(type);
    GotoState(State_Class_Name_Colon_Name);
    return;
  }

  AddError(CompileError_Parse_Class_ExpectBaseName, token);
  return;

case State_Class_Name_Colon_Name:
  ASSERT(!!class_def_);
  ASSERT(outer_ns_member_ != class_def_);

  if (token.Is(*Op__Comma)) {
    GotoState(State_Class_Name_Colon);
    return;
  }

  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    outer_ns_member_ = class_def_;
    GotoState(State_Class_Member);
    return;
  }

  AddError(CompileError_Parse_Class_BadBase, token);
  return;

// Field
case State_Field_Name_Eq:
  CallExpr(*Ty_Object, State_Field_Name_Eq_Expr);
  continue;

case State_Field_Name_Eq_Expr:
  Expect(*Op__SemiColon, token);
  GotoState(State_Class_Member);
  return;
