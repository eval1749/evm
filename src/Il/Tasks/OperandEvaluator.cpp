#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - OperandEvaluator
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./OperandEvaluator.h"

#include "../Ir/Instructions.h"
#include "../Ir/Module.h"
#include "../Ir/Operands.h"

namespace Il {
namespace Tasks {

namespace {
class LiteralValue {
  public: enum TypeCode {
      TypeCode_None,
      TypeCode_Int32,
      TypeCode_UInt32,
  }; // TypeCode

  private: TypeCode m_eTypeCode;
  private: int32 m_int32;
  private: uint32 m_uint32;

  public: LiteralValue(const Operand* const pSx) :
          m_eTypeCode(TypeCode_None),
          m_int32(0),
          m_uint32(0) {
      ASSERT(pSx != nullptr);

      if (auto const pLx = pSx->DynamicCast<Literal>()) {
          if (pLx->CanBeInt32()) {
              m_eTypeCode = TypeCode_Int32;
              m_int32 = pLx->GetInt32();
              return;
          }
      }
  }

  // [G]
  public: int32 GetInt32() const {
      ASSERT(GetTypeCode() == TypeCode_Int32);
      return m_int32;
  }

  public: TypeCode GetTypeCode() const { return m_eTypeCode; }

  public: uint32 GetUInt32() const {
      ASSERT(GetTypeCode() == TypeCode_UInt32);
      return m_uint32;
  }

  // [I]
  public: bool IsOne() const {
      switch (GetTypeCode()) {
      case TypeCode_Int32:
          return GetInt32() == 1;

      case TypeCode_UInt32:
          return GetUInt32() == 1;

      default:
          return false;
      }
  }

