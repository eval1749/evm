// @(#)$Id$
//
// Evita Common BigInteger
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_BigInteger_h)
#define INCLUDE_Common_BigInteger_h

#include "./BigIntegerImpl.h"

#include "../Object.h"

namespace Common {
namespace Numerics {

class BigInteger : public WithCastable_<BigInteger, Object> {
  CASTABLE_CLASS(BigInteger);

  // units_ contains 2's complement integer in little endian order.
  // least signficant unit is at 0 and most signficant unit at end of
  // Array. Most signficant bit of most signficant unit denotes sign of
  // BigInteger.
  private: BigIntegerImpl value_;

  public: BigInteger(const BigInteger&);
  public: BigInteger(BigInteger&&);
  public: BigInteger(int32);
  public: BigInteger(int64);
  public: BigInteger(float32);
  public: BigInteger(float64);
  public: BigInteger(uint32);
  public: BigInteger(uint64);
  public: explicit BigInteger(const Vector_<uint32>&);
  private: BigInteger(const BigIntegerImpl&);
  public: virtual ~BigInteger() {}

  // operators
  public: BigInteger& operator =(const BigInteger&);
  public: BigInteger& operator =(BigInteger&&);

  public: operator bool() const;
  public: operator float32() const;
  public: operator float64() const;
  public: operator int32() const;
  public: operator int64() const;
  public: operator uint32() const;
  public: operator uint64() const;

  public: BigInteger operator -() const;
  public: BigInteger operator ~() const;

  public: bool operator !() const;

  #define DEFINE_BINARY_OPERATOR(op) \
    public: BigInteger operator op(const BigInteger& rhs) const { \
      return BigInteger(value_ op rhs.value_); \
    } \
    public: BigInteger& operator op ## =(const BigInteger& rhs) { \
      return *this = operator op(rhs); \
    } \
    public: BigInteger operator op(float32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: BigInteger operator op(float64 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: BigInteger operator op(int32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: BigInteger operator op(int64 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: BigInteger operator op(uint32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: BigInteger operator op(uint64 rhs) const { \
      return operator op(BigInteger(rhs)); \
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

  public: BigInteger operator <<(int) const;
  public: BigInteger& operator <<=(int);
  public: BigInteger operator >>(int) const;
  public: BigInteger& operator >>=(int);

  #define DEFINE_RELATIONAL_OPERATOR(op) \
    public: bool operator op(const BigInteger& rhs) const { \
      return value_ op rhs.value_; \
    } \
    public: bool operator op(float32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: bool operator op(float64 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: bool operator op(int32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: bool operator op(int64 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: bool operator op(uint32 rhs) const { \
      return operator op(BigInteger(rhs)); \
    } \
    public: bool operator op(uint64 rhs) const { \
      return operator op(BigInteger(rhs)); \
    }

  DEFINE_RELATIONAL_OPERATOR(==)
  DEFINE_RELATIONAL_OPERATOR(!=)
  DEFINE_RELATIONAL_OPERATOR(<)
  DEFINE_RELATIONAL_OPERATOR(<=)
  DEFINE_RELATIONAL_OPERATOR(>)
  DEFINE_RELATIONAL_OPERATOR(>=)
  #undef DEFINE_RELATIONAL_OPERATOR

  // [P]
  public: BigInteger Pow(int) const;

  // [T]
  public: virtual String ToString() const override;
};

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_BigInteger_h)
