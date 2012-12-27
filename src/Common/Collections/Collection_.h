// @(#)$Id$
//
// Common Collections Array_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Collection_h)
#define INCLUDE_Common_Collections_Collection_h

#include "../DebugHelper.h"
#include "../Object.h"

#define PARAMS_DECL_1(ty) Box a
#define PARAMS_DECL_2(ty) PARAMS_DECL_1(ty), Box b
#define PARAMS_DECL_3(ty) PARAMS_DECL_2(ty), Box c
#define PARAMS_DECL_4(ty) PARAMS_DECL_3(ty), Box d
#define PARAMS_DECL_5(ty) PARAMS_DECL_4(ty), Box e
#define PARAMS_DECL_6(ty) PARAMS_DECL_5(ty), Box f
#define PARAMS_DECL_7(ty) PARAMS_DECL_6(ty), Box g
#define PARAMS_DECL_8(ty) PARAMS_DECL_7(ty), Box h
#define PARAMS_DECL_9(ty) PARAMS_DECL_8(ty), Box i

#define PARAMS_1 a
#define PARAMS_2 PARAMS_1, b
#define PARAMS_3 PARAMS_2, c
#define PARAMS_4 PARAMS_3, d
#define PARAMS_5 PARAMS_4, e
#define PARAMS_6 PARAMS_5, f
#define PARAMS_7 PARAMS_6, g
#define PARAMS_8 PARAMS_7, h
#define PARAMS_9 PARAMS_8, i

#define DECLARE_VARDIC_METHODS(rety_name, param, ty) \
    rety_name(param, PARAMS_DECL_1(ty)); \
    rety_name(param, PARAMS_DECL_2(ty)); \
    rety_name(param, PARAMS_DECL_3(ty)); \
    rety_name(param, PARAMS_DECL_4(ty)); \
    rety_name(param, PARAMS_DECL_5(ty)); \
    rety_name(param, PARAMS_DECL_6(ty)); \
    rety_name(param, PARAMS_DECL_7(ty)); \
    rety_name(param, PARAMS_DECL_8(ty)); \
    rety_name(param, PARAMS_DECL_9(ty))

#define REPEAT_MACRO_1 REPEATED_MACRO(1)
#define REPEAT_MACRO_2 REPEAT_MACRO_1 REPEATED_MACRO(2)
#define REPEAT_MACRO_3 REPEAT_MACRO_2 REPEATED_MACRO(3)
#define REPEAT_MACRO_4 REPEAT_MACRO_3 REPEATED_MACRO(4)
#define REPEAT_MACRO_5 REPEAT_MACRO_4 REPEATED_MACRO(5)
#define REPEAT_MACRO_6 REPEAT_MACRO_5 REPEATED_MACRO(6)
#define REPEAT_MACRO_7 REPEAT_MACRO_6 REPEATED_MACRO(7)
#define REPEAT_MACRO_8 REPEAT_MACRO_7 REPEATED_MACRO(8)
#define REPEAT_MACRO_9 REPEAT_MACRO_8 REPEATED_MACRO(9)

namespace Common {
class Box;
}

namespace Common {
namespace Collections {

template<typename T, class TAllocable> class Array_;
template<typename T, class TAllocable> class ArrayList_;
template<typename K, typename V, class TAllocable> class HashMap_;
template<typename T, class TAllocable> class HashSet_;

// Fixed lengthed array
template<typename T, class TAllocable = Allocable>
class Collection_ : public Object_<Collection_<T>> {
  // Data of Collection.
  private: class Data : public RefCounted_<Data> {
    private: T* const elements_;
    private: size_t length_;

    public: explicit Data(size_t const vn)
        : elements_(new T[vn]), length_(vn) {
      AddRef();
    }

    public: Data(const T* const v, size_t const vn)
        : elements_(new T[vn]), length_(vn) {
      AddRef();
      for (size_t i = 0; i < vn; i++) {
        elements_[i] = v[i];
      }
    }

    public: T* elements() const { return elements_; }
    public: size_t length() const { return length_; }

    DISALLOW_COPY_AND_ASSIGN(Data);
  };

