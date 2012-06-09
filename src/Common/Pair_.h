// @(#)$Id$
//
// Evita Common Pair
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Pair_h)
#define INCLUDE_Common_Pair_h

namespace Common {

// Imuutable pair
template<typename T>
class Pair_ {
  private: T first_;
  private: T second_;

  public: Pair_(const T& a, const T& b) : first_(a), second_(b) {}
  public: Pair_(const Pair_& r) : first_(r.first()), second_(r.second()) {}
  public: Pair_(Pair_&& r) : first_(r.first()), second_(r.second()) {}

  // operators
  public: Pair_& operator =(const Pair_& r) {
    first_ = r.first_;
    seoncd_ = r.second_;
  }

  public: Pair_& operator =(Pair_&& r) {
    first_ = r.first_;
    seoncd_ = r.second_;
  }

  public: bool operator ==(const Pair_& rhs) const {
    return this == &rhs
        || (first() == rhs.first() && second() == rhs.second());
  }

  public: bool operator !=(const Pair_& rhs) const {
    return !operator ==(rhs);
  }

  // properties
  public: T& first() { return first_; }
  public: const T& first() const { return first_; }
  public: T& second() { return second_; }
  public: const T& second() const { return second_; }
};

} // Common

#endif // !defined(INCLUDE_Common_Pair_h)
