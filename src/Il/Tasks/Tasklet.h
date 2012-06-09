// @(#)$Id$
//
// Evita Il - Tasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_Tasklet_h)
#define INCLUDE_Il_Tasks_Tasklet_h

#include "./Session.h"
#include "../../Common/Io.h"

namespace Il {
namespace Tasks {

using namespace Il::Ir;

class Tasklet {
  private: Session& session_;
  private: String name_;

  protected: Tasklet(const String&, Session&);

  public: Session& session() const {
    return const_cast<Tasklet*>(this)->session_;
  }

  // [A]
  public: void AddErrorInfo(const ErrorInfo&);

  // [G]
  public: const char16* name() const { return name_.value(); }

  // [R]
  public: void ReplaceAll(const Operand&, const SsaOutput&);

  DISALLOW_COPY_AND_ASSIGN(Tasklet);
}; // Tasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_Tasklet_h)
