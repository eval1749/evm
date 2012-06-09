#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Namespace
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Namespace.h"

#include "../Name.h"

namespace Il {
namespace Ir {

Namespace* Namespace::Common;
Namespace* Namespace::Global;

// ctor
Namespace::Namespace(const Namespace& outer, const Name& name)
    : Base(outer, name) {
  const_cast<Namespace&>(outer).Add(name, *this);
}


Namespace::Namespace()
    : ALLOW_THIS_IN_INITIALIZER_LIST(
        Base(*this, Name::Intern("::"))) {}

// [C]
int Namespace::ComputeHashCode() const {
  return Common::ComputeHashCode(name().ComputeHashCode(), 'N');
} // ComputeHashCode

Namespace& Namespace::CreateGlobalNamespace() {
  return *new Namespace();
}

} // Ir
} // Il
