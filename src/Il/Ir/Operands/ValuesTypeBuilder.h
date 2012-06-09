// @(#)$Id$
//
// Evita IL - IR - ValuesType
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ValuesTypeBuilder_h)
#define INCLUDE_Il_Ir_ValuesTypeBuilder_h

#include "../../../Common/Collections/ArrayList_.h"
#include "./Type.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

class ValuesTypeBuilder {

  public: class Enum : public Type::List::Enum {
    private: typedef Type::List::Enum Base;
    public: Enum(const ValuesTypeBuilder& r) : Base(r.type_list_) {}
    public: Enum(const ValuesTypeBuilder* p) : Base(p->type_list_) {}
    public: void Set(const Type& ty) { Base::Set(&ty); }
    DISALLOW_COPY_AND_ASSIGN(Enum);
  }; // Enum

  private: Type::List type_list_;

  // ctor
  public: explicit ValuesTypeBuilder(int const iCapacity = 0);
  public: explicit ValuesTypeBuilder(const Type::Collection&);
  public: explicit ValuesTypeBuilder(const ValuesType&);

  // [A]
  public: void Append(const Type&);

  // [C]
  public: void Clear();
  public: int Count() const;

  // [G]
  public: const Type& Get(int const iIndex) const;
  public: const ValuesType& GetValuesType() const;

  // [S]
  public: void Set(int, const Type&);

  DISALLOW_COPY_AND_ASSIGN(ValuesTypeBuilder);
}; // ValuesTypeBuilder

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ValuesTypeBuilder_h)
