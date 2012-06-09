// @(#)$Id$
//
// Evita Il - Ir - GenericMethod.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_GenericMethod_h)
#define INCLUDE_Il_Ir_GenericMethod_h

#include "./Method.h"

#include "./TypeArgs.h"
#include "./TypeParam.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

/// <summary>
///     This class represents generic method.
/// </summary>
class GenericMethod : public Method_<GenericMethod, Method> {
  CASTABLE_CLASS(GenericMethod);

  private: typedef HashMap_<TypeArgs::MapKey, ConstructedMethod*>
    ConsMethodMap;

  private: typedef Collection_<const TypeParam*> TypeParams;

  public: class EnumTypeParam : public TypeParams::Enum {
    private: typedef TypeParams::Enum Base;

    public: class EnumTypeParam(const GenericMethod& r) 
        : Base(r.type_params_) {}

    DISALLOW_COPY_AND_ASSIGN(EnumTypeParam);
  }; // EnumTypeParam

  private: TypeParams type_params_;
  private: mutable ConsMethodMap cons_method_map_;

  // ctor
  public: GenericMethod(
      const MethodGroup&,
      int const iModifiers,
      const FunctionType&,
      const Collection_<const TypeParam*>& type_params);

  public: virtual ~GenericMethod() {}

  // [C]
  public: ConstructedMethod& Construct(const TypeArgs&) const;

  DISALLOW_COPY_AND_ASSIGN(GenericMethod);
}; // GenericMethod

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_GenericMethod_h)
