// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita Compiler - Parser - Statement
//
case State_Stmt:
  ASSERT(builder_ != nullptr);
  builder_->set_source_info(token.source_info());

  if (token.Is(*Op__OpenBrace)) {
    PushParenthesis(token);
    GotoState(State_Stmt_Brace);
    return;
  }

  if (token.Is(*Op__SemiColon)) {
    PopState();
    return;
  }

  if (token.Is(*K_break)) {
    foreach (Stack_<Control*>::Enum, oEnum, &control_stack_) {
      auto const pControl = oEnum.Get();

      if (auto const pWhile = pControl->DynamicCast<WhileControl>()) {
        builder_->EmitJumpI(*pWhile->m_pBreakBB);

        builder_->EmitCloses(
            *builder_->GetCurr()->GetLastI(),
            frame_reg_,
            pWhile->frame_reg_);

        builder_->ResetCurr();
        GotoState(State_Stmt_Break);
        return;
      }

      if (auto const pSwitch = pControl->DynamicCast<SwitchControl>()) {
        builder_->EmitJumpI(*pSwitch->m_pSuccBB);

        builder_->EmitCloses(
            *builder_->GetCurr()->GetLastI(),
            frame_reg_,
            pSwitch->frame_reg_);

        builder_->ResetCurr();
        GotoState(State_Stmt_Break);
        return;
      }
    }

    AddError(CompileError_Parse_Stmt_Break, token);
    return;
  }

  if (token.Is(*K_continue)) {
    foreach (Stack_<Control*>::Enum, oEnum, &control_stack_) {
      auto const pControl = oEnum.Get();
      if (auto const pWhile = pControl->DynamicCast<WhileControl>()) {
        builder_->EmitJumpI(*pWhile->m_pBreakBB);

        builder_->EmitCloses(
            *builder_->GetCurr()->GetLastI(),
            frame_reg_,
            pWhile->frame_reg_);

        builder_->ResetCurr();
        GotoState(State_Stmt_Continue);
        return;
      }
    }

    AddError(CompileError_Parse_Stmt_Continue, token);
    return;
  }

  if (token.Is(*K_do)) {
    GotoState(State_Stmt_Do);
    continue;
  }

  if (token.Is(*K_for)) {
    GotoState(State_Stmt_For);
    return;
  }

  if (token.Is(*K_goto)) {
    GotoState(State_Stmt_Goto);
    return;
  }

  if (token.Is(*K_if)) {
    GotoState(State_Stmt_If);
    return;
  }

  if (token.Is(*K_return)) {
    if (method_def_->return_type() == *Ty_Void) {
      expression_ = Void;
      GotoState(State_Stmt_Return_Expr);
    } else {
      CallExpr(method_def_->return_type(), State_Stmt_Return_Expr);
    }
    return;
  }

  if (token.Is(*K_switch)) {
    GotoState(State_Stmt_Switch);
    return;
  }

  if (token.Is(*K_throw)) {
    CallExpr(*Ty_Exception, State_Stmt_Throw_Expr);
    return;
  }

  if (token.Is(*K_try)) {
    GotoState(State_Stmt_Try);
    return;
  }

  if (token.Is(*K_using)) {
    GotoState(State_Stmt_Using);
    return;
  }

  if (token.Is(*Q_var)) {
    current_type_ = new TypeVar();
    GotoState(State_Stmt_VarDef);
    return;
  }

  if (token.Is(*K_while)) {
    GotoState(State_Stmt_While);
    return;
  }

  if (auto const keyword = token.DynamicCast<KeywordToken>()) {
    if (auto const type = keyword->value().GetTy()) {
      current_type_ = type;
      GotoState(State_Stmt_Type);
      return;
    }

  } else if (auto const pNameRef = GetNameRef(token)) {
    name_ref_ = pNameRef;
    GotoState(State_Stmt_Name);
    return;
  }

  CallExpr(*Ty_Void, State_Stmt_Expr);
  continue;

case State_Stmt_Break:
case State_Stmt_Continue:
case State_Stmt_Expr:
  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  PopState();
  return;

// "{" statement* "}"
case State_Stmt_Brace:
  PushNameScope();
  CallState(State_Stmt, State_Stmt_Brace_Stmt);
  continue;

case State_Stmt_Brace_Stmt:
  ASSERT(builder_ != nullptr);
  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    builder_->set_source_info(token.source_info());
    PopNameScope();
    PopState();
    return;
  }

  CallState(State_Stmt);
  continue;

// "do"  Statement "while" "(" BooleanExpr ")"
//
//  curr:
//      ...
//      JUMP continue
//  continue:
//      ... Statement ...
//      ... BooleanExpr ...
//      BRANCH continue break
//  break:
//
case State_Stmt_Do:
  {
    auto const pContinueBB = &builder_->NewBBlock();
    auto const pBreakBB = &builder_->NewSucc();
    builder_->EmitJumpI(*pContinueBB);
    builder_->SetCurrSucc(*pContinueBB, *pContinueBB);
    control_stack_.Push(
        new WhileControl(frame_reg_, pBreakBB, pContinueBB));
  }

  CallState(State_Stmt, State_Stmt_Do_Stmt);
  return;

