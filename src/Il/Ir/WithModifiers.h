// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_WithModifiers_h)
#define INCLUDE_Il_Ir_WithModifiers_h

namespace Il {
namespace Ir {

class WithModifiers {
  private: int const modifiers_;

  protected: WithModifiers(int const iModifiers = 0)
    : modifiers_(iModifiers) {}

  // properteis
  public: int modifiers() const { return modifiers_; }

  // [G]
  public: int GetModifiers() const { return modifiers_; }

  // [I]
  #define DEFMODIFIER(mp_Name, mp_name, mp_nth) \
    public: bool Is ## mp_Name () const { \
      return (modifiers_ & Modifier_ ## mp_Name) != 0; \
    }

  #include "./Modifiers.inc"

  DISALLOW_COPY_AND_ASSIGN(WithModifiers);
}; // WithModifiers


} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_WithModifiers_h)
