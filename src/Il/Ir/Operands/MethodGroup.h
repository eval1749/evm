// @(#)$Id$
//
// Evita Il - IR - IL - MethodGroup.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_MethodGroup_h)
#define INCLUDE_Il_Ir_MethodGroup_h

#include "./Operand.h"

#include "./Method.h"

namespace Il {
namespace Ir {

class Class;
class Name;

/// <summary>
///   Compilation module contains functions. This is a unit of compilation.
/// </summary>
class MethodGroup : public Operand_<MethodGroup> {
  CASTABLE_CLASS(MethodGroup);

  public: class EnumMethod : public Method::List::Enum {
    private: typedef Method::List::Enum Base;
    public: EnumMethod(const MethodGroup& r) : Base(&r.methods_) {}
    public: EnumMethod(const MethodGroup* p) : Base(&p->methods_) {}
    public: Method& operator*() const { return *Get(); }
  };

  private: Method::List methods_;
  private: Class& owner_class_;
  private: const Name& name_;

  public: MethodGroup(Class&, const Name&);

  // properties
  public: Class& owner_class() const { return owner_class_; }
  public: const Name& name() const { return name_; }

  // [A]
  public: void Add(Method&);

  // [F]
  public: Method* Find(const FunctionType&) const;

  // [G]
  public: Namespace& GetNamespace() const;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(MethodGroup);
}; // MethodGroup

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_MethodGroup_h)