  // Enumerate elements in Collection.
  public: class Enum {
    private: T* current_;
    private: T* const end_;
    private: Data* const data_;

    public: Enum(const Collection_& cln)
        : current_(cln.data_->elements()),
          end_(cln.data_->elements() + cln.data_->length()),
          data_(cln.data_) {
      data_->AddRef();
    }

    public: ~Enum() { data_->Release(); }
    public: T operator *() const { return Get(); }
    public: T operator ->() const { return Get(); }

    public: bool AtEnd() const {
      ASSERT(current_ <= end_);
      return current_ == end_;
    }

    public: T Get() const { ASSERT(!AtEnd()); return *current_; }
    public: void Next() { ASSERT(!AtEnd()); ++current_; }
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  public: class ConstForwardIterator {
    private: const Data* data_;
    private: size_t index_;

    public: ConstForwardIterator(const Data* data, size_t index)
        : data_(data), index_(index) {}
    public: T operator*() const { return data_->elements()[index_]; }

    public: bool operator==(const ConstForwardIterator& another) const {
      ASSERT(data_ == another.data_);
      return index_ == another.index_;
    }

    public: bool operator!=(const ConstForwardIterator& another) const {
      return !operator==(another);
    }

    public: ConstForwardIterator& operator++() {
      DCHECK_LT(index_, data_->length());
      ++index_;
      return *this;
    }
  };

  public: class ForwardIterator {
    private: Data* data_;
    private: size_t index_;

    public: ForwardIterator(Data* data, size_t index)
        : data_(data), index_(index) {}
    public: T operator*() const { return data_->elements()[index_]; }

    public: bool operator==(const ForwardIterator& another) const {
      ASSERT(data_ == another.data_);
      return index_ == another.index_;
    }

    public: bool operator!=(const ForwardIterator& another) const {
      return !operator==(another);
    }

    public: ForwardIterator& operator++() {
      DCHECK_LT(index_, data_->length());
      ++index_;
      return *this;
    }
  };

  private: Data* data_;

  // ctor
  public: Collection_(const Collection_& list)
      : data_(list.data_) {
    data_->AddRef();
  }

  public: Collection_()
      : data_(new Data(0)) {}

  public: Collection_(const T* const v, size_t const vn)
      : data_(new Data(v, vn)) {}

  public: template<class A> Collection_(
      const Array_<T, A>& arr)
      : data_(new Data(arr.length())) {
    typedef Array_<T, A> Array;
    T* p = data_->elements();
    for (auto const value: arr)
      *p++ = value;
  }

  public: template<class A> Collection_(
      const ArrayList_<T, A>& list)
      : data_(new Data(list.Count())) {
    typedef ArrayList_<T, A> ArrayList;
    T* p = data_->elements();
    for (auto const value: list)
      *p++ = value;
  }

  public: template<class V, class A> Collection_(
      const HashMap_<T, V, A>& map)
      : data_(new Data(map.Count())) {
    typedef HashMap_<T, V, A> HashMap;
    T* p = data_->elements();
    foreach (HashMap::Enum, elems, map) {
      *p++ = elems.Get().GetKey();
    }
  }

  public: template<class A> Collection_(
      const HashSet_<T, A>& set)
      : data_(new Data(set.Count())) {
    typedef HashSet_<T, A> HashSet;
    T* p = data_->elements();
    foreach (HashSet::Enum, elems, set) {
      *p++ = elems.Get();
    }
  }

  public: virtual ~Collection_() { data_->Release(); }

  public: ForwardIterator begin() {
    return ForwardIterator(data_, 0);
  }

  public: ConstForwardIterator begin() const {
    return ConstForwardIterator(data_, 0);
  }

  public: ConstForwardIterator end() const {
    return ConstForwardIterator(data_, data_->length());
  }

  public: ForwardIterator end() {
    return ForwardIterator(data_, data_->length());
  }

  // operators
  public: Collection_& operator=(const Collection_& r) {
    data_->Release();
    data_ = r.data_;
    data_->AddRef();
    return *this;
  }

  private: class Eq {
    public: static bool Compare(const T& a, const T& b) { return a == b; }
    public: static bool const Shorter() { return false; }
  };

  private: class Ge {
    public: static bool Compare(const T& a, const T& b) { return a >= b; }
    public: static bool const Shorter() { return false; }
  };

  private: class Le {
    public: static bool Compare(const T& a, const T& b) { return a <= b; }
    public: static bool const Shorter() { return true; }
  };

  private: template<class Trait> bool Compare(
      const Collection_& another) const {
    if (this == &another)
      return true;

    auto another_it = another.begin();
    for (auto const this_value: *this) {
      if (another_it == another.end())
        return Trait::Shorter();
      if (!Trait::Compare(this_value, *another_it))
        return false;
      ++another_it;
    }
    return another_it == another.end();
  }

  public: bool operator==(const Collection_& r) const {
    return Count() == r.Count() && Compare<Eq>(r);
  }

  public: bool operator!=(const Collection_& r) const {
    return !operator==(r);
  }

  public: bool operator<(const Collection_& r) const {
    return !operator>=(r);
  }

  public: bool operator<=(const Collection_& r) const {
    return Compare<Le>(r);
  }

  public: bool operator>(const Collection_& r) const {
    return !operator<=(r);
  }

  public: bool operator>=(const Collection_& r) const {
    return Compare<Ge>(r);
  }

  // [C]
  public: int Count() const { return static_cast<int>(data_->length()); }

  // [G]
  public: T Get(size_t const index) const {
    DCHECK_LT(index, data_->length());
    return data_->elements()[index];
  }

  // [I]
  public: bool IsEmpty() const {
    return !Count();
  }

  // [T]
  // See CollectionToString.h for implementation.
  public: virtual String ToString() const override;
}; // Collection_

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Collection_h)
