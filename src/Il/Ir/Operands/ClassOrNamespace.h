// @(#)$Id$
//
// Evita Il - IR - ClassOrNamespace
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ClassOrNamespace_h)
#define INCLUDE_Il_Ir_ClassOrNamespace_h

#include "./Type.h"

namespace Il {
namespace Ir {

class ClassOrNamespace : public Type_<ClassOrNamespace> {
  CASTABLE_CLASS(ClassOrNamespace)

  // Entry of NameTable is one of
  //  o Class
  //  o ConstructedClass
  //  o Field
  //  o FunctionType
  //  o GenericClass
  //  o GenericFunctionType
  //  o MethodGroup
  private: typedef HashMap_<const Name*, Operand*> NameTable;

  // For FASL.
  public: class EnumOuterAndSelf {
    private: ClassOrNamespace* cur_;
    private: bool seen_global_ns_;

    public: EnumOuterAndSelf(const ClassOrNamespace& r)
      : cur_(&const_cast<ClassOrNamespace&>(r)), seen_global_ns_(false) {}

    public: bool AtEnd() const { return seen_global_ns_; }
    public: ClassOrNamespace& Get() const { ASSERT(!AtEnd()); return *cur_; }

    public: void Next() {
      ASSERT(!AtEnd());
      seen_global_ns_ = cur_->IsGlobalNamespace();
      cur_ = &cur_->outer();
    }
  };

  private: NameTable name_table_;
  private: const Name& name_;
  private: ClassOrNamespace& outer_;

  // ctor
  protected: ClassOrNamespace(const ClassOrNamespace&, const Name&);

  public: virtual ~ClassOrNamespace();

  // properties
  public: const NameTable& entries() const { return name_table_; }
  public: const NameTable::Values members() const {
    return name_table_.values();
  }
  public: const Name& name() const { return name_; }
  public: ClassOrNamespace& outer() const { return outer_; }

  // [A]
  public: void Add(const Name&, Operand&);

  // [F]
  public: Operand* Find(const Name&) const;

  // [I]
  public: bool IsGlobalNamespace() const { return &outer_ == this; }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(ClassOrNamespace);
};

template<class TSelf, class TParent = ClassOrNamespace>
class ClassOrNamespace_ :
  public WithCastable2_<
    TSelf,
    TParent,
    const ClassOrNamespace&,
    const Name&> {

  protected: typedef ClassOrNamespace_<TSelf, TParent> Base;

  // ctor
  protected: ClassOrNamespace_(
    const ClassOrNamespace& outer,
    const Name& name) : WithCastable2_(outer, name) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<TSelf*>(this));
  }

  DISALLOW_COPY_AND_ASSIGN(ClassOrNamespace_);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ClassOrNamespace_h)
