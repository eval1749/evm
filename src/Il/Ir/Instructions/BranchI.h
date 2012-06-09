// @(#)$Id$
//
// Evita Il - IR - Instructions - BranchI
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Instructions_BranchI_h)
#define INCLUDE_Il_Ir_Instructions_BranchI_h

#include "./LastInstruction.h"

namespace Il {
namespace Ir {

/// <summary>
///   Branch instruction. Transfers controls to BBy if %bx is true, otherwise
///   transfers to BBz.
///   <example>
///     BRANCH %bx BBy BBz
///   </example>
///   <para>
///     We prefer positive branch over negative branch and call positive
///     branch as normal form. The method Optimize transform negative
///     branch to positive branch.
///   </para>
///   <para>
///     Postive branch
///     <example>
///       NE %b1 &lt;= %r2 nil
///       BRANCH %b1 BB3 BB4        ; branch to BB3 if %r2 is true
///     </example>
///   </para>
///   <para>
///     Postive branch
///     <example>
///       EQ %b1 &lt;= %r2 nil
///       BRANCH %b1 BB3 BB4        ; branch to BB3 if %r2 is false
///     </example>
///   </para>
/// </summary>
class BranchI : public Instruction_<BranchI, Op_Branch, LastInstruction> {
  // ctor
  public: BranchI();
  public: BranchI(const BoolOutput&, const BBlock&, const BBlock&);

  // [G]
  public: BBlock* GetFalseBB() const;
  public: BBlock* GetTrueBB() const;

  // [R]
  public: virtual void Realize() override;

  // [U]
  public: virtual void Unrealize() override;

  DISALLOW_COPY_AND_ASSIGN(BranchI);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Instructions_BranchI_h)
