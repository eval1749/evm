// @(#)$Id$
//
// Evita IL - IR - UnresolvedConstructedType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_UnresolvedConstructedType_h)
#define INCLUDE_Il_Ir_UnresolvedConstructedType_h

#include "./UnresolvedType.h"

namespace Il {
namespace Ir {

/// <summary>
///   This class represents unresolved constructed type constructed by parser
///   during compliation.
/// </summary>
/// Example:
///   Pair<A, B>
///     => UnresolvedConstructedType(
///         'Pair'
///         [ UnresolvedType('A'), UnresolvedType('B') ])
class UnresolvedConstructedType
    : public Type2_<
        UnresolvedConstructedType,
        UnresolvedType,
        const NameRef&> {
  CASTABLE_CLASS("UnresolvedConstructedType");

  private: Type::Collection types_;

  // ctor
  public: UnresolvedConstructedType(const NameRef&, const Type::Collection&);

  // protperties
  public: const Type::Collection& types() const { return types_; }

  // [C]
  public: virtual int ComputeHashCode() const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(UnresolvedConstructedType);
}; // UnresolvedConstructedType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_UnresolvedConstructedType_h)
