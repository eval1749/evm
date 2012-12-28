// -*- Mode: C++; -*-
//
// @(#)$Id$
// Evita Compiler - Parser - Expr
//
case State_BoolExpr:
  CallExpr(*Ty_Boolean, State_BoolExpr_Expr);
  continue;

case State_BoolExpr_Expr:
  if (expression_ == Void) {
    AddError(CompileError_Parse_Expect_BooleanExpr, token);
  } else {
    expression_ = &CoerceToBoolOutput(*expression_);
  }

  PopState();
  continue;

case State_Expr:
  expression_ = Void;
  GotoState(State_Expr_Assign);
  continue;

case State_Expr_Expr: {
  ASSERT(expression_ != nullptr);

  name_lexer_.PopMode();
  auto const pTy = expect_stack_.Pop();
  if (pTy == Ty_Void) {
    if (auto const pR1 = expression_->DynamicCast<Register>()) {
      // pR1 may be used by StoreI.
      if (!pR1->HasUser()) {
        auto const pDefI = pR1->GetDefI();
        pDefI->set_output_type(*Ty_Void);
        pDefI->set_output(*Void);
      }
    }

  } else {
    expression_ = &EmitCast(*expression_, *pTy);
  }

  PopState();
  continue;
}

// Add := Mul
//     | Add "+" Mul
//     | Add "-" Mul
case State_Expr_Add:
  CallState(State_Expr_Mul, State_Expr_Add_Mul);
  continue;

case State_Expr_Add_Mul:
  if (ProcessExprLeft(
          token,
          Operator::Category_Add,
          State_Expr_Mul,
          State_Expr_Add_Mul_Op__Mul)) {
      return;
  }
  continue;

case State_Expr_Add_Mul_Op__Mul:
  if (ProcessExprRight(State_Expr_Add_Mul)) {
    continue;
  }
  return;

// Assign := Unary AssignOp Expr
// AssignOp := "=" "*=" ... "|="
case State_Expr_Assign:
  CallState(State_Expr_Cond, State_Expr_Assign_Expr);
  continue;

case State_Expr_Assign_Expr:
  if (expression_ == Void) {
      PopState();
      continue;
  }

  if (token.Is(*Op__Assign)) {
    if (auto const getter_inst = GetGetterInstruction(*expression_)) {
      instruction_stack_.Push(getter_inst);
      CallExpr(expression_->GetTy(), State_Expr_Assign_Expr_Simple_Expr);
      return;
    }

    AddError(CompileError_Parse_Expect_LeftValue, token);
    PopState();
    return;
  }

  if (auto const op_token = token.DynamicCast<OperatorToken>()) {
    if (op_token->value().GetCategory() == Operator::Category_Assign) {
      if (auto const getter_inst = GetGetterInstruction(*expression_)) {
        instruction_stack_.Push(getter_inst);
        operand_stack_.Push(
            new NameRef(
                op_token->value().binary_operator_name(),
                token.source_info()));
        CallExpr(expression_->GetTy(), State_Expr_Assign_Expr_Compound_Expr);
        return;
      }

      AddError(CompileError_Parse_Expect_LeftValue, token);
      PopState();
      return;
    }
  }

  PopState();
  continue;

// Compound assignment
//  VALUES %v1 <=
//  CALL %r2 <= get_Item %v1
//  ... %r3 <=
//  VALUES %v4 <= %r3 ...
//  CALL set_Item %v4
case State_Expr_Assign_Expr_Compound_Expr: {
  auto& getter_inst = *instruction_stack_.Pop();
  auto& binary_op = *operand_stack_.Pop()->StaticCast<NameRef>();

  auto& args = builder_->EmitValuesI(getter_inst.output(), *expression_);
  expression_ = &builder_->EmitCallI(
      *new TypeVar(),
      builder_->EmitNameRefI(binary_op),
      args);

  if (getter_inst.Is<CallI>()) {
    auto& getter_args_inst = *getter_inst.op1().def_inst()
        ->StaticCast<ValuesI>();
    auto& args = builder_->EmitValuesI();
    auto& args_inst = *args.def_inst()->StaticCast<ValuesI>();
    args_inst.AppendOperand(*expression_);
    for (auto& operand: getter_args_inst.operands()) {
      args_inst.AppendOperand(operand);
    }
    args_inst.UpdateOutputType();

    auto& getter_name_load_inst = *getter_inst.op0().def_inst()
        ->StaticCast<LoadI>();

    auto& getter_name_ref_inst = *getter_name_load_inst.op0().def_inst()
        ->StaticCast<NameRefI>();

    auto& setter = builder_->EmitLoadI(
        builder_->EmitNameRefI(
            PointerType::Intern(*Ty_Void),
            getter_name_ref_inst.op0(),
            *new NameRef(*Q_set_Item, getter_inst.source_info())));
    builder_->EmitCallI(setter, args);

  } else if (getter_inst.Is<LoadI>()) {
    builder_->EmitStoreI(getter_inst.op0(), *expression_);
  }

  PopState();
  continue;
}

