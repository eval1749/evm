// @(#)$Id$
//
// Evita Common BigIntegerImpl
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Numerics_BigIntegerImpl_h)
#define INCLUDE_Common_Numerics_BigIntegerImpl_h

#include "../Pair_.h"
#include "../Collections/Vector_.h"

namespace Common {
namespace Numerics {

using Common::Pair_;
using Common::Collections::Vector_;

// Represents Bignum implementation.
//
//  Arbitrary  integer is stored as vector of an "unit" in two's complement.
//  An "unit" is 32-bit unsigned integer.
//
//  units_ contains bigits in least significant bigit first (little endian):
//      units_[0]      = least significant bigit
//      ...
//      units_[n-1]    = most significant bigit
//
//  A most signficant bigit is 32-bit integer instead of unsigned integer.
//
//  A units_[n-1] is 0 or -1 when
//       0      if units_[n-2] >= 0x8000_0000
//      -1      if units_[n-2] <  0x8000_0000
//
class BigIntegerImpl {
  public: typedef uint64 DoubleUnit;
  public: typedef int64 DoubleSignedUnit;
  public: typedef int32 SignedUnit;
  public: typedef uint32 Unit;
  public: static const int kUnitSize = sizeof(Unit) * 8;

  // units_ contains 2's complement integer in little endian order.
  // least signficant unit is at 0 and most signficant unit at end of
  // Array. Most signficant bit of most signficant unit denotes sign of
  // BigIntegerImpl.
  private: Vector_<Unit> units_;

  private: typedef Vector_<Unit>::Iterator Iterator;
  private: typedef Vector_<Unit>::ConstIterator ConstIterator;
  private: typedef Vector_<Unit>::ConstReverseIterator ConstReverseIterator;
  private: typedef Vector_<Unit>::ReverseIterator ReverseIterator;

  public: BigIntegerImpl(const BigIntegerImpl&);
  public: BigIntegerImpl(BigIntegerImpl&&);
  public: BigIntegerImpl(int32);
  public: BigIntegerImpl(int64);
  public: BigIntegerImpl(float32);
  public: BigIntegerImpl(float64);
  public: BigIntegerImpl(uint32);
  public: BigIntegerImpl(uint64);
  public: explicit BigIntegerImpl(const Vector_<uint32>&);
  public: explicit BigIntegerImpl(Vector_<uint32>&&);
  public: ~BigIntegerImpl() {}

  // operators
  public: BigIntegerImpl& operator=(const BigIntegerImpl&);
  public: BigIntegerImpl& operator=(BigIntegerImpl&&);

  public: operator bool() const;
  public: operator float32() const;
  public: operator float64() const;
  public: operator int32() const;
  public: operator int64() const;
  public: operator uint32() const;
  public: operator uint64() const;
  public: bool operator !() const;
  public: BigIntegerImpl operator -() const;
  public: BigIntegerImpl operator ~() const;

  #define DEFINE_BINARY_OPERATOR(op) \
    public: BigIntegerImpl operator op(const BigIntegerImpl&) const; \
    public: BigIntegerImpl& operator op ## =(const BigIntegerImpl& rhs) { \
      return *this = operator op(rhs); \
    } \
    public: BigIntegerImpl operator op(float32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: BigIntegerImpl operator op(float64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: BigIntegerImpl operator op(int32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: BigIntegerImpl operator op(int64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: BigIntegerImpl operator op(uint32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: BigIntegerImpl operator op(uint64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    }

  DEFINE_BINARY_OPERATOR(+)
  DEFINE_BINARY_OPERATOR(-)
  DEFINE_BINARY_OPERATOR(*)
  DEFINE_BINARY_OPERATOR(/)
  DEFINE_BINARY_OPERATOR(%)
  DEFINE_BINARY_OPERATOR(&)
  DEFINE_BINARY_OPERATOR(|)
  DEFINE_BINARY_OPERATOR(^)
  #undef DEFINE_BINARY_OPERATOR

  public: BigIntegerImpl operator <<(int) const;
  public: BigIntegerImpl operator >>(int) const;
  public: BigIntegerImpl& operator <<=(int);
  public: BigIntegerImpl& operator >>=(int);

  #define DEFINE_RELATIONAL_OPERATOR(op) \
    public: bool operator op(const BigIntegerImpl&) const; \
    public: bool operator op(float32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: bool operator op(float64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: bool operator op(int32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: bool operator op(int64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: bool operator op(uint32 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    } \
    public: bool operator op(uint64 rhs) const { \
      return operator op(BigIntegerImpl(rhs)); \
    }

  DEFINE_RELATIONAL_OPERATOR(==)
  DEFINE_RELATIONAL_OPERATOR(!=)
  DEFINE_RELATIONAL_OPERATOR(<)
  DEFINE_RELATIONAL_OPERATOR(<=)
  DEFINE_RELATIONAL_OPERATOR(>)
  DEFINE_RELATIONAL_OPERATOR(>=)
  #undef DEFINE_RELATIONAL_OPERATOR

  // properties
  public: ConstIterator begin() const { return units_.begin(); }
  public: int bit_length() const;
  public: ConstIterator end() const { return units_.end(); }
  public: ConstReverseIterator rbegin() const { return units_.rbegin(); }
  public: ConstReverseIterator rend() const { return units_.rend(); }
  public: const Vector_<Unit>& units() const { return units_; }
  public: int size() const { return units_.size(); }

  // [C]
  public: int CompareTo(const BigIntegerImpl&) const;

  // [I]
  public: bool IsMinus() const;
  public: bool IsMinusOne() const;
  public: bool IsOne() const;
  public: bool IsPlus() const;
  public: bool IsZero() const;

  // [P]
  public: BigIntegerImpl Pow(int) const;

  // [T]
  public: String ToString() const;
  public: Pair_<BigIntegerImpl> Truncate(const BigIntegerImpl&) const;
};

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_Numerics_BigIntegerImpl_h)
