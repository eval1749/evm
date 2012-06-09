#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Method Group
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MethodGroupDef.h"

#include "./ClassDef.h"
#include "../Functor.h"

namespace Compiler {

MethodGroupDef::MethodGroupDef(
    ClassDef& owner,
    const Name& name)
    : Base(owner, name, SourceInfo()) {}

MethodGroup& MethodGroupDef::method_group() const {
  return *operand().StaticCast<MethodGroup>();
}

ClassDef& MethodGroupDef::owner_class_def() const {
  return *owner().StaticCast<ClassDef>();
}

// [A]
void MethodGroupDef::Add(MethodDef& mt) {
  method_defs_.Append(&mt);
}

// [C]
int MethodGroupDef::CountMethods() const {
  return method_defs_.Count();
}

} // Compiler
