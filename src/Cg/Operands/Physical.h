// @(#)$Id$
//
// Evita Il - Cg - Operands
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_Operands_Physical_h)
#define INCLUDE_Il_Cg_Operands_Physical_h

#include "./CgOperand.h"

namespace Il {
namespace Cg {

/// <summary>
///   Represents physical register in IR.
/// </summary>
class Physical : public CgOperand_<Physical, CgOutput> {
  CASTABLE_CLASS(Physical)

  private: int use_count_;
  private: const RegDesc& reg_desc_;

  // ctor
  public: explicit Physical(const RegDesc&);

  // properties
  public: const RegDesc& reg_desc() const { return reg_desc_; }
  public: int use_count() const { return use_count_; }

  // [G]
  public: const RegDesc* GetDesc() const { return &reg_desc_; }
  public: virtual char16 GetPrefixChar() const override { return 'P'; }

  // [R]
  public: void ResetUseCount() { use_count_ = 0; }

  // [U]
  public: void Use() { ++use_count_; }
  public: void UnUse() { --use_count_; }

  DISALLOW_COPY_AND_ASSIGN(Physical);
}; // Physical

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_Operands_Physical_h)
