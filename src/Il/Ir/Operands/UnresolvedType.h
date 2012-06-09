// @(#)$Id$
//
// Evita IL - IR - UnresolvedType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_UnresolvedType_h)
#define INCLUDE_Il_Ir_UnresolvedType_h

#include "./Type.h"

namespace Il {
namespace Ir {

/// <summary>
///     Parse only?
/// </summary>
class UnresolvedType : public Type_<UnresolvedType> {
  CASTABLE_CLASS(UnresolvedType);

  private: const NameRef& name_ref_;
  private: const Type* resolved_type_;

  // ctor
  public: UnresolvedType(const NameRef&);

  // properties
  public: const NameRef& name_ref() const { return name_ref_; }
  public: const Type& resolved_type() const;
  public: const SourceInfo& source_info() const;

  // [B]
  public: void Bind(const Type&);

  // [C]
  public: virtual const Type& ComputeBoundType() const override;
  public: virtual int ComputeHashCode() const override;
  public: virtual const Type& Construct(const TypeArgs&) const override;

  // [I]
  public: virtual bool IsBound() const override;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(UnresolvedType);
}; // UnresolvedType

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_UnresolvedType_h)
