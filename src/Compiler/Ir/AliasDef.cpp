#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- AliasDef
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AliasDef.h"

#include "./NamespaceBody.h"
#include "./NamespaceDef.h"
#include "../Functor.h"

namespace Compiler {

AliasDef::AliasDef(
    NamespaceBody* namespace_body,
    const Name& name,
    const NameRef& real_name,
    const SourceInfo& source_info)
    : Base(namespace_body->namespace_def(), name, source_info),
      namespace_body_(namespace_body),
      real_name_(real_name) {}

} // Compiler
