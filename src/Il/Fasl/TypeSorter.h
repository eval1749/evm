// @(#)$Id$
//
// Evita Il - TypeSorter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_TypeSorter_h)
#define INCLUDE_Il_Fasl_TypeSorter_h

#include "../Ir/Operands/Type.h"

namespace Il {
namespace Fasl {

using namespace Il::Ir;

// Toplogical sort types.
class TypeSorter {
  private: Type::Set pending_set_;
  private: Type::List result_list_;

  private: TypeSorter(const Type::Set&);

  // [I]
  private: bool IsReady(const Class&) const;
  private: bool IsReady(const Type&) const;

  // [M]
  private: void Map(Type::Queue&, Type::Queue&);

  // [S]
  private: Type::Array* Sort();
  public: static Type::Array* LibExport Sort(const Type::Set&);

  DISALLOW_COPY_AND_ASSIGN(TypeSorter);
};

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_TypeSorter_h)
