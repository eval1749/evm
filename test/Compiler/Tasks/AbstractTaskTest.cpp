#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractTaskTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractTaskTest.h"

namespace CompilerTest {

using namespace Compiler;

// [F]
CallI* AbstractTaskTest::FindCall(const NameRefI& name_ref_inst) const {
  foreach (
      Register::EnumUser,
      users,
      name_ref_inst.output().StaticCast<Register>()) {
    if (auto const load_inst = users->instruction().DynamicCast<LoadI>()) {
      foreach (
          Register::EnumUser,
          users,
          load_inst->output().StaticCast<Register>()) {
        if (auto const call_inst = users->instruction().DynamicCast<CallI>()) {
          return call_inst;
        }
      }
      return nullptr;
    }
  }
  return nullptr;
}

NameRefI* AbstractTaskTest::FindNameRef(
    const Method& method, const NameRef& name_ref) const {
  for (auto& inst: method.GetFunction()->instructions()) {
    if (auto const name_ref_inst = inst.DynamicCast<NameRefI>()) {
      if (name_ref_inst->op1() == name_ref) {
        return name_ref_inst;
      }
    }
  }
  return nullptr;
}

// [N]
const NameRef& AbstractTaskTest::NewNameRef(
    const String& name1) const {
  Vector_<const NameRef::Item*> names(1);
  names[0] = new NameRef::SimpleName(
      Name::Intern(name1), SourceInfo("test", 1, 1));
  return *new NameRef(names);
}

const NameRef& AbstractTaskTest::NewNameRef(
    const String& name1, const String& name2) const {
  Vector_<const NameRef::Item*> names(2);
  names[0] = new NameRef::SimpleName(
      Name::Intern(name1), SourceInfo("test", 1, 1));
  names[1] = new NameRef::SimpleName(
      Name::Intern(name2), SourceInfo("test", 1, 2));
  return *new NameRef(names);
}

} // CompilerTest