case State_Expr_Assign_Expr_Simple_Expr: {
  auto& getter_inst = *instruction_stack_.Pop();

  if (getter_inst.Is<CallI>()) {
    auto& getter_args_inst = *getter_inst.op1().def_inst()
        ->StaticCast<ValuesI>();
    auto& args = builder_->EmitValuesI();
    auto& args_inst = *args.def_inst()->StaticCast<ValuesI>();
    args_inst.AppendOperand(*expression_);
    for (auto& operand: getter_args_inst.operands()) {
      args_inst.AppendOperand(operand);
    }
    args_inst.UpdateOutputType();

    auto& getter_name_load_inst = *getter_inst.op0().def_inst()
        ->StaticCast<LoadI>();

    auto& getter_name_ref_inst = *getter_name_load_inst.op0().def_inst()
        ->StaticCast<NameRefI>();

    auto& setter = builder_->EmitLoadI(
        builder_->EmitNameRefI(
            PointerType::Intern(*Ty_Void),
            getter_name_ref_inst.op0(),
            *new NameRef(*Q_set_Item, getter_inst.source_info())));
    builder_->EmitCallI(setter, args);

    getter_inst.GetBBlock()->RemoveI(getter_inst);
    getter_args_inst.GetBBlock()->RemoveI(getter_args_inst);
    getter_name_load_inst.GetBBlock()->RemoveI(getter_name_load_inst);
    getter_name_ref_inst.GetBBlock()->RemoveI(getter_name_ref_inst);

  } else if (getter_inst.Is<LoadI>()) {
    builder_->EmitStoreI(getter_inst.op0(), *expression_);
    getter_inst.GetBBlock()->RemoveI(getter_inst);

  } else {
    AddError(
        getter_inst.source_info(),
        CompileError_Parse_Internal_Unexpected,
        getter_inst);
  }

  PopState();
  continue;
}

// 7.7.5 Prefix increment and decrement operators
//  PreInc := "++" Unary
case State_Expr_Add1_Unary:
  EmitUnaryOperator(
          *new NameRef(
              *Q_operator_Increment, token.source_info()));
  PopState();
  continue;

// Equality = Relational
//          | Equality "==" Relational
//          | Equality "!=" Relational
case State_Expr_Equality:
  CallState(
      State_Expr_Relational,
      State_Expr_Equality_Relational);
  continue;

case State_Expr_Equality_Relational:
  if (ProcessExprLeft(
          token,
          Operator::Category_Equality,
          State_Expr_Relational,
          State_Expr_Equality_Relational_Op__Relational)) {
      return;
  }
  continue;

case State_Expr_Equality_Relational_Op__Relational:
  if (ProcessExprRight(State_Expr_Equality_Relational)) {
    continue;
  }
  return;

// Cond := NullOr
//      | NullOr "?" Expr ":" Expr
case State_Expr_Cond:
  CallState(State_Expr_NullOr, State_Expr_Cond_Bool_Op);
  continue;

case State_Expr_Cond_Bool_Op:
  if (expression_ == Void) {
    PopState();
    continue;
  }

  if (!token.Is(*Op__Question)) {
    PopState();
    continue;
  }

  StartIf(CoerceToBoolOutput(*expression_));
  CallExpr(State_Expr_Cond_Bool_Op__Then);
  return;

case State_Expr_Cond_Bool_Op__Then: {
  auto const pElseBB = bblock_stack_.Pop();

  bblock_stack_.Push(builder_->GetCurr());
  operand_stack_.Push(expression_);

  auto& end_if_block = builder_->GetSucc();
  builder_->EmitJumpI(end_if_block);
  builder_->SetCurrSucc(*pElseBB, end_if_block);

  Expect(*Op__Colon, token);
  CallExpr(State_Expr_Cond_Bool_Op__Then_Else);
  return;
}

