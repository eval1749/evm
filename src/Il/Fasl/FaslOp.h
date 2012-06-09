// @(#)$Id$
//
// Evita Il - Fasl - FaslOp
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FaslOp_h)
#define INCLUDE_Il_Fasl_FaslOp_h

namespace Il {
namespace Fasl {

enum FaslOp {
    #define DEFOP(mp_name) FaslOp_ ## mp_name,
    #include "./FaslOp.inc"
    FaslOp_Limit,
    FaslOp_Invalid = -1,
    FaslOp_NumBits = 5,
}; // FaslOp

static_assert(FaslOp_Zero == 0, "FaslOp_Zero must be 0.");

// Name64(11 0x0B)
//  E = 15 0x0F 0000 1111
//  v = 58 0x2A 0010 1010
//  O = 25 0x19 0001 1001
//
//  93 97 
const char* const FileMagic = "EvO";

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FaslOp_h)
