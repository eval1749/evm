// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_GenericClass_h)
#define INCLUDE_Il_Ir_GenericClass_h

#include "./Class.h"

#include "./TypeArgs.h"
#include "./TypeParam.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

class GenericClass : public Class_<GenericClass> {
  CASTABLE_CLASS(GenericClass);

  private: typedef HashMap_<TypeArgs::MapKey, ConstructedClass*>
    ConsClassMap;

  private: typedef Collection_<const TypeParam*> TypeParams;

  public: class EnumTypeParam : public TypeParams::Enum {
    private: typedef TypeParams::Enum Base;

    public: class EnumTypeParam(const GenericClass& r)
        : Base(const_cast<GenericClass&>(r).type_params_) {}

    DISALLOW_COPY_AND_ASSIGN(EnumTypeParam);
  }; // EnumTypeParam

  private: TypeParams type_params_;
  private: mutable ConsClassMap cons_class_map_;

  // ctor
  public: GenericClass(
      const ClassOrNamespace&,
      int,
      const Name& name,
      const Collection_<const TypeParam*>&);

  public: virtual ~GenericClass();

  // [C]
  public: virtual Type& Construct(const TypeArgs&) const override;
  // TODO(yosi) 2012-02-18 Remove GenericClass::Construct(Type&)
  public: ConstructedClass& Construct(const Type&);

  // [G]
  // ConstructedClass uses GenericClass::GetTypeParams in IsOpen.
  public: const Collection_<const TypeParam*>& GetTypeParams() const;

  // [I]
  public: bool IsBound() const { return false; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(GenericClass);
}; // GenericClass

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_GenericClass_h)