case State_Expr_Cond_Bool_Op__Then_Else: {
  auto const pElseBB = builder_->GetCurr();
  auto const pElseValue = expression_;

  auto const pEndIfBB = &builder_->GetSucc();

  auto const pThenBB = bblock_stack_.Pop();
  auto const pThenValue = operand_stack_.Pop();

  builder_->EmitJumpI(*pEndIfBB);
  builder_->SetCurrSucc(*pEndIfBB, *pEndIfBB);

  auto& phi_inst = builder_->EmitPhiI(*expect_stack_.GetTop());
  phi_inst.AddOperand(pThenBB, pThenValue);
  phi_inst.AddOperand(pElseBB, pElseValue);
  expression_ = &phi_inst.output();
  PopState();
  continue;
}

// CondAnd := LogIor
//         | CondAnd "&&" LogIor
case State_Expr_CondAnd:
  CallState(State_Expr_LogIor, State_Expr_CondAnd_LogIor);
  continue;

case State_Expr_CondAnd_LogIor:
  if (expression_ == Void) {
    PopState();
    continue;
  }

  if (!token.Is(*Op__CondAnd)) {
    PopState();
    continue;
  }

  {
    auto& bool_src = CoerceToBoolOutput(*expression_);
    auto& then_block = builder_->NewBBlock();
    auto& else_block = *builder_->GetCurr();
    auto& endif_block = builder_->NewSucc();
    bblock_stack_.Push(&else_block);
    builder_->EmitBranchI(bool_src, then_block, endif_block);
    builder_->SetCurrSucc(then_block, endif_block);
  }

  CallState(State_Expr_LogIor, State_Expr_CondAnd_LogIor_Op__LogIor);
  return;

case State_Expr_CondAnd_LogIor_Op__LogIor: {
  ASSERT(expression_ != Void);

  auto const pElseBB = bblock_stack_.Pop();
  auto const pEndIfBB = &builder_->GetSucc();

  auto const pThenBB = builder_->GetCurr();
  auto const pThenValue = &CoerceToBoolean(*expression_);

  builder_->EmitJumpI(*pEndIfBB);
  builder_->SetCurrSucc(*pEndIfBB, *pEndIfBB);

  auto& phi_inst = builder_->EmitPhiI(*Ty_Boolean);
  phi_inst.AddOperand(*pThenBB, *pThenValue);
  phi_inst.AddOperand(*pElseBB, builder_->NewLiteral(false));
  expression_ = &phi_inst.output();
  GotoState(State_Expr_CondAnd_LogIor);
  continue;
}

// CondOr := CondAnd
//        | CondOr "&&" CondAnd
case State_Expr_CondOr:
  CallState(State_Expr_CondAnd, State_Expr_CondOr_CondAnd);
  continue;

case State_Expr_CondOr_CondAnd:
  if (expression_ == Void) {
    PopState();
    continue;
  }

  if (!token.Is(*Op__CondOr)) {
    PopState();
    continue;
  }

  {
    auto& bool_src = CoerceToBoolOutput(*expression_);
    auto& else_block = builder_->NewBBlock();
    auto& endif_block = builder_->NewSucc();
    bblock_stack_.Push(builder_->GetCurr());
    builder_->EmitBranchI(bool_src, endif_block, else_block);
    builder_->SetCurrSucc(else_block, endif_block);
  }

  CallState(State_Expr_CondAnd, State_Expr_CondOr_CondAnd_Op_CondAnd);
  return;

case State_Expr_CondOr_CondAnd_Op_CondAnd: {
  auto& then_bbBB = *bblock_stack_.Pop();
  auto& endif_bb = builder_->GetSucc();

  if (expression_ == Void) {
    PopState();
    continue;
  }

  auto& else_bb = *builder_->GetCurr();
  auto& else_value = CoerceToBoolean(*expression_);

  builder_->EmitJumpI(endif_bb);
  builder_->SetCurrSucc(endif_bb, endif_bb);

  auto& phi_inst = builder_->EmitPhiI(*Ty_Boolean);
  phi_inst.AddOperand(&then_bbBB, &builder_->NewLiteral(true));
  phi_inst.AddOperand(&else_bb, &else_value);
  expression_ = &phi_inst.output();
  GotoState(State_Expr_CondOr_CondAnd);
  continue;
}

// LogAnd := Equality
//        | LogAnd "&&" Equality
case State_Expr_LogAnd:
  CallState(State_Expr_Equality, State_Expr_LogAnd_Equality);
  continue;

case State_Expr_LogAnd_Equality:
  if (ProcessExprLeft(
          token,
          Operator::Category_LogAnd,
          State_Expr_Equality,
          State_Expr_LogAnd_Equality_Op__Equality)) {
      return;
  }
  continue;

