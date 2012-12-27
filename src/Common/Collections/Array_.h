// @(#)$Id$
//
// Common Collections Array_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Array_h)
#define INCLUDE_Common_Collections_Array_h

#include "./Collection_.h"
#include "../Object.h"
#include "../MemoryZone.h"

namespace Common {
namespace Collections {

template<typename T, class TAllocable = Allocable>
class Array_
    : public Object_<Array_<T>>,
      public TAllocable {
  CASTABLE_CLASS(Array);

  private: typedef Array_<T> Array;

  public: class Enum : public EnumArray_<T> {
    public: Enum(const Array* const p)
        : EnumArray_<T>(p->elements_, p->length_) {
    }

    public: Enum(const Array& r)
        : EnumArray_<T>(r.elements_, r.length_) {
    }

    DISALLOW_COPY_AND_ASSIGN(Enum);
  }; // Enum

  DEFINE_ENUMERATOR(Array_, T);

  public: class ConstForwardIterator {
    private: const Array_* array_;
    private: size_t index_;

    public: ConstForwardIterator(const Array_* array, size_t index = 0)
        : array_(array), index_(index) {
      DCHECK(!!array_);
      DCHECK_LE(index_, array_->length());
    }

    public: const T& operator*() const { return (*array_)[index_]; }

    public: bool operator==(const ConstForwardIterator& another) const {
      DCHECK_EQ(array_, another.array_);
      return index_ == another.index_;
    }

    public: bool operator!=(const ConstForwardIterator& another) const {
      return !operator==(another);
    }

    public: ConstForwardIterator& operator++() {
      DCHECK_LT(index_, array_->length());
      ++index_;
      return *this;
    }
  };

  public: class ForwardIterator {
    private: Array_* array_;
    private: size_t index_;

    public: ForwardIterator(Array_* array, size_t index)
        : array_(array), index_(index) {
      DCHECK(!!array_);
      DCHECK_LE(index_, array_->length());
    }

    public: T& operator*() const { return (*array_)[index_]; }

    public: bool operator==(const ForwardIterator& another) const {
      DCHECK_EQ(array_, another.array_);
      return index_ == another.index_;
    }

    public: bool operator!=(const ForwardIterator& another) const {
      return !operator==(another);
    }

    public: ForwardIterator& operator++() {
      DCHECK_LT(index_, array_->length());
      ++index_;
      return *this;
    }
  };

  // Note: length_ must be initialized before elements_.
  private: size_t const length_;
  private: T* const elements_;

  // ctor
  public: explicit Array_(size_t const count)
    : length_(count),
      elements_(new T[count]) {}

  public: explicit Array_(const Array& array)
      : length_(array.Count()), elements_(new T[array.Count()]) {
    array.CopyTo(*this);
  }

  public: explicit Array_(const Collection_<T>& collection)
      : length_(collection.Count()),
        elements_(new T[collection.Count()]) {
    auto index = 0;
    for (auto const value : collection) {
      Set(index, value);
      ++index;
    }
  }

  // TODO(yosi) We should use CopyMemory if possible.
  public: Array_(const T* const elements, size_t const count)
      : length_(count),
        elements_(new T[length_]) {
    for (size_t i = 0; i < count; i++) {
      elements_[i] = elements[i];
    }
  }

  public: Array_& operator=(const Array_& array) {
    ASSERT(length_ == array.length_);
    array.CopyTo(*this);
    return *this;
  }

  public: T& operator [](size_t const index) {
    ASSERT(index < length_);
    return elements_[index];
  }

  public: const T& operator [](size_t const index) const {
    ASSERT(index < length_);
    return elements_[index];
  }

  public: bool operator==(const Array& another) const {
    if (length_ != another.length_)
      return false;
    auto another_it = another.begin();
    for (auto const this_value: *this) {
      if (this_value != *another_it)
        return false;
      ++another_it;
    }
    return true;
  }

  public: bool operator!=(const Array& r) const { return !(*this == r); }

  // properties
  public: ForwardIterator begin() { return ForwardIterator(this, 0); }
  public: ConstForwardIterator begin() const {
    return ConstForwardIterator(this, 0);
  }
  public: ForwardIterator end() { return ForwardIterator(this, length()); }
  public: ConstForwardIterator end() const {
    return ConstForwardIterator(this, length());
  }
  public: size_t length() const { return length_; }

  // [C]
  public: void CopyTo(Array& array, size_t const offset = 0) const {
    ASSERT(length_ + offset <= array.length_);
    for (size_t i = 0; i < length_; i++) {
      array.elements_[offset + i] = elements_[i];
    }
  }

  public: void CopyTo(T* elements) const {
    for (size_t i = 0; i < length_; i++) {
      elements[i] = elements_[i];
    }
  }

  public: int Count() const { return static_cast<int>(length_); }

  // [E]
  public: bool Equals(const Array& another) const {
    if (length_ != another.length_)
      return false;
    auto another_it = another.begin();
    for (auto const this_value: *this) {
      if (!this_value.Equals(*another_it))
        return false;
      ++another_it;
    }
    return true;
  }

  // [G]
  public: T Get(size_t const index) const {
    ASSERT(index < length_);
    return elements_[index];
  }

  // [S]
  public: void Set(size_t const index, T oValue) {
    ASSERT(index < length_);
    elements_[index] = oValue;
  }

  public: void Set(Enumerator_<T>& en, size_t const offset = 0) {
    auto i = offset;
    foreach (Enum_<T>, elems, en) {
      Set(i, *elems);
      i++;
    }
  }

  public: void Set(const T* const v,
                   size_t const vn,
                   size_t const offset = 0) {
    ASSERT(offset + vn <= length_);
    auto p = v;
    for (size_t i = offset; i < length_; i++) {
      Set(i, *p++);
    }
  }

  public: typedef int (LibCallback *Comparer)(T const p1, T const p2);

  public: Array& LibExport Sort(Comparer pfnComaprer) {
    ASSERT(pfnComaprer != nullptr);

    class Local {
      public: static void Heapify(
          Array& array,
          Comparer pfnComaprer,
          int const index,
          int const last) {

        ASSERT(pfnComaprer != nullptr);

        auto const child1 = (index + 1) * 2 - 1;
        if (child1 > last) {
          return;
        }

        auto const child2 = child1 + 1;

        auto const child =
            child2 <= last
                && pfnComaprer(
                        array.Get(child1),
                        array.Get(child2)) <= 0
                ? child2
                : child1;

        if (pfnComaprer(array.Get(index), array.Get(child)) <= 0) {
          Local::Swap(array, index, child);
        }

        Local::Heapify(array, pfnComaprer, child, last);
      }

      public: static void Swap(
          Array& array,
          int const index1,
          int const index2) {
        auto const temp = array.Get(index1);
        array.Set(index1, array.Get(index2));
        array.Set(index2, temp);
      }
    }; // Local

    auto const last = Count() - 1;

    // Build heap
    for (auto index = last / 2; index >= 0; index--) {
      Local::Heapify(*this, pfnComaprer, index, last);
    }

    // Sort
    for (auto index = last; index > 0; index--) {
      Local::Swap(*this, index, 0);
      Local::Heapify(*this, pfnComaprer, 0, index - 1);
    }

    return *this;
  }

  // [T]
  // See ArrayToString.h for implementation.
  public: virtual String ToString() const override;
}; // Array_

template<typename T, typename TAllocable = Allocable>
class ArrayV_ : public Array_<T, TAllocable> {
  private: typedef Array_<T, TAllocable> Base;