case State_Stmt_Do_Stmt:
  if (!token.Is(*K_while)) {
    AddError(CompileError_Parse_Expect, token, K_while);
    return;
  }
  GotoState(State_Stmt_Do_Stmt_While);
  return;

case State_Stmt_Do_Stmt_While:
  if (Expect(*Op__OpenParen, token)) {
    PushParenthesis(token);
    CallState(State_BoolExpr, State_Stmt_Do_Stmt_While_Expr);
    return;
  }

  return;

case State_Stmt_Do_Stmt_While_Expr:
  if (!ExpectCloseParen(token)) {
      return;
  }

  {
      auto const pBx = expression_->StaticCast<BoolOutput>();
      auto const pLoop = PopControl<WhileControl>();
      auto const pBreakBB = pLoop->m_pBreakBB;
      auto const pContinueBB = pLoop->m_pContinueBB;
      builder_->EmitBranchI(*pBx, *pContinueBB, *pBreakBB);
      builder_->SetCurrSucc(*pBreakBB, *pBreakBB);
  }

  PopState();
  return;

// "for" "(" type name ":" expr ")" statement
// "for" "(" init ";" cond ";" step ")" statement
//  init ::= local-var-decl statement-expr-list
//  cond ::= boolean-expr
//  step ::= statement-expr-list
//  statement-expr-list ::= statement ("," statement)*
//
//  curr:
//      ...
//      JUMP test
//  loop:
//      ... Statement ...
//      JUMP continue
//  continue:
//      ... Step ...
//      JUMP test
//  test:
//      ... Boolean Expr ...
//      BRANCH loop break
//  break:
case State_Stmt_For:
  if (!Expect(*Op__OpenParen, token)) {
    return;
  }

  PushParenthesis(token);
  PushNameScope();
  GotoState(State_Stmt_For_Paren);
  return;

case State_Stmt_For_Paren:
  if (token.Is(*Q_var)) {
    GotoState(State_Stmt_For_Paren_Var);
    return;
  }

  CallExpr(*Ty_Void, State_Stmt_For_Paren_Expr);
  continue;

case State_Stmt_For_Paren_Expr:
  if (token.Is(*Op__Comma)) {
    CallExpr(*new TypeVar(), State_Stmt_For_Paren_Init);
    return;
  }

  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  GotoState(State_Stmt_For_Paren_Init);
  return;

case State_Stmt_For_Paren_Init:
  {
    auto& test_block = StartFor();
    builder_->EmitJumpI(test_block);
    builder_->SetCurrSucc(test_block, test_block);
  }

  if (token.Is(*Op__SemiColon)) {
    // No test expression.
    auto& loop = *control_stack_.GetTop()->StaticCast<ForControl>();
    builder_->EmitJumpI(*loop.m_pLoopBB);
    builder_->SetCurrSucc(*loop.m_pContinueBB, *loop.m_pContinueBB);
    CallExpr(*Ty_Void, State_Stmt_For_Paren_Init_Cond_Step);
    return;
  }

  CallState(State_BoolExpr, State_Stmt_For_Paren_Init_Cond);
  continue;

case State_Stmt_For_Paren_Init_Cond:
  if (expression_ == Void) {
    AddError(CompileError_Parse_Expect_BooleanExpr, token);
    return;
  }

  if (!Expect(*Op__SemiColon, token)) {
      return;
  }

  {
    auto const pLoop = control_stack_.GetTop()->StaticCast<ForControl>();
    auto const pBx = expression_->StaticCast<BoolOutput>();
    builder_->EmitBranchI(*pBx, *pLoop->m_pLoopBB, *pLoop->m_pBreakBB);
    builder_->SetCurrSucc(*pLoop->m_pContinueBB, *pLoop->m_pContinueBB);
  }

  CallExpr(*Ty_Void, State_Stmt_For_Paren_Init_Cond_Step);
  return;

case State_Stmt_For_Paren_Init_Cond_Step:
  if (token.Is(*Op__Comma)) {
    CallExpr(*Ty_Void, State_Stmt_For_Paren_Init_Cond_Step);
    return;
  }

  if (!ExpectCloseParen(token)) {
    return;
  }

  {
    auto const control = control_stack_.GetTop()->StaticCast<ForControl>();
    builder_->EmitJumpI(*control->m_pTestBB);
    builder_->SetCurrSucc(*control->m_pLoopBB, *control->m_pContinueBB);
  }

  CallState(State_Stmt, State_Stmt_For_Finish);
  return;

case State_Stmt_For_Finish:
  {
    auto const pLoop = PopControl<ForControl>();

    if (builder_->GetCurr() != pLoop->m_pContinueBB) {
      builder_->EmitJumpI(*pLoop->m_pContinueBB);
    }

    auto const pBreakBB = pLoop->m_pBreakBB;
    builder_->SetCurrSucc(*pBreakBB, *pBreakBB);

    DEBUG_FORMAT("EndFor: Break=%s", pBreakBB);
  }
  PopState();
  continue;

