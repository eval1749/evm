#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Name
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Name.h"

#include "../../Common/GlobalMemoryZone.h"
#include "../../Common/String.h"
#include "../../Common/Collections/HashMap_.h"
#include "../../Common/Io/TextWriter.h"


namespace Il {
namespace Ir {

using Common::Collections::HashMap_;

typedef HashMap_<String, const Name*> NameTable;

static NameTable* s_pNameTable;

// ctor
Name::Name(const String str, uint hash_code)
    : name_(str), hash_code_(hash_code) {}

// [I]
void Name::Init() {
  ASSERT(s_pNameTable == nullptr);
  s_pNameTable = new NameTable(GlobalMemoryZone::Get());
  ASSERT(Intern("value") == Intern("value"));
} // Init

const Name& Name::Intern(const String& str) {
  if (auto const present = s_pNameTable->Get(str)) {
    return *present;
  }

  auto& name = *new Name(str, str.ComputeHashCode());
  s_pNameTable->Add(str, &name);
  return name;
}

// [T]
String Name::ToString() const {
  return name_;
}

} // Ir
} // Il
