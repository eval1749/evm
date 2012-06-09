// @(#)$Id$
//
// Evita Common Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Decimal128_h)
#define INCLUDE_Common_Decimal128_h

#include "../Object.h"

#include "./Decimal128Impl.h"

namespace Common {
namespace Numerics {

class Decimal128 : public WithCastable_<Decimal128, Object> {
  CASTABLE_CLASS(Decimal128);

  private: Decimal128Impl value_;

  private: Decimal128(const Decimal128Impl&);
  public: Decimal128(const Decimal128&);
  public: Decimal128(int32);
  public: Decimal128(int64);
  public: Decimal128(float64);
  public: Decimal128(uint32);
  public: Decimal128(uint64);

  // operators
  public: operator float64() const;
  public: Decimal128& operator =(const Decimal128&);

  #define DEFINE_BINARY_OPERATOR(op) \
    public: Decimal128 operator op(const Decimal128&) const; \
    public: Decimal128& operator op ## =(const Decimal128&); \
    public: Decimal128 operator op(float32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: Decimal128 operator op(float64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: Decimal128 operator op(int32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: Decimal128 operator op(int64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: Decimal128 operator op(uint32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: Decimal128 operator op(uint64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    }

  DEFINE_BINARY_OPERATOR(+)
  DEFINE_BINARY_OPERATOR(-)
  DEFINE_BINARY_OPERATOR(*)
  DEFINE_BINARY_OPERATOR(/)
  #undef DEFINE_BINARY_OPERATOR

  #define DEFINE_RELATIONAL_OPERATOR(op) \
    public: bool operator op(const Decimal128&) const; \
    public: bool operator op(float32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: bool operator op(float64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: bool operator op(int32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: bool operator op(int64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: bool operator op(uint32 rhs) const { \
      return operator op(Decimal128(rhs)); \
    } \
    public: bool operator op(uint64 rhs) const { \
      return operator op(Decimal128(rhs)); \
    }

  DEFINE_RELATIONAL_OPERATOR(==)
  DEFINE_RELATIONAL_OPERATOR(!=)
  DEFINE_RELATIONAL_OPERATOR(<)
  DEFINE_RELATIONAL_OPERATOR(<=)
  DEFINE_RELATIONAL_OPERATOR(>)
  DEFINE_RELATIONAL_OPERATOR(>=)
  #undef DEFINE_RELATIONAL_OPERATOR

  // properties
  public: const Decimal128Impl& value() const { return value_; }

  // [F]
  public: static Decimal128 LibExport FromString(const String&);

  // [R]
  public: Decimal128 Remainder(const Decimal128&) const;

  // [T]
  public: virtual String ToString() const override;
  public: Decimal128 Truncate(const Decimal128&) const;
};

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_Decimal128_h)
