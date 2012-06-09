#include "precomp.h"
// @(#)$Id$
//
// Evita Om
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MemoryManager.h"
#include "./Object.h"
#include "./Thread.h"

namespace Om {

void StaticObjectInit() {
  #define DEFARRAY(mp_name, mp_elemty) { \
    DEFCLASS(mp_name, ArrayType, ArrayType) \
    auto& clazz = *Ty_ ## mp_name; \
    clazz.rank_ = 1; \
  }

  #define DEFCLASS(mp_name, mp_base, mp_meta) { \
      auto& clazz = *Ty_ ## mp_name; \
      clazz.type_ = Ty_ ## mp_meta; \
      clazz.base_type_ = Ty_ ## mp_base; \
      clazz.align_ = Arch::Align_Object; \
      clazz.element_size_ = sizeof(Om::Layout::mp_name); \
      clazz.fixed_size_ = clazz.element_size_; \
      clazz.rank_ = 0; \
      DEBUG_PRINTF("%p %hs size=%d\n", \
          clazz, \
          # mp_name, \
          clazz.fixed_size_); \
  } // DEFCLASS

  #define DEFPRIMITIVE(mp_name, mp_ctype) { \
    DEFCLASS(mp_name, Value, PrimitiveType); \
    auto& clazz = *Ty_ ## mp_name; \
    clazz.element_size_ = sizeof(mp_ctype); \
    clazz.fixed_size_ = sizeof(Om::Layout::Value) + sizeof(mp_ctype); \
  }

  #include "./StaticClass.inc"

  Ty_String->element_type_ = Ty_Char;

  ASSERT(Ty_String->ComputeSize() == sizeof(String));
  ASSERT(Ty_CharVector->ComputeSize(1) > sizeof(Vector) + sizeof(void*));
}

} // Om
