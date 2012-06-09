// @(#)$Id$
//
// Common Collections PtrKey_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_PtrKey_h)
#define INCLUDE_Common_Collections_PtrKey_h

namespace Common {
namespace Collections {

template<typename T>
class PtrKey_ {
  private: typedef PtrKey_<T> PtrKey;
  private: T* ptr_;

  public: PtrKey_(T* const p = nullptr) : ptr_(p) {}

  public: operator T*() const { return ptr_; }

  public: bool operator==(const PtrKey& that) const {
    return ptr_ == that.ptr_;
  }

  public: int ComputeHashCode() const {
    return ptr_->ComputeHashCode(); 
  }

  public: bool Equals(const PtrKey& that) const {
    return ptr_ == that.ptr_;
  }
}; // PtrKey_

template<typename T>
inline int ComputeHashCode(const PtrKey_<T>& r) {
  return r.ComputeHashCode();
}

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_PtrKey_h)
