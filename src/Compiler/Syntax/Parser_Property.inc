// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Property
//
case State_Property_Accessor:
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);
  ASSERT(property_def_ != nullptr)

  if (token.Is(*Op__SemiColon)) {
    GotoState(State_Property_Accessor_Finish);
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

  AddError(CompileError_Parse_Property_Accessor_Invalid, token);
  return;

case State_Property_Accessor_Finish:
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);
  ASSERT(property_def_ != nullptr)

  EndMethod();
  PopNameScope();
  method_def_ = nullptr;
  GotoState(State_Property_Members);
  continue;

case State_Property_Members:
  ASSERT(class_def_ != nullptr);
  ASSERT(!method_def_);
  ASSERT(property_def_ != nullptr)

  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    EndProperty(IsProperty);
    property_def_ = nullptr;
    GotoState(State_Class_Member);
    return;
  }

  if (token.Is(*Q_get) || token.Is(*Q_set)) {
    StartAccessor(
        IsProperty,
        token.StaticCast<NameToken>()->name(),
        token.source_info());
    GotoState(State_Property_Accessor);
    return;
  }

  AddError(CompileError_Parse_Property_Member_Invalid, token);
  return;
