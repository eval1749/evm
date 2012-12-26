#include "precomp.h"
// @(#)$Id$
//
// Evita Common - CommonInit
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./BigInteger.h"

#include "../String.h"

namespace Common {
namespace Numerics {

// ctor
BigInteger::BigInteger(const BigInteger& rhs) : value_(rhs.value_) {}
BigInteger::BigInteger(BigInteger&& rhs) : value_(rhs.value_) {}
BigInteger::BigInteger(const Vector_<uint32>& v) : value_(v) {}
BigInteger::BigInteger(const BigIntegerImpl& value) : value_(value) {}

#define DEFINE_TYPE_CONVERTER(type) \
  BigInteger::BigInteger(type value) : value_(value) {} \
  BigInteger::operator type() const { return value_; }

DEFINE_TYPE_CONVERTER(float32)
DEFINE_TYPE_CONVERTER(float64)
DEFINE_TYPE_CONVERTER(int32)
DEFINE_TYPE_CONVERTER(int64)
DEFINE_TYPE_CONVERTER(uint32)
DEFINE_TYPE_CONVERTER(uint64)

// operators
BigInteger& BigInteger::operator =(const BigInteger& rhs) {
  value_ = rhs.value_;
  return *this;
}

BigInteger& BigInteger::operator =(BigInteger&& rhs) {
  value_ = rhs.value_;
  return *this;
}

BigInteger::operator bool() const { return value_; }
bool BigInteger::operator !() const { return !value_; }

BigInteger BigInteger::operator -() const { return BigInteger(-value_); }
BigInteger BigInteger::operator ~() const { return BigInteger(~value_); }

BigInteger BigInteger::operator <<(int const shift_amount) const {
  return BigInteger(value_ << shift_amount);
}

BigInteger& BigInteger::operator <<=(int const shift_amount) { 
  return *this = operator <<(shift_amount);
}

BigInteger BigInteger::operator >>(int const shift_amount) const {
  return BigInteger(value_ >> shift_amount);
}

BigInteger& BigInteger::operator >>=(int const shift_amount) { 
  return *this = operator >>(shift_amount);
}

// [P]
BigInteger BigInteger::Pow(int k) const { return value_.Pow(k); }

// [T]
String BigInteger::ToString() const { return value_.ToString(); }

} // Numerics
} // Common
