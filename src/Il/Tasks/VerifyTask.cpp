#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - VerifyTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VerifyTask.h"

#include "../Ir.h"
#include "../Ir/VmErrorInfo.h"

namespace Il {
namespace Tasks {

using Il::Ir::VmErrorInfo;

namespace {
class Static {
  public: static bool IsBoxType(const Type& type) {
    auto const box_class = type.DynamicCast<ConstructedClass>();
    return box_class&& box_class->generic_class() == *Ty_Box;
  }
};
} // namespace

struct VerifyTask::Entry {
  const Instruction& inst_;
  const String format_;
  const Collection_<Box> params_;

  Entry(
    const Instruction& inst,
    String format,
    const Collection_<Box>& params)
    : inst_(inst), format_(format), params_(params) {}

  DISALLOW_COPY_AND_ASSIGN(Entry);
};

// ctor
VerifyTask::VerifyTask(Session& session, Module& module)
    : Base(L"VerifyTask", session, module) {}

VerifyTask::~VerifyTask() {
  foreach (EntryList::Enum, entries, fail_list_) {
    delete entries.Get();
  }
}

// [A]
void VerifyTask::Add(const Instruction& inst, String msg) {
  fail_list_.Add(new Entry(inst, msg, CollectionV_<Box>()));
}

void VerifyTask::Add(const Instruction& inst, String msg, Box a) {
  fail_list_.Add(new Entry(inst, msg, CollectionV_<Box>(a)));
}

void VerifyTask::Add(const Instruction& inst, String msg, Box a, Box b) {
  fail_list_.Add(new Entry(inst, msg, CollectionV_<Box>(a, b)));
}

void VerifyTask::Add(const Instruction& inst, String msg, Box a, Box b,
                     Box c) {
  fail_list_.Add(new Entry(inst, msg, CollectionV_<Box>(a, b, c)));
}

void VerifyTask::Add(const Instruction& inst, String msg, Box a, Box b,
                     Box c, Box d) {
  fail_list_.Add(new Entry(inst, msg, CollectionV_<Box>(a, b, c, d)));
}

// [P][A]
void VerifyTask::Process(ArithmeticInstruction* const pI) {
  Process(static_cast<ArithmeticInstruction::Base*>(pI));
  auto& outy = pI->output_type();

  for (const auto& operand: pI->operands()) {
      auto& opty = operand.GetTy();

      if (outy != opty) {
          Add(*pI, "Operand[%s] type %s must be %s",
              operand,
              opty,
              outy);
      }
  }
}

// [P][B]
void VerifyTask::Process(BBlock* const pBBlock) {
  ASSERT(pBBlock != nullptr);

  auto const pFunction = pBBlock->GetFunction();

  DEBUG_FORMAT("Process %s %s", name(), pBBlock);

  if (pBBlock->GetFirstI() == nullptr) {
      AddErrorInfo(
          VmErrorInfo(
            pFunction->GetEntryBB()->GetFirstI()->source_info(), 
            VmError_BBlock_Empty, 
            *pFunction,
            *pBBlock));
      return;
  }

  if (pBBlock->CountInEdges() == 0) {
      if (pFunction->GetEntryBB() != pBBlock) {
          Add(*pBBlock->GetFirstI(), "Unreachable");
      }
  }

  if (pBBlock->CountOutEdges() == 0) {
      if (pFunction->GetExitBB() != pBBlock) {
          Add(*pBBlock->GetLastI(), "No out edge");
      }
  }

  for (auto& inst: pBBlock->instructions()) {
    if (Verify(inst))
      inst.Apply(this);
  }
}

void VerifyTask::Process(BoxI* const pI) {
  Process(static_cast<BoxI::Base*>(pI));

  if (!Static::IsBoxType(pI->output_type())) {
      Add(*pI, "%s must be Box<T>.", pI->output_type());
      return;
  }
}

// [P][C]
void VerifyTask::Process(CallI* const pI) {
  Process(static_cast<CallI::Base*>(pI));

  auto const pVy = pI->GetVy();
  if (pVy == nullptr) {
      Add(*pI, "%s must be values.", pI->GetSy());
      return;
  }
}

// [P][E]
void VerifyTask::Process(EltRefI* const pI) {
  Process(static_cast<EltRefI::Base*>(pI));

  auto const pArrayType = pI->GetSx()->GetTy().DynamicCast<ArrayType>();
  if (pArrayType == nullptr) {
      Add(*pI, "%s must be Array<T>", pI->GetSx()->GetTy());
      return;
  }

  auto const pPointerType = pI->output_type().DynamicCast<PointerType>();
  if (pPointerType == nullptr) {
      Add(*pI, "%s must be Pointer<T>", pI->output_type());
      return;
  }

  if (&pArrayType->element_type() != &pPointerType->pointee_type()) {
      Add(*pI, "%s must be %s",
          &pPointerType->pointee_type(),
          &pArrayType->element_type());
  }
}

void VerifyTask::Process(EntryI* const pI) {
  Process(static_cast<EntryI::Base*>(pI));

  if (pI->GetPrev() != nullptr) {
      Add(*pI, "%s must be the first instruction.", pI);
      return;
  }
}

// [P][F]
void VerifyTask::Process(FieldPtrI* const pI) {
  Process(static_cast<FieldPtrI::Base*>(pI));

  auto const pPointerTy = pI->output_type().DynamicCast<PointerType>();

  if (pPointerTy == nullptr) {
      Add(*pI, "Output type %s must be pointer type.",
          pI->output_type());
      return;
  }

  auto const pField = pI->GetSy()->DynamicCast<Field>();
  if (pField == nullptr) {
      Add(*pI, "%s must be a Field.",
          pI->GetSy());
      return;
  }

  {
      auto& expected = PointerType::Intern(pField->storage_type());
      if (pPointerTy != &expected) {
          Add(*pI, "Output type %s must be %s",
              pPointerTy,
              &expected);
      }
  }

  if (pField->IsStatic()) {
      if (pI->GetSx() != Void) {
          Add(*pI, "Operand must be void");
      }

  } else {
      auto const pSx = pI->GetSx();
      if (pField->owner_class() != pSx->GetTy()) {
          Add(*pI, "Type of %s(%s) must be %s.",
              pSx,
              pSx->GetTy(),
              &pField->owner_class());
      }
  }

}

// [P][I]
void VerifyTask::Process(IfI* const pI) {
  ASSERT(pI != nullptr);
  if (pI->GetBx() == nullptr) {
      Add(*pI, "%s must be BoolOutput.", pI->GetSx());
  }
}

void VerifyTask::Process(Instruction* const pI) {
  auto& outy = pI->output_type();

  if (auto const pBd = pI->GetBd()) {
      if (outy != *Ty_Bool) {
          Add(*pI, "Type must be bool.");
      }

  } else if (auto const pFd = pI->GetOutput()->DynamicCast<Float>()) {
      if (!outy.IsFloat()) {
          Add(*pI, "Type must be Float32/Float64.");
      }

  } else if (auto const pVd = pI->GetVd()) {
      if (!outy.Is<ValuesType>()) {
          Add(*pI, "Type must be ValuesType.");
      }
  }

  // Note: We have LE Boolean %r1 <= %r2 %r3 for Boolean value
  // rather than control Boolean.

  if (outy.Is<ValuesType>()) {
      if (!pI->GetOutput()->Is<Values>()) {
          Add(*pI, "Output must be Values.");
      }
  }

  if (!pI->Is<LastInstruction>()) {
      if (pI->GetNext() == nullptr) {
          Add(*pI, "Can't be the last instruction.");
      }
  }
}

// [P][L]
void VerifyTask::Process(LastInstruction* const pI) {
  Process(static_cast<LastInstruction::Base*>(pI));

  if (pI->GetNext() != nullptr) {
      Add(*pI, "Must be the last instruction.");
  }
}

void VerifyTask::Process(LoadI* const pI) {
  Process(static_cast<LoadI::Base*>(pI));

  auto const pSx = pI->GetSx();
  if (pSx == nullptr) {
      return;
  }

  auto const pPointerType = pSx->GetTy().DynamicCast<PointerType>();

  if (pPointerType == nullptr) {
      Add(*pI, "Type of %s(%s) must be pointer type.",
          pSx,
          pSx->GetTy());
      return;
  }

  auto const pOutput = pI->GetOutput();
  if (pOutput == nullptr) {
      return;
  }

  if (pPointerType->pointee_type() != pOutput->GetTy()) {
      Add(*pI, "Type of %s(%s) must be %s.",
          pOutput,
          pOutput->GetTy(),
          &pPointerType->pointee_type());
      return;
  }
}

// [P][N]
void VerifyTask::Process(NewI* const pI) {
  auto& new_inst = *pI;
  if (new_inst.output_type().Is<ArrayType>()) {
    Add(new_inst, "Output type must not an array type.");
    return;
  }
}

void VerifyTask::Process(NewArrayI* const pI) {
  auto& new_array_inst = *pI;
  auto const arrty = new_array_inst.output_type().DynamicCast<ArrayType>();
  if (!arrty) {
    Add(new_array_inst, "Output type must be an array type.");
    return;
  }

  auto const argsop = new_array_inst.op0().DynamicCast<Values>();
  if (!argsop) {
    Add(new_array_inst, "Operand must be a Values.");
    return;
  }

  auto const args_inst = argsop->GetDefI();
  if (!args_inst) {
    Add(new_array_inst, "No bind instruction for %s", argsop);
    return;
  }

  if (!args_inst->Is<ValuesI>()) {
    Add(new_array_inst,
        "NEWARRAY arguments must be initialized by VALUES instruction");
    return;
  }

  if (arrty->rank() != args_inst->CountOperands()) {
    Add(new_array_inst,
        "Expect %d arguments but %d",
        arrty->rank(),
        args_inst->CountOperands());
    return;
  }
}

// [P][P]
void VerifyTask::Process(PhiI* const pI) {
  Process(static_cast<PhiI::Base*>(pI));

  if (auto const pPrevI = pI->GetPrev()) {
      if (!pPrevI->Is<PhiI>()) {
          Add(*pI, "Must be at start of bblock.");
      }
  }

  {
      auto& outy = pI->output_type();
      for (const auto& operand: pI->operands()) {
          auto& operand_ty = operand.type();
          if (operand_ty.IsSubtypeOf(outy) != Subtype_Yes) {
              Add(*pI, "Type of %s(%s) must be subtype of %s.",
                  operand,
                  operand_ty,
                  outy);
          }
      }
  }

  HashSet_<BBlock*> oBBlockSet;

  foreach (BBlock::EnumPred, oEnum, pI->GetBBlock()) {
      auto const pPredBB = oEnum.Get();
      oBBlockSet.Add(pPredBB);
      if (pI->FindOperandBox(pPredBB) == nullptr) {
          Add(*pI, "No operand for %s", pPredBB);
      }
  }

  for (const auto& phi_box: pI->phi_operand_boxes()) {
      auto& bblock = phi_box.bblock();
      if (!oBBlockSet.Contains(&bblock)) {
          Add(*pI, "Extra operand for %s", bblock);
      }
  }
}

// [P][R]
void VerifyTask::Process(RelationalInstruction* const pI) {
  Process(static_cast<RelationalInstruction::Base*>(pI));

  if (pI->output_type() != *Ty_Bool) {
      Add(*pI, "Output type(%s) must be %s",
          pI->output_type(),
          Ty_Bool);
  }

  if (pI->GetSx()->GetTy() != pI->GetSy()->GetTy()) {
      Add(*pI, "Type of %s(%s) and %s(%s) must be same.",
          pI->GetSx(),
          pI->GetSx()->GetTy(),
          pI->GetSy(),
          pI->GetSy()->GetTy());
  }
}

void VerifyTask::Process(RetI* const pI) {
  Process(static_cast<RetI::Base*>(pI));

  auto const pFun = pI->GetBBlock()->GetFunction();
  auto& expected = pFun->function_type().return_type();
  auto const pSx = pI->GetSx();

  if (pSx->GetTy().IsSubtypeOf(expected) != Subtype_Yes) {
      Add(*pI, "Type of %s(%s) must be subtype of %s",
          pSx,
          pSx->GetTy(),
          expected);
  }
}

// [P][S]
void VerifyTask::Process(StoreI* const pI) {
  Process(static_cast<StoreI::Base*>(pI));

  auto const pSx = pI->GetSx();
  if (pSx == nullptr) {
      return;
  }

  auto const pPointerType = pSx->GetTy().DynamicCast<PointerType>();

  if (pPointerType == nullptr) {
      Add(*pI, "Type of %s(%s) must be pointer type.",
          pSx,
          pSx->GetTy());
      return;
  }

  auto const pSy = pI->GetSy();
  if (pSy == nullptr) {
      return;
  }

  if (pSy->GetTy().IsSubtypeOf(pPointerType->pointee_type())
          != Subtype_Yes) {
      Add(*pI, "Type of %s(%s) must be %s.",
          pSy,
          pSy->GetTy(),
          &pPointerType->pointee_type());
      return;
  }
}

// [P][V]
void VerifyTask::Process(ValuesI* const pI) {
  Process(static_cast<ValuesI::Base*>(pI));

  if (auto const pValuesType = pI->output_type().DynamicCast<ValuesType>()) {
      if (pValuesType->Count() != pI->CountOperands()) {
          Add(*pI, "Number of elements is mismatched.");
      }

  } else {
      Add(*pI, "Expect values type but %s", pI->output_type());
  }
}

void VerifyTask::ProcessFunction(Function& fn) {
  auto const pFunction = &fn;
  pFunction->Apply(this);

  auto const iCount = fail_list_.Count();
  if (iCount == 0) {
      DEBUG_FORMAT("%s is valid.", pFunction);
      return;
  }

  DEBUG_FORMAT("Invalid instructions in %s", pFunction);

  foreach (EntryList::Enum, entries, fail_list_) {
    auto& entry = *entries.Get();
    auto reason = String::Format(entry.format_, entry.params_);
    DEBUG_FORMAT("%s: %s", entry.inst_, reason);
    AddErrorInfo(
      VmErrorInfo(
        *entry.inst_.GetSourceInfo(),
        VmError_Instruction_Invalid,
        entry.inst_,
        reason));
  }
}

// [V]
void VerifyTask::VerifyFunction(const Function& fn) {
  ProcessFunction(const_cast<Function&>(fn));
}

bool VerifyTask::Verify(const Instruction& inst) {
  bool is_valid = true;
  {
    auto const cOperands = inst.CountOperands();

    switch (inst.GetArity()) {
    case Instruction::Arity_0:
        if (0 != cOperands) {
            Add(inst, "Too many operands");
            is_valid = false;
        }
        break;

    case Instruction::Arity_1:
        if (1 != cOperands) {
            Add(inst, "Number of operands must be 1.");
            is_valid = false;
        }
        break;

    case Instruction::Arity_2:
        if (2 != cOperands) {
            Add(inst, "Number of operands must be 2.");
            is_valid = false;
        }
        break;

    case Instruction::Arity_3:
        if (3 != cOperands) {
            Add(inst, "Number of operands must be 3.");
            is_valid = false;
        }
        break;

    case Instruction::Arity_Any:
        break;

    case Instruction::Arity_MoreThanZero:
        if (cOperands == 0) {
            Add(inst, "Too few operands.");
            is_valid = false;
        }
        break;

    default:
        Add(inst, "Invalid arity %s", inst.GetArity());
        is_valid = false;
        break;
    }
  }

  if (inst.output() == *Useless) {
    Add(inst, "%s is useless.", inst);
    is_valid = false;
  }

  if (auto const out = inst.output().DynamicCast<Register>()) {
    if (out->GetNext() || out->GetPrev()) {
      Add(inst, "%s is still in list.", out);
      is_valid = false;
    }
  }

  for (const auto& operand: inst.operands()) {
    if (auto const pRx = operand.DynamicCast<SsaOutput>()) {
      if (pRx == False || pRx == True) {
        continue;
      }

      if (pRx->GetDefI() == nullptr) {
        Add(inst, "No definition for %s", pRx);
        is_valid = false;
      }
    }
  }

  return is_valid;
}

} // Tasks
} // Il
