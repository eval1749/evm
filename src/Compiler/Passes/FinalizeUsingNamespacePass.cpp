#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FinalizeUsingNamespacePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FinalizeUsingNamespacePass.h"

#include "../CompileSession.h"
#include "../Ir.h"

namespace Compiler {

// ctor
FinalizeUsingNamespacePass::FinalizeUsingNamespacePass(
    CompileSession* const pCompileSession)
    : Base(L"FinalizeUsingNamespacePass", pCompileSession) {}

// [P]
void
FinalizeUsingNamespacePass::Process(AliasDef* const alias_def) {
  ASSERT(alias_def != nullptr);
  ASSERT(!alias_def->IsRealized());

  auto& nsb = *alias_def->namespace_body();
  auto& real_name = *alias_def->real_name();

  NameRef::Enum names(real_name);
  auto& most_outer_name = names->StaticCast<NameRef::SimpleName>()
      ->name();
  names.Next();

  ResolveContext context(ExcludeImmediateNamespaceBody, nsb);

  auto const result = FindMostOuter(
      context,
      most_outer_name,
      real_name.source_info());
  auto const most_outer_ns = result.operand_->DynamicCast<Namespace>();
  if (!most_outer_ns) {
    // Alias isn't found or class alias.
    return;
  }

  auto ns = most_outer_ns;
  while (!names.AtEnd()) {
    auto const simple_name = names->DynamicCast<NameRef::SimpleName>();
    ASSERT(simple_name != nullptr);
    ns = ns->Find(simple_name->name())->DynamicCast<Namespace>();
    if (!ns) {
      return;
    }
    names.Next();
  }

  DEBUG_FORMAT("Realize namespace alias %s to %s",
      alias_def->name(), ns);

  alias_def->Realize(*ns);
}

void FinalizeUsingNamespacePass::Process(UsingNamespace* const uzing) {
  ASSERT(uzing != nullptr);
  ASSERT(!uzing->IsRealized());

  NameRef::Enum names(uzing->name_ref());
  auto& most_outer = *names;
  auto& most_outer_name = most_outer.name();
  names.Next();

  ResolveContext context(
    ExcludeImmediateNamespaceBody,
    *uzing->namespace_body());

  auto const result = FindMostOuter(
      context,
      most_outer_name,
      uzing->source_info());

  auto const most_outer_ns = result.operand_->DynamicCast<Namespace>();
  if (!most_outer_ns) {
    if (!result.ready_) {
      compile_session().AddError(
          uzing->source_info(),
          CompileError_Resolve_Namespace_NotFound,
          most_outer_name);
      return;
    }

    if (auto const root_ns = Namespace::Global->Find(most_outer_name)) {
      auto ns = ExpectNamespace(most_outer, *root_ns);
      if (!ns) {
        return;
      }

      while (!names.AtEnd()) {
        auto const simple_name = names->StaticCast<NameRef::SimpleName>();
        ns = new Namespace(*ns, simple_name->name());
        names.Next();
      }

      DEBUG_FORMAT("Using namespace %s", ns);
      uzing->Realize(ns);
      return;
    }

    compile_session().AddError(
        uzing->source_info(),
        CompileError_Resolve_Namespace_NotFound,
        most_outer_name);
    return;
  }

  auto ns = most_outer_ns;
  while (!names.AtEnd()) {
    auto& item = *names->StaticCast<NameRef::SimpleName>();
    auto const present = ns->Find(item.name());
    if (!present) {
      compile_session().AddError(
          item.source_info(),
          CompileError_Resolve_Namespace_NotFound,
          item);
        return;
    }

    ns = ExpectNamespace(item, *present);
    if (!ns) {
      return;
    }
    names.Next();
  }

  DEBUG_FORMAT("Using namespace %s", ns);
  uzing->Realize(ns);
}

} // Compiler
