#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Instruction.h"

#include "../Instructions.h"
#include "../Operands.h"

namespace Il {
namespace Ir {

// ctor
Instruction::Instruction(
    Op const eOp,
    const Type* const outy,
    const Output* const out)
    : opcode_(eOp),
      output_(const_cast<Output*>(out)),
      output_type_(outy) {
  ASSERT(out != nullptr);
  ASSERT(outy != nullptr);
}

BBlock& Instruction::bblock() const {
  ASSERT(m_pParent != nullptr);
  return *const_cast<Instruction*>(this)->m_pParent;
}

String Instruction::mnemonic() const {
  return GetMnemonic(opcode());
}

void Instruction::set_output(const Output& out) {
  output_ = &const_cast<Output&>(out);
  if (IsRealized()) {
      output_->SetDefI(this);
  }
}

Variable& Instruction::variable() const {
  return *op0().StaticCast<Variable>();
}

// [A]
void Instruction::AppendOperand(const Operand& operand) {
  AppendOperand(new OperandBox(&operand));
}

void Instruction::AppendOperand(OperandBox* const pBox) {
  static_cast<OperandBoxes*>(this)->Append(pBox);
  if (IsRealized()) {
      pBox->GetOperand()->Realize(pBox);
  }
}

// [C]
int Instruction::CountOperands() const {
  auto cOperands = 0;
  foreach (EnumOperand, oEnum, this) {
      cOperands += 1;
  }
  return cOperands;
}

// [E]
// TODO(yosi) 2012-02-06 Who use Instruction::Equals?
bool Instruction::Equals(const Instruction* const that) const {
  ASSERT(that != nullptr);
  if (GetOp() != that->GetOp()) {
      return false;
  }

  if (output() == that->output()) {
      return false;
  }

  EnumOperand oEnumThat(that);
  foreach (EnumOperand, oEnumThis, this) {
      if (oEnumThat.AtEnd()) {
          return false;
      }

      if (*oEnumThis.Get() != *oEnumThat.Get()) {
          return false;
      }

      oEnumThat.Next();
  }

  return oEnumThat.AtEnd();
}

// [G]
Instruction::Arity Instruction::GetArity(Op const opcode) {
  return GetDescription(opcode).arity_;
}

Instruction::Arity Instruction::GetArity() const {
  return GetArity(GetOp());
}

BoolOutput* Instruction::GetBd() const
  { return GetOutput()->DynamicCast<BoolOutput>(); }

BoolOutput* Instruction::GetBx() const
  { return GetSx()->DynamicCast<BoolOutput>(); }

// Note: Instruction operand must have Int32. We don't want to handle
// other IntX types.
int Instruction::GetIy() const {
  auto const pLy = GetSy()->StaticCast<Literal>();
  if (pLy->CanBeInt32()) {
      return pLy->GetInt32();
  }

  CAN_NOT_HAPPEN();
}

OperandBox* Instruction::GetLastOperandBox() const {
  return static_cast<const OperandBoxes*>(this)->GetLast();
}

Literal* Instruction::GetLx() const
  { return GetSx()->StaticCast<Literal>(); }

Literal* Instruction::GetLy() const
  { return GetSy()->StaticCast<Literal>(); }

Literal* Instruction::GetLz() const
  { return GetSz()->StaticCast<Literal>(); }

const char* LibExport Instruction::GetMnemonic(Op const opcode) {
  return GetDescription(opcode).mnemonic_;
}

const char* Instruction::GetMnemonic() const {
  return GetMnemonic(GetOp());
}

Operand* Instruction::GetOperand(int const iNth) const {
  ASSERT(iNth >= 0);
  auto const box = GetOperandBox(iNth);
  ASSERT(box != nullptr);
  return box->GetOperand();
}

OperandBox* Instruction::GetOperandBox(int const iNthIn) const {
  auto iNth = iNthIn;
  ASSERT(iNth >= 0);
  foreach (EnumOperand, oEnum, this) {
      if (iNth == 0) {
          return oEnum.GetBox();
      }

      iNth -= 1;
  }

  return nullptr;
}

Output* Instruction::GetOutput() const
  { ASSERT(output_ != nullptr); return output_; }

PseudoOutput* Instruction::GetQd() const
  { return GetOutput()->DynamicCast<PseudoOutput>(); }

PseudoOutput* Instruction::GetQx() const
  { return GetSx()->DynamicCast<PseudoOutput>(); }

PseudoOutput* Instruction::GetQy() const
  { return GetSy()->DynamicCast<PseudoOutput>(); }

PseudoOutput* Instruction::GetQz() const
  { return GetSz()->DynamicCast<PseudoOutput>(); }

Register* Instruction::GetRd() const
  { return GetOutput()->DynamicCast<Register>(); }

Register* Instruction::GetRx() const
  { return GetSx()->DynamicCast<Register>(); }

Register* Instruction::GetRy() const
  { return GetSy()->DynamicCast<Register>(); }

Register* Instruction::GetRz() const
  { return GetSz()->DynamicCast<Register>(); }

SsaOutput* Instruction::GetSsaOutput() const
  { return GetOutput()->DynamicCast<SsaOutput>(); }

Values* Instruction::GetVd() const
  { return GetOutput()->DynamicCast<Values>(); }

Values* Instruction::GetVx() const
  { return GetSx()->DynamicCast<Values>(); }

Values* Instruction::GetVy() const
  { return GetSy()->DynamicCast<Values>(); }


// [I]
// For inserting "this" on method call.
const Operand* Instruction::InsertOperandBefore(
  const Operand* const pSx,
  OperandBox* const pRef) {
  ASSERT(pSx != nullptr);
  InsertOperandBefore(new OperandBox(pSx), pRef);
  return pSx;
}

OperandBox* Instruction::InsertOperandBefore(
  OperandBox* const pBox,
  OperandBox* const pRef) {

  ASSERT(pBox != nullptr);

  if (pRef == nullptr) {
      AppendOperand(pBox);
      return pBox;
  }

  static_cast<OperandBoxes*>(this)->InsertBefore(pBox, pRef);
  if (IsRealized()) {
      pBox->GetOperand()->Realize(pBox);
  }

  return pBox;
}

bool Instruction::IsRealized() const
  { return nullptr != GetBBlock(); }

bool Instruction::IsUseless() const {
  if (output_ == Useless) {
    return true;
  }

  if (output_ == Void) {
      return false;
  }

  if (auto pRd = GetSsaOutput()) {
    return pRd->IsEmpty();
  }

  // For non-SSA output, We don't know whether this instruction is
  // useless or not.
  return false;
}

// [N]
Instruction& Instruction::New(MemoryZone&, Op const opcode) {
  return *GetDescription(opcode).ctor_();
}

// [R]
void Instruction::Realize() {
  output_->SetDefI(this);

  foreach (EnumOperand, oEnum, this) {
      auto const pBox = oEnum.GetBox();
      pBox->GetOperand()->Realize(pBox);
  }
}

void Instruction::RemoveOperand(OperandBox* const pBox) {
  ASSERT(pBox != nullptr);
  pBox->GetOperand()->Unrealize(pBox);
  static_cast<OperandBoxes*>(this)->Delete(pBox);
}

// [T]
String Instruction::ToString() const {
  StringBuilder builder;

  if (IsRealized()) {
    builder.AppendFormat("%s:", bblock());
  }

  builder.AppendFormat("%s(", mnemonic());

  if (output_type() != *Ty_Void) {
    builder.AppendFormat("%s %s = ", output_type(), output());
  }

  const char* comma = "";
  foreach (EnumOperand, operands, *this) {
    builder.Append(comma);
    comma = " ";
    builder.Append(operands.Get()->ToString());
  }
  builder.Append(')');
  return builder.ToString();
}

// [U]
void Instruction::Unrealize() {
  foreach (EnumOperand, oEnum, this) {
    auto pBox = oEnum.GetBox();
    pBox->GetOperand()->Unrealize(pBox);
  }

  if (output_->GetDefI() == this) {
    output_->SetDefI(nullptr);
  }
}

} // Ir
} // Il
