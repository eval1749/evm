// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Output_h)
#define INCLUDE_Il_Ir_Output_h

#include "./Operand.h"

namespace Il {
namespace Ir {

/// <summary>
///   Internal representation Output.
/// </summary>
class Output :
      public Operand_<Output>,
      public WithIndex,
      public WithWorkArea {
  CASTABLE_CLASS(Output);

  private: mutable int name_;
  private: Instruction* m_pDefI;

  // ctor
  protected: Output();
  public: int name() const { return name_; }
  public: void set_name(int name) const { name_ = name; }

  // [G]
  public: Instruction* GetDefI() const { return m_pDefI; }
  public: virtual char16 GetPrefixChar() const { return 'r'; }
  public: virtual const Type& GetTy() const override;

  // [I]
  public: virtual bool IsFalse() const { return false; }
  public: virtual bool IsTrue() const { return false; }

  // [S]
  public: void SetDefI(Instruction* const p) { m_pDefI = p; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Output);
}; // Output

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Output_h)
