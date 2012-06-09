// @(#)$Id$
//
// Evita Common - Statck (FILO - First In Last Out)
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Stack_h)
#define INCLUDE_Common_Collections_Stack_h

namespace Common {
namespace Collections {

template<typename Element>
class Stack_ {
  private: typedef Stack_<Element> Stack;

  public: class Enum {
    private: Element* end_;
    private: Element* runner_;

    public: Enum(const Stack& stack)
        : end_(stack.elemv_),
          runner_(&stack.elemv_[stack.count_ -1]) {}

    public: Enum(Stack* const stack)
        : end_(stack->elemv_),
          runner_(&stack->elemv_[stack->count_ -1]) {}

    public: Element operator *() const { return Get(); }
    public: Element operator ->() const { return Get(); }
    public: bool AtEnd() const { return runner_ < end_; }
    public: Element Get() const { ASSERT(!AtEnd()); return *runner_; }
    public: void Next() { ASSERT(!AtEnd()); runner_--; }
  }; // Enum

  DEFINE_ENUMERATOR(Stack_, Element);

  private: int capacity_;
  private: int count_;
  private: Element* elemv_;

  // ctor
  public: Stack(uint const capacity = 10)
      : capacity_(capacity),
        count_(0),
        elemv_(new Element[capacity]) {
  }

  // operators
  public: Element& operator [](int const index) {
    ASSERT(count_ - index - 1 >= 0);
    return elemv_[count_ - index - 1];
  }

  public: const Element& operator [](int const index) const {
    ASSERT(count_ - index - 1 >= 0);
    return elemv_[count_ - index - 1];
  }

  // [C]
  public: void Clear() {
    while (!IsEmpty()) {
      Pop();
    }
  }

  public: int Count() const { return count_; }

  // [G]
  public: const Element& GetTop() const {
    return (*this)[0];
  }

  // [I]
  public: bool IsEmpty() const { return count_ == 0; }

  // [P]
  public: Element Pop() {
    ASSERT(count_ > 0);
    count_ -= 1;
    return elemv_[count_];
  } // Pop

  public: void Push(Element const elem) {
    if (capacity_ == count_) {
      auto const old_capacity = capacity_;
      auto const old_elemv = elemv_;
      capacity_ = old_capacity * 3 / 2;
      elemv_ = new Element[capacity_];
      ::CopyMemory(elemv_, old_elemv, sizeof(Element) * old_capacity);
      delete[] old_elemv;
    } // if

    elemv_[count_] = elem;
    count_ += 1;
  } // Push

  DISALLOW_COPY_AND_ASSIGN(Stack);
}; // Stack_

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Stack_h)
