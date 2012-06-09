// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Enum
//

case State_Enum:
  ASSERT(namespace_body_ != nullptr);

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    class_def_ = new ClassDef(
        *namespace_body_,
        *outer_ns_member_,
        modifiers_ | Modifier_Static,
        name_token->name(),
        name_token->source_info());

    class_def_->AddBaseSpec(*Ty_Enum);
    class_def_->SetClass();
    AddClassDef(*class_def_);
    outer_ns_member_ = class_def_;

    GotoState(State_Enum_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Enum_Finish:
  EndEnum();
  PopState();
  return;

case State_Enum_Member:
  ASSERT(class_def_ != nullptr);
  ASSERT(last_enum_field_ != nullptr);

  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    ContinueState(State_Enum_Finish);
    continue;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    enum_field_ = new(memory_zone_) EnumFieldDef(
        *class_def_,
        Modifier_Const | Modifier_Public | Modifier_Static,
        *last_enum_field_->GetTy(),
        name_token->name(),
        name_token->source_info());

    class_def_->Add(*enum_field_);
    GotoState(State_Enum_Member_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Enum_Member_Name:
  ASSERT(class_def_ != nullptr);
  ASSERT(enum_field_ != nullptr);
  ASSERT(last_enum_field_ != nullptr);

  if (token.Is(*Op__Assign)) {
    CallExpr(*enum_field_->GetTy(), State_Enum_Member_Name_Eq_Expr);
    return;
  }

  {
    auto& ty = *enum_field_->GetTy();
    auto& last = last_enum_field_->expr();
    auto& one = builder_->NewLiteral(ty, 1);
    expression_ = &builder_->EmitI(Op_Add, ty, last, one);
  }

  ContinueState(State_Enum_Member_Name_Eq_Expr);
  continue;

case State_Enum_Member_Name_Eq_Expr:
  ASSERT(class_def_ != nullptr);
  ASSERT(expression_ != nullptr);

  {
    auto& r1 = builder_->EmitNameRefI(
        *enum_field_->GetTy(),
        *Void,
        *new NameRef(enum_field_->name(), token.source_info()));
    builder_->EmitStoreI(r1, *expression_);
    enum_field_->set_expr(*const_cast<Operand*>(expression_));
    last_enum_field_ = enum_field_;
  }

  if (token.Is(*Op__Comma)) {
    GotoState(State_Enum_Member);
    return;
  }

  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    ContinueState(State_Enum_Finish);
    continue;
  }

  AddError(CompileError_Parse_Enum_Member, token);
  return;

case State_Enum_Name:
  ASSERT(class_def_ != nullptr);

  if (token.Is(*Op__Colon)) {
    CallState(State_Type, State_Enum_Name_Colon_Type);
    return;
  }

  if (Expect(*Op__OpenBrace, token)) {
    current_type_ = Ty_Int32;
    ContinueState(State_Enum_Name_Colon_Type);
    continue;
  }

  return;

case State_Enum_Name_Colon_Type:
  ASSERT(class_def_ != nullptr);
  ASSERT(current_type_ != nullptr);

  if (Expect(*Op__OpenBrace, token)) {
    PushParenthesis(token);
    StartEnum(*class_def_, *current_type_);
    GotoState(State_Enum_Member);
  }
  return;