case State_Stmt_For_Paren_Var:
  ASSERT(method_def_ != nullptr);

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();
    current_type_ = new TypeVar();

    local_var_def_ = new LocalVarDef(
        *method_def_,
        *current_type_,
        name,
        *new Variable(name),
        name_token->source_info());

    method_def_->Add(*local_var_def_);
    name_scope_->Add(*local_var_def_);

    GotoState(State_Stmt_For_Paren_Var_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

case State_Stmt_For_Paren_Var_Name:
  ASSERT(local_var_def_ != nullptr);
  ASSERT(current_type_ != nullptr);

  if (token.Is(*Op__Assign)) {
    GotoState(State_Stmt_For_Paren_Var_Name_Eq);
    return;
  }

  if (token.Is(*Op__Colon)) {
    operand_stack_.Push(current_type_);
    CallExpr(*Ty_Object, State_Stmt_ForEach);
    return;
  }

  AddError(CompileError_Parse_Stmt_For_Init, token);
  return;

case State_Stmt_For_Paren_Var_Name_Eq:
  ASSERT(local_var_def_ != nullptr);
  CallExpr(*new TypeVar(), State_Stmt_For_Paren_Var_Name_Eq_Expr);
  continue;

case State_Stmt_For_Paren_Var_Name_Eq_Expr: {
  ASSERT(local_var_def_ != nullptr);

  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  ASSERT(expression_ != nullptr);
  auto& var = local_var_def_->GetVariable();
  const_cast<Variable&>(var).SetTy(expression_->GetTy());
  EmitVarDefI(var, *expression_);
  GotoState(State_Stmt_For_Paren_Init);
  return;
}

// "for" "(" "var" Name ":" Expr) Statement
// "for" "(" Type Name ":" Expr) Statement
case State_Stmt_ForEach:
  if (!ExpectCloseParen(token)) {
    return;
  }

  if (expression_->type().Is<ArrayType>()) {
    ProcessForEachOnArray(*expression_);
  } else {
    ProcessForEachOnEnumerator(*expression_);
  }
  return;

case State_Stmt_ForEach_EndLoop:
  control_stack_.Pop();
  CallState(State_Stmt_For_Finish, State_Stmt_ForEach_Finish);
  continue;

case State_Stmt_ForEach_Finish:
  CloseFinally();
  frame_reg_ = frame_reg_->GetOuter();
  PopState();
  continue;

// "goto" name ";"
// "goto" "case" expr ";"
// "goto" "default" ";"
case State_Stmt_Goto:
  if (token.Is(*K_case)) {
    CallExpr(*new TypeVar(), State_Stmt_Goto_Case_Expr);
    return;
  }

  if (token.Is(*K_default)) {
    if (auto const pSwitch = FindSwitchControl()) {
      builder_->EmitJumpI(*pSwitch->m_pDefaultBB);
      builder_->ResetCurr();
      GotoState(State_Stmt_Goto_Label);
      return;
    }

    AddError(CompileError_Parse_Stmt_Goto_Default, token);
    return;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();
    if (auto const present = name_scope_->Find(name)) {
      if (auto const pLabelDef = present->DynamicCast<LabelDef>()) {
        builder_->EmitJumpI(*pLabelDef->GetBBlock());

      } else {
        AddError(CompileError_Parse_Label_Invalid, token, present);
      }

    } else {
      // Forward referenced label.
      auto const pBBlock = &builder_->NewBBlock();
      builder_->EmitJumpI(*pBBlock);

      auto const pLabelDef = new LabelDef(
          method_def_,
          name,
          pBBlock,
          token.source_info());

      pLabelDef->Add(builder_->GetCurr(), frame_reg_);
      name_scope_->Add(*pLabelDef);
    }

    builder_->ResetCurr();
    GotoState(State_Stmt_Goto_Label);
    return;
  }

  AddError(CompileError_Parse_Stmt_Goto, token);
  return;

case State_Stmt_Goto_Case_Expr:
  if (expression_ == Void) {
    AddError(CompileError_Parse_Stmt_Goto_Case_Expr, token);
    return;
  }

  if (auto const pSwitch = FindSwitchControl()) {
    if (auto const box = pSwitch->FindCase(*expression_)) {
      builder_->EmitJumpI(*box->GetBB());

    } else {
      auto const pBBlock = &builder_->NewBBlock();
      builder_->EmitJumpI(*pBBlock);
      pSwitch->m_pSwitchI->AddOperand(
          pBBlock,
          const_cast<Operand*>(expression_));
    }

    builder_->ResetCurr();
    GotoState(State_Stmt_Goto_Label);
    continue;
  }

  AddError(CompileError_Parse_Stmt_Goto_Case, token);
  return;

case State_Stmt_Goto_Label:
  Expect(*Op__SemiColon, token);
  PopState();
  return;

// "if" "(" BooleanExpr ")" then else?
case State_Stmt_If:
  if (!Expect(*Op__OpenParen, token)) {
    return;
  }

  PushParenthesis(token);
  CallState(State_BoolExpr, State_Stmt_If_Expr);
  return;

case State_Stmt_If_Expr:
  if (expression_ == Void) {
    AddError(CompileError_Parse_Expect_BooleanExpr, token);
    return;
  }

  if (!ExpectCloseParen(token)) {
    return;
  }

  StartIf(*expression_->StaticCast<BoolOutput>());
  CallState(State_Stmt, State_Stmt_If_Expr_Then);
  return;

case State_Stmt_If_Expr_Then: {
  auto const pElseBB = bblock_stack_.Pop();

  auto const pEndIfBB = &builder_->GetSucc();
  builder_->EmitJumpI(*pEndIfBB);

  builder_->SetCurrSucc(*pElseBB, *pEndIfBB);

  if (token.Is(*K_else)) {
    CallState(State_Stmt, State_Stmt_If_Expr_Then_Else);
    return;
  }

  builder_->set_source_info(token.source_info());
  builder_->EmitJumpI(*pEndIfBB);
  builder_->SetCurrSucc(*pEndIfBB, *pEndIfBB);

  PopState();
  continue;
}

case State_Stmt_If_Expr_Then_Else: {
  auto& endif_block = builder_->GetSucc();
  builder_->set_source_info(token.source_info());
  builder_->EmitJumpI(endif_block);
  builder_->SetCurrSucc(endif_block, endif_block);
  PopState();
  return;
}

// Label ::= Name ":"
// NameRef ::= Name ("." Name)
// ArgList ::= Expr ("," Expr)*
// ArrayRef ::= "[" Expr ("," Expr)* "]"
// VarDef ::= NameRef Name ("=" Expr)? ("," Name ("=" Expr))* ";"
// InvokeExpr ::= NameRef "(" ArgList? ")" PostFix* ";"
// AssignExpr ::= NameRef ArrayRef? "=" Expr ";"
case State_Stmt_Name:
  ASSERT(method_def_ != nullptr);
  ASSERT(name_ref_ != nullptr);

  if (token.Is(*Op__Colon)) {
    auto& name = GetSimpleName(
        *name_ref_,
        CompileError_Parse_Label_Invalid);

    // TODO(yosi) 2012-02-18 Label namespace should be isolated from
    // other namespace.
    if (auto const pPresent = name_scope_->Find(name)) {
      // Forward referenced label.
      if (auto const pLabelDef = pPresent->DynamicCast<LabelDef>()) {
        if (pLabelDef->IsDefined()) {
          AddError(CompileError_Parse_Label_Multiple, token);

        } else {
          pLabelDef->Define(frame_reg_);
          name_scope_->Add(*pLabelDef);

          foreach (LabelDef::Enum, oEnum, pLabelDef) {
            auto const oLabelRef = oEnum.Get();
            builder_->EmitCloses(
                *oLabelRef.m_pBBlock->GetLastI(),
                oLabelRef.m_pFrameReg,
                pLabelDef->GetFrameReg());
          }

          auto const pBBlock = pLabelDef->GetBBlock();
          builder_->EmitJumpI(*pBBlock);
          builder_->SetCurr(*pBBlock);
        }
      }

    } else {
      // New label.
      auto const pBBlock = &builder_->NewBBlock();
      builder_->EmitJumpI(*pBBlock);
      builder_->SetCurr(*pBBlock);

      auto const pLabelDef = new LabelDef(
          method_def_,
          name,
          pBBlock,
          token.source_info());

      pLabelDef->Define(frame_reg_);
      name_scope_->Add(*pLabelDef);
    }

    GotoState(State_Stmt);
    return;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    current_type_ = &ProcessTypeName(*name_ref_);
    name_ref_ = new NameRef(
        name_token->name(),
        name_token->source_info());
    GotoState(State_Stmt_Type_Name);
    return;
  }

  // Process last name token as start of expression then process current
  // token.
  CallExpr(*Ty_Void, *name_ref_, State_Stmt_Expr);
  continue;

// ReturnStmt ::= "return" expression? ";"
case State_Stmt_Return_Expr:
  ASSERT(expression_ != nullptr);

  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  if (method_def_->return_type() == *Ty_Void) {
    if (expression_ != Void) {
      AddError(CompileError_Parse_Stmt_Return_Expr, token);
    }
  } else {
    if (expression_ == Void) {
      AddError(CompileError_Parse_Stmt_Return_Void, token);
    }
  }

  builder_->EmitRetI(*expression_);
  builder_->ResetCurr();
  PopState();
  return;

// SwitchStmt ::= "switch" "(" Expr ", token)" SwitchBlock
// SwitchBlock ::= "{" SwitchSection* "}"
// SwtichSection ::= SwitchLabel+ StatementList
// SwitchLabel ::=  "case" ConstantExpr ":" | "default" ":"
case State_Stmt_Switch:
  if (!Expect(*Op__OpenParen, token)) {
    return;
  }

  PushParenthesis(token);
  PushNameScope();
  CallState(State_Expr, State_Stmt_Switch_Expr);
  return;

case State_Stmt_Switch_Body:
  if (token.Is(*Op__CloseBrace)) {
    PopParenthesis(token);
    if (builder_->GetCurr()) {
      AddError(CompileError_Parse_Stmt_Switch_FallThrough, token);
      return;
    }

    auto const pControl = control_stack_.Pop()
        ->DynamicCast<SwitchControl>();

    builder_->SetCurr(*pControl->m_pLabelsBB);
    builder_->EmitJumpI(*pControl->m_pSwitchI->GetBBlock());

    builder_->GetCurr()->GetLastI()->SetSourceInfo(
      pControl->m_pSwitchI->GetSourceInfo());

    builder_->SetCurrSucc(*pControl->m_pSuccBB);
    PopNameScope();
    PopState();
    return;
  }

  if (token.Is(*K_case)) {
    bblock_stack_.Push(builder_->GetCurr());

    auto const pControl = control_stack_.GetTop()
        ->StaticCast<SwitchControl>();

    builder_->SetCurrSucc(*pControl->m_pLabelsBB);

    CallExpr(
        pControl->m_pSwitchI->GetSx()->GetTy(),
        State_Stmt_Switch_Case_Expr);
    return;
  }

  if (token.Is(*K_default)) {
    GotoState(State_Stmt_Switch_Default);
    return;
  }

  if (!builder_->GetCurr()) {
    AddError(CompileError_Parse_Stmt_Switch_NotLabel, token);
    return;
  }

  CallState(State_Stmt, State_Stmt_Switch_Body);
  continue;

case State_Stmt_Switch_Case_Expr: {
  auto const pCurrBB = bblock_stack_.Pop();
  if (pCurrBB && pCurrBB->GetFirstI()) {
    AddError(CompileError_Parse_Stmt_Switch_FallThrough, token);
  }

  if (!Expect(*Op__Colon, token)) {
    return;
  }

  if (expression_ == Void) {
    AddError(CompileError_Parse_Stmt_Switch_BadCase, token);
  }

  auto& sw_frame = *control_stack_.GetTop()->StaticCast<SwitchControl>();

  if (auto const box = sw_frame.FindCase(*expression_)) {
    auto const pBBlock = box->GetBB();
    box->Replace(*expression_);
    if (pCurrBB) {
      builder_->SetCurr(*pCurrBB);
      builder_->EmitJumpI(*pBBlock);
    }
    builder_->SetCurr(*pBBlock);

  } else {
    auto& bblock = builder_->NewBBlock();
    if (pCurrBB) {
      builder_->SetCurr(*pCurrBB);
      builder_->EmitJumpI(bblock);
    }
    DEBUG_FORMAT("case %s to %s", expression_, bblock);
    sw_frame.m_pSwitchI->AddOperand(
        &bblock,
        const_cast<Operand*>(expression_));
    builder_->SetCurr(bblock);
  }

  GotoState(State_Stmt_Switch_Body);
  return;
}

case State_Stmt_Switch_Default:
  if (!Expect(*Op__Colon, token)) {
      return;
  }

  {
    auto const pControl = control_stack_.GetTop()
        ->StaticCast<SwitchControl>();

    if (pControl->m_fHasDefault) {
      AddError(CompileError_Parse_Stmt_Switch_MultipleDefault, token);
    }

    builder_->EmitJumpI(*pControl->m_pDefaultBB);

    pControl->m_fHasDefault = true;
    builder_->SetCurrSucc(*pControl->m_pDefaultBB, *pControl->m_pSuccBB);
  }

  GotoState(State_Stmt_Switch_Body);
  return;

case State_Stmt_Switch_Expr:
  if (!ExpectCloseParen(token)) {
      return;
  }

  GotoState(State_Stmt_Switch_Expr_Paren);
  return;

case State_Stmt_Switch_Expr_Paren:
  if (!Expect(*Op__OpenBrace, token)) {
    return;
  }

  PushParenthesis(token);

  {
    auto const pLabelsBB = builder_->GetCurr();
    auto const pSwitchBB = &builder_->NewBBlock();
    auto const pDefaultBB = &builder_->NewBBlock();
    auto const pSwitchI = new SwitchI(
        const_cast<Operand*>(expression_),
        &pDefaultBB->label());
    builder_->SetCurr(*pSwitchBB);
    builder_->EmitI(*pSwitchI);
    builder_->ResetCurr();

    control_stack_.Push(
        new SwitchControl(
            frame_reg_,
            pLabelsBB,
            pSwitchI,
            pDefaultBB,
            &builder_->NewSucc()));
  }

  GotoState(State_Stmt_Switch_Body);
  return;

// 8.9.5 The throw statement
// ThrowStmt ::= "throw" expression? ";"
case State_Stmt_Throw_Expr:
  ASSERT(expression_ != nullptr);

  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  builder_->EmitI(*new ThrowI(const_cast<Operand*>(expression_)));
  builder_->ResetCurr();
  PopState();
  return;

// 8.10 The try statement
// TryStmt ::= "try" Block Catch* Finally?
// Catch ::= "catch" "(" Type Name ")" Block
// Finally ::= "finally" Block
case State_Stmt_Try:
  if (!Expect(*Op__OpenBrace, token)) {
    return;
  }

  PushParenthesis(token);
  OpenFinally();

  {
    frame_reg_ = new FrameReg(frame_reg_, FrameReg::Kind_Catch);
    builder_->EmitI(*new OpenCatchI(frame_reg_));
    auto& bblock = builder_->NewBBlock();
    builder_->EmitJumpI(bblock);
    builder_->SetCurr(bblock);
  }

  CallState(State_Stmt_Brace, State_Stmt_Try_);
  return;

case State_Stmt_Try_:
  ASSERT(frame_reg_->IsCatch());

  {
    auto& succ_block = builder_->NewSucc();
    if (&succ_block != &builder_->GetSucc()) {
      builder_->EmitJumpI(succ_block);
      builder_->SetSucc(succ_block);
    }
  }

  GotoState(State_Stmt_Try_Block);
  continue;

case State_Stmt_Try_Block:
  if (token.Is(*K_catch)) {
    GotoState(State_Stmt_Try_Catch);
    return;
  }

  builder_->SetCurr(builder_->GetSucc());

  // Close catch frame
  {
    auto const pOpenCatchI = frame_reg_->GetDefI()
        ->StaticCast<OpenCatchI>();

    if (!pOpenCatchI->IsUseless()) {
      builder_->EmitI(*new CloseI(frame_reg_));
    }

    frame_reg_ = frame_reg_->GetOuter();
    ASSERT(frame_reg_ != nullptr);
    ASSERT(frame_reg_->IsFinally());
  }

  if (token.Is(*K_finally)) {
    GotoState(State_Stmt_Try_Block_Finally);
    return;
  }

  // Discard finally frame
  frame_reg_ = frame_reg_->GetOuter();
  ASSERT(frame_reg_ != nullptr);
  ASSERT(frame_reg_->IsFinally());

  GotoState(State_Stmt);
  continue;

case State_Stmt_Try_Block_Finally:
  ASSERT(frame_reg_->IsFinally());

  if (!Expect(*Op__OpenBrace, token)) {
    return;
  }

  PushParenthesis(token);
  {
    auto& finfun = CloseFinally();
    frame_reg_ = frame_reg_->GetOuter();
    builder_ = &Builder::StartFunction(builder_, finfun);
  }

  CallState(State_Stmt_Brace, State_Stmt_Try_Block_Finally_Block);
  return;

case State_Stmt_Try_Block_Finally_Block:
  if (builder_->GetCurr()) {
    builder_->EmitRetI(*Void);
  }
  builder_ = builder_->EndFunction();
  PopState();
  continue;

case State_Stmt_Try_Catch:
  ASSERT(frame_reg_->IsCatch());

  if (!Expect(*Op__OpenParen, token)) {
    return;
  }

  PushParenthesis(token);
  GotoState(State_Stmt_Try_Catch_Paren);
  return;

case State_Stmt_Try_Catch_Block:
  builder_->EmitJumpI(builder_->GetSucc());
  GotoState(State_Stmt_Try_Block);
  continue;

case State_Stmt_Try_Catch_Paren:
  ASSERT(frame_reg_->IsCatch());

  if (auto const pNameRef = GetNameRef(token)) {
    name_ref_ = pNameRef;
    GotoState(State_Stmt_Try_Catch_Paren_Name);
    return;
  }

  AddError(CompileError_Parse_Stmt_Catch_Invalid, token);
  return;

case State_Stmt_Try_Catch_Paren_Name: {
  // name_ref_ contains type name from "catch" clouse.
  ASSERT(frame_reg_->IsCatch());
  ASSERT(name_ref_ != nullptr);

  PushNameScope();

  builder_->set_source_info(token.source_info());
  auto const pCatchBB = &builder_->NewBBlock();
  builder_->SetCurr(*pCatchBB);

  // Emit CatchI instruction.
  {
    auto const pRefI = frame_reg_->GetDefI()->GetBB()->GetLastI();
    auto const pR1 = &builder_->NewRegister();
    auto const pNameRefI = new NameRefI(*Ty_Type, *pR1, *Void, *name_ref_);
    pRefI->GetBB()->InsertBeforeI(*pNameRefI, pRefI);
    frame_reg_->m_nCount++;
    auto const pCatchI = new CatchI(frame_reg_, pR1, pCatchBB);
    pRefI->GetBB()->InsertBeforeI(*pCatchI, pRefI);
    pRefI->GetBB()->AddEdge(pCatchBB)->SetEdgeKind(CfgEdge::Kind_Nonlocal);
  }

  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    GotoState(State_Stmt_Try_Catch_Paren_Name_Paren);
    return;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();
    auto const pVar = new Variable(name);

    auto const pTy = new TypeVar();
    auto const pLocalVarDef = new LocalVarDef(
        *method_def_,
        *pTy,
        name,
        *pVar,
        token.source_info());

    method_def_->Add(*pLocalVarDef);
    name_scope_->Add(*pLocalVarDef);

    bblock_stack_.Push(builder_->GetCurr());

    auto const pV1 = new Values();
    builder_->EmitI(*new NonLocalI(ValuesType::Intern(*pTy), *pV1));

    auto& r2 = builder_->NewRegister();
    builder_->EmitI(*new SelectI(*pTy, r2, *pV1, 0));

    EmitVarDefI(*pVar, r2);

    GotoState(State_Stmt_Try_Catch_Paren_Name_Name);
    return;
  }

  AddError(CompileError_Parse_Stmt_Catch_Invalid, token);
  return;
}