case State_Expr_LogAnd_Equality_Op__Equality:
  if (ProcessExprRight(State_Expr_LogAnd_Equality)) {
      continue;
  }
  return;

// LogIor := LogXor
//        | LogIor "<<" LogXor
//        | LogIor "<<" LogXor
case State_Expr_LogIor:
  CallState(State_Expr_LogXor, State_Expr_LogIor_LogXor);
  continue;

case State_Expr_LogIor_LogXor:
  if (ProcessExprLeft(
          token,
          Operator::Category_LogIor,
          State_Expr_LogXor,
          State_Expr_LogIor_LogXor_Op__LogXor)) {
      return;
  }
  continue;

case State_Expr_LogIor_LogXor_Op__LogXor:
  if (ProcessExprRight(State_Expr_LogIor_LogXor)) {
      continue;
  }
  return;

// 7.7.4 Bitwise complement operator
//  LogNot := "~" Unary
case State_Expr_LogNot_Unary:
  ASSERT(expression_ != nullptr);

  if (expression_->GetTy().IsInt()) {
    auto& type = expression_->GetTy();
    auto& zero = builder_->NewLiteral(type, 0);
    auto& r0 = builder_->EmitI(Op_LogXor, type, *expression_, zero);
    expression_ = &r0;
    PopState();
    continue;
  }

  EndUnary(*Op__LogNot);
  continue;

// LogXor := LogAnd
//        | LogXor "^" LogAnd
//        | LogXor "^" LogAnd
case State_Expr_LogXor:
  CallState(State_Expr_LogAnd, State_Expr_LogXor_LogAnd);
  continue;

case State_Expr_LogXor_LogAnd:
  if (ProcessExprLeft(
          token,
          Operator::Category_LogXor,
          State_Expr_LogAnd,
          State_Expr_LogXor_LogAnd_Op__LogAnd)) {
      return;
  }
  continue;

case State_Expr_LogXor_LogAnd_Op__LogAnd:
  if (ProcessExprRight(State_Expr_LogXor_LogAnd)) {
      continue;
  }
  return;

// 7.7.2 Unary minus operator
case State_Expr_Minus_Unary:
  ASSERT(expression_ != nullptr);

  if (expression_->GetTy().IsInt() || expression_->GetTy().IsFloat()) {
    auto& type = expression_->GetTy();
    auto& zero = builder_->NewLiteral(type, 0);
    expression_ = &builder_->EmitI(Op_Sub, type, zero, *expression_);
    PopState();
    continue;
  }

  EndUnary(*Op__Sub);
  continue;

// Mul := Unary
//     | Mul "*" Mul
//     | Mul "/" Mul
//     | Mul "%" Mul
case State_Expr_Mul:
  CallState(State_Expr_Unary, State_Expr_Mul_Unary);
  continue;

case State_Expr_Mul_Unary:
  if (ProcessExprLeft(
          token,
          Operator::Category_Mul,
          State_Expr_Unary,
          State_Expr_Mul_Unary_Op__Mul)) {
      return;
  }
  continue;

case State_Expr_Mul_Unary_Op__Mul:
  if (ProcessExprRight(State_Expr_Mul_Unary)) {
      continue;
  }
  return;

// 7.7.3 Logical negation operator
//  Not := "!" Unary
case State_Expr_Not_Unary:
  ASSERT(expression_ != nullptr);

  if (expression_->GetTy() == *Ty_Boolean) {
      expression_ = &builder_->EmitCallI(
        *Ty_Boolean,
        *new NameRef(*Q_operator_CondNot, builder_->source_info()),
        *expression_);
      PopState();
      continue;
  }

  EndUnary(*Op__Not);
  continue;

// NullOr := CondOr
//        | CondOr "??" NullOr
case State_Expr_NullOr:
  CallState(State_Expr_CondOr, State_Expr_NullOr_Expr);
  continue;

case State_Expr_NullOr_Expr:
  if (expression_ == Void) {
      PopState();
      continue;
  }

  if (!token.Is(*Op__Question2)) {
      PopState();
      continue;
  }

  {
    auto const pElseBB = &builder_->NewBBlock();
    auto const pEndIfBB = &builder_->NewSucc();
    bblock_stack_.Push(builder_->GetCurr());
    operand_stack_.Push(expression_);

    auto& null_reg = builder_->EmitNullI(expression_->type());
    auto& b1 = builder_->EmitBool(Op_Ne, *expression_, null_reg);
    builder_->EmitBranchI(b1, *pEndIfBB, *pElseBB);
    builder_->SetCurrSucc(*pElseBB, *pEndIfBB);
  }

  CallState(State_Expr_CondOr, State_Expr_NullOr_Expr_Op__Expr);
  return;