  public: bool IsZero() const {
      switch (GetTypeCode()) {
      case TypeCode_Int32:
          return GetInt32() == 0;

      case TypeCode_UInt32:
          return GetUInt32() == 0;
      default:
          return false;
      }
  }
}; // LiteralValue
}

// ctor
OperandEvaluator::OperandEvaluator(Session& session, const Module& module)
  : Tasklet(L"OperandEvaluator", session),
    module_(module),
    value_(nullptr) {}

// [E]
const Operand& OperandEvaluator::Eval(const Operand& operand) {
  if (auto const pRx = operand.DynamicCast<SsaOutput>()) {
    if (auto const pDefI = pRx->GetDefI()) {
      value_ = nullptr;
      pDefI->Apply(this);
      return value_ ? *value_ : operand;
    }
  }
  return operand;
}

// [H]
// TODO 2010-01-07 yosi@msn.com Cache HasNonLocalExitPoint result.
bool OperandEvaluator::HasNonLocalExitPoint(const Function& fun) {
  return fun.HasNonLocalExitPoint();
}

// [I]
// TODO 2010-01-07 yosi@msn.com Cache IsSsaVariable result.
bool OperandEvaluator::IsSsaVariable(const Variable& var) {
  class Local {
      public: static Instruction* HasStore(const SsaOutput* const pRd) {
          foreach (Register::EnumUser, oEnum, pRd) {
              auto const pFieldPtrI =
                  oEnum.GetI()->DynamicCast<FieldPtrI>();

              if (pFieldPtrI == nullptr) {
                  continue;
              }

              foreach (Register::EnumUser, oEnum, pFieldPtrI->GetRd()) {
                  if (oEnum.GetI()->Is<StoreI>()) {
                      return oEnum.GetI();
                  }
              }
          }
          return nullptr;
      }
  }; // Local

  auto const pRd = var.GetRd();
  if (pRd == nullptr) {
      // There are no associated register.
      return false;
  }

  if (auto const pStoreI = Local::HasStore(pRd)) {
      DEBUG_FORMAT("%s is modified by %s", var, pStoreI);
      return false;
  }

  auto& module = var.GetOwner()->module();
  for (auto& fun: module.functions()) {
      if (var.GetOwner() != &fun) {
          if (auto const pQd = fun.FindUpVar(&var)) {
              if (Local::HasStore(pQd)) {
                  DEBUG_FORMAT("%s is modified in %s", var, fun);
                  return false;
              }
          }
      }
  }

  return true;
}

// [P][A]
void OperandEvaluator::Process(AddI* const pI) {
  ASSERT(pI != nullptr);

  if (pI->GetSx()->GetTy() != pI->output_type()) {
      return;
  }

  if (pI->GetSy()->GetTy() != pI->output_type()) {
      return;
  }

  LiteralValue oX(&Resolve(pI->op0()));
  LiteralValue oY(&Resolve(pI->op1()));

  if (oX.IsZero()) {
      SetValue(pI->op1());
      return;
  }

  if (oY.IsZero()) {
      SetValue(pI->op0());
      return;
  }

  if (oX.GetTypeCode() != oY.GetTypeCode()) {
      return;
  }

  switch (oX.GetTypeCode()) {
    case LiteralValue::TypeCode_Int32: {
      // TODO(yosi) 2012-03-15 We should check integer overflow.
      auto const i64 = static_cast<int64>(oX.GetInt32()) + oY.GetInt32();
      SetValue(module_.NewLiteral(pI->GetSx()->GetTy(), i64));
      break;
    }

    default:
      DEBUG_FORMAT("%s: %s:%s is unspported numeric type.",
        pI,
        pI->GetSx(),
        pI->GetSx()->GetTy());
      return;
  }
}

// [P][I]
// Value of CastInstruction is one of
//  o Right hand side value
//  o Integral value of constant field.
void OperandEvaluator::Process(CastInstruction* const pI) {
  ASSERT(pI != nullptr);
  auto& inst = *pI;

  if (inst.output_type() == *Ty_Bool) {
    return;
  }

  auto& op = inst.op0();
  auto& outy = inst.output_type();

  if (auto const field = op.DynamicCast<Field>()) {
    if (field->IsConst()
          && field->GetOperand() != Void
          && outy.IsInt()
          && field->storage_type().IsSubtypeOf(outy) == Subtype_Yes) {
      return SetValue(*field->GetOperand());
    }
    DEBUG_FORMAT("Unresolved constant field %s", field);
    return;
  }

  auto& opty = op.type();
  if (opty.IsSubtypeOf(outy) == Subtype_Yes) {
    SetValue(op);
    return;
  }
}

// [P][E]
//  1 EQ Bool %b1 <= %true %b3  -> %b3
//  2 EQ Bool %b1 <= %b2 %true  -> %b2
//  3 EQ Bool %b1 <= %b2 %b2    -> %true
//  4 IF ty %r1 <= %b2 %r3 %r4
//    EQ Bool %b5 <= %r6 %r3    -> %b2
//
void OperandEvaluator::Process(EqI* const pI) {
  auto& inst = *pI;
  auto const r0 = inst.GetBd();
  if (!r0) {
    return;
  }

  if (inst.op0() == *True && inst.op1().Is<BoolOutput>()) {
    DEBUG_FORMAT("%s => %s: compare to true", inst, inst.op1());
    SetValue(inst.op1());
    return;
  }

  if (inst.op1() == *True && inst.op0().Is<BoolOutput>()) {
    DEBUG_FORMAT("%s => %s: compare to true", inst, inst.op0());
    SetValue(inst.op0());
    return;
  }

  if (inst.op0() == inst.op1()) {
    DEBUG_FORMAT("%s => %s: both operands are same.", inst, inst.op0())
    SetValue(*True);
    return;
  }

  if (auto const r1 = inst.GetRx()) {
    if (auto const if_inst = r1->GetDefI()->DynamicCast<IfI>()) {
      if (inst.op1() == if_inst->op1()) {
        auto& value = if_inst->op0();
        DEBUG_FORMAT("%s => %s: taken from %s", inst, value, if_inst);
        SetValue(value);
        return;
      }
    }
  }
}

// [P][I]
/// <summary>
///     XX %b1 <= ...
///     If Bool %r2 <= %b1 %true %false
///     => %b1
///
//      If Bool %b1 <= %b2 %false %true
///     If Bool %b3 <= %b1 %false %true
///     =>
///     %b2
/// </summary>
void OperandEvaluator::Process(IfI* const pI) {
  ASSERT(pI != nullptr);

  if (*pI->GetSy() == *pI->GetSz()) {
    SetValue(pI->op1());
    return;
  }

  if (pI->GetSy() == True && pI->GetSz() == False) {
    SetValue(pI->op0());
    return;
  }

  if (IfI::IsNot(*pI)) {
    if (auto const pI2 = pI->GetBx()->GetDefI()->DynamicCast<IfI>()) {
      if (IfI::IsNot(*pI2)) {
        SetValue(pI2->op0());
      }
    }
  }
}

// [P][L]
/// <summary>
///   Fold SSA variable.
/// </summary>
//      VarDef %r1 <= (Variable V) %r2
//      FieldPtr %r2 <= %r1 (Field C F)
//      Load %r3 <= %r2
void OperandEvaluator::Process(LoadI* const pI) {
  ASSERT(pI != nullptr);

  auto const pRx = pI->GetRx();
  if (pRx == nullptr) {
      return;
  }

  auto const pFieldPtrI = pRx->GetDefI()->DynamicCast<FieldPtrI>();
  if (pFieldPtrI == nullptr) {
      return;
  }

  if (auto const pField = pFieldPtrI->GetSy()->StaticCast<Field>()) {
    if (pField->IsConst()) {
      if (pField->owner_class().GetBaseClass() == Ty_Enum) {
        // Returns Field object instead of integral value. If you need
        // integral value, use Resolve method.
        SetValue(*pField);
        return;
      }

      auto const pSx = pField->GetOperand();
      if (pSx != Void) {
        SetValue(*pSx);
        return;
      }
    }
  }

  auto const pRobj = pFieldPtrI->GetRx();
  if (pRobj == nullptr) {
      return;
  }

  auto const pPtrI = pRobj->GetDefI();
  if (pPtrI == nullptr) {
      return;
  }

  if (auto const pVarDefI = pPtrI->DynamicCast<VarDefI>()) {
      auto const pVar = pVarDefI->GetSx()->DynamicCast<Variable>();
      if (IsSsaVariable(*pVar)) {
          auto const pSy = pVarDefI->GetSy();
          if (pSy->Is<Literal>()) {
              SetValue(*pSy);

          } else if (!HasNonLocalExitPoint(*pVar->GetOwner())) {
              SetValue(*pSy);
          }
      }
  }
}

// [P][M]
/// <summary>
///     <list type="bullet">
///         <item>x * 0 = 0</item>
///         <item>0 * y = 0</item>
///         <item>x * 1 = x</item>
///         <item>1 * y = y</item>
///     </list>
/// </summary>
void OperandEvaluator::Process(MulI* const pI) {
  ASSERT(pI != nullptr);

  if (pI->GetSx()->GetTy() != pI->output_type()) {
      return;
  }

  if (pI->GetSy()->GetTy() != pI->output_type()) {
      return;
  }

  LiteralValue oX(pI->GetSx());
  LiteralValue oY(pI->GetSy());

  if (oX.GetTypeCode() != oY.GetTypeCode()) {
      return;
  }

  if (oX.IsZero()) {
      SetValue(module_.NewLiteral(pI->output_type(), 0));
      return;
  }

  if (oY.IsZero()) {
      SetValue(module_.NewLiteral(pI->output_type(), 0));
      return;
  }

  if (oX.IsOne()) {
      SetValue(pI->op1());
      return;
  }

  if (oY.IsOne()) {
      SetValue(pI->op0());
      return;
  }

  switch (oX.GetTypeCode()) {
    case LiteralValue::TypeCode_Int32:
      // TODO(yosi) 2012-03-12 Check integer overflow of constant MulI
      // TODO(yosi) 2021-03-12 Allocate Literal in Module.
      SetValue(*new Int32Literal(oX.GetInt32() * oY.GetInt32()));
      break;

    default:
      break;
  }
}

// [P][E]
//  NE Bool %b1 <= %b2 %false
//  => %b2
void OperandEvaluator::Process(NeI* const pI) {
  ASSERT(pI != nullptr);

  if (auto const pB1 = pI->GetBd()) {
      if (pI->GetSx() == False) {
          SetValue(pI->op1());
          return;
      }

      if (pI->GetSy() == False) {
          SetValue(pI->op0());
          return;
      }
  }

  if (pI->GetSx() == pI->GetSy()) {
      SetValue(*False);
      return;
  }
}

// [P][P]
void OperandEvaluator::Process(PhiI* const pI) {
  ASSERT(pI != nullptr);
  auto const pSx = pI->GetSx();
  foreach (PhiI::EnumOperand, oEnum, pI) {
      if (*pSx != *oEnum.Get()) {
          return;
      }
  }

  SetValue(*pSx);
}

// [P][S]
/// <summary>
///     Values %v1 <= %s1 %s2 %s3
///     Select %r4 <= %v1 1
///     => %s2
/// </summary>
void OperandEvaluator::Process(SelectI* const pI) {
  ASSERT(pI != nullptr);
  if (auto const pValuesI =
          pI->GetVx()->GetDefI()->DynamicCast<ValuesI>()) {
      auto const iNth = pI->GetIy();
      SetValue(*pValuesI->GetOperand(iNth));
  }
}

// [R]
const Operand& OperandEvaluator::Resolve(const Operand& operand) {
  if (auto const field = operand.DynamicCast<Field>()) {
    return field->IsConst()
        ? *field->GetOperand()
        : operand;
  }
  return operand;
}

// [S]
void OperandEvaluator::SetValue(const Operand& operand) {
  ASSERT(value_ == nullptr);
  value_ = &operand;
}

} // Tasks
} // Il
