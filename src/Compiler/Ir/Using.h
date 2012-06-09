// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Ir_Using_h)
#define INCLUDE_Evita_Compiler_Ir_Using_h

#include "./Entity.h"

namespace Compiler {

class Using
    : public DoubleLinkedItem_<Using, NamespaceBody>,
      public Entity_<Using> {

  CASTABLE_CLASS(Using);

  public: typedef DoubleLinkedList_<Using, NamespaceBody> List;

  private: ClassOrNamespace* class_or_namespace_;
  private: NameRef* const name_ref_;

  // ctor
  public: Using(NameRef*);

  // properties
  public: ClassOrNamespace* class_or_namespace() const {
    return class_or_namespace_;
  }

  public: NameRef* name_ref() const { return name_ref_; }

  // [R]
  public: void Realize(ClassOrNamespace* const p) {
    ASSERT(p != nullptr);
    ASSERT(class_or_namespace_ == nullptr);
    class_or_namespace_ = p;
  }

  DISALLOW_COPY_AND_ASSIGN(Using);
}; // Using

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_Ir_Using_h)
