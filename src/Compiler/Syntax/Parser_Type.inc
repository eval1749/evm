// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Using directive
// Type ::= TypeName RankSpec*
case State_Type:
  current_type_ = nullptr;

  if (auto const name_ref = GetTypeNameRef(token)) {
    current_type_ = &ResolveTypeName(*name_ref);
    GotoState(State_Type_TypeName);
    return;
  }

  if (auto const keyword = token.DynamicCast<KeywordToken>()) {
    if (auto const type = keyword->value().GetTy()) {
      current_type_ = type;
      GotoState(State_Type_TypeName);
      return;
    }

    AddError(CompileError_Parse_ExpectType, token);
    return;
  }

  AddError(CompileError_Parse_ExpectType, token);
  return;

case State_Type_TypeName:
  ASSERT(current_type_ != nullptr);

  if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
    current_type_ = &ArrayType::Intern(*current_type_, rank_token->rank());
    return;
  }

  // TODO(yosi) 2012-03-10 NYI: construct type parsing
  PopState();
  continue;
