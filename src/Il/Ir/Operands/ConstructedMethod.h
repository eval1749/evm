// @(#)$Id$
//
// Evita Il - Ir - ConstructedMethod.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ConstructedMethod_h)
#define INCLUDE_Il_Ir_ConstructedMethod_h

#include "./GenericMethod.h"

#include "./TypeArgs.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

/// <summary>
///   This class represents constructed method constructed from a generic
//    method with type arguments.
/// </summary>
class ConstructedMethod : public Method_<ConstructedMethod, Method> {
  CASTABLE_CLASS(ConstructedMethod);

  public: class EnumTypeArg : public TypeArgs::Enum {
    private: typedef TypeArgs::Enum Base;
    public: EnumTypeArg(const ConstructedMethod& r) : Base(r.type_args_) {}
    public: EnumTypeArg(const ConstructedMethod* p) : Base(p->type_args_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumTypeArg);
  }; // EnumTypeArg

  private: const GenericMethod& generic_method_;
  private: const TypeArgs type_args_;

  // For using ctor.
  friend class GenericMethod;

  // ctor
  private: ConstructedMethod(const GenericMethod&, const TypeArgs&);

  // properties
  public: const GenericMethod& generic_method() const { return generic_method_; }
  public: const TypeArgs& type_args() const { return type_args_; }

  DISALLOW_COPY_AND_ASSIGN(ConstructedMethod);
}; // ConstructedMethod

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ConstructedMethod_h)
