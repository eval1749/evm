// @(#)$Id$
//
// Evita Common Decimal64
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Decimal64Impl_h)
#define INCLUDE_Common_Decimal64Impl_h

#include "./IeeeClass.h"

namespace Common {
class String;
} // Common

namespace Common {
namespace Numerics {

enum DivideResult {
  FloatResult,
  IntegerResult,
};

// This class represents decimal base floating pointer number.
//
// Encoded Format:
// 76543210 76543210 76543210 76543210 ...
// SEEEEEEE EEEEcccc cccccccc cccccccc ...
//  S = sign (1 bit), 0 = plus, 1 = minus
//  E = exponent continuation (11 bit)
//  c = coefficent (116 bit)
class Decimal64Impl {
  public: static int const Emax = 2047; // largest exponent value
  public: static int const Ebias = 1023; // subtracted from encoded exponent

  public: static int const NumberOfDigits = 16;
  public: static int const Precision = 16;

  public: struct EncodedData {
    EncodedData(uint64);
    EncodedData(uint64, int, int);
    EncodedData(uint64, uint64, int, int);

    uint64 coefficent() const;
    int exponent() const;
    bool is_finite() const;
    bool is_special() const;
    bool is_zero() const;
    int raw_exponent() const;
    int sign() const;

    void set_sign(int);

    int CountDigits() const;
    uint64 u64_;
  };

  private: EncodedData data_;

  // ctor
  public: Decimal64Impl(int32);
  public: Decimal64Impl(float64);
  public: Decimal64Impl(const Decimal64Impl&);
  public: explicit Decimal64Impl(const EncodedData&);
  public: Decimal64Impl(uint64, int, int);

  // operators
  public: operator float64() const;
  public: Decimal64Impl& operator =(const Decimal64Impl&);
  public: bool operator ==(const Decimal64Impl&) const;
  public: bool operator !=(const Decimal64Impl&) const;
  public: bool operator <(const Decimal64Impl&) const;
  public: bool operator <=(const Decimal64Impl&) const;
  public: bool operator >(const Decimal64Impl&) const;
  public: bool operator >=(const Decimal64Impl&) const;

  public: Decimal64Impl operator +(const Decimal64Impl&) const;
  public: Decimal64Impl operator -(const Decimal64Impl&) const;
  public: Decimal64Impl operator *(const Decimal64Impl&) const;
  public: Decimal64Impl operator /(const Decimal64Impl&) const;

  // properties
  public: int exponent() const;
  public: IeeeClass ieee_class() const;
  public: bool is_finite() const { return data_.is_finite(); }
  public: bool is_special() const { return data_.is_special(); }
  public: bool is_zero() const { return data_.is_zero(); }
  public: int sign() const;
  public: const EncodedData& value() const { return data_; }
  public: void set_sign(int);

  // [C]
  private: Decimal64Impl CompareTo(const Decimal64Impl&) const;

  // [D]
  public: Decimal64Impl Divide(const Decimal64Impl&, DivideResult) const;

  // [F]
  private: bool FastEquals(const Decimal64Impl&) const;

  public: static Decimal64Impl LibExport FromString(const String&);

  // [I]
  public: static Decimal64Impl LibExport Infinity(int);

  // [Q]
  public: static Decimal64Impl LibExport QuietNaN(int);

  // [R]
  public: Decimal64Impl Remainder(const Decimal64Impl&) const;
  private: Decimal64Impl RoundAt(int);

  // [T]
  public: String ToString() const;

  public: Decimal64Impl Truncate(const Decimal64Impl& divisor) const {
    return Divide(divisor, IntegerResult);
  }

  // [Z]
  public: static Decimal64Impl LibExport Zero(int);
};

static_assert(
    sizeof(Decimal64Impl) == 8,
    "sizeof(Decimal64Impl) == 8");

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_Decimal64Impl_h)
