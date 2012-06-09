// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_SsaOutput_h)
#define INCLUDE_Il_Ir_SsaOutput_h

#include "./Output.h"

#include "../Instructions/OperandBox.h"

namespace Il {
namespace Ir {

/// <summary>
///   Internal representation SSA Output.
/// </summary>
class SsaOutput
    : public Operand_<SsaOutput, Output>,
      public DoubleLinkedList_<OperandBox, Operand> {
  CASTABLE_CLASS(SsaOutput)

  public: class EnumUser : public Users::Enum {
    private: typedef Users::Enum Base;

    public: EnumUser(const SsaOutput& r) : Base(r) {}
    public: EnumUser(const SsaOutput* p) : Base(p) {}

    public: Instruction* GetI() const {
      ASSERT(! AtEnd());
      return Get()->GetI();
    }
  };

  protected: SsaOutput() {}

  // properties
  public: virtual Instruction* def_inst() const override {
    return GetDefI();
  }

  // [G]
  public: OperandBox* GetFirstUser() const;
  public: OperandBox* GetSingleUser() const;
  public: virtual const Type& GetTy() const override;

  // [H]
  public: bool HasUser() const;

  // [R]
  public: virtual void Realize(OperandBox* const pBox) override;

  // [U]
  public: virtual void Unrealize(OperandBox* const pBox) override;

  DISALLOW_COPY_AND_ASSIGN(SsaOutput);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_SsaOutput_h)
