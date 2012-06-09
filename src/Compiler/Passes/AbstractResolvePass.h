// @(#)$Id$
//
// Evita Compiler - AbstractResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Passes_AbstractResolvePass_h)
#define INCLUDE_Compiler_Passes_AbstractResolvePass_h

#include "../../Common/Collections.h"

#include "../CompilePass.h"
#include "../VisitFunctor.h"

#include "../Ir/ClassOrNamespaceDef.h"

namespace Compiler {

class AbstractResolvePass
    : public CompilePass,
      public LocalMemoryZone,
      public VisitFunctor {

  protected: typedef VisitFunctor Base;

  protected: struct FindResult {
    const Operand* operand_;
    bool ready_;

    explicit FindResult(Operand& r)
        : operand_(&r), ready_(true) {}

    explicit FindResult(bool ready = true)
        : operand_(nullptr), ready_(ready) {}
  };

  protected: enum LookupMode {
    IncludeImmediateNamespaceBody,
    ExcludeImmediateNamespaceBody,
  };

  protected: enum ResolveMode {
    AllowUnresolved,
    DoNotAllowUnresolved,
  };

  protected: class ResolveContext {
    private: const LookupMode lookup_mode_;
    private: const NamespaceBody& namespace_body_;
    private: ResolveMode const resolve_mode_;
    private: const Class* const enclosing_class_;

    public: ResolveContext(LookupMode, const NamespaceBody&);
    public: ResolveContext(const NamespaceBody&, const Class&);
    public: ResolveContext(const NamespaceBody&, const Class*, ResolveMode);

    public: const NamespaceBody& namespace_body() const {
      return namespace_body_;
    }

    public: bool ShouldInclude(const NamespaceBody& namespace_body) const {
      return lookup_mode_ == IncludeImmediateNamespaceBody
          || &namespace_body_ != &namespace_body;
    }

    public: void Resolve(
        const Name&,
        ArrayList_<const Operand*>&) const;

    public: bool IsAllowUnresolved() const {
      return resolve_mode_ == AllowUnresolved;
    }

    DISALLOW_COPY_AND_ASSIGN(ResolveContext);
  };

  // ctor
  protected: AbstractResolvePass(const char16*, CompileSession*);

  // [E]
  private: const Class* ExpectClass(
      const ResolveContext&,
      const NameRef::Item&,
      const Operand&);

  protected: Namespace* ExpectNamespace(
      const NameRef::Item&,
      const Operand&);

  private: const Operand* ExpectNamespaceOrType(
      const ResolveContext&,
      const NameRef::Item&,
      const Operand&);

  private: const Type* ExpectType(
      const ResolveContext&,
      const NameRef::Item&,
      const Operand&);

  private: const Type* ExpectType(
      const NameRef::Item&,
      const Operand&);

  // [F]
  protected: FindResult FindMostOuter(
      const ResolveContext&,
      const Name&,
      const SourceInfo&);

  // [L]
  protected: const Operand* LookupNamespaceOrType(
      const ResolveContext&,
      const NameRef&);

  // Note: If we define Process method here, derived classes can't access
  // Process method in VisitorFunctor class.

  // [R]
  protected: const Type& Resolve(const ResolveContext&, const Type&);

  private: const Type& ResolveUnresolvedType(
      const ResolveContext&,
      const UnresolvedType&);

  // [S]
  protected: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(AbstractResolvePass);
}; // AbstractResolvePass

} // Compiler

#endif // !defined(INCLUDE_Compiler_Passes_AbstractResolvePass_h)
