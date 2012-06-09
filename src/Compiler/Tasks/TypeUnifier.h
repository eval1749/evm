// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_TypeUnifier_h)
#define INCLUDE_Compiler_Passes_TypeUnifier_h

#include "../../Il/Tasks/Tasklet.h"

namespace Compiler {

using Il::Tasks::Tasklet;

// interface Foo<T> {}
// class Bar<T> : Foo<T> {}
// class Baz : Foo<int> {}
// class Bazz : Foo<long>, Foo<String>, Bar<int> {}
class TypeUnifier : public Tasklet {
  private: class Binding {
    private: const Type* lhs_;
    private: const Type* rhs_;
    public: Binding() : lhs_(nullptr), rhs_(nullptr) {}

    public: Binding(const Binding& r)
        : lhs_(r.lhs_), rhs_(r.rhs_) {}

    public: Binding(const Type& lhs, const Type& rhs)
        : lhs_(&lhs), rhs_(&rhs) {}

    public: const Type& lhs() const { return *lhs_; }
    public: const Type& rhs() const { return *rhs_; }
  };

  private: typedef HashSet_<const TypeParam*> TypeParamSet;
  private: typedef HashSet_<const TypeVar*> TypeVarSet;

  private: class Bindings {
    private: Stack_<Binding> binding_stack_;
    private: TypeParamSet typaram_set_;
    private: TypeVarSet tyvar_set_;

    public: Bindings() {}
    public: const TypeParamSet& typaram_set() const { return typaram_set_; }
    public: const TypeVarSet& tyvar_set() const { return tyvar_set_; }
    public: void Bind(const Type&, const Type&);
    public: bool Equiv(const Type&, const Type&);
    public: const Type* Eval(const Type&) const;
    public: void Restore(int);
    public: int Save() const;

    DISALLOW_COPY_AND_ASSIGN(Bindings);
  };

  public: class EnumTypeParam : public TypeParamSet::Enum {
    private: typedef TypeParamSet::Enum Base;
    private: const Bindings& bindings_;
    public: EnumTypeParam(const TypeUnifier&);
    public: const TypeParam& Get() const;
    public: const Type& GetBoundType() const;
    DISALLOW_COPY_AND_ASSIGN(EnumTypeParam);
  };

  public: class EnumTypeVar : public TypeVarSet::Enum {
    private: typedef TypeVarSet::Enum Base;
    private: const Bindings& bindings_;
    public: EnumTypeVar(const TypeUnifier&);
    public: const TypeVar& Get() const;
    public: const Type& GetBoundType() const;
    DISALLOW_COPY_AND_ASSIGN(EnumTypeVar);
  };

  private: Bindings bindings_;
  private: const SourceInfo source_info_;

  // ctor
  public: TypeUnifier(CompileSession&, const SourceInfo&);

  // [A]
  private: bool AddEquiv(const Type&, const Type&);

  // [B]
  private: bool Bind(const Type&, const Type&);

  // [U]
  public: bool Unify(const Type&, const Type&);
  private: bool UnifyClasses(const Class&, const Class&);

  private: bool UnifyConstructedClasses(
    const ConstructedClass&, const ConstructedClass&);

  private: bool UnifyConstructedClassAndGenericClass(
    const ConstructedClass&, const GenericClass&);

  private: bool UnifyGenericClassAndConstructedClass(
    const GenericClass&, const ConstructedClass&);

  private: bool UnifyTypes(const Type&, const Type&);

  DISALLOW_COPY_AND_ASSIGN(TypeUnifier);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_TypeUnifier_h)
