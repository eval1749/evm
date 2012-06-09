// @(#)$Id$
//
// Common Collections ArrayList_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_ArrayList_h)
#define INCLUDE_Common_Collections_ArrayList_h

#include "./Array_.h"
#include "./Vector_.h"

namespace Common {
namespace Collections {

template<typename T, class TAllocable = Allocable>
class ArrayList_
    : public Object_<ArrayList_<T>>,
      public TAllocable {
  private: typedef Array_<T> Array;
  private: typedef ArrayList_<T> ArrayList;

  public: class Enum {
    private: int current_;
    private: const ArrayList& list_;

    public: Enum(const ArrayList& list)
        : current_(0), list_(list) {
    }

    public: Enum(const ArrayList* list)
        : current_(0), list_(*list) {
      ASSERT(list != nullptr);
    }

    public: T operator *() const {
      ASSERT(!AtEnd());
      return list_[current_];
    }

    public: T operator ->() const {
      ASSERT(!AtEnd());
      return list_[current_];
    }

    public: bool AtEnd() const {
      ASSERT(current_ <= list_.Count());
      return current_ == list_.Count();
    }

    public: T Get() const {
      ASSERT(!AtEnd());
      return list_.Get(current_);
    }

    public: void Next() {
      ASSERT(!AtEnd());
      current_++;
    }

    public: void Set(T value) {
      ASSERT(!AtEnd());
      const_cast<ArrayList&>(list_).Set(current_, value);
    }

    DISALLOW_COPY_AND_ASSIGN(Enum);
  }; // Enum

  DEFINE_ENUMERATOR(ArrayList_, T);

  private: Array* elements_;
  private: int length_;

  public: explicit ArrayList_(int const capacity = 10)
      : elements_(new Array(max(capacity, 3))),
        length_(0) {
    ASSERT(elements_ != nullptr);
  }

  public: explicit ArrayList_(const ArrayList_<T>& list)
      : elements_(new Array(list.length_)),
        length_(0) {
    ASSERT(elements_ != nullptr);
    foreach (ArrayList::Enum, elements, list) {
      Add(*elements);
    }
  }

  public: template<class C> explicit ArrayList_(const C& collection)
      : elements_(new Array(collection.Count())),
        length_(0) {
    ASSERT(elements_ != nullptr);
    AddAll(collection);
  }

  public: ArrayList_(ArrayList_&& list)
    : elements_(list.elements_),
      length_(list.length_) {
    list.elements_ = nullptr;
    list.length_ = 0;
  }

  public: virtual ~ArrayList_() {
    delete elements_;
  }

  public: ArrayList& operator=(const ArrayList& list) {
    Clear();
    AddAll(list);
    return *this;
  }

  public: T& operator [](int const index) {
    ASSERT(index >= 0 && index < length_);
    return (*elements_)[index];
  }

  public: const T& operator [](int const index) const {
    ASSERT(index >= 0 && index < length_);
    return (*elements_)[index];
  }

  public: bool operator==(const ArrayList& r) const {
    if (Count() != r.Count()) return false;
    Enum this_enum(this);
    Enum that_enum(r);
    while (!this_enum.AtEnd()) {
      if (this_enum.Get() != that_enum.Get()) {
        return false;
      }
      this_enum.Next();
      that_enum.Next();
    }
    return true;
  }

  public: bool operator!=(const ArrayList& r) const { return !(*this == r); }

  // [A]
  public: void Add(T value) {
    auto const capacity = elements_->Count();
    if (Count() == capacity) {
      auto const old_elements = elements_;
      elements_ = new Array(capacity * 3 / 2);
      old_elements->CopyTo(*elements_);
      delete old_elements;
    }

    length_++;
    Set(length_ - 1, value);
  }

  public: template<class C> void AddAll(const C& collection) {
    foreach (C::Enum, elements, collection) {
      Add(*elements);
    }
 }

  // [C]
  public: void Clear() { length_ = 0; }

  public: bool Contains(const T value) {
    foreach (Enum, elements, this) {
      if (*elements == value) {
        return true;
      }
    }
    return false;
  }

  public: void CopyTo(T* elements) const {
    for (auto i = 0; i < length_; i++) {
      elements[i] = elements_->Get(i);
    }
  }

  public: int Count() const { return length_; }

  // [E]
  #if 0
  public: bool Equals(const ArrayList* that) const {
    if (that == nullptr) {
      return this == nullptr;
    }

    if (this == nullptr) {
      return false;
    }

    if (Count() != that->Count()) {
      return false;
    }

    Enum this_enum(this);
    Enum that_enum(that);
    while (!this_enum.AtEnd()) {
      if (this_enum.Get() != that_enum.Get()) {
        return false;
      }
      this_enum.Next();
      that_enum.Next();
    }
    return true;
  }
  #else
  public: bool Equals(const ArrayList& that) const {
    if (Count() != that.Count()) {
      return false;
    }

    Enum this_enum(this);
    Enum that_enum(that);
    while (!this_enum.AtEnd()) {
      if (this_enum.Get() != that_enum.Get()) {
        return false;
      }
      this_enum.Next();
      that_enum.Next();
    }
    return true;
  }
  #endif

  // [G]
  public: T Get(int const index) const {
    ASSERT(index >= 0 && index < length_);
    return elements_->Get(index);
  }

  // [I]
  public: bool IsEmpty() const { return Count() == 0; }

  // [S]
  public: void Set(int const index, T value) {
    ASSERT(index >= 0 && index < length_);
    return elements_->Set(index, value);
  }

  // [T]
  public: Array* ToArray() const {
    auto const array = new Array_<T>(Count());
    auto index = 0;
    foreach (Enum, elems, this) {
      array->Set(index, *elems);
      index++;
    }
    return array;
  }

  public: Vector_<T> ToVector() const {
    Vector_<T> vector(Count());
    auto index = 0;
    foreach (Enum, elems, this) {
      vector[index] = *elems;
      ++index;
    }
    return vector;
  }

  // See ArrayListToString.h for implementation.
  public: virtual String ToString() const override;
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_ArrayList_h)