case State_Expr_NullOr_Expr_Op__Expr: {
  auto const pThenBB = bblock_stack_.Pop();
  auto const pThenValue = operand_stack_.Pop();

  auto const pEndIfBB = &builder_->GetSucc();

  if (expression_ == Void) {
      PopState();
      continue;
  }

  auto const pElseBB = builder_->GetCurr();
  auto const pElseValue = expression_;

  builder_->EmitJumpI(*pEndIfBB);
  builder_->SetCurrSucc(*pEndIfBB, *pEndIfBB);

  auto& phi_inst = builder_->EmitPhiI(*new TypeVar());
  phi_inst.AddOperand(pThenBB, pThenValue);
  phi_inst.AddOperand(pElseBB, pElseValue);
  expression_ = &phi_inst.output();

  GotoState(State_Expr_NullOr_Expr);
  continue;
}

// 7.7.1 Unary plus operator
case State_Expr_Plus_Unary:
  ASSERT(expression_ != nullptr);

  if (expression_->GetTy().IsInt() || expression_->GetTy().IsFloat()) {
      PopState();
      continue;
  }

  EndUnary(*Op__Add);
  continue;

// Primary Expression
case State_Expr_Primary:
  ASSERT(expression_ != nullptr);

  if (token.Is(*Op__OpenParen)) {
      PushParenthesis(token);
      expect_stack_.Push(Ty_Object);
      CallExpr(State_Expr_Primary_Paren_Expr);
      return;
  }

  if (auto const lit_token = token.DynamicCast<LiteralToken>()) {
      expression_ = &lit_token->ToLiteral(memory_zone_);
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*K_base)) {
      expression_ = method_def_ == nullptr
          ? static_cast<Operand*>(Void)
          : &method_def_->this_operand();

      if (expression_ == Void) {
        expression_ = Void;
        AddError(CompileError_Parse_Expr_NoBase, token);
      }

      // TODO 2011-04-29 eval1794 cast this to base.
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*K_false)) {
      expression_ = &builder_->NewLiteral(false);
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*K_new)) {
      GotoState(State_Expr_Primary_New);
      return;
  }

  if (token.Is(*K_null)) {
      expression_ = &builder_->EmitNullI(*new TypeVar());
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*K_this)) {
      expression_ = method_def_ == nullptr
          ? Void
          : &method_def_->this_operand();

      if (expression_ == Void) {
          expression_ = Void;
          AddError(CompileError_Parse_Expr_NoThis, token);
      }

      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*K_true)) {
      expression_ = &builder_->NewLiteral(true);
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (auto const pNameRef = GetNameRef(token)) {
      name_ref_ = pNameRef;
      GotoState(State_Expr_Primary_Name);
      return;
  }

  expression_ = Void;
  PopState();
  continue;

case State_Expr_Primary_Dot:
  ASSERT(expression_ != Void);

  if (auto const name_ref = GetNameRef(token)) {
    // Name reference
    //  %r0 <= ...
    //  NAMEREF T* %r1 <= %r0 $NameRef
    //  LOAD T %r2 <= %r1
    auto& ty = *new TypeVar();
    auto& r1 = builder_->EmitNameRefI(ty, *expression_, *name_ref);
    expression_ = &builder_->EmitLoadI(r1);
    GotoState(State_Expr_Primary_Primary);
    return;
  }

  AddError(CompileError_Parse_Expr_BadAfterDot, token);
  return;

case State_Expr_Primary_Name: {
  ASSERT(name_ref_ != nullptr);

  NameRef::Enum it(*name_ref_);
  if (auto const simple_name = it->DynamicCast<NameRef::SimpleName>()) {
    auto const present = Find(simple_name->name());
    if (auto const var_def = present->DynamicCast<VarDef>()) {
      // Local variable reference
      //  FIELDPTR T* %r1 <= %r2 ClosedCell value
      //  LOAD T %r2 <= %r1
      auto& var = var_def->GetVariable();
      auto& r1 = EmitVarPtr(var);
      auto& r2 = builder_->EmitLoadI(r1);
      it.Next();
      if (it.AtEnd()) {
        expression_ = &r2;
      } else {
        ArrayList_<const NameRef::Item*> items;
        while (!it.AtEnd()) {
          items.Add(&*it);
          it.Next();
        }
        expression_ = &EmitLoadNameRef(r2, *new NameRef(items.ToVector()));
      }
      GotoState(State_Expr_Primary_Primary);
      continue;
    }
  }

  expression_ = &EmitLoadNameRef(*Void, *name_ref_);
  GotoState(State_Expr_Primary_Primary);
  continue;
}

// InvokeExpr ::= Primary "(" ArgList ")"
// ArrayRefExpr ::= Primary "[" Expr ("," Expr)* "]"
case State_Expr_Primary_Primary:
  ASSERT(expression_ != Void);

  if (token.Is(*Op__Dot)) {
      GotoState(State_Expr_Primary_Dot);
      return;
  }

  if (token.Is(*Op__OpenBracket)) {
      PushParenthesis(token);
      operand_stack_.Push(expression_);
      PushI(*new(builder_->zone()) ValuesI());
      CallExpr(*new TypeVar(), State_Expr_Primary_Primary_Bracket_Expr);
      return;
  }

  if (token.Is(*Op__OpenParen)) {
      PushParenthesis(token);
      auto const pR1 = &builder_->NewRegister();
      auto const pV2 = new Values();
      PushI(*new CallI(*new TypeVar(), *pR1, *expression_, *pV2));
      PushI(*new ValuesI(*pV2));

      GotoState(State_Expr_Primary_Primary_Paren);
      return;
  }

  ContinueState(State_Expr_Primary_Post);
  continue;

case State_Expr_Primary_Primary_Bracket_Expr: {
  ASSERT(expression_ != Void);

  auto& array = *operand_stack_.Pop();
  auto& args_inst = *instruction_stack_.Pop();
  ASSERT(args_inst.Is<ValuesI>());
  args_inst.AppendOperand(*expression_);

  if (token.Is(*Op__CloseBracket)) {
    PopParenthesis(token);
    auto& callee_name = *new NameRef(*Q_get_Item, token.source_info());
    auto& callee = builder_->EmitLoadI(
        builder_->EmitNameRefI(
            PointerType::Intern(*Ty_Void),
            array,
            callee_name));
    auto& args = builder_->NewValues();
    args_inst.set_output(args);
    args_inst.StaticCast<ValuesI>()->UpdateOutputType();
    builder_->EmitI(args_inst);
    expression_ = &builder_->EmitCallI(*new TypeVar(), callee, args);
    GotoState(State_Expr_Primary_Primary);
    return;
  }

  if (token.Is(*Op__Comma)) {
    operand_stack_.Push(&array);
    PushI(args_inst);
    CallExpr(*new TypeVar(), State_Expr_Primary_Primary_Bracket_Expr);
    return;
  }

  AddError(CompileError_Parse_Expr_EltRef_Invalid, token);
  return;
}

case State_Expr_Primary_Primary_Paren: {
  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    expression_ = ProcessInvoke();
    GotoState(State_Expr_Primary_Primary);
    return;
  }

  CallExpr(*new TypeVar(), State_Expr_Primary_Primary_Paren_Expr);
  continue;
}

