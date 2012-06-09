// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Indexer
//
case State_Indexer:
  ASSERT(class_def_ != nullptr);
  ASSERT((!method_def_));
  ASSERT(property_def_ != nullptr);

  if (token.DynamicCast<RankSpecToken>()) {
    AddError(CompileError_Parse_Indexer_Param_Missing, token);
    return;
  }

  if (Expect(*Op__OpenBracket, token)) {
    PushParenthesis(token);
  }

  GotoState(State_Indexer_Param);
  return;

case State_Indexer_Accessor:
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);
  ASSERT(property_def_ != nullptr);

  if (token.Is(*Op__SemiColon)) {
    GotoState(State_Indexer_Accessor_Finish);
    return;
  }

  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    builder_ = &Builder::StartMethodFunction(
        builder_,
        *method_def_,
        token.source_info());

    CallState(State_Block, State_Indexer_Accessor_Finish);
    return;
  }

  AddError(CompileError_Parse_Indexer_Accessor_Invalid, token);
  return;

case State_Indexer_Accessor_Finish:
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);
  ASSERT(property_def_ != nullptr);

  EndMethod();
  PopNameScope();
  method_def_ = nullptr;
  GotoState(State_Indexer_Members);
  continue;

case State_Indexer_Members:
  ASSERT(class_def_ != nullptr);
  ASSERT((!method_def_));
  ASSERT(property_def_ != nullptr);

  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    EndProperty(IsIndexer);
    GotoState(State_Class_Member);
    return;
  }

  if (token.Is(*Q_get) || token.Is(*Q_set)) {
     StartAccessor(
        IsIndexer,
        token.StaticCast<NameToken>()->name(),
        token.source_info());
     GotoState(State_Indexer_Accessor);
     return;
  }

  AddError(CompileError_Parse_Indexer_Member_Invalid, token);
  return;

case State_Indexer_Param:
  CallState(State_Type, State_Indexer_Param_Type);
  continue;

case State_Indexer_Param_CloseBracket:
  ASSERT(class_def_ != nullptr);
  ASSERT((!method_def_));
  ASSERT(property_def_ != nullptr);

  if (Expect(*Op__OpenBrace, token)) {
    PushParenthesis(token);
    GotoState(State_Indexer_Members);
  }
  return;

case State_Indexer_Param_Type:
  ASSERT(current_type_ != nullptr);
  ASSERT(class_def_ != nullptr);
  ASSERT((!method_def_));
  ASSERT(property_def_ != nullptr);

  if (current_type_ == Ty_Void) {
    AddError(CompileError_Parse_Indexer_Param_Void, token);
    current_type_ = Ty_Object;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();
    if (name == *Q_value) {
      AddError(CompileError_Parse_Indexer_Param_Reserved, token);

    } else if (property_def_->Find(name)) {
      AddError(CompileError_Parse_Indexer_Param_Duplicate, token);

    } else {
      auto& param_def = *new MethodParamDef(
          *property_def_,
          *current_type_,
          name,
          *new Variable(name),
          name_token->source_info());

      property_def_->Add(param_def);
    }
    GotoState(State_Indexer_Param_Type_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Indexer_Param_Type_Name:
  ASSERT(class_def_ != nullptr);
  ASSERT((!method_def_));
  ASSERT(property_def_ != nullptr);

  if (token.Is(*Op__Comma)) {
    GotoState(State_Indexer_Param);
    return;
  }

  if (token.Is(*Op__CloseBracket)) {
    PopParenthesis(token);
    GotoState(State_Indexer_Param_CloseBracket);
    return;
  }

  AddError(CompileError_Parse_Indexer_Param_Invalid, token);
  return;
