// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_CallI_h)
#define INCLUDE_Il_Ir_Instructions_CallI_h

#include "./GeneralInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Call instruction.
///   <para>
///     CALL ty %sd &lt;= %sx %vy
///   </para>
/// </summary>
class CallI : public Instruction_<CallI, Op_Call> {
  private: FrameReg* m_pFrameReg;

  // ctor
  public: CallI();

  public: CallI(
      const Type& outy,
      const Output& output,
      const Operand& callee,
      const Values& args);

  // properties
  public: const ValuesType& args_type() const;

  // [G]
  public: FrameReg* GetFrameReg() const { return m_pFrameReg; }

  // [I]
  public: virtual bool IsUseless() const override;

  // [S]
  public: FrameReg* SetFrameReg(FrameReg* p) { return m_pFrameReg = p; }

  DISALLOW_COPY_AND_ASSIGN(CallI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_CallI_h)
