// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Using directive
//
//  AliasDef ::= "using" name "=" name ("." name)* ";"
//  ImportNs ::= "using" name ("." name)* ";"

case State_Using:
  ASSERT(namespace_body_ != nullptr);

  // "using" name
  if (auto const name_token = token.DynamicCast<NameToken>()) {
    name_ref_ = &name_token->ToNameRef();
    GotoState(State_Using_Name);
    return;
  }

  // "using" name "." name
  if (auto const qname_token = token.DynamicCast<NameRefToken>()) {
    namespace_body_->Add(
        *new UsingNamespace(
            namespace_body_,
            qname_token->name_ref()));
    GotoState(State_Using_DottedName);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Using_Name: {
  ASSERT(namespace_body_ != nullptr);
  ASSERT(name_ref_ != nullptr);
  ASSERT(name_ref_->GetSimpleName() != nullptr);

  // "using" name "="
  if (token.Is(*Op__Assign)) {
    auto& name = name_ref_->GetSimpleName()->name();
    if (auto const present = namespace_body_->Find(name)) {
      AddError(
          token.source_info(),
          CompileError_Parse_RedefineSame,
          name,
          present);
      return;
    }
    GotoState(State_Using_Name_Eq);
    return;
  }

  // "using" name ";"
  if (token.Is(*Op__SemiColon)) {
    namespace_body_->Add(
        *new(memory_zone_) UsingNamespace(
            namespace_body_,
            *name_ref_));
    name_ref_ = nullptr;
    PopState();
    return;
  }

  AddError(token.source_info(), CompileError_Parse_Using_BadUsing, token);
  return;
}

case State_Using_Name_Eq:
  ASSERT(namespace_body_ != nullptr);
  ASSERT(name_ref_ != nullptr);
  ASSERT(name_ref_->GetSimpleName() != nullptr);

  // "using" name "=" name ("." name)*
  if (auto const name_ref = GetNameRef(token)) {
    namespace_body_->Add(
        *new(memory_zone_) AliasDef(
            namespace_body_,
            name_ref_->GetSimpleName()->name(),
            *name_ref,
            name_ref_->source_info()));
    GotoState(State_Using_Name_Eq_Name);
    return;
  }

  AddError(CompileError_Parse_UsingName_BadTarget, token);
  return;

case State_Using_DottedName:
case State_Using_Name_Eq_Name:
  name_ref_ = nullptr;
  if (Expect(*Op__SemiColon, token)) {
    PopState();
  }
  return;
