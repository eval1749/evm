// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_NamespaceMember_h)
#define INCLUDE_Compiler_Ir_NamespaceMember_h

#include "./Entity.h"

namespace Compiler {

class NamespaceBody;

// This class is abstract class of namespace member. Derived classes
// represent namespace declaration and type declaration.
class NamespaceMember
    : public DoubleLinkedItem_<NamespaceMember, NamespaceBody>,
      public Entity_<NamespaceMember> {
  CASTABLE_CLASS(NamespaceMember);

  // List of NamespaceMember.
  public: typedef DoubleLinkedList_<NamespaceMember, NamespaceBody> List;

  // ctor
  protected: NamespaceMember(const SourceInfo*);

  DISALLOW_COPY_AND_ASSIGN(NamespaceMember);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NamespaceMember_h)
