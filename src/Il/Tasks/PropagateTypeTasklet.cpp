#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - EditInstructionTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./PropagateTypeTasklet.h"

#include "../Ir.h"
#include "../Ir/VmErrorInfo.h"

namespace Il {
namespace Tasks {

// ctor
PropagateTypeTasklet::PropagateTypeTasklet(
    Session& session,
    const Module& module)
    : EditInstructionTasklet("PropagateTypeTasklet", session, module) {}

void PropagateTypeTasklet::EditInstruction(Instruction* const inst_ptr) {
  auto& inst = *inst_ptr;

  if (auto const fieldptr_inst = inst.DynamicCast<FieldPtrI>()) {
    if (auto const r1 = fieldptr_inst->GetRx()) {
      if (auto const vardef_inst = r1->GetDefI()->DynamicCast<VarDefI>()) {
        auto& cell_class = *vardef_inst->output_type()
          .StaticCast<ConstructedClass>();

        fieldptr_inst->GetOperandBox(1)->Replace(*cell_class.Find(*Q_value));

        UpdateOutputType(
          inst,
          PointerType::Intern(cell_class.GetTypeArg()));
      }
    }

  } else if (inst.Is<LoadI>()) {
    if (auto const  ptrty =
            inst.GetSx()->GetTy().DynamicCast<PointerType>()) {
      UpdateOutputType(inst, ptrty->pointee_type());
    } else {
      AddErrorInfo(
        VmErrorInfo(
            inst.source_info(),
            VmError_Instruction_Invalid,
            inst,
            String::Format("Expect pointer type: %s", inst.op0().GetTy())));
    }

  } else if (auto const select_inst = inst.DynamicCast<SelectI>()) {
    auto const nth = select_inst->GetIy();
    auto& valsty = *select_inst->GetVx()->GetDefI()->output_type()
            .StaticCast<ValuesType>();
    if (nth >= valsty.Count()) {
      AddErrorInfo(
        VmErrorInfo(
            inst.source_info(),
            VmError_Instruction_Invalid,
            inst,
            String::Format("Too large index(%d) for %s", nth, valsty)));
      return;
    }

    UpdateOutputType(inst, valsty.Get(nth));

  } else if (auto const vardef_inst = inst.DynamicCast<VarDefI>()) {
    auto& ty = vardef_inst->GetSy()->GetTy();
    UpdateOutputType(inst, Ty_ClosedCell->Construct(ty));

  } else {
    UpdateOutputType(inst, inst.output_type());
  }
}

void PropagateTypeTasklet::UpdateOutputType(Instruction& inst, const Type& ty) {
  auto& newty = ty.ComputeBoundType();
  if (inst.output_type() == newty) {
    return;
  }

  inst.set_output_type(newty);
  Add(&inst.output());
}

} // Tasks
} // Il