case State_Expr_Primary_Primary_Paren_Expr: {
  ASSERT(expression_ != Void);

  {
    auto const pValuesI = instruction_stack_.GetTop()->StaticCast<ValuesI>();
    pValuesI->AppendOperand(expression_);
  }

  if (token.Is(*Op__CloseParen)) {
      PopParenthesis(token);
      expression_ = ProcessInvoke();
      GotoState(State_Expr_Primary_Primary);
      return;
  }

  if (token.Is(*Op__Comma)) {
      CallExpr(*new TypeVar(), State_Expr_Primary_Primary_Paren_Expr);
      return;
  }

  AddError(CompileError_Parse_Expr_BadExpr, token);
  return;
}

// 7.6.10 The new operator
//  "new" Type "(" Expr ("," Expr)* ")"
//  "new" Type "[" Expr ("," Expr)* "]" RankSpec*
//  "new" Type RankSpec+ "{" Expr+ "}"
//  "new" Type "{" ... "}" // initialization
//  "new" "{" ... "}"   // anonymous class
case State_Expr_Primary_New:
  if (auto const name_ref = GetNameRef(token)) {
      current_type_ = &ProcessTypeName(*name_ref);
      expression_ = nullptr;
      GotoState(State_Expr_Primary_New_Type);
      return;
  }

  if (auto const keyword = token.DynamicCast<KeywordToken>()) {
    if (auto const type = keyword->value().GetTy()) {
      current_type_ = type;
      expression_ = nullptr;
      GotoState(State_Expr_Primary_New_Type);
      return;
    }
  }

  if (auto const pType = token.DynamicCast<Type>()) {
    current_type_ = pType;
    expression_ = nullptr;
    GotoState(State_Expr_Primary_New_Type);
    return;
  }

  if (token.Is(*Op__OpenBrace)) {
    AddError(
        token.source_info(),
        CompileError_Parse_NYI,
        "Anonymous class creation expresion");
      return;
  }

  AddError(CompileError_Parse_Expr_New_Name, token);
  return;

