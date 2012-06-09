// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_OmDefs_h)
#define INCLUDE_Om_OmDefs_h

namespace Om {

template<typename T> T Ceiling(T const x, T const n) {
  return (x + n - 1) / n;
}

template<typename T> T RoundUp(T const x, T const n) {
  return Ceiling(x, n) * n;
}

#define DEFCLASS(mp_name, mp_base, mp_meta) \
    class mp_name;

#include "./StaticClass.inc"

typedef intptr_t Val;
typedef Object Record;

class Thread;

void Init();

} // Om

#include "./Arch.h"

namespace Il {
namespace Ir {
class Type;
} // Ir
} // Il

#endif // !defined(INCLUDE_Om_OmDefs_h)
