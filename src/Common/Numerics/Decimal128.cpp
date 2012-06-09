#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Decimal128.h"

#include "../Box.h"
#include "../DebugHelper.h"
#include "../StringBuilder.h"

namespace Common {
namespace Numerics {

// ctor
Decimal128::Decimal128(int32 const i32) : value_(i32) {}
Decimal128::Decimal128(int64 const i64) : value_(i64) {}
Decimal128::Decimal128(float64 const f64) : value_(f64) {}
Decimal128::Decimal128(uint32 const u32) : value_(u32) {}
Decimal128::Decimal128(uint64 const u64) : value_(u64) {}
Decimal128::Decimal128(const Decimal128& rhs) : value_(rhs.value_) {}
Decimal128::Decimal128(const Decimal128Impl& value) : value_(value) {}

// operators
Decimal128::operator float64() const {
  return float64(value_);
}

Decimal128& Decimal128::operator =(const Decimal128& rhs) {
  value_ = rhs.value_;
  return *this;
}

#define DEFINE_RELATION_OPERATOR(op) \
  bool Decimal128::operator op (const Decimal128& rhs) const { \
    return value_ op rhs.value_; \
  }

DEFINE_RELATION_OPERATOR(==)
DEFINE_RELATION_OPERATOR(!=)
DEFINE_RELATION_OPERATOR(<)
DEFINE_RELATION_OPERATOR(<=)
DEFINE_RELATION_OPERATOR(>)
DEFINE_RELATION_OPERATOR(>=)

#define DEFINE_BINARY_OPERATOR(op) \
  Decimal128 Decimal128::operator op(const Decimal128& rhs) const { \
    return Decimal128(value_ op rhs.value_); \
  } \
  Decimal128& Decimal128::operator op ##=( \
      const Decimal128& rhs) { \
    return *this = operator op(Decimal128(rhs)); \
  }

DEFINE_BINARY_OPERATOR(+)
DEFINE_BINARY_OPERATOR(-)
DEFINE_BINARY_OPERATOR(*)
DEFINE_BINARY_OPERATOR(/)

// [F]
Decimal128 LibExport Decimal128::FromString(const String& str) {
  return Decimal128(Decimal128Impl::FromString(str));
}

//[R]
Decimal128 Decimal128::Remainder(const Decimal128& that) const {
  return Decimal128(value_.Remainder(that.value_));
}

// [T]
String Decimal128::ToString() const {
  return value_.ToString();
}

Decimal128 Decimal128::Truncate(const Decimal128& that) const {
  return Decimal128(value_.Divide(that.value_, IntegerResult));
}

} // Numerics
} // Common
