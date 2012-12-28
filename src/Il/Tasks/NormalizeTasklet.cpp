#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - NormalizeTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NormalizeTasklet.h"

#include "./CodeEmitter.h"
#include "../Ir.h"
#include "../Ir/VmErrorInfo.h"

namespace Il {
namespace Tasks {

namespace {
class Static {
  public: static int CountBit(uint32 x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3F;
  }

  public: static bool IsPowerOf2(uint const x) {
    return !(x & (x - 1));
  }

  public: static int Log2(uint32 x) {
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return CountBit(x) - 1;
  }
};
} // namespace

// ctor
NormalizeTasklet::NormalizeTasklet(Session& session, const Module& module)
    : EditInstructionTasklet(L"NormalizeTasklet", session, module),
      m_oEvaluator(session, module) {}

// [E]
/// <summary>
///     Update operands with evaluated value.
/// </summary>
void NormalizeTasklet::EditInstruction(Instruction* const pI) {
  bool fChanged = false;

  for (auto& box: pI->operand_boxes()) {
    auto& operand = box.operand();
    auto& new_operand = m_oEvaluator.Eval(operand);
    if (operand != new_operand) {
      if (!fChanged) {
        DEBUG_FORMAT("Update %s", pI);
        fChanged = true;
      }

      DEBUG_FORMAT("Replace %s to %s", operand, new_operand);
      box.Replace(new_operand);
      Add(pI);

      if (auto const pRx = operand.DynamicCast<SsaOutput>()) {
        if (auto const pDefI = pRx->GetDefI()) {
          Add(pDefI);
        }
      }
    }
  }

  if (fChanged) {
    if (pI->Is<RetI>()) {
      UpdateReturnType(pI->GetBBlock()->GetFunction());
    }
  }

  if (!pI->IsUseless()) {
    pI->Apply(this);
  }

  if (pI->IsUseless()) {
    DEBUG_FORMAT("Remove useless instruction %s", pI);

    for (auto& operand: pI->operands()) {
      if (auto const pRx = operand.DynamicCast<SsaOutput>()) {
        if (auto const pDefI = pRx->GetDefI()) {
          Add(pDefI);
        }
      }
    }

    pI->GetBBlock()->RemoveI(*pI);
    return;
  }

  if (auto const pRx = pI->GetOutput()->DynamicCast<SsaOutput>()) {
    auto const pSx = &m_oEvaluator.Eval(*pRx);
    if (pRx != pSx) {
      DEBUG_FORMAT("Propagate value of %s of %s", pSx, pI);
      Add(pRx);
    }
  }

  if (auto const tyvar = pI->output_type().DynamicCast<TypeVar>()) {
    if (tyvar->IsBound()) {
      DEBUG_FORMAT("Rewrite bound type varible: %s", pI);
      pI->set_output_type(tyvar->ComputeBoundType());
      Add(pI->output());
    }
  }
}

// [G]
Field* NormalizeTasklet::GetField(Operand* const pSx) {
  if (pSx == nullptr) {
    return nullptr;
  }

  if (auto const pField = pSx->DynamicCast<Field>()) {
    return pField;
  }

  if (auto const pRx = pSx->DynamicCast<SsaOutput>()) {
    if (auto const pPtrI = pRx->GetDefI()) {
      if (pPtrI->Is<FieldPtrI>()) {
        return pPtrI->GetSy()->DynamicCast<Field>();
      }
    }
  }

  return nullptr;
}

// [P][A]
/// <summary>
///     Set output type from operand type.
/// </summary>
void NormalizeTasklet::Process(ArithmeticInstruction* const pI) {
  ASSERT(pI != nullptr);
  UpdateOutputType(*pI, pI->GetSx()->GetTy());
}

// [P][I]
/// <summary>
///     // b1 = !b2 ? Y : Z : b1 = b2 ? Z : Y
///     IF %b1 <= %b2 %false %true
//      IF %r1 <= %b1 Y Z
///     =>
///     IF %r1 <= %b2 Z Y
/// </summary>
void NormalizeTasklet::Process(BranchI* const pI) {
  ASSERT(pI != nullptr);
  auto const pBx = pI->GetBx();
  if (pBx == nullptr) {
    DEBUG_FORMAT("%s: %s must be BoolOutput.", pI, pI->GetSx());
    return;
  }

  auto const pDefI = pBx->GetDefI();
  if (IfI::IsNot(*pDefI)) {
    auto const pSy = pI->GetSy();
    auto const pSz = pI->GetSz();
    pI->GetOperandBox(0)->Replace(*pDefI->GetBx());
    pI->GetOperandBox(1)->SetOperand(pSz);
    pI->GetOperandBox(2)->SetOperand(pSy);
    DEBUG_FORMAT("Fold %s => %s", pDefI, pI);
    SetChanged();
  }
}

// [P][C]
/// <summary>
///     Set output type from callee.
///         Method => Return Type
///         Type => Type == constructor of Array.
/// </summary>
void NormalizeTasklet::Process(CallI* const pI) {
  ASSERT(pI != nullptr);

  if (pI->GetOutput() == Void) {
    return;
  }

  if (auto const pMethod = pI->GetSx()->DynamicCast<Method>()) {
    UpdateOutputType(*pI, pMethod->return_type());

  } else if (auto const pType = pI->GetSx()->DynamicCast<Type>()) {
    UpdateOutputType(*pI, *pType);
  }
}

// [P][E]
/// <summary>
///     Make the first operand to register.
/// </summary>
void NormalizeTasklet::Process(EqI* const pI) {
  ASSERT(pI != nullptr);

  if (!pI->GetSx()->Is<SsaOutput>()) {
      SwapOperands(*pI);
  }
}

// [P][F]
/// <summary>
///     Set output type from operand type.
/// </summary>
void NormalizeTasklet::Process(FieldPtrI* const pI) {
  ASSERT(pI != nullptr);

  auto pField = pI->GetSy()->DynamicCast<Field>();
  if (pField == nullptr) {
    DEBUG_FORMAT("Expect Field: %s", pI);
    return;
  }

  if (!pField->IsStatic()) {
    auto const pClass = pI->GetSx()->GetTy().DynamicCast<Class>();
    if (pClass == nullptr) {
      DEBUG_FORMAT("Expect class: %s", pI);
      return;
    }

    if (&pField->owner_class() != pClass) {
      auto const pThing = pClass->Find(pField->name());

      pField = pThing == nullptr
          ? nullptr
          : pThing->DynamicCast<Field>();

      if (pThing == nullptr) {
        DEBUG_FORMAT("No field %s in %s",
            pField->name().ToString(),
            pClass);
        return;
      }

      DEBUG_FORMAT("update to %s",  pField);
      pI->GetOperandBox(1)->Replace(*pField);
    }
  }

  auto const pPointerType = pI->output_type().DynamicCast<PointerType>();
  if (pPointerType == nullptr) {
    DEBUG_FORMAT("Expect PointerType: %s", pI);
    return;
  }

  auto& pFieldType = pField->storage_type();
  UpdateOutputType(*pI, PointerType::Intern(pFieldType));
}

// [P][I]
/// <summary>
///     // b1 = !b2 ? Y : Z : b1 = b2 ? Z : Y
///     IF %b1 <= %b2 %false %true
//      If %r1 <= %b1 Y Z
///     =>
///     If %r1 <= %b2 Z Y
/// </summary>
void NormalizeTasklet::Process(IfI* const pI) {
  ASSERT(pI != nullptr);
  auto const pDefI = pI->GetBx()->GetDefI();
  if (IfI::IsNot(*pDefI)) {
    auto const pSy = pI->GetSy();
    auto const pSz = pI->GetSz();
    pI->GetOperandBox(0)->Replace(*pDefI->GetBx());
    pI->GetOperandBox(1)->SetOperand(pSz);
    pI->GetOperandBox(2)->SetOperand(pSy);
    DEBUG_FORMAT("Fold %s => %s", pDefI, pI);
    Add(pI->GetOutput());
    SetChanged();
  }
}

// [P][L]
/// <summary>
///     Set output type from operand type.
/// </summary>
void NormalizeTasklet::Process(LoadI* const pI) {
  ASSERT(pI != nullptr);

  auto const pPointerType = pI->GetSx()->GetTy().DynamicCast<PointerType>();
  if (pPointerType == nullptr) {
   DEBUG_FORMAT("Expect PointerType: %s", pI);
   return;
  }

  UpdateOutputType(*pI, pPointerType->pointee_type());
}

// [P][M]
// Mul Int32 %out <= %src 2 => Add Int32 %out <= %src %src
// Mul Int32 %out <= %src 2^n => Shl Int32 %out <= %src n
void NormalizeTasklet::Process(MulI* const pI) {
  auto& mul_inst = *pI;
  if (!mul_inst.GetRd()) {
    return;
  }

  if (mul_inst.op0().Is<Literal>()) {
    SwapOperands(mul_inst);
  }

  if (!mul_inst.op0().Is<Register>()) {
    return;
  }

  auto& outy = mul_inst.output_type();
  auto& out = *mul_inst.output().StaticCast<Register>();
  auto& lhs = *mul_inst.op0().StaticCast<Register>();
  if (outy.IsInt()) {
    if (auto const imm = mul_inst.op1().DynamicCast<Literal>()) {
      if (imm->CanBeInt32()) {
        auto const i32 = imm->GetInt32();
        if (i32 > 0 && Static::IsPowerOf2(i32)) {
          CodeEmitter emitter(session(), mul_inst);
          auto& r1 = i32 == 2
              ? emitter.Add(outy, lhs, lhs)
              : emitter.Shl(outy, lhs, Static::Log2(i32));
          ReplaceAll(r1, out);
          return;
        }

        if (i32 < 0 && Static::IsPowerOf2(-i32)) {
          CodeEmitter emitter(session(), mul_inst);
          auto& r1 = i32 == -2
              ? emitter.Add(outy, lhs, lhs)
              : emitter.Shl(outy, lhs, Static::Log2(-i32));
          auto& zero = emitter.NewLiteral(outy, 0);
          auto& r2 = emitter.Sub(outy, zero, r1);
          ReplaceAll(r2, out);
          return;
        }
      }
    }
  }
}

// [P][N]
/// <summary>
///     Make the first operand to register.
/// </summary>
void NormalizeTasklet::Process(NeI* const pI) {
  ASSERT(pI != nullptr);

  if (!pI->GetSx()->Is<SsaOutput>()) {
    SwapOperands(*pI);
  }
}

// [P]
/// <summary>
///     Set output type from operand types.
/// </summary>
void NormalizeTasklet::Process(PhiI* const pI) {
  ASSERT(pI != nullptr);

  const Type* pType = Ty_Void;
  for (auto& operand: pI->operands()) {
    auto& or_type = Type::Or(*pType, operand.GetTy());
    DEBUG_FORMAT("or %s %s => %s", pType, operand.GetTy(), &or_type);
    pType = &or_type;
  }

  if (pI->output_type() != *pType) {
    DEBUG_FORMAT("Change %s to %s", pI->output_type(), pType);
    pI->set_output_type(*pType);
    Add(pI->GetOutput());
    SetChanged();
  }
}

/// <summary>
///     Set output type from operand type.
/// </summary>
void NormalizeTasklet::Process(SelectI* const pI) {
  ASSERT(pI != nullptr);
  auto const pValuesType = pI->GetSx()->GetTy().StaticCast<ValuesType>();
  auto const iIndex = pI->GetIy();
  UpdateOutputType(*pI, pValuesType->Get(iIndex));
}

void NormalizeTasklet::Process(StaticCastI* const pI) {
  ASSERT(pI != nullptr);
  auto& inst = *pI;
  switch (inst.op0().type().IsSubtypeOf(inst.output_type())) {
    case Subtype_No:
      session().AddErrorInfo(
          VmErrorInfo(
              inst.source_info(),
              VmError_Type_NotMatched,
              inst,
              inst.op0().type()));
      break;

    case Subtype_Yes:
    case Subtype_Unknown:
      return;

    default:
      CAN_NOT_HAPPEN();
  }
}


/// <summary>
///   Populate const field.
///   <code>
///     FieldPtr %r1 <= %r2 (Field C F)
///     Store %r1 %r3
///   </code>
/// </summary>
void NormalizeTasklet::Process(StoreI* const pI) {
  ASSERT(pI != nullptr);
  auto& inst = *pI;

  if (auto const field = GetField(inst.GetSx())) {
    if (field->IsConst()) {
      DEBUG_FORMAT("set enum field: %s", inst);
      auto& op = m_oEvaluator.Resolve(inst.op1());
      if (field->GetOperand() != &op) {
        DEBUG_FORMAT("Set const field %s to %s", field, op);
        field->SetOperand(&op);
      }
    }
  }
}

/// <summary>
///     Update types
/// </summary>
void NormalizeTasklet::Process(ValuesI* const pI) {
  ASSERT(pI != nullptr);

  auto fChanged = false;

  ValuesTypeBuilder builder(*pI->output_type().StaticCast<ValuesType>());

  auto iterator = pI->operands().begin();

  foreach (ValuesTypeBuilder::Enum, oEnum, builder) {
    if (iterator == pI->operands().end())
      break;

    auto const pExpected = &iterator->GetTy().ComputeBoundType();
    auto const pPresent = oEnum.Get();
    if (pPresent != pExpected) {
      DEBUG_FORMAT("Update operand %s to %s", pPresent, pExpected);
      oEnum.Set(*pExpected);
      fChanged = true;
    }

    ++iterator;
  }

  if (fChanged) {
    pI->set_output_type(builder.GetValuesType());
    Add(pI->GetOutput());
    SetChanged();
  }
}

/// <summary>
///     Set output type from operand type.
///     VarDef ClosedCell<T> %r1 <= (Variable name) %r2
/// </summary>
void NormalizeTasklet::Process(VarDefI* const pI) {
  ASSERT(pI != nullptr);

  auto const pClass = pI->output_type().DynamicCast<ConstructedClass>();
  auto const pVar = pI->GetSx()->DynamicCast<Variable>();
  auto& expected_ty = pI->GetSy()->GetTy();
  auto& type_arg = pClass->GetTypeArg();

  if (expected_ty != type_arg) {
    pVar->SetTy(expected_ty);
    auto& cell_class = Ty_ClosedCell->Construct(expected_ty);
    UpdateOutputType(*pI, cell_class);

    // Reduce number of Class::Find calls.
    auto const pField = cell_class.Find(*Q_value)->StaticCast<Field>();
    auto& ptrty = PointerType::Intern(expected_ty);

    foreach (Register::EnumUser, oEnum, pI->GetRd()) {
      auto const pUserI = oEnum.GetI();
      if (auto const pFieldPtrI = pUserI->DynamicCast<FieldPtrI>()) {
        pFieldPtrI->GetOperandBox(1)->Replace(*pField);
        pFieldPtrI->set_output_type(ptrty);
        Add(pFieldPtrI->GetRd());
      }
    }
  }
}

// [U]
void NormalizeTasklet::UpdateOutputType(
    Instruction& inst,
    const Type& newty) {
  if (inst.output_type() != newty) {
   DEBUG_FORMAT("Update output type of %s to %s", inst, newty);
   inst.set_output_type(newty);
   Add(inst.output());
   SetChanged();
  }
}

void NormalizeTasklet::UpdateReturnType(Function* const pFunction) {
  auto& funty = pFunction->function_type();
  auto& rety = funty.return_type();

  if (rety == *Ty_Void) {
    return;
  }

  const Type* pType = Ty_Void;
  foreach (BBlock::EnumPred, oEnum, pFunction->GetExitBB()) {
    auto const pBBlock = oEnum.Get();
    if (auto const pRetI = pBBlock->GetLastI()->DynamicCast<RetI>()) {
      pType = &Type::Or(*pType, pRetI->GetSx()->GetTy());
    }
  }

  if (rety != *pType) {
    DEBUG_FORMAT("Update return type %s to %s", rety, pType);

    pFunction->SetFunctionType(
        FunctionType::Intern(*pType, funty.params_type()));
  }
}

} // Tasks
} // Il
