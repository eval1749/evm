#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- TypeParamDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TypeParamDef.h"

#include "./ClassDef.h"
#include "./MethodDef.h"

namespace Compiler {

// ctor
TypeParamDef::TypeParamDef(
    ClassOrNamespaceDef& owner,
    const Name& name,
    const SourceInfo& source_info)
    : Base(owner, 0, name, source_info),
      newable_(0),
      owner_(nullptr),
      type_param_(*new TypeParam(name)) {}

// properties
void TypeParamDef::set_owner(const NameDef& owner) {
  ASSERT(!owner_);
  ASSERT(owner.Is<ClassDef>() || owner.Is<MethodDef>());
  owner_ = &owner;
}

// [A]
void TypeParamDef::AddConstraint(const Type& type) {
  ASSERT(!!owner_);
  constraints_.Add(&type);
}

// [M]
void TypeParamDef::MarkNewable() {
  ASSERT(!!owner_);
  ASSERT(!newable_);
  ++newable_;
}

} // Compiler
