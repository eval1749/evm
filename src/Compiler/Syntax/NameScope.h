// @(#)$Id$
//
// Evita Compiler - Syntax - NameScope
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Syntax_NameScope_h)
#define INCLUDE_Compiler_Syntax_NameScope_h

#include "../../Common/Collections/HashMap_.h"
#include "../../Common/Collections/PtrKey_.h"

#include "../Ir/NameDef.h"

namespace Compiler {

class NameDef;

/// <summary>
///  Represents name scope for local functions and local variables.
/// </symmary>
class NameScope {
  public: class EnumAncestor {
    private: const NameScope* runner_;

    public: EnumAncestor(const NameScope* const p) : runner_(p) {}
    public: bool AtEnd() const { return runner_ == nullptr; }

    public: NameScope* Get() const {
      ASSERT(!AtEnd()); return const_cast<NameScope*>(runner_);
    }

    public: void Next() {
      ASSERT(!AtEnd());
      runner_ = runner_->GetOuter();
    } // Next
  }; // EnumAncestor

  private: typedef HashMap_<const Name*, NameDef*> NameTable;

  private: NameTable name_table_;
  private: NameScope* const outer_;

  // ctor
  public: NameScope(MemoryZone&, NameScope* pOuer);

  public: const NameTable& entries() const { return name_table_; }

  // [A]
  public: void Add(NameDef&);

  // [F]
  public: NameDef* Find(const Name& name) const;

  // [G]
  public: NameScope* GetOuter() const { return outer_; }

  DISALLOW_COPY_AND_ASSIGN(NameScope);
}; // NameScope

} // Compiler

#endif // !defined(INCLUDE_Compiler_Syntax_NameScope_h)
