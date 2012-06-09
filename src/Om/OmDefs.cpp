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

void StaticObjectInit();

void Init() {
  {
    InitParam oInitParam;
    oInitParam.m_hSelf = ::GetModuleHandle(nullptr);
    oInitParam.m_nTotalMb = 16;
    oInitParam.m_pvStaticEnd = reinterpret_cast<void*>(STATIC_OBJECT_END);
    Mm::Start(&oInitParam);
  }

  Thread::StaticInit();
  StaticObjectInit();
}

} // Om