  public: ArrayV_(T a) : Base(1) {
    T v[] = { a };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b) : Base(2) {
    T v[] = { a, b };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c) : Base(3) {
    T v[] = { a, b, c };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d) : Base(4) {
    T v[] = { a, b, c, d };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e) : Base(5) {
    T v[] = { a, b, c, d, e };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e, T f) : Base(6) {
    T v[] = { a, b, c, d, e, f };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e, T f, T g)  : Base(7) {
    T v[] = { a, b, c, d, e, f, g };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e, T f, T g, T h)  : Base(8) {
    T v[] = { a, b, c, d, e, f, g, h };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e, T f, T g, T h, T i)  : Base(9) {
    T v[] = { a, b, c, d, e, f, g, h, i };
    Set(v, ARRAYSIZE(v));
  }

  public: ArrayV_(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j)
      : Base(10) {
    T v[] = { a, b, c, d, e, f, g, h, i, j };
    Set(v, ARRAYSIZE(v));
  }
};

template<typename T, typename TAllocable = Allocable>
class CollectionV_ : public Collection_<T, TAllocable> {
  private: typedef Collection_<T, TAllocable> Base;
  private: typedef ArrayV_<T, TAllocable> ArrayV;
  public: CollectionV_() : Base() {}
  public: CollectionV_(T a) : Base(ArrayV(a)) {}
  public: CollectionV_(T a, T b) : Base(ArrayV(a, b)) {}
  public: CollectionV_(T a, T b, T c) : Base(ArrayV(a, b, c)) {}
  public: CollectionV_(T a, T b, T c, T d) : Base(ArrayV(a, b, c, d)) {}

  public: CollectionV_(T a, T b, T c, T d, T e)
      : Base(ArrayV(a, b, c, d, e)) {}

  public: CollectionV_(T a, T b, T c, T d, T e, T f)
      : Base(ArrayV(a, b, c, d, e, f)) {}

  public: CollectionV_(T a, T b, T c, T d, T e, T f, T g)
      : Base(ArrayV(a, b, c, d, e, f, g)) {}

  public: CollectionV_(T a, T b, T c, T d, T e, T f, T g, T h)
      : Base(ArrayV(a, b, c, d, e, f, g, h)) {}

  public: CollectionV_(T a, T b, T c, T d, T e, T f, T g, T h, T i)
      : Base(ArrayV(a, b, c, d, e, f, g, h, i)) {}

  public: CollectionV_(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j)
      : Base(ArrayV(a, b, c, d, e, f, g, h, i, j)) {}
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Array_h)
