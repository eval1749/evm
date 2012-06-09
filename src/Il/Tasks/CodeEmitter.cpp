#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - CodeEmitter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CodeEmitter.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

CodeEmitter::CodeEmitter(Session& session, const Instruction& ref_inst)
    : bblock_(*ref_inst.GetBBlock()),
      module_(ref_inst.GetBBlock()->GetFunction()->module()),
      ref_inst_(ref_inst),
      session_(session) {}

MemoryZone& CodeEmitter::zone() const { return module_.zone(); }

// [A]
const Register& CodeEmitter::Add(
    const Type& outy,
    const Operand& lhs,
    const Operand& rhs) {
  auto& out = *module_.NewOutput(outy).StaticCast<Register>();
  Emit(*new(zone()) AddI(outy, out, lhs, rhs));
  return out;
}

// [C]
const Register& CodeEmitter::Call(
    const Type& outy,
    const Operand& callee,
    const Operand& arg1) {
  auto& v1 = module_.NewValues();
  Emit(*new(zone()) ValuesI(ValuesType::Intern(arg1.type()), v1, arg1));
  auto& out = NewRegister();
  Emit(*new(zone()) CallI(outy, out, callee, v1));
  return out;
}

const Register& CodeEmitter::Call(
    const Type& outy,
    const Operand& callee,
    const Operand& arg1,
    const Operand& arg2) {
  auto& v1 = module_.NewValues();
  auto& args_inst = *new(zone()) ValuesI();
  args_inst.set_output(v1);
  args_inst.AppendOperand(arg1);
  args_inst.AppendOperand(arg2);
  args_inst.UpdateOutputType();
  Emit(args_inst);
  auto& out = NewRegister();
  Emit(*new(zone()) CallI(outy, out, callee, v1));
  return out;
}

// [E]
void CodeEmitter::Emit(const Instruction& inst) {
  bblock_.InsertBeforeI(inst, ref_inst_);
}

const Register& CodeEmitter::EltRef(const Register& arr, int index) {
  ASSERT(index >= 0);
  auto& out = NewRegister();
  auto& arrty = *arr.type().StaticCast<ArrayType>();
  auto& outy = PointerType::Intern(arrty.element_type());
  auto& index_src =  NewLiteral(*Ty_Int32, index);
  Emit(*new(zone()) EltRefI(outy, out, arr, index_src));
  return out;
}

// [F]
const Register& CodeEmitter::FieldPtr(
    const Operand& base,
    const Field& field) {
  auto& out = NewRegister();
  Emit(*new(zone()) FieldPtrI(out, base, field));
  return out;
}

// [L]
const Output& CodeEmitter::Load(const Operand& src) {
  auto& outy = src.type().StaticCast<PointerType>()->pointee_type();
  auto& out = module_.NewOutput(outy);
  Emit(*new(zone()) LoadI(outy, out, src));
  return out;
}

// [M]
const Register& CodeEmitter::Mul(
    const Type& outy,
    const Operand& lhs,
    const Operand& rhs) {
  auto& out = *module_.NewOutput(outy).StaticCast<Register>();
  Emit(*new(zone()) MulI(outy, out, lhs, rhs));
  return out;
}

// [N]
const BoolOutput& CodeEmitter::NewBoolOutput() {
  return module_.NewBoolOutput();
}

const Register& CodeEmitter::NewArray(
    const Type& elemty,
    int const num_elems) {
  ASSERT(num_elems >= 0);
  auto& args = module_.NewValues();
  auto& args_inst = *new(zone()) ValuesI(ValuesType::Intern(*Ty_Int32), args);
  args_inst.AppendOperand(NewLiteral(*Ty_Int32, num_elems));
  Emit(args_inst);
  auto& arrty = ArrayType::Intern(elemty, 1);
  auto& out = NewRegister();
  Emit(*new(zone()) NewArrayI(arrty, out, args));
  return out;
}

const Literal& CodeEmitter::NewLiteral(bool value) {
  return module_.NewLiteral(value);
}

const Literal& CodeEmitter::NewLiteral(const Type& type, int64 const value) {
  return module_.NewLiteral(type, value);
}

const Output& CodeEmitter::NewOutput(const Type& type) {
  return module_.NewOutput(type);
}

const PseudoOutput& CodeEmitter::NewPseudoRegister() {
  return *new(zone()) PseudoOutput();
}

const Register& CodeEmitter::NewRegister() {
  return module_.NewRegister();
}

const Values& CodeEmitter::NewValues() {
  return module_.NewValues();
}

// [S]
const Register& CodeEmitter::Shl(
    const Type& outy,
    const Operand& src,
    int const amount) {
  auto& out = *NewOutput(outy).StaticCast<Register>();
  Emit(*new(zone()) ShlI(outy, out, src, NewLiteral(*Ty_Int32, amount)));
  return out;
}

void CodeEmitter::Store(const SsaOutput& ptr, const Operand& src) {
  Emit(*new(zone()) StoreI(ptr, src));
}

const Register& CodeEmitter::Sub(
    const Type& outy,
    const Operand& lhs,
    const Operand& rhs) {
  auto& out = *module_.NewOutput(outy).StaticCast<Register>();
  Emit(*new(zone()) SubI(outy, out, lhs, rhs));
  return out;
}


} // Tasks
} // Il
