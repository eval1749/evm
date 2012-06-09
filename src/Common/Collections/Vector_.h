// @(#)$Id$
//
// Common Collections Array_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Vector_h)
#define INCLUDE_Common_Collections_Vector_h

#include "../DebugHelper.h"
#include "./Iterator_.h"

namespace Common {
namespace Collections {

template<typename T>
class Vector_ {
  public: class Enum : public EnumArray_<T> {
    public: Enum(const Vector_& r) : EnumArray_<T>(r.elements_, r.size_) {}
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  #if defined(_DEBUG)
  public: class Iterator {
    public: typedef T ElementType;

    protected: Vector_* vector_;
    protected: int index_;

    public: Iterator() : vector_(nullptr), index_(0) {}

    public: Iterator(const Iterator& it)
        : vector_(it.vector_), index_(it.index_) {}

    public: Iterator(Vector_& v, int n) : vector_(&v), index_(n) {}

    public: T& operator *() const {
      DCHECK_GE(index_, 0);
      DCHECK_LT(index_, vector_->size());
      return (*vector_)[index_];
    }

    public: T* operator ->() const {
      DCHECK_GE(index_, 0);
      DCHECK_LT(index_, vector_->size());
      return (*vector_)[index_];
    }

    public: Iterator operator ++() { ++index_; return *this; }
    public: Iterator operator --() { --index_; return *this; }

    public: Iterator operator +=(int n) { index_ += n; return *this; }
    public: Iterator operator -=(int n) { index_ -= n; return *this; }

    public: intptr_t operator -(const Iterator& r) const {
      return index_ - r.index_;
    }

    public: Iterator operator +(int n) {
      return Iterator(*vector_, index_ + n);
    }

    public: Iterator operator -(int n) {
      return Iterator(*vector_, index_ - n);
    }

    public: bool operator ==(const Iterator& rhs) const {
      DCHECK_EQ(vector_, rhs.vector_);
      return index_ == rhs.index_;
    }

    public: bool operator !=(const Iterator& rhs) const {
      DCHECK_EQ(vector_, rhs.vector_);
      return index_ != rhs.index_;
    }
  };
  #else // defined(_DEBUG)
  public: class Iterator {
    public: typedef T ElementType;

    private: T* ptr_;

    public: Iterator() : ptr_(nullptr) {}
    public: Iterator(const Iterator& it) : ptr_(it.ptr_) {}
    public: Iterator(const Vector_& v, int n) : ptr_(v.elements_ + n) {}
    protected: explicit Iterator(T* ptr) : ptr_(ptr) {}

    public: T& operator *() const {
      DCHECK_NE(ptr_, static_cast<T*>(nullptr));
      return *ptr_;
    }

    public: T* operator ->() const {
      DCHECK_NE(ptr_, static_cast<T*>(nullptr));
      return *ptr_;
    }

    public: intptr_t operator -(const Iterator& r) const {
      return int(ptr_ - r.ptr_);
    }

    public: Iterator operator ++() { ++ptr_; return *this; }
    public: Iterator operator --() { --ptr_; return *this; }

    public: Iterator operator +=(int n) { ptr_ += n; return *this; }
    public: Iterator operator -=(int n) { ptr_ -= n; return *this; }

    public: Iterator operator +(int n) { return Iterator(ptr_ + n); }
    public: Iterator operator -(int n) { return Iterator(ptr_ - n); }

    public: bool operator ==(const Iterator& rhs) const {
      return ptr_ == rhs.ptr_;
    }

    public: bool operator !=(const Iterator& rhs) const {
      return ptr_ != rhs.ptr_;
    }
  };
  #endif // defined(_DEBUG)

  public: typedef ConstIterator_<Iterator> ConstIterator;
  public: typedef ReverseIterator_<Iterator> ReverseIterator;
  public: typedef ConstIterator_<ReverseIterator> ConstReverseIterator;

  private: T* elements_;
  private: int size_;

  // ctor
  public: Vector_() : elements_(nullptr), size_(0) {}

  public: explicit Vector_(int size) : elements_(new T[size]), size_(size) {
    DCHECK_NE(elements_, static_cast<T*>(nullptr));
    DCHECK_GE(size_, 0);
  }

  public: Vector_(const Vector_& r)
      : elements_(new T[r.size_]), size_(r.size_) {
    *this = r;
  }

  public: Vector_(Vector_&& r) : elements_(r.elements_), size_(r.size_) {
    r.elements_ = nullptr;
    r.size_ = 0;
  }

  public: Vector_(const ConstIterator& begin, const ConstIterator& end)
      : elements_(new T[end - begin]), size_(end - begin) {
    auto dst = begin();
    for (auto it = begin; it != end; ++it) {
      *dst = *it;
      ++dst;
    }
  }

  public: Vector_(const Iterator& begin, const Iterator& end)
      : elements_(new T[end - begin]), size_(end - begin) {
    auto dst = this->begin();
    for (auto it = begin; it != end; ++it) {
      *dst = *it;
      ++dst;
    }
  }

  public: ~Vector_() {
    delete[] elements_;
  }

  // operators
  public: Vector_& operator =(const Vector_& source) {
    delete elements_;
    size_ = source.size();
    elements_ = new T[size_];
    DCHECK_NE(elements_, static_cast<T*>(nullptr));
    auto dst = begin();
    for (auto src = source.begin(); src != source.end(); ++src) {
      *dst = *src;
      ++dst;
    }
    return *this;
  }

  public: Vector_& operator =(Vector_&& r) {
    delete elements_;
    elements_ = r.elements;
    size_ = r.size;
    r.elements = nullptr;
    r.size = 0;
    return *this;
  }

  public: bool operator ==(const Vector_& rhs) const {
    if (this == &rhs) {
      return true;
    }

    if (size() != rhs.size()) {
      return false;
    }

    auto rhs_it = rhs.begin();
    for (auto lhs_it = begin(); lhs_it != end(); ++lhs_it) {
      if (*lhs_it != *rhs_it) {
        return false;
      }
      ++rhs_it;
    }
    return true;
  }

  public: bool operator !=(const Vector_& rhs) const {
    return !operator ==(rhs);
  }

  public: T& operator[](int const index) {
    DCHECK_GE(index, 0);
    DCHECK_LT(index, size_);
    return elements_[index];
  }

  public: T operator[](int const index) const {
    DCHECK_GE(index, 0);
    DCHECK_LT(index, size_);
    return elements_[index];
  }

  // properties
  public: Iterator begin() { return Iterator(*this, 0); }

  public: ConstIterator begin() const {
    return ConstIterator(const_cast<Vector_*>(this)->begin());
  }

  public: Iterator end() { return Iterator(*this, size_); }

  public: ConstIterator end() const {
    return ConstIterator(const_cast<Vector_*>(this)->end());
  }

  public: ReverseIterator rbegin() {
    return ReverseIterator(Iterator(*this, size_ - 1));
  }

  public: ConstReverseIterator rbegin() const {
    return ConstReverseIterator(const_cast<Vector_*>(this)->rbegin());
  }

  public: ReverseIterator rend() {
    return ReverseIterator(Iterator(*this, -1));
  }

  public: ConstReverseIterator rend() const {
    return ConstReverseIterator(const_cast<Vector_*>(this)->rend());
  }

  public: int size() const { return size_; }

  // [C]
  public: void CopyTo(Vector_& destination, int offset = 0) const {
    DCHECK_GE(offset, 0);
    DCHECK_LT(offset, destination.size());
    auto dst = destination.begin() + offset;
    for (auto it = begin(); it != end(); ++it) {
      *dst = *it;
      ++dst;
    }
  }

  // [F]
  public: void Fill(int const offset, int const size, T const value) {
    DCHECK_GE(offset, 0);
    DCHECK_LT(offset, size_);
    DCHECK_GE(size, 0);
    auto const end = offset + size;
    DCHECK_LE(end, size_);
    for (int i = offset; i < end; ++i) {
      (*this)[i] = value;
    }
  }
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Vector_h)