case State_Stmt_Try_Catch_Paren_Name_Name:
  if (!ExpectCloseParen(token)) {
    return;
  }

  GotoState(State_Stmt_Try_Catch_Paren_Name_Name_Paren);
  return;

case State_Stmt_Try_Catch_Paren_Name_Name_Paren:
case State_Stmt_Try_Catch_Paren_Name_Paren:
  if (!Expect(*Op__OpenBrace, token)) {
    return;
  }

  PushParenthesis(token);
  CallState(State_Stmt_Brace, State_Stmt_Try_Catch_Block);
  return;

// UsingStatement ::= using "(" UsingExpr ")" Statement
// UsingExpr ::= Expr | LocalVarDef
case State_Stmt_Using:
  if (!Expect(*Op__OpenParen, token)) {
      return;
  }

  PushParenthesis(token);
  PushNameScope();
  control_stack_.Push(new UsingControl(frame_reg_));

  GotoState(State_Stmt_Using_Paren);
  return;

case State_Stmt_Using_Expr:
  ASSERT(expression_ != nullptr);
  ASSERT(name_scope_ != nullptr);

  if (!ExpectCloseParen(token)) {
    return;
  }

  if (expression_ == Void) {
    return;
  }

  current_type_ = &expression_->GetTy();
  AddVarDef(NewTempName("using"), token.source_info());

  {
    auto& vardef = ProcessVarDef(*expression_);
    ProcessDisposer(*K_using, *vardef.GetDefI()->GetRy());
    ASSERT(frame_reg_ != nullptr);
  }

  CallState(State_Stmt, State_Stmt_Using_Finish);
  return;

