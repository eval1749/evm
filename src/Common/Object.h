// @(#)$Id$
//
// Evita Common Object
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Object_h)
#define INCLUDE_Common_Object_h

#include "./Allocable.h"

namespace Common {

class FormatProvider;
class String;

class Object : public Castable_<Object> {
  CASTABLE_CLASS(Object);

  // ctor
  public: Object() {}
  public: virtual ~Object() {}
  public: bool operator==(const Object& r) const { return this == &r; }
  public: bool operator!=(const Object& r) const { return !operator==(r); }

  // [T]
  public: virtual String ToString() const;

  public: virtual String ToStringWithFormat(
      const char16*, FormatProvider&) const;

  DISALLOW_COPY_AND_ASSIGN(Object);
}; // Object

template<class TSelf, class TParent = Object>
class Object_ : public WithCastable_<TSelf, TParent> {
  protected: typedef Object_<TSelf, TParent> Base;
  protected: Object_() {}
  DISALLOW_COPY_AND_ASSIGN(Object_);
}; // Object_

NoReturn void LibExport Error(const char* const pszFormat, ...);
int LibExport ComputeHashCode(int const iHashCode, int const iSeed = 0);

} // Common

#endif // !defined(INCLUDE_Common_Object_h)
