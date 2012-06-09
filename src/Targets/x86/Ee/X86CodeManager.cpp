#include "precomp.h"
// @(#)$Id$
//
// Evita Ee X86 - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86CodeManager.h"

#include "./X86CodeDesc.h"

namespace Ee {

namespace X86 {
void RuntimeInit();
} // X86

namespace {

static X86CodeManager* s_CodeManager;

} // namespace

void* CodeManager::CodeHeap::Alloc(size_t size) {
  ASSERT(size > 0);
  for (;;) {
    if (auto const codes = tail_->Alloc(size)) {
      return codes;
    }

    auto const kAllocUnit = 64 * 1024;
    auto const alloc_size = ROUNDUP(int32(size + sizeof(Block)), kAllocUnit);
    auto const ptr = ::VirtualAlloc(
          nullptr,
          alloc_size,
          MEM_COMMIT | MEM_RESERVE,
          PAGE_EXECUTE_READWRITE);
    if (!ptr) {
      auto const error = ::GetLastError();
      Debugger::Fail("VirtualAlloc: err=%d %d byte\n", error, alloc_size);
    }
  
    tail_ = new(ptr) Block(tail_, alloc_size);
  }
}

// [G]
CodeManager* CodeManager::Get() {
  return s_CodeManager;
}

// [I]
void CodeManager::Init() {
  s_CodeManager = new X86CodeManager();
}


// [N]
CodeDesc& CodeManager::NewCodeDesc(
    const Function& fun,
    const Collection_<CodeDesc::Annotation>& annots,
    uint32 const code_size) {
  ASSERT(code_size > 0);
  auto const codes = code_heap_.Alloc(code_size);
  auto& code_desc = *new X86CodeDesc(fun, annots, codes, code_size);

  {
    ScopedLock lock(code_desc_map_lock_);
    code_desc_map_.Insert(code_desc);
  }

  return code_desc;
}

// ctor
X86CodeManager::X86CodeManager() {}
X86CodeManager::~X86CodeManager() {}

} // Ee
