// @(#)$Id$
//
// Evita Common - Statck (FILO - First In Last Out)
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Queue_h)
#define INCLUDE_Common_Collections_Queue_h

namespace Common {
namespace Collections {

template<typename Element>
class Queue_ {
  private: typedef Queue_<Element> Queue;
  private: struct Node;

  public: class Enum {
    private: Node* runner_;

    public: Enum(const Queue& queue)
        : runner_(queue.head_) {
    }

    public: Enum(Queue* const pQueue)
        : runner_(pQueue->head_) {
    }

    public: Element operator *() const { return Get(); }
    public: Element& operator ->() const { return &Get(); }
    public: bool AtEnd() const { return !runner_; }

    public: Element Get() const {
      ASSERT(!AtEnd());
      return runner_->datum_;
    } // Get

    public: void Next() {
      ASSERT(!AtEnd());
      runner_ = runner_->next_;
    } // Next
  }; // Enum

  DEFINE_ENUMERATOR(Queue_, Element);

  private: struct Node {
    Element const datum_;
    Node* next_;

    Node(Element datum) : datum_(datum), next_(nullptr) {
    }

    DISALLOW_COPY_AND_ASSIGN(Node);
  }; // Node

  private: uint count_;
  private: Node* head_;
  private: Node* tail_;

  // ctor
  public: Queue_()
    : count_(0),
      head_(nullptr),
      tail_(nullptr) {
  }

  public: ~Queue_() {
    Clear();
  } // ~Queue

  // [C]
  public: void Clear() {
    auto runner = head_;
    while (runner != nullptr) {
      auto const pNext = runner->next_;
      delete runner;
      runner = pNext;
    } // while

    count_ = 0;
    head_ = nullptr;
    tail_ = nullptr;
  } // Clear

  public: int Count() const { return count_; }

  // [G]
  public: Element Get() const {
    ASSERT(!IsEmpty());
    return head_->datum_;
  }

  public: void Give(Element element) {
    auto const node = new Node(element);
    if (tail_ == nullptr) {
      ASSERT(head_ == nullptr);
      head_ = node;
    } else {
      tail_->next_ = node;
    } // if
    count_++;
    tail_ = node;
  } // Give

  // [I]
  public: bool IsEmpty() const { return count_ == 0; }

  // [T]
  public: Element Take() {
    ASSERT(head_ != nullptr);
    count_--;
    auto const element = head_->datum_;
    head_ = head_->next_;
    if (head_ == nullptr) {
      tail_ = nullptr;
    } // if
    return element;
  } // Take

  DISALLOW_COPY_AND_ASSIGN(Queue);
}; // Queue_

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Queue_h)
