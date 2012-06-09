// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Method
//
case State_Method_Body:
  ASSERT(builder_ != nullptr);
  ASSERT(method_def_ != nullptr);

  // Note: Here, a token is next token of close brace.
  EndMethod();
  PopNameScope();

  GotoState(State_Class_Member);
  continue;

case State_Method_Param:
  ASSERT(method_def_ != nullptr);

  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    GotoState(State_Method_Param_CloseParen);
    return;
  }

  CallState(State_Type, State_Method_Param_Type);
  continue;

case State_Method_Param_CloseParen:
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);
  ASSERT(method_def_->owner_class_def() == *class_def_);

  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    if (class_def_->IsInterface()) {
      AddError(CompileError_Parse_Interface_Method, token);
      return;
    }

    builder_ = &Builder::StartMethodFunction(
        builder_, 
        *method_def_, 
        token.source_info());
    CallState(State_Block, State_Method_Body);
    return;
  }

  if (token.Is(*Op__SemiColon)) {
    if (class_def_->IsInterface() || method_def_->IsExtern()) {
      EndMethod();
      PopNameScope();
      GotoState(State_Class_Member);
      return;
    }

    AddError(CompileError_Parse_Method_ExpectBody, token);
    return;
  }

  // Expect {...} or ";".
  AddError(CompileError_Parse_Method_Block_Invalid, token);
  return;

case State_Method_Param_Type:
  ASSERT(current_type_ != nullptr);
  ASSERT(method_def_ != nullptr);

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();

    if (current_type_ == Ty_Void) {
      AddError(CompileError_Parse_Method_Param_Void, token);
      current_type_ = Ty_Object;
    }

    // TODO(yosi) 2012-03-10 Where do we allocate Variable in parser?
    auto& var = *new Variable(name);
    var.SetTy(*current_type_);

    if (method_def_->Find(name)) {
      AddError(CompileError_Parse_Method_Param_Duplicate, token);
    }

    auto const pParamDef = new MethodParamDef(
        *method_def_,
        *current_type_,
        name,
        var,
        name_token->source_info());

    method_def_->Add(*pParamDef);
    name_scope_->Add(*pParamDef);

    GotoState(State_Method_Param_Type_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Method_Param_Type_Name:
  ASSERT(method_def_ != nullptr);

  // TODO(yosi) 2010-01-05 Check duplicated method parameter name.
  if (token.Is(*Op__Comma)) {
    GotoState(State_Method_Param);
    return;
  }

  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    GotoState(State_Method_Param_CloseParen);
    return;
  }

  AddError(CompileError_Parse_Method_Param_Invalid, token);
  return;