case State_Expr_Primary_New_ArrayType:
  if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
    current_type_ = &ArrayType::Intern(*current_type_, rank_token->rank());
    return;
  }

  if (token.Is(*Op__OpenBrace)) {
    // TODO(yosi) 2012-05-03 NYI: "new" ArrayType "{" ... "}"
    AddError(
        token.source_info(),
        CompileError_Parse_NYI,
        "Array initializer");
    return;
  }

  AddError(
      token.source_info(),
      CompileError_Parse_Expr_NewArray_Invalid,
      "Array creation expression requires array size or initializer.");
  return;

case State_Expr_Primary_New_Type:
  ASSERT(current_type_ != nullptr);

  if (token.Is(*Op__OpenBrace)) {
    // TODO(yosi) 2012-05-03 NYI: "new" Type "{" ... "}"
    AddError(
        token.source_info(),
        CompileError_Parse_NYI,
        "Object initialization expression");
      return;
  }

  if (token.Is(*Op__OpenBracket)) {
   PushParenthesis(token);
    operand_stack_.Push(current_type_);
    auto& args_inst = *new(builder_->zone()) ValuesI(builder_->NewValues());
    PushI(args_inst);
    GotoState(State_Expr_Primary_New_Type_Bracket);
    return;
  }

  if (token.Is(*Op__OpenParen)) {
    PushParenthesis(token);
    operand_stack_.Push(current_type_);
    auto& args_inst = *new(builder_->zone()) ValuesI(builder_->NewValues());
    PushI(args_inst);
    GotoState(State_Expr_Primary_New_Type_Paren);
    return;
  }

  if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
    current_type_ = &ArrayType::Intern(*current_type_, rank_token->rank());
    GotoState(State_Expr_Primary_New_ArrayType);
    return;
  }

  AddError(CompileError_Parse_Expr_New_Invalid, token);
  return;

case State_Expr_Primary_New_Type_Bracket:
  CallExpr(*Ty_Int32, State_Expr_Primary_New_Type_Bracket_Expr);
  continue;

case State_Expr_Primary_New_Type_Bracket_Expr: {
  auto& args_inst = *instruction_stack_.Pop()->StaticCast<ValuesI>();
  if (expression_ != Void) {
    args_inst.AppendOperand(*expression_);
  }

  if (token.Is(*Op__CloseBracket)) {
    PopParenthesis(token);
    builder_->EmitI(args_inst);
    args_inst.UpdateOutputType();
    auto& elemty = *operand_stack_.Pop()->StaticCast<Type>();
    auto& new_array = builder_->NewRegister();
    builder_->EmitI(
        *new(builder_->zone())
            NewArrayI(
                ArrayType::Intern(elemty, args_inst.CountOperands()),
                new_array,
                *args_inst.output().StaticCast<Values>()));
    expression_ = &new_array;
    GotoState(State_Expr_Primary_New_Type_Bracket_Expr_Bracket);
    return;
  }

  if (token.Is(*Op__Comma)) {
    PushI(args_inst);
    GotoState(State_Expr_Primary_New_Type_Bracket);
    return;
  }

  AddError(CompileError_Parse_Expr_NewArray_Invalid, token);
  return;
}

case State_Expr_Primary_New_Type_Bracket_Expr_Bracket:
  if (auto const rank_token = token.DynamicCast<RankSpecToken>()) {
    auto& new_array_inst = *builder_->GetCurr()->GetLastI();
    ASSERT(new_array_inst.Is<NewArrayI>());
    new_array_inst.set_output_type(
        ArrayType::Intern(new_array_inst.output_type(), rank_token->rank()));
    return;
  }
  PopState();
  continue;

case State_Expr_Primary_New_Type_Paren:
  CallExpr(*new TypeVar(), State_Expr_Primary_New_Type_Paren_Expr);
  continue;

