#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- UsingNamespace
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./UsingNamespace.h"

namespace Compiler {

// ctor
UsingNamespace::UsingNamespace(
    NamespaceBody* const nsb,
    const NameRef& name_ref)
    : Base(name_ref.GetSourceInfo()),
      namespace_(nullptr),
      namespace_body_(nsb),
      name_ref_(name_ref) {
} // UsingNamespace

} // Compiler
