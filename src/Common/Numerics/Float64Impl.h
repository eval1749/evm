// @(#)$Id$
//
// Evita Common Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Numerics_Float64Impl_h)
#define INCLUDE_Common_Numerics_Float64Impl_h

#include "./IeeeClass.h"

namespace Common {
namespace Numerics {

//  Exponent    Significand
//  -------------------------------------------------------
//  2047        0000 0000 ... 0000   Infinity
//  2047        1uuu uuuu ... uuuu   NaN        bit51 == 1
//  2047        0uuu uuuu ... uuuu   SNaN       bit51 == 0
//     0        0000 0000 ... 0000   Zero
//     0        uuuu uuuu ... uuuu   Subnormal (f x 2^-1074)
//  others      uuuu uuuu ... uuuu   Normal    (2^52+f) x 2(be-1075)
//
//  Note: 1075 = ExponentBias(1023) + kSignificandBits(52)
//
//  76543210 76543210
//  SEEEEEEE EEEEssss
class Float64Impl {
  // sign(1) exponent(11) signficand(53)
  public: static int const kSignificandBits = 52;
  public: static uint64 const
      kSignificandMask = (uint64(1) << kSignificandBits) - 1;
  public: static int const kEbias = 1023;
  public: static int const kEmax= 2047;
  public: static int const kEsubnormal = -1074;
  public: static int const kPrecision = kSignificandBits + 1;
  public: static int const kDigits = 16;

  public: static uint64 const kMostPositivePattern = 0x7FEFFFFFFFFFFFFF;

  public: static uint64 const kNegativeInfinityPattern = uint64(0xFFF0) << 48;
  public: static uint64 const kPositiveInfinityPattern = uint64(0x7FF0) << 48;

  private: static uint64 const kNanMask = uint64(0xFFF00) < 48;

  public: static uint64 const kNegativeQuietNaNPattern = uint64(0xFFF8) << 48;
  public: static uint64 const kPositiveQuietNaNPattern = uint64(0x7FF8) << 48;

  public: static uint64 const kNegativeSignalingNaNPattern =
      uint64(0xFFF4) << 48;
  public: static uint64 const kPositiveSignalingNaNPattern =
      uint64(0x7FF4) << 48;

  public: static uint64 const kNegativeZeroPattern = uint64(0x8000) << 48;
  public: static uint64 const kPositiveZeroPattern = 0;

  private: union { float64 f64_; uint64 u64_; } fu_;

  public: explicit Float64Impl(float64 f64) {
    fu_.f64_ = f64;
  }

  public: explicit Float64Impl(uint64 u64) {
    fu_.u64_ = u64;
  }

  // [T]
  public: operator float64() const { return fu_.f64_; }
  public: operator uint64() const { return fu_.u64_; }

  // properties
  public: int exponent() const {
    return raw_exponent() - kEbias;
  }

  public: IeeeClass ieee_class() const {
    switch (raw_exponent()) {
      case kEmax: {
        auto const signficand52 = raw_significand();
        if (!signficand52) {
          return Class_Infinity;
        }

        auto const high4 = uint32(signficand52 >> 48) & 0x0F;
        return high4 > 4 ? Class_QuietNaN : Class_SignalingNaN;
      }

      case 0:
        return !fu_.u64_ || !(fu_.u64_ << 1) ? Class_Zero : Class_Subnormal;

      default:
        return Class_Normal;
    }
  }

  public: int raw_exponent() const {
    return uint32(fu_.u64_ >> kSignificandBits) & 0x7FF;
  }

  public: uint64 raw_significand() const {
    return fu_.u64_ & kSignificandMask;
  }

  public: uint64 significand() const {
    return raw_significand() | (uint64(1) << kSignificandBits);
  }

  public: int sign() const {
    // To avoid comparing with signaling NaN, use sign bit.
    return int64(fu_.u64_) < 0;
  }
};

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_Numerics_Float64Impl_h)
