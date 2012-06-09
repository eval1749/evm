// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Namespace_h)
#define INCLUDE_Il_Ir_Namespace_h

#include "./ClassOrNamespace.h"

namespace Il {
namespace Ir {

// Q: Why is Namespace derived from Class?
// A: Use Namespace as outer class of Class.

/// <summary>
///   Compilation module contains functions. This is a unit of compilation.
/// </summary>
class Namespace : public ClassOrNamespace_<Namespace> {
  CASTABLE_CLASS(Namespace)

  public: static Namespace* Common;
  public: static Namespace* Global;

  // ctor
  public: Namespace(const Namespace&, const Name&);

  // For compilation/runtime global namespace
  private: Namespace();

  // [C]
  public: virtual int ComputeHashCode() const override;

  public: virtual const Type& Construct(const TypeArgs&) const override {
    CAN_NOT_HAPPEN();
  }

  public: static Namespace& LibExport CreateGlobalNamespace();

  // [I]
  public: virtual Subtype IsSubtypeOf(const Type&) const override {
    CAN_NOT_HAPPEN();
  }

  DISALLOW_COPY_AND_ASSIGN(Namespace);
}; // Namespace

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_Namespace_h)
