#include "precomp.h"
// @(#)$Id$
//
// Evita Om - Thread
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Thread.h"

#include "./Object.h"

namespace Om {

// ctor
Thread::Thread() {
    Reset();
}

// [A]
void* Thread::Alloc(size_t size) {
  ASSERT(size > 0);
  auto const alloc_size = RoundUp(size, Arch::Align_Object);
  for (;;) {
    if (auto const pv = object_area_->Alloc(alloc_size)) {
      return pv;
    }
    object_area_ = Mm::GetDataArea(Mm::Area::ScanType_Record, alloc_size);
  }
}

Vector& Thread::AllocVector(const VectorType& arrty, int const length) {
  auto const size = arrty.ComputeSize(length);
  auto& vector = *new(Alloc(size)) Vector(arrty, length);
  return vector;
}

// [N]

String& Thread::NewString(const Common::String& string) {
  auto& chars = AllocVector(*Ty_CharVector, int(string.length()));
  auto& strobj = *new(Alloc(sizeof(String))) String(chars);
  ::CopyMemory(
      const_cast<char16*>(strobj.elements()),
      string.value(),
      string.length() * sizeof(char16));
  return strobj;
}

// [R]
void Thread::Reset() {
  context_.frame_ = nullptr;
  context_.param_ = nullptr;
  ResetAlloc();
}

void Thread::ResetAlloc() {
  object_area_ = Mm::GetEmptyArea();
}

// [S]
void Thread::StaticInit() {
  PlatformThread::StaticInit();
}

} // Om
