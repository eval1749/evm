// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Ir_UsingNamespace_h)
#define INCLUDE_Evita_Compiler_Ir_UsingNamespace_h

#include "./Entity.h"

namespace Compiler {

// Class UsingNamespace represent using namespace directive.
// using-name-directive ::= 'using' namespace-name ';'
class UsingNamespace
    : public DoubleLinkedItem_<UsingNamespace, NamespaceBody>,
      public Entity_<UsingNamespace> {

  CASTABLE_CLASS(UsingNamespace);

  public: typedef DoubleLinkedList_<UsingNamespace, NamespaceBody> List;

  private: Namespace* namespace_;
  private: NamespaceBody* namespace_body_;
  private: const NameRef& name_ref_;

  // ctor
  public: UsingNamespace(NamespaceBody*, const NameRef&);

  // properties
  public: Namespace* namezpace() const {
    ASSERT(IsRealized());
    return namespace_;
  }

  public: NamespaceBody* namespace_body() const { return namespace_body_; }
  public: const NameRef& name_ref() const { return name_ref_; }

  // [C]
  public: int ComputeHashCode() const {
    return reinterpret_cast<int>(this);
  }

  // [I]
  public: bool IsRealized() const { return namespace_ != nullptr; }

  // [R]
  public: void Realize(Namespace* const ns) {
    ASSERT(ns != nullptr);
    ASSERT(namespace_ == nullptr);
    namespace_ = ns;
  }

  DISALLOW_COPY_AND_ASSIGN(UsingNamespace);
}; // UsingNamespace

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_Ir_UsingNamespace_h)
