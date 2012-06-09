// @(#)$Id$
//
// Common Collections HashKey_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_HashKey_h)
#define INCLUDE_Common_Collections_HashKey_h

namespace Common {
namespace Collections {

template<typename T>
class HashKey_ {
  private: typedef HashKey_<T> TKey;
  private: T* ptr_;

  public: HashKey_(T* const p = nullptr) : ptr_(p) {}

  public: operator T*() const { return ptr_; }
  public: bool operator==(const HashKey_ r) { return *ptr_ == *r.ptr_; }
  public: bool operator!=(const HashKey_ r) { return !operator==(r); }

  public: int ComputeHashCode() const {
    return ptr_->ComputeHashCode(); 
  }
}; // HashKey_

template<typename T>
inline int ComputeHashCode(const HashKey_<T>& r) {
  return r.ComputeHashCode();
}

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_HashKey_h)
