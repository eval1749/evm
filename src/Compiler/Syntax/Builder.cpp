#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Parsre - Builder
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Builder.h"

#include "../Ir.h"

#include "../../Il/Ir/BBlock.h"
#include "../../Il/Ir/Instructions.h"
#include "../../Il/Ir/Operands.h"

namespace Compiler {

using namespace Il::Ir;

// ctor
Builder::Builder(
    Builder* const outer,
    const Function& fun,
    const SourceInfo& source_info)
    : curr_block_(fun.GetStartBB()),
      function_(fun),
      module_(fun.module()),
      num_insts_(0),
      outer_(outer),
      source_info_(source_info),
      succ_block_(fun.GetStartBB()) {
  DEBUG_FORMAT("%p %s", this, fun);
}

Builder::~Builder() {
  DEBUG_FORMAT("%p %s", this, function_);
}

// properties
MemoryZone& Builder::zone() {
  return module_.zone();
}

// [E]
const BoolOutput& Builder::EmitBool(Op opcode, const Operand& lhs, bool rhs) {
  return EmitBool(opcode, lhs, NewLiteral(rhs));
}

const BoolOutput& Builder::EmitBool(
    Op opcode, 
    const Operand& lhs, 
    const Operand& rhs) {
  auto& out = NewBoolOutput();
  auto& inst = Instruction::New(zone(), opcode);
  inst.set_output(out);
  inst.set_output_type(*Ty_Bool);
  inst.AppendOperand(lhs);
  inst.AppendOperand(rhs);
  EmitI(inst);
  return out;
}

void Builder::EmitBranchI(
    const BoolOutput& bool_src,
    const BBlock& true_block,
    const BBlock& false_block) {
  EmitI(*new(zone()) BranchI(bool_src, true_block, false_block));
}

void Builder::EmitCallI(const Operand& callee, const Operand& arg1) {
  ASSERT(!callee.Is<Type>());
  auto& args = EmitValuesI(arg1);
  EmitCallI(callee, args);
}

void Builder::EmitCallI(const Operand& callee, const Values& args) {
  ASSERT(!callee.Is<Type>());
  EmitI(*new(zone()) CallI(*Ty_Void, *Void, callee, args));
}

const Register& Builder::EmitCallI(const Type& outy, const Operand& callee) {
  return EmitCallI(outy, callee, EmitValuesI());
}

const Register& Builder::EmitCallI(const Type& outy, const Operand& callee,
                                   const Operand& arg1) {
  return EmitCallI(outy, callee, EmitValuesI(arg1));
}

const Register& Builder::EmitCallI(const Type& outy, const Operand& callee,
                                   const Operand& arg1, const Operand& arg2) {
  return EmitCallI(outy, callee, EmitValuesI(arg1, arg2));
}

const Register& Builder::EmitCallI(const Type& outy, const Operand& callee,
                                   const Values& args) {
  auto& r2 = NewRegister();
  EmitI(*new(zone()) CallI(outy, r2, callee, args));
  return r2;
}

// See also Parser::CloseFinally.
void Builder::EmitCloses(
  Instruction& ref_inst,
  FrameReg* const pStart,
  FrameReg* const pEnd) {

  for (auto pRunner = pStart;
    pRunner != pEnd;
    pRunner = pRunner->GetOuter()) {

    ref_inst.bblock().InsertBeforeI(*new(zone()) CloseI(pRunner), &ref_inst);

    if (auto const pOpenI = pRunner->GetDefI()->DynamicCast<OpenFinallyI>()) {
      auto& v1 = NewValues();

      ref_inst.bblock().InsertBeforeI(
          *new(zone()) ValuesI(
              v1,
              *pOpenI->GetVy()->GetDefI()->StaticCast<ValuesI>()),
          &ref_inst);

      ref_inst.bblock().InsertBeforeI(
        *new(zone()) CallI(*Ty_Void, *Void, pOpenI->op0(), v1),
        &ref_inst);
    }
  }
}

void Builder::EmitI(Instruction& inst) {
  if (!curr_block_) {
    DEBUG_FORMAT("discard %s", inst);
    return;
  }

  ++num_insts_;
  inst.SetSourceInfo(&source_info_);
  inst.SetIndex(num_insts_);
  curr_block_->AppendI(inst);
  DEBUG_FORMAT("%s", inst);
}

const Register& Builder::EmitI(Op const opcode, const Type& outy,
                               const Operand& src) {
  auto& inst = Instruction::New(zone(), opcode);
  auto& out = *NewOutput(outy).StaticCast<Register>();
  inst.set_output(out);
  inst.set_output_type(outy);
  inst.AppendOperand(src);
  EmitI(inst);
  return out;
}

const Register& Builder::EmitI(Op const opcode, const Type& outy,
                               const Operand& src1, const Operand& src2) {
  auto& inst = Instruction::New(zone(), opcode);
  auto& out = *NewOutput(outy).StaticCast<Register>();
  inst.set_output(out);
  inst.set_output_type(outy);
  inst.AppendOperand(src1);
  inst.AppendOperand(src2);
  EmitI(inst);
  return out;
}

void Builder::EmitJumpI(BBlock& bblock) {
  if (!curr_block_) {
    return;
  }

  DEBUG_FORMAT("jump from %s to %s", *curr_block_, bblock);
  EmitI(*new(zone()) JumpI(&bblock));
}

const Register& Builder::EmitLoadI(const Operand& ptr) {
  auto& outy = ptr.type().StaticCast<PointerType>()->pointee_type();
  auto& out = NewOutput(outy);
  EmitI(*new(zone()) LoadI(outy, out, ptr));
  return *out.StaticCast<Register>();
}

const Register& Builder::EmitNameRefI(const NameRef& name_ref) {
  return EmitNameRefI(PointerType::Intern(*Ty_Void), *Void, name_ref);
}
const Register& Builder::EmitNameRefI(
    const Type& ty,
    const Operand& base,
    const NameRef& name_ref) {
  auto& out = NewRegister();
  EmitI(*new(zone()) NameRefI(PointerType::Intern(ty), out, base, name_ref));
  return out;
}

const Register& Builder::EmitNullI(const Type& outy) {
  auto& out = NewRegister();
  EmitI(*new(zone()) NullI(outy, out));
  return out;
}

PhiI& Builder::EmitPhiI(const Type& outy) {
  auto& out = NewRegister();
  auto& phi_inst = *new PhiI(outy, out);
  EmitI(phi_inst);
  return phi_inst;
}

void Builder::EmitRetI(const Operand& value) {
  EmitI(*new(zone()) RetI(value));
}

void Builder::EmitStoreI(const Operand& ptr, const Operand& src) {
  EmitI(*new(zone()) StoreI(ptr, src));
}

const Values& Builder::EmitValuesI() {
  auto& v1 = module_.NewValues();
  EmitI(*new(zone()) ValuesI(v1));
  return v1;
}

const Values& Builder::EmitValuesI(const Operand& src1) {
  auto& v2 = module_.NewValues();
  EmitI(*new(zone()) ValuesI(v2, src1));
  return v2;
}

const Values& Builder::EmitValuesI(const Operand& src1, const Operand& src2) {
  auto& v2 = module_.NewValues();
  auto& values_inst = *new(zone()) ValuesI(v2);
  values_inst.AppendOperand(src1);
  values_inst.AppendOperand(src2);
  values_inst.UpdateOutputType();
  EmitI(values_inst);
  return v2;
}

Builder* Builder::EndFunction() {
  function_.GetExitBB()->GetLastI()->set_source_info(source_info_);
  auto const outer = outer_;
  delete this;
  return outer;
}

// [G]
BBlock& Builder::GetSucc() const {
  ASSERT(succ_block_ != nullptr);
  return *succ_block_;
}

// [N]
const BoolOutput& Builder::NewBoolOutput() {
  return module_.NewBoolOutput();
}

BBlock& Builder::NewBBlock() {
  auto& bblock = const_cast<BBlock&>(module_.NewBBlock());
  const_cast<Function&>(function_).InsertBefore(
      bblock, 
      *function_.GetExitBB());
  DEBUG_FORMAT("BB%d", bblock.name());
  return bblock;
}

const Literal& Builder::NewLiteral(bool const value) {
  return NewLiteral(*Ty_Boolean, value);
}

const Literal& Builder::NewLiteral(const Type& type, int64 const value) {
  return module_.NewLiteral(type, value);
}

const Output& Builder::NewOutput(const Type& type) {
  return module_.NewOutput(type);
}

const Register& Builder::NewRegister() {
  return module_.NewRegister();
}

BBlock& Builder::NewSucc() {
  return curr_block_ == succ_block_ ? NewBBlock() : *succ_block_;
}

const Values& Builder::NewValues() {
  return module_.NewValues();
}

// [R]
void Builder::ResetCurr() {
  curr_block_ = nullptr;
}

// [S]
void Builder::SetCurr(BBlock& new_curr_block) {
  if (curr_block_ == succ_block_) {
      succ_block_ = &new_curr_block;
  }

  curr_block_ = &new_curr_block;
}

void Builder::SetCurrSucc(BBlock& curr, BBlock& succ) {
  curr_block_ = &curr;
  succ_block_ = &succ;
}

void Builder::SetCurrSucc(BBlock& new_curr_block) {
  curr_block_ = &new_curr_block;
  succ_block_ = &new_curr_block;
}

void Builder::SetSucc(BBlock& new_succ_block) {
  succ_block_ = &new_succ_block;
}

/// <summary>
///   <list type="number">
///     <item>Set output type of PrologueI.</item>
///     <item>Load parameters</item>
///   </list>
/// </summary>
void Builder::SetUpMethodBody(Function& fun, MethodDef& method_def) {
  auto const pPrologueI = fun.GetStartBB()->GetFirstI();
  auto const pRefI = pPrologueI->GetNext();
  auto const pBB = pPrologueI->GetBBlock();
  auto& v1 = *pPrologueI->GetVd();

  ValuesTypeBuilder tybuilder(
      method_def.IsStatic()
          ? method_def.CountParams()
          : method_def.CountParams() + 1);

  auto nth = 0;

  // "this" parameter.
  if (!method_def.IsStatic()) {
    tybuilder.Append(method_def.owner_class_def().GetClass());
    nth++;
  }

  foreach (MethodDef::EnumParam, oEnum, method_def) {
    auto const pParamDef = oEnum.Get();
    auto& var = pParamDef->GetVariable();
    auto& varty = var.GetTy();
    tybuilder.Append(varty);
    DEBUG_FORMAT("%s %s", varty, var);

    auto& cell_class = Ty_ClosedCell->Construct(varty);
    //auto& r2 = *NewOutput(varty).StaticCast<Register>();
    auto& r2 = *NewOutput(varty).StaticCast<Register>();
    r2.SetVariable(var);

    auto const pSelectI = new(zone()) SelectI(varty, r2, v1, nth);
    pSelectI->SetSourceInfo(pParamDef->GetSourceInfo());
    pBB->InsertBeforeI(*pSelectI, pRefI);

    auto& r3 = NewRegister();
    auto& vardef_inst = *new(zone()) VarDefI(cell_class, r3, var, r2);
    vardef_inst.set_source_info(pParamDef->source_info());
    pBB->InsertBeforeI(vardef_inst, pRefI);

    ++nth;
  }

  pPrologueI->set_output_type(tybuilder.GetValuesType());

  fun.SetFunctionType(
      FunctionType::Intern(
          method_def.return_type(),
          *pPrologueI->output_type().StaticCast<ValuesType>()));

  foreach (Function::EnumI, insts, fun) {
    insts.Get()->set_source_info(source_info_);
  }
}

Builder& Builder::StartFunction(
    Builder* const outer,
    const Function& fun) {
  const SourceInfo& source_info = outer->source_info();
  auto const builder = new Builder(outer, fun, source_info);

  foreach (Function::EnumI, insts, fun) {
    insts.Get()->set_source_info(source_info);
  }

  return *builder;
}

Builder& Builder::StartMethodFunction(
    Builder* outer,
    MethodDef& method_def,
    const SourceInfo& source_info) {
  auto& fun = method_def.module().NewFunction(
      nullptr,
      Function::Flavor_Named,
      method_def.name());
  fun.SetUp();
  method_def.set_function(fun);
  auto const builder = new Builder(outer, fun, source_info);
  builder->SetUpMethodBody(fun, method_def);
  return *builder;
}

} // Compiler
