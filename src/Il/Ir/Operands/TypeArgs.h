// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_TypeArgs_h)
#define INCLUDE_Il_Ir_TypeArgs_h

#include "./Type.h"

#include "../../../Common/Collections.h"

namespace Il {
namespace Ir {

using namespace Common::Collections;

class TypeArgs {
  private: typedef HashMap_<HashKey_<const TypeParam>, const Type*>
    BindingMap;

  public: class Enum : public BindingMap::Enum {
    private: typedef BindingMap::Enum Base;
    public: Enum(const TypeArgs& r) : Base(r.binding_map_) {}
    public: Enum(const TypeArgs* const p) : Base(p->binding_map_) {}

    public: const TypeParam& GetTypeParam() const {
      return *Get().GetKey();
    }

    public: const Type& GetTypeArg() const {
      return *Get().GetValue();
    }
  };

  public: class MapKey {
    private: const TypeArgs* type_args_;

    public: MapKey(const TypeArgs* const pTypeArgs = nullptr)
        : type_args_(pTypeArgs) {}

    public: bool operator==(const MapKey& r) { return Equals(r); }

    public: int ComputeHashCode() const {
      return type_args_->ComputeHashCode();
    }

    public: bool Equals(const MapKey& rThat) const {
      return type_args_->HashEquals(rThat.type_args_);
    }
  };

  private: BindingMap binding_map_;

  public: TypeArgs(const TypeArgs&);
  public: TypeArgs(const TypeParam&, const Type&);
  public: TypeArgs();

  public: TypeArgs& operator=(const TypeArgs&);

  // [A]
  public: void Add(const TypeParam&, const Type&);

  // [C]
  public: void Clear();
  public: int ComputeHashCode() const;
  public: int Count() const { return binding_map_.Count(); }

  // [G]
  public: const Type* Get(const TypeParam&) const;

  // [H]
  public: bool HashEquals(const TypeArgs*) const;
};

inline int ComputeHashCode(const TypeArgs::MapKey& r) {
  return r.ComputeHashCode();
}

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_TypeArgs_h)