case State_Stmt_Using_Finish:
  ASSERT(frame_reg_ != nullptr);
  ASSERT(name_scope_ != nullptr);

  if (builder_->GetCurr()) {
    auto const pDummyI = new CloseI(frame_reg_);
    builder_->EmitI(*pDummyI);

    builder_->EmitCloses(
        *pDummyI,
        frame_reg_,
        GetControl<UsingControl>()->frame_reg_);

    pDummyI->GetBB()->RemoveI(*pDummyI);
  }

  control_stack_.Pop();
  PopNameScope();
  PopState();
  continue;

case State_Stmt_Using_Paren:
  if (token.Is(*Q_var)) {
    current_type_ = new TypeVar();
    GotoState(State_Stmt_Using_Type);
    return;
  }

  if (auto const name_ref = GetNameRef(token)) {
    operand_stack_.Push(name_ref);
    CallState(State_Type, State_Stmt_Using_Paren_Name);
    continue;
  }

  if (auto const keyword = token.DynamicCast<KeywordToken>()) {
    if (auto const type = keyword->value().GetTy()) {
      current_type_ = type;
      GotoState(State_Stmt_Using_Type);
      return;
    }
  }

  CallExpr(*Ty_Disposable, State_Stmt_Using_Expr);
  continue;

case State_Stmt_Using_Paren_Name: {
  auto& name_ref = *operand_stack_.Pop()->StaticCast<NameRef>();

  if (token.DynamicCast<NameToken>()) {
    current_type_ = &ProcessTypeName(name_ref);
    GotoState(State_Stmt_Using_Type);
    continue;
  }

  CallExpr(*Ty_Disposable, name_ref, State_Stmt_Using_Expr);
  continue;
}

