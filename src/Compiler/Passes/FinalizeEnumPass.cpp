#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeEnumPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FinalizeEnumPass.h"

#include "../CompileSession.h"
#include "../Ir.h"

#include "../../Il/Tasks/NormalizeTasklet.h"

namespace Compiler {

using namespace Il::Tasks;

// ctor
FinalizeEnumPass::FinalizeEnumPass(CompileSession* const pCompileSession)
    : Base(L"FinalizeEnumPass", pCompileSession) {
}

// [F]
bool FinalizeEnumPass::FixStaticConstFields(Class* const clazz) {
  ASSERT(clazz != nullptr);

  auto const class_ctor_group = clazz->Find(*QD_cctor);
  if (class_ctor_group == nullptr) {
      return false;
  } // if

  auto const cctor = class_ctor_group->StaticCast<MethodGroup>()
      ->Find(FunctionType::Intern(*Ty_Void, ValuesType::Intern()));

  if (cctor == nullptr) {
    return false;
  } // if

  auto const fun = cctor->StaticCast<Method>()->GetFunction();
  auto const start_bb = fun->GetStartBB();
  if (!start_bb->GetLastI()->Is<RetI>()) {
    // Note: This is sanity check. Class ctor should end with RetI.
    return false;
  } // if

  {
    NormalizeTasklet normalize_task(session(), fun->module());

    auto need_normalize = false;
    for (auto& inst: start_bb->instructions()) {
      if (inst.Is<PrologueI>()) {
        continue;
      } // if

      if (inst.Is<RetI>()) {
        continue;
      } // if

      normalize_task.Add(&inst);

      if (inst.Is<FieldPtrI>()) {
        continue;
      } // if

      need_normalize = true;
    } // for

    if (need_normalize) {
      normalize_task.Start();

    } else {
      normalize_task.MakeEmpty();
    } // if
  }

  // Do we still have unresolved enum field?
  foreach (Class::EnumField, fields, clazz) {
    auto const field = fields.Get();
    if (field->IsStatic() && field->IsConst()) {
      if (!field->GetOperand()->Is<Literal>()) {
          return true;
      } // if
    } // if
  } // for

  // Make class constructor empty
  // Note: FASD will remove Empty class constructor.
  {
    NormalizeTasklet normalize_task(session(), fun->module());
    WorkList_<Instruction> removes;
    for (auto& inst: start_bb->instructions()) {
      if (inst.Is<StoreI>())
        removes.Push(&inst);
      else
        normalize_task.Add(&inst);
    }

    while (!removes.IsEmpty()) {
      start_bb->RemoveI(*removes.Pop());
    } // while

    normalize_task.Start();
  }

  return false;
} // FixStaticConstFields

// [P]
/// <summary>
///     Resolves base class name and interface names.
/// </summary>
void FinalizeEnumPass::Process(ClassDef* const class_def) {
  ASSERT(class_def != nullptr);
  auto const clazz = &class_def->GetClass();
  if (FixStaticConstFields(clazz)) {
    pending_queue_.Give(clazz);
  } // if
} // Process

// [S]
void FinalizeEnumPass::Start() {
  compile_session().Apply(this);

  if (pending_queue_.IsEmpty()) {
      return;
  } // if

  Queue_<Class*> ready_queue;

  while (!pending_queue_.IsEmpty()) {
    auto const clazz = pending_queue_.Take();
    if (FixStaticConstFields(clazz)) {
      ready_queue.Give(clazz);
    } // if
  } // while

  while (!ready_queue.IsEmpty()) {
    auto const clazz = ready_queue.Take();
    if (FixStaticConstFields(clazz)) {
      pending_queue_.Give(clazz);
    } // if
  } // while

  while (!pending_queue_.IsEmpty()) {
    auto const clazz = pending_queue_.Take();

    foreach (Class::EnumField, fields, clazz) {
      auto const field = fields.Get();
      if (field->IsStatic() && field->IsConst()) {
          DEBUG_FORMAT("%s %s", field, field->GetOperand());
          if (!field->GetOperand()->Is<Literal>()) {
            compile_session().AddError(
                field->source_info(),
                CompileError_Finalize_Field_StaticConst_NotConst,
                field);
        } // if
      } // if
    } // for field
  } // while
} // Start

} // Compiler
