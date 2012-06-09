// @(#)$Id$
//
// Common Collections HashSet_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_HashSet_h)
#define INCLUDE_Common_Collections_HashSet_h

namespace Common {
namespace Collections {

template<typename Key, class TAllocable = Allocable>
class HashSet_ : public TAllocable {
  private: typedef HashSet_<Key> HashSet;

  private: struct Slot : public Allocable {
    private: enum State {
      State_Empty,
      State_HasEntry,
      State_Removed,
    };

    private: State state_;
    private: Key key_;

    public: Slot() : state_(State_Empty) {}

    public: Key GetKey() const { return key_; }
    public: bool HasEntry() const { return state_ == State_HasEntry; }
    public: bool IsEmpty() const { return state_ == State_Empty; }
    public: bool IsRemoved() const { return state_ == State_Removed; }

    public: void Set(Key key)  {
      key_ = key;
      state_ = State_HasEntry;
    }

    public: void SetRemoved() { state_ = State_Removed; }
  };

  public: class Enum {
    private: const Slot* runner_;
    private: int count_;

    public: Enum(const HashSet_& hash_set_)
        : count_(hash_set_.count_),
          runner_(hash_set_.slots_) {
      Start();
    }

    public: Enum(const HashSet_* const hash_set_)
        : count_(hash_set_->count_),
          runner_(hash_set_->slots_) {
      Start();
    }

    public: Key operator*() const { return Get(); }
    public: bool AtEnd() const { return 0 == count_; }
    public: Key Get() const { ASSERT(!AtEnd()); return runner_->GetKey(); }

    public: void Next() {
      ASSERT(!AtEnd());

      count_--;
      if (AtEnd()) {
        return;
      }

      do {
        runner_++;
      } while (!runner_->HasEntry());
    }

    private: void Start() {
      if (!AtEnd()) {
        count_++;
        runner_--;
        Next();
      }
    }
  };

  DEFINE_ENUMERATOR(HashSet_, Key);

  private: static uint const kDefaultCapacity = 57;
  private: static uint const kDefaultRehashThreshold = 63;

  private: uint capacity_;
  private: uint count_;
  private: uint const rehash_threshold_;
  private: Slot* slots_;

  // ctor
  public: explicit HashSet_(
      uint const capacity = kDefaultCapacity,
      uint const rehash_threshold = kDefaultRehashThreshold)
      : capacity_(capacity),
        count_(0),
        rehash_threshold_(rehash_threshold),
        slots_(new Slot[capacity]) {}

  public: template<class C> explicit HashSet_(
      const C& collection,
      uint const capacity = kDefaultCapacity,
      uint const rehash_threshold = kDefaultRehashThreshold)
      : capacity_(capacity),
        count_(0),
        rehash_threshold_(rehash_threshold),
        slots_(new Slot[capacity]) {
    AddAll(collection);
  }

  public: HashSet_(
      const HashSet_& collection,
      uint const capacity = kDefaultCapacity,
      uint const rehash_threshold = kDefaultRehashThreshold)
      : capacity_(capacity),
        count_(0),
        rehash_threshold_(rehash_threshold),
        slots_(new Slot[capacity]) {
    AddAll(collection);
  }

  public: virtual ~HashSet_() {
    delete[] slots_;
  }

  public: HashSet_& operator=(const HashSet_& r) {
    Clear();
    AddAll(r);
  }

  // [A]
  public: void Add(Key const key) {
    auto& slot = FindSlot(key);

    if (slot.HasEntry()) {
      slot.Set(key);
      return;
    }

    if (count_ * 100 > capacity_ * rehash_threshold_) {
      Rehash();
      return Add(key);
    }

    count_++;
    slot.Set(key);
  }

  public: template<class C> void AddAll(const C& collection) {
    foreach (C::Enum, elems, collection) {
      Add(elems.Get());
    }
  }

  // [C]
  public: void Clear() {
    delete[] slots_;
    count_ = 0;
    slots_ = new Slot[capacity_];
  }

  public: bool Contains(const Key key) const {
    auto& slot = FindSlot(key);
    return slot.HasEntry();
  }

  public: int Count() const { return count_; }

  // [F]
  private: Slot& FindSlot(const Key key) const {
    auto const start = &slots_[ComputeHashCode(key) % capacity_];
    auto const end = &slots_[capacity_];
    auto runner = start;
    Slot* home = nullptr;
    for (;;) {
      if (runner->IsEmpty()) {
        return home ? *const_cast<Slot*>(home) : *const_cast<Slot*>(runner);
      }

      if (runner->IsRemoved()) {
        if (home == nullptr) {
          home = runner;
        }

      } else if (runner->GetKey() == key) {
        return *const_cast<Slot*>(runner);
      }

      runner++;

      if (runner >= end) {
        runner = &slots_[0];
      }

      if (runner == start) {
        CAN_NOT_HAPPEN();
      }
    }
  }

  // [G]
  public: Key Get(const Key key) const {
    auto& slot = FindSlot(key);
    return slot.HasEntry() ? slot.GetKey() : static_cast<Key>(0);
  }

  // [I]
  public: bool IsEmpty() const { return Count() == 0; }

  // [R]
  private: void Rehash() {
    auto const old_slots = slots_;
    auto const old_size = capacity_;
    capacity_ = capacity_ * 3 / 2;
    slots_ = new Slot[capacity_];

    for (
        auto runner = old_slots;
        runner < old_slots + old_size;
        runner++) {
        if (runner->HasEntry()) {
          Add(runner->GetKey());
        }
    }

    delete[] old_slots;
  }

  public: bool Remove(const Key key) {
    auto& slot = FindSlot(key);
    if (slot.HasEntry()) {
      slot.SetRemoved();
      --count_;
      ASSERT(count_ >= 0);
      return true;
    }
    return false;
  }
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_HashSet_h)