case State_Stmt_Using_Type:
  ASSERT(current_type_ != nullptr);
  ASSERT(name_scope_ != nullptr);

  if (current_type_ == Ty_Void) {
    AddError(CompileError_Parse_Stmt_Using_Void, token);
    current_type_ = Ty_Object;
  }

  GetControl<UsingControl>()->current_type_ = current_type_;

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    auto& name = name_token->name();
    AddVarDef(name, name_token->source_info());
    GotoState(State_Stmt_Using_Type_Name);
    return;
  }

  AddError(CompileError_Parse_Stmt_Using_Var, token);
  return;

case State_Stmt_Using_Type_Name:
  if (Expect(*Op__Assign, token)) {
    CallExpr(*current_type_, State_Stmt_Using_Type_Name_Eq_Expr);
    return;
  }
  return;

case State_Stmt_Using_Type_Name_Eq_Expr:
  ASSERT(expression_ != nullptr);
  ASSERT(name_scope_ != nullptr);

  if (expression_ == Void) {
    return;
  }

  {
    auto& vardef = ProcessVarDef(*expression_);
    ProcessDisposer(*K_using, *vardef.GetDefI()->GetRy());
    ASSERT(frame_reg_ != nullptr);
  }

  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    CallState(State_Stmt, State_Stmt_Using_Finish);
    return;
  }

  if (token.Is(*Op__Comma)) {
    GotoState(State_Stmt_Using_Type);
    return;
  }

  AddError(CompileError_Parse_Stmt_Using_Expr, token);
  return;

