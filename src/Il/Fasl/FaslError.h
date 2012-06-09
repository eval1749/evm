// @(#)$Id$
//
// Evita Il - FastError
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_FastError_h)
#define INCLUDE_Il_Fasl_FastError_h

namespace Il {
namespace Fasl {


enum FaslError {
    FaslError_None,

    #define DEFERROR(mp_cat, mp_name) FaslError_ ## mp_cat ## _ ## mp_name,
    #include "./FaslError.inc"
}; // FaslError

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_FastError_h)
