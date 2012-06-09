#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Module
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Tasklet.h"

#include "./Session.h"

#include "../Ir.h"

namespace Il {
namespace Tasks {

Tasklet::Tasklet(const String& name, Session& session)
    : name_(name), session_(session) {}

// [A]
void Tasklet::AddErrorInfo(const ErrorInfo& error_info) {
  session_.AddErrorInfo(error_info);
}

// [R]
void Tasklet::ReplaceAll(const Operand& new_op, const SsaOutput& old_op) {
  while (auto const box = old_op.GetFirstUser()) {
    ASSERT(box->GetOperand() == &old_op);
    const_cast<SsaOutput&>(old_op).Unrealize(box);
    box->SetOperand(&const_cast<Operand&>(new_op));
    const_cast<Operand&>(new_op).Realize(box);
  } // while
}
} // Tasks
} // Il
