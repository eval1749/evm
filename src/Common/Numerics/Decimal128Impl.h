// @(#)$Id$
//
// Evita Common Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Decimal128Impl_h)
#define INCLUDE_Common_Decimal128Impl_h

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

// This class implements IEEE754 decimal128 operations.
//
// Encoded Format:
// Combination Type     Exponent    Coefficent Exponent
// Field(5bit)          MSB(2bit)   MSD(4bit)  Cont(10bit)
//  a b c d e  Finite   a b         0 c d e    -
//  1 1 c d e  Finite   c d         1 0 0 e    -
//  1 1 1 1 0  Infinite - -         - - - -    -
//  1 1 1 1 1  NaN      - -         - - - -    0
//  1 1 1 1 1  sNaN     - -         - - - -    1

// 76543210 76543210 76543210 76543210 ...
// SCCCCCEE EEEEEEEE EEcccccc cccccccc ...
//  S = sign (1 bit), 0 = plus, 1 = minus
//  C = combination field (5 bit)
//  E = exponent continuation (12 bit)
//  c = coefficent (110 bit)
//
//  Mask of combination field       = 7C00 0000
//  Mask of exponent continuation   = 03FF C000
//  Mask of coefficent continuation = 0000 3FFF
class Decimal128Impl {
  public: static int const kElimit = 12287; // maximum encoded exponent
  public: static int const kEmax = 6144; // largest exponent value
  public: static int const kEmin = -6143; // smallest exponent value
  public: static int const kEbias = 6176; // subtracted from encoded exponent
  public: static int const kEbig = kElimit - kEbias;
  public: static int const kEtiny = -kEbias;

  public: static int const kDigits = 34;
  public: static int const kPrecision = 34;

  public: static int const kSignBits = 1;
  public: static int const kCombinationFieldBits = 5;
  public: static int const kExponentContBits = 12;
  public: static int const kCoefficentContBits = 110;

  public: struct EncodedData {
    int coefficent_msd() const;
    int combination_field() const;
    int dpd_at(int) const;
    int exponent_biased() const;
    int exponent_continuation() const;
    int exponent_msb() const;
    bool is_finite() const;
    bool is_negative() const { return !!sign(); }
    bool is_positive() const { return !sign(); }
    bool is_special() const;
    bool is_zero() const;
    int sign() const;
    void set_combination_field(int);
    void set_combination_field(int exponent, int coefficent_msd);
    void set_dpd_at(int const dpd_index, int const dpd);
    void set_exponent(int);
    void set_exponent_continuation(int);
    void set_sign(int);
    uint32 words_[4];
  };

  private: EncodedData data_;

  // ctor
  public: Decimal128Impl();
  public: Decimal128Impl(float64);
  public: Decimal128Impl(int32);
  public: Decimal128Impl(int64);
  public: Decimal128Impl(uint32);
  public: Decimal128Impl(uint64);
  public: Decimal128Impl(const Decimal128Impl&);
  public: explicit Decimal128Impl(const EncodedData&);
  private: Decimal128Impl(int, uint32, uint32);
  private: Decimal128Impl(int, uint64);

  // operators
  public: operator float64() const;
  public: Decimal128Impl& operator =(const Decimal128Impl&);

  #define DEFINE_BINARY_OPERATOR(op) \
    public: Decimal128Impl operator op(const Decimal128Impl&) const; \
    public: Decimal128Impl& operator op ## =(const Decimal128Impl& rhs) { \
      return *this = operator op(rhs); \
    } \
    public: Decimal128Impl operator op(float32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: Decimal128Impl operator op(float64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: Decimal128Impl operator op(int32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: Decimal128Impl operator op(int64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: Decimal128Impl operator op(uint32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: Decimal128Impl operator op(uint64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    }

  DEFINE_BINARY_OPERATOR(+)
  DEFINE_BINARY_OPERATOR(-)
  DEFINE_BINARY_OPERATOR(*)
  DEFINE_BINARY_OPERATOR(/)
  #undef DEFINE_BINARY_OPERATOR

  #define DEFINE_RELATIONAL_OPERATOR(op) \
    public: bool operator op(const Decimal128Impl&) const; \
    public: bool operator op(float32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: bool operator op(float64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: bool operator op(int32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: bool operator op(int64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: bool operator op(uint32 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    } \
    public: bool operator op(uint64 rhs) const { \
      return operator op(Decimal128Impl(rhs)); \
    }

  DEFINE_RELATIONAL_OPERATOR(==)
  DEFINE_RELATIONAL_OPERATOR(!=)
  DEFINE_RELATIONAL_OPERATOR(<)
  DEFINE_RELATIONAL_OPERATOR(<=)
  DEFINE_RELATIONAL_OPERATOR(>)
  DEFINE_RELATIONAL_OPERATOR(>=)
  #undef DEFINE_RELATIONAL_OPERATOR

  // properties
  public: int exponent() const;
  public: IeeeClass ieee_class() const;
  public: bool is_finite() const { return data_.is_finite(); }
  public: bool is_negative() const { return data_.is_negative(); }
  public: bool is_positive() const { return data_.is_positive(); }
  public: bool is_special() const { return data_.is_special(); }
  public: bool is_zero() const { return data_.is_zero(); }
  public: int sign() const;
  public: const EncodedData& value() const { return data_; }
  public: void set_sign(int);

  // [C]
  private: Decimal128Impl CompareTo(const Decimal128Impl&) const;

  // [D]
  public: Decimal128Impl Divide(const Decimal128Impl&, DivideResult) const;

  // [F]
  private: bool FastEquals(const Decimal128Impl&) const;

  public: static Decimal128Impl LibExport FromString(const String&);

  // [I]
  public: static Decimal128Impl LibExport Infinity(int);

  // [Q]
  public: static Decimal128Impl LibExport QuietNaN(int);

  // [R]
  public: Decimal128Impl Remainder(const Decimal128Impl&) const;
  private: Decimal128Impl RoundAt(int);

  // [S]
  public: static Decimal128Impl LibExport SignalingNaN(int);

  // [T]
  public: String ToString() const;

  // [Z]
  public: static Decimal128Impl LibExport Zero(int);
};

static_assert(
    sizeof(Decimal128Impl) == 16,
    "sizeof(Decimal128Impl) == 16");

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_Decimal128Impl_h)