case State_Expr_Primary_New_Type_Paren_Expr: {
  auto& args_inst = *instruction_stack_.Pop()->StaticCast<ValuesI>();
  if (expression_ != Void) {
    args_inst.AppendOperand(*expression_);
  }

  if (token.Is(*Op__CloseParen)) {
    PopParenthesis(token);
    auto& type = *operand_stack_.Pop()->StaticCast<Type>();
    auto& new_object = builder_->NewRegister();
    builder_->EmitI(*new(builder_->zone()) NewI(type, new_object));
    builder_->EmitI(args_inst);
    args_inst.UpdateOutputType();

    auto& ctor_ref = builder_->EmitNameRefI(
        PointerType::Intern(*Ty_Void),
        new_object,
        *new NameRef(*QD_ctor, token.source_info()));

    builder_->EmitCallI(
        builder_->EmitLoadI(ctor_ref),
        *args_inst.output().StaticCast<Values>());

    expression_ = &new_object;
    GotoState(State_Expr_Primary_New_Type_Bracket_Expr_Bracket);
    return;
  }

  if (token.Is(*Op__Comma)) {
    PushI(args_inst);
    GotoState(State_Expr_Primary_New_Type_Paren);
    return;
  }

  AddError(CompileError_Parse_Expr_New_Invalid, token);
  return;
}

// "(" expr ")"
case State_Expr_Primary_Paren_Expr:
  expect_stack_.Pop();
  if (!ExpectCloseParen(token)) {
      return;
  }

  GotoState(State_Expr_Primary_Primary);
  return;

// 7.6.9 Postfix increment and decrement operators
//      PostInc ::= Primary "++"
case State_Expr_Primary_Post:
  if (token.Is(*Op__Add1)) {
    EmitUnaryOperator(
        *new NameRef(
            *Q_operator_Increment, token.source_info()));
    PopState();
    return;
  }

  if (token.Is(*Op__Sub1)) {
    EmitUnaryOperator(
        *new NameRef(
            *Q_operator_Decrement, token.source_info()));
    PopState();
    return;
  }

  PopState();
  continue;

// Relation = Shift
//          | Relation "<" Shift
//          | Relation "<=" Shift
//          | Relation ">" Shift
//          | Relation ">=" Shift
case State_Expr_Relational:
  CallState(State_Expr_Shift, State_Expr_Relational_Shift);
  continue;

case State_Expr_Relational_Shift:
  if (ProcessExprLeft(
          token,
          Operator::Category_Relational,
          State_Expr_Shift,
          State_Expr_Relational_Shift_Op__Shift)) {
      return;
  }
  continue;

case State_Expr_Relational_Shift_Op__Shift:
  if (ProcessExprRight(State_Expr_Relational_Shift)) {
      continue;
  }
  return;

// Shift = Add
//          | Shift "<<" Add
//          | Shift "<<" Add
case State_Expr_Shift:
  CallState(State_Expr_Add, State_Expr_Shift_Expr);
  continue;

case State_Expr_Shift_Expr:
  if (ProcessExprLeft(
          token,
          Operator::Category_Shift,
          State_Expr_Add,
          State_Expr_Shift_Expr_Op__Expr)) {
      return;
  }
  continue;

case State_Expr_Shift_Expr_Op__Expr:
  if (ProcessExprRight(State_Expr_Shift_Expr)) {
      continue;
  }
  return;

// 7.7.5 Prefix increment and decrement operators
//  PreInc := "--" Unary
case State_Expr_Sub1_Unary:
  EmitUnaryOperator(
          *new NameRef(
              *Q_operator_Decrement, token.source_info()));
  PopState();
  continue;

// 7.7 Unary operators
case State_Expr_Unary:
  if (token.Is(*Op__Add)) {
      CallState(State_Expr_Unary, State_Expr_Plus_Unary);
      return;
  }

  if (token.Is(*Op__Add1)) {
      CallState(State_Expr_Unary, State_Expr_Add1_Unary);
      return;
  }

  if (token.Is(*Op__LogNot)) {
      CallState(State_Expr_Unary, State_Expr_LogNot_Unary);
      return;
  }

  if (token.Is(*Op__Not)) {
      CallState(State_Expr_Unary, State_Expr_Not_Unary);
      return;
  }

  if (token.Is(*Op__Sub)) {
      CallState(State_Expr_Unary, State_Expr_Minus_Unary);
      return;
  }

  if (token.Is(*Op__Sub1)) {
      CallState(State_Expr_Unary, State_Expr_Add1_Unary);
      return;
  }

  ContinueState(State_Expr_Primary);
  continue;
