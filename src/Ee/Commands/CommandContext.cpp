#include "precomp.h"
// @(#)$Id: /proj/evcl3/boot/precomp.h 13 2006-07-29 01:55:00 yosi $
//
// Evia Executor - CommandContext
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
#include "./CommandContext.h"

#include "../../Common/Io.h"
#include "../ExecuteSession.h"

namespace Executor {

CommandContext::CommandContext(
    ExecuteSession& session,
    TextWriter& stdout,
    TextWriter& stderr)
    : module_(nullptr),
      session_(session),
      stderr_(stderr),
      stdout_(stdout) {}

CommandContext::~CommandContext() {
  DEBUG_PRINTF("%p\n", this);
}

// [E]
Module* CommandContext::EnsureModule() {
  if (module_) {
    return module_;
  }

  if (name_.IsEmpty()) {
    auto& Ty_StringVector = ArrayType::Intern(*Ty_String);
    auto methods = FindMethods(
        Name::Intern("Main"),
        CollectionV_<const FunctionType*>(
            &FunctionType::Intern(*Ty_Void, ValuesType::Intern()),
            &FunctionType::Intern(*Ty_Int32, ValuesType::Intern()),
            &FunctionType::Intern(
                *Ty_Void,
                ValuesType::Intern(Ty_StringVector)),
            &FunctionType::Intern(
                *Ty_Int32,
                ValuesType::Intern(Ty_StringVector))));

    ArrayList_<Method*> public_methods;
    for (auto const method: methods) {
      if (method->IsPublic()) {
        public_methods.Add(method);
      }
    }

    switch (public_methods.Count()) {
      case 0:
        if (methods.IsEmpty()) {
          stderr_.WriteLine("No such method %s.", name_);
        } else {
          stderr_.WriteLine(
            "There are method %s, but they aren't callable:",
            name_);
          for (auto const method: methods)
            stderr_.WriteLine("  %s", *method);
        }
        return nullptr;

      case 1: {
        auto& method = *public_methods.Get(0);
        auto const fun = method.GetFunction();
        if (!fun) {
          stderr_.WriteLine("No function body: %s", name_);
          return nullptr;
        }
        return &fun->module();
      }

      default: {
        stderr_.WriteLine("There are two many entry point methods:");
        for (auto& method: public_methods) {
          stderr_.WriteLine("  %s", method);
        }
        return nullptr;
      }
    }
  }

  auto& name = Name::Intern(name_);
  for (auto const clazz: class_list_) {
    if (auto const mtg = clazz->Find(name)->DynamicCast<MethodGroup>()) {
      for (auto& method: mtg->methods()) {
        if (auto const fun = method.GetFunction())
          return &fun->module();
      }
    }
  }
  stderr_.WriteLine("No such method: %s", name_);
  return nullptr;
}

const Operand* CommandContext::EnsureOperand() {
  if (name_.IsEmpty()) {
    return class_list_.IsEmpty() 
        ? static_cast<const Operand*>(Namespace::Global)
        : static_cast<const Operand*>(class_list_.Get(0));
  }

  {
    auto& name_atom = Name::Intern(name_);
    for (auto const clazz: class_list_) {
      if (clazz->name() == name_atom) {
        return clazz;
      }

      if (auto const thing = clazz->Find(name_atom)) {
        return thing;
      }
    }
  }

  const Operand* operand = Namespace::Global;
  String runner = name_;
  for (;;) {
    auto index = runner.IndexOf('.');
    if (index < 0) {
      operand = operand->StaticCast<ClassOrNamespace>()
          ->Find(Name::Intern(runner));
      break;
    }

    auto const piece = runner.Substring(0, index);

    auto const present =
      operand->StaticCast<ClassOrNamespace>()
          ->Find(Name::Intern(piece));

    if (!present->DynamicCast<ClassOrNamespace>()) {
      stderr_.WriteLine("Not found %s in %s", piece, operand);
      return nullptr;
    }

    runner = runner.Substring(index + 1);
    operand = present;
  }

  if (!operand|| operand == Namespace::Global) {
    stderr_.WriteLine("Not found: %s", name_);
    return nullptr;
  }

  return operand;
}

// [F]
Collection_<Method*> CommandContext::FindMethods(
    const Name& name,
    const Collection_<const FunctionType*> funty_list) const {
  ArrayList_<Method*> methods;

  for (auto const clazz: class_list_) {
    auto const mtg = clazz->Find(name)->DynamicCast<MethodGroup>();
    if (!mtg) {
      continue;
    }

    for (auto const funty: funty_list) {
      if (auto const mt = mtg->Find(*funty)) {
        methods.Add(mt);
      }
    }
  }
  return methods;
}

// [H]
void CommandContext::HandleClass(Class& clazz) {
  class_list_.Add(&clazz);
}

bool CommandContext::HasError() const {
  return session_.HasError();
}

} // Executor
