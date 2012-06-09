// @(#)$Id$
//
// Evita Compiler - TypeArgList.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Compiler_Syntax_TypeArgList_h)
#define INCLUDE_evm_Compiler_Syntax_TypeArgList_h

#include "../Ir/Entity.h"

namespace Compiler {

/// <summary>
///   This class represents type argument list lexical token. This token
///   will be spread into NAMEREF instruction for resolving generic type
///   and generic method.
/// </summary>
class TypeArgList : public Entity_<TypeArgList> {
  CASTABLE_CLASS(TypeArgList);

  private: typedef Type::Types Types;

  public: class Enum : public Types::Enum {
    public: Enum(TypeArgList* const p) :
        Types::Enum(p->m_pTypes) {}
  }; // Enum

  private: Types m_pTypes;

  // ctor
  public: TypeArgList(const Type::Collection& types)
    : m_pTypes(types) {
  }

  DISALLOW_COPY_AND_ASSIGN(TypeArgList);
}; // TypeArgList

} // Compiler

#endif // !defined(INCLUDE_evm_Compiler_Syntax_TypeArgList_h)
