// @(#)$Id$
//
// Evita Common - Lockable
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Lockable_h)
#define INCLUDE_Common_Lockable_h

namespace Common {

class Lockable {
  public: void Lock() const {}
  public: void Unlock() const {}
};

class ScopedLock {
  private: const Lockable& lockable_;

  public: ScopedLock(const Lockable& lockable) : lockable_(lockable) {
    lockable_.Lock();
  }

  public: ~ScopedLock() {
    lockable_.Unlock();
  }

  DISALLOW_COPY_AND_ASSIGN(ScopedLock);
};

} // Common

#endif // !defined(INCLUDE_Common_Lockable_h)