// VarDef ::= Type Name ("=" Expr)? ("," Name ("=" Expr)?)*
//
case State_Stmt_Type:
  ASSERT(current_type_ != nullptr);

  if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
    current_type_ = &ArrayType::Intern(*current_type_, rank_token->rank());
    GotoState(State_Stmt_Type);
    return;
  }

  if (auto const name_token = token.DynamicCast<NameToken>()) {
    name_ref_ = &name_token->ToNameRef();
    GotoState(State_Stmt_Type_Name);
    return;
  }

  AddError(CompileError_Parse_Stmt_VarDef, token);
  return;

case State_Stmt_Type_Name:
  ASSERT(method_def_ != nullptr);
  ASSERT(name_ref_ != nullptr);
  ASSERT(name_ref_->GetSimpleName() != nullptr);
  ASSERT(current_type_ != nullptr);

  AddVarDef(name_ref_->GetSimpleName()->name(), name_ref_->source_info());

  if (token.Is(*Op__Assign)) {
    CallExpr(*current_type_, State_Stmt_Type_Name_Eq_Expr);
    return;
  }

  if (token.Is(*Op__Comma)) {
    GotoState(State_Stmt_Type_Name_Comma);
    return;
  }

  if (token.Is(*Op__SemiColon)) {
    PopState();
    return;
  }

  AddError(CompileError_Parse_Stmt_VarDef, token);
  return;

