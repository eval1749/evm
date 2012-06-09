// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_Thread_h)
#define INCLUDE_Om_Thread_h

#include "./MemoryManager.h"
#include "./PlatformThread.h"

namespace Om {

struct Frame;

class Thread :
  public DoubleLinkedItem_<Thread>,
  public PlatformThread {

  public: struct Context {
    Frame* frame_;
    void* param_;
    void* values_[128];
  };

  private: Context context_;

  // TODO 2008-01-06 yosi@msn.com We should use Mm::AreaManager.
  private: Mm::Area* object_area_;

  // ctor
  public: Thread();

  // properties
  public: Context* context() const { 
    return &const_cast<Thread*>(this)->context_;
  }

  public: intptr_t& value(uint i) {
    ASSERT(i < ARRAYSIZE(context_.values_));
    return reinterpret_cast<intptr_t&>(context_.values_[i]);
  }

  public: intptr_t value(uint i) const {
    ASSERT(i < ARRAYSIZE(context_.values_));
    return reinterpret_cast<intptr_t>(context_.values_[i]);
  }

  // [A]
  private: void* Alloc(size_t);
  public: Vector& AllocVector(const VectorType&, int);

  // [N]
  public: String& NewString(const Common::String&);

  // [R]
  public: void Reset();
  public: void ResetAlloc();

  // [S]
  public: static void StaticInit();
}; // Thread

} // Om

#endif // !defined(INCLUDE_Om_Thread_h)
