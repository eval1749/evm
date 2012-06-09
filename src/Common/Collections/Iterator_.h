// @(#)$Id$
//
// Common Collections Array_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Iterator_h)
#define INCLUDE_Common_Collections_Iterator_h

namespace Common {
namespace Collections {

// Note(2012-04-07): VC10 can't compile ConstIterator_ if we use identifier
// "Iterator".
template<class BaseIterator>
class ConstIterator_ : public BaseIterator {
  public: ConstIterator_() {}
  public: explicit ConstIterator_(BaseIterator it) : BaseIterator(it) {}
  public: ConstIterator_(const ConstIterator_& it) : BaseIterator(it) {}

  public: typename BaseIterator::ElementType operator *() const {
    return BaseIterator::operator *();
  }

  public: const typename BaseIterator::ElementType* operator ->() const {
    return BaseIterator::operator ->();
  }
};

template<class BaseIterator>
class ReverseIterator_ : public BaseIterator {
  public: ReverseIterator_() {}
  public: explicit ReverseIterator_(BaseIterator it) : BaseIterator(it) {}
  public: ReverseIterator_(const ReverseIterator_& it) : BaseIterator(it) {}

  public: ReverseIterator_ operator ++() {
    BaseIterator::operator --();
    return *this;
  }

  public: ReverseIterator_ operator --() {
    BaseIterator::operator ++();
    return *this;
  }

  public: ReverseIterator_ operator +=(int n) {
    BaseIterator::operator -=(n);
    return *this;
  }

  public: ReverseIterator_ operator -=(int n) {
    BaseIterator::operator +=(n);
    return *this;
  }

  public: ReverseIterator_ operator +(int n) {
    return ReverseIterator_(BaseIterator::operator -(n));
  }

  public: ReverseIterator_ operator -(int n) {
    return ReverseIterator_(BaseIterator::operator +(n));
  }
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Iterator_h)
