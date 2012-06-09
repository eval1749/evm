#include "precomp.h"
// @(#)$Id$
//
// Evita Cg X86 Register
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86Register.h"

namespace Il {
namespace Cg {

X86Register::X86Register(const RegDesc& reg_desc)
    : CgOperand1_(reg_desc) {}

String X86Register::ToString() const {
  return reg_desc().m_pszName;
}

} // Cg
} // Il