case State_Stmt_Type_Name_Comma:
  if (auto const pNameRef = GetNameRef(token)) {
    name_ref_ = pNameRef;
    GotoState(State_Stmt_Type_Name);
    return;
  }

  AddError(CompileError_Parse_Stmt_VarDef, token);
  return;

case State_Stmt_Type_Name_Eq_Expr:
  ProcessVarDef(*expression_);

  if (token.Is(*Op__Comma)) {
    GotoState(State_Stmt_Type_Name_Comma);
    return;
  }

  if (!Expect(*Op__SemiColon, token)) {
    return;
  }

  PopState();
  return;

// "var" name "=" expr ";"
case State_Stmt_VarDef:
  ASSERT(method_def_ != nullptr);

  if (auto const pNameRef = GetNameRef(token)) {
    name_ref_ = pNameRef;
    GotoState(State_Stmt_Type_Name);
    return;
  }

  AddError(CompileError_Parse_ExpectName, token);
  return;

// "while" "(" BooleanExpr ")" Statement
//
//  curr:
//      ...
//      JUMP continue
//  loop:
//      ... Statement ...
//      JUMP continue
//  continue:
//      ... BooleanExpr ...
//      BRANCH loop break
//  break:
case State_Stmt_While:
  if (!Expect(*Op__OpenParen, token)) {
    return;
  }

  PushParenthesis(token);
  {
    auto const pContinueBB = &builder_->NewBBlock();
    builder_->EmitJumpI(*pContinueBB);
    builder_->SetCurrSucc(*pContinueBB, *pContinueBB);
  }

  CallState(State_BoolExpr, State_Stmt_While_Expr);
  return;

case State_Stmt_While_Expr:
  if (expression_ == Void) {
    AddError(CompileError_Parse_Expect_BooleanExpr, token);
    return;
  }

  if (!ExpectCloseParen(token)) {
    return;
  }

  {
    auto const pContinueBB = builder_->GetCurr();
    auto const pLoopBB = &builder_->NewBBlock();
    auto const pBreakBB = &builder_->NewSucc();
    auto& bool_src = *expression_->StaticCast<BoolOutput>();
    builder_->EmitBranchI(bool_src, *pLoopBB, *pBreakBB);
    builder_->SetCurrSucc(*pLoopBB, *pLoopBB);
    control_stack_.Push(
        new WhileControl(frame_reg_, pBreakBB, pContinueBB));
  }

  CallState(State_Stmt, State_Stmt_While_Expr_Stmt);
  return;

case State_Stmt_While_Expr_Stmt: {
  auto const pLoop = PopControl<WhileControl>();
  auto const pBreakBB = pLoop->m_pBreakBB;
  auto const pContinueBB = pLoop->m_pContinueBB;
  builder_->EmitJumpI(*pContinueBB);
  builder_->SetCurrSucc(*pBreakBB, *pBreakBB);
  PopState();
  continue;
}
