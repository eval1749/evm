// @(#)$Id$
//
// Evita Common Decimal128
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_IeeeClass_h)
#define INCLUDE_Common_IeeeClass_h

namespace Common {
namespace Numerics {

enum IeeeClass {
  Class_Infinity,
  Class_Normal,
  Class_QuietNaN,
  Class_SignalingNaN,
  Class_Subnormal,
  Class_Zero,
};

} // Numerics
} // Common

#endif // !defined(INCLUDE_Common_IeeeClass_h)
