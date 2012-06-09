// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_VmError_h)
#define INCLUDE_Il_VmError_h

namespace Il {

enum VmError {
    VmError_None = 0,
    VmError_Base = 0,

    #define DEFERROR(mp_cat, mp_name) \
        VmError_ ## mp_cat ## _ ## mp_name,

    #include "./VmError.inc"

    VmError_Max_1,
}; // VmError

} // Il

#endif // !defined(INCLUDE_Il_VmError_h)
