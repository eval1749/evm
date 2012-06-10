// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_Class_h)
#define INCLUDE_Il_Ir_Class_h

#include "./ClassOrNamespace.h"

#include "./Field.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

class Class
    : public ClassOrNamespace_<Class>,
      public WithModifiers {

  CASTABLE_CLASS(Class);

  public: typedef ArrayList_<const Class*> List;
  public: typedef Collection_<const Class*> Collection;

  // For GenericClass::Construct, TypeSorter
  public: class EnumBaseSpec : public Collection::Enum {
    private: typedef Collection::Enum Base;
    public: EnumBaseSpec(const Class& r) : Base(r.base_specs_) {}
    public: const Class& operator*() const { return Get(); }
    public: const Class& Get() const { return *Base::Get(); }
    DISALLOW_COPY_AND_ASSIGN(EnumBaseSpec);
  };

  class EnumClassTree {
    private: Queue_<const Class*> queue_;
    public: EnumClassTree(const Class&);
    public: bool AtEnd() const;
    public: const Class& Get() const;
    public: void Next();
    private: void QueueBaseSpecs(const Class&);
    DISALLOW_COPY_AND_ASSIGN(EnumClassTree);
  };

  public: class EnumField : public Field::List::Enum {
    public: EnumField(const Class& r)
        : Field::List::Enum(&r.fields_) {}

    public: EnumField(const Class* const p)
        : Field::List::Enum(&p->fields_) {}

    public: Field& operator*() const { return *Get(); }
  };

  private: Field::List fields_;
  private: Collection base_specs_;

  // ctor
  public: Class(const ClassOrNamespace&, int, const Name&);
  protected: Class(const GenericClass&);
  public: virtual ~Class();

  // [A]
  // To support dynamic construction of class.
  public: virtual void Add(const Name&, Operand&);

  // [C]
  // For GenericClass: caching constructed classes.
  public: virtual int ComputeHashCode() const override;

  // For GenericClass: construct base calss.
  public: virtual Type& Construct(const TypeArgs&) const override;

  // [G]
  public: const Class* GetBaseClass() const;
  public: Namespace* GetNamespace() const;
  public: const Class* GetOuterClass() const;
  public: virtual const Type& GetTy() const override { return *Ty_Class; }

  // [I]
  public: bool IsClass() const;
  public: bool IsInterface() const;
  public: bool IsStruct() const;
  public: virtual Subtype IsSubtypeOf(const Type&) const override;

  // [R]
  public: virtual void RealizeClass(const Collection_<const Class*>&);

  DISALLOW_COPY_AND_ASSIGN(Class);
}; // Class

// Note: To make world simple, we dont' want to have GenericInterface and
// ConstructedInterface. We use modifier to distinguish class and interface.
typedef Class Interface;

template<class TSelf, class TParent = Class>
class Class_
    : public WithCastable3_<
          TSelf,
          TParent,
          const ClassOrNamespace&,
          int,
          const Name&> {

  protected: typedef Class_<TSelf, TParent> Base;

  protected: Class_(
      const ClassOrNamespace& outer,
      int const modifiers,
      const Name& name)
      : WithCastable3_(outer, modifiers, name) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(Class_);
}; // Class_

} // Ir
} // Common

#endif // !defined(INCLUDE_Il_Ir_Class_h)
