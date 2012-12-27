// @(#)$Id$
//
// Common Collections HashMap_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_HashMap_h)
#define INCLUDE_Common_Collections_HashMap_h

#include "../GlobalMemoryZone.h"

namespace Common {
namespace Collections {

template<typename Key, typename Value, class TAllocable = Allocable>
class HashMap_  : public TAllocable {
  private: typedef HashMap_<Key, Value> HashMap;

  public: struct Entry : public Allocable {
    private: Key key_;
    private: Value value_;

    public: Entry() {}

    public: Entry(Key key, Value value)
      : key_(key),
        value_(value) {
    }

    public: Key GetKey() const { return key_; }
    public: Value GetValue() const { return value_; }
    public: void SeKey(Key const key) { key_ = key; }
    public: void SeValue(Value const value) { value_ = value; }
  }; // Entry

  private: struct Slot : public Entry {
    private: enum State {
      State_Empty,
      State_HasEntry,
      State_Removed,
    }; // State

    private: State state_;

    public: Slot() : state_(State_Empty) {}

    bool HasEntry() const { return state_ == State_HasEntry; }
    bool IsEmpty() const { return state_ == State_Empty; }
    bool IsRemoved() const { return state_ == State_Removed; }

    void Set(Key key, Value value)  {
      SeKey(key);
      SeValue(value);
      state_ = State_HasEntry;
    }

    void SetRemoved() { state_ = State_Removed; }
  }; // Slot

  public: class Enum {
    private: const Slot* runner_;
    private: int count_;

    public: Enum(const HashMap_& hash_map_)
        : count_(hash_map_.count_),
          runner_(hash_map_.slots_) {
      Start();
    }

    public: Enum(const HashMap_* const hash_map_)
        : count_(hash_map_->count_),
          runner_(hash_map_->slots_) {
      Start();
    }

    public: Entry operator*() const { return Get(); }
    public: bool AtEnd() const { return 0 == count_; }
    public: Entry Get() const { ASSERT(!AtEnd()); return *runner_; }

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
  }; // Enum

  DEFINE_ENUMERATOR(HashMap_, Entry);

  public: class ConstIterator {
    private: size_t count_;
    private: const HashMap_* hash_map_;
    private: const Slot* runner_;

    public: ConstIterator(const HashMap_& hash_map, size_t count)
        : count_(count), hash_map_(&hash_map), runner_(hash_map.slots_) {
      if (count_) {
        ++count_;
        --runner_;
        operator++();
      }
    }

    public: Entry operator*() const {
      DCHECK_GT(count_, 0u);
      return *runner_;
    }

    public: bool operator==(const ConstIterator& another) const {
      DCHECK_EQ(hash_map_, another.hash_map_);
      return count_ == another.count_;
    }

    public: bool operator!=(const ConstIterator& another) const {
      return !operator==(another);
    }

    public: ConstIterator& operator++() {
      DCHECK_GT(count_, 0u);
      --count_;
      if (!count_)
        return *this;
      do {
        ++runner_;
      } while(!runner_->HasEntry());
      return *this;
    }
  };

  public: class Iterator {
    private: size_t count_;
    private: HashMap_* hash_map_;
    private: const Slot* runner_;

    public: Iterator(HashMap_& hash_map, size_t count)
        : count_(count), hash_map_(&hash_map), runner_(hash_map.slots_) {
      if (count_) {
        ++count_;
        --runner_;
        operator++();
      }
    }

    public: Entry operator*() const {
      DCHECK_GT(count_, 0u);
      return *runner_;
    }

    public: bool operator==(const Iterator& another) const {
      DCHECK_EQ(hash_map_, another.hash_map_);
      return count_ == another.count_;
    }

    public: bool operator!=(const Iterator& another) const {
      return !operator==(another);
    }

    public: Iterator& operator++() {
      DCHECK_GT(count_, 0u);
      --count_;
      if (!count_)
        return *this;
      do {
        ++runner_;
      } while(!runner_->HasEntry());
      return *this;
    }
  };

  // zone_ should be the first member variable for initialization.
  private: MemoryZone& zone_;

  private: uint capacity_;
  private: uint count_;
  private: uint const rehash_threshold_;
  private: Slot* slots_;

  // ctor
  public: HashMap_(
      MemoryZone& zone,
      uint const capacity = 103,
      uint const rehash_threshold = 63)
      : count_(0),
        rehash_threshold_(rehash_threshold),
        capacity_(capacity),
        slots_(new(zone) Slot[capacity]),
        zone_(zone) {
  }

  public: HashMap_(
      uint const capacity = 103,
      uint const rehash_threshold = 63)
      : count_(0),
          rehash_threshold_(rehash_threshold),
          capacity_(capacity),
          slots_(new(zone_) Slot[capacity]),
          zone_(GlobalMemoryZone::Get()) {
  }

  public: ~HashMap_() {
    delete[] slots_;
  }

  public: Iterator begin() { return Iterator(*this, count_); }
  public: ConstIterator begin() const { return ConstIterator(*this, count_); }
  public: Iterator end() { return Iterator(*this, 0); }
  public: ConstIterator end() const { return ConstIterator(*this, 0); }

  // [A]
  public: void Add(Key const key, Value const value) {
    auto const slot = FindSlot(key);

    if (slot->HasEntry()) {
      slot->Set(key, value);
      return;
    }

    if (count_ * 100 > capacity_ * rehash_threshold_) {
      Rehash();
      return Add(key, value);
    }

    count_++;
    slot->Set(key, value);
  }

  // [C]
  public: void Clear() {
    count_ = 0;
    delete[] slots_;
    slots_ = new(zone_) Slot[capacity_];
  }

  public: bool Contains(const Key key) const {
    auto const slot = FindSlot(key);
    return slot->HasEntry();
  }

  public: int Count() const { return count_; }

  // [F]
  private: Slot* FindSlot(const Key key) const {
    auto const start = &slots_[ComputeHashCode(key) % capacity_];
    auto const end = &slots_[capacity_];
    auto runner = start;
    Slot* home = nullptr;
    for (;;) {
      if (runner->IsEmpty()) {
        return home ? home : runner;
      }

      if (runner->IsRemoved()) {
        if (home == nullptr) {
          home = runner;
        }

      } else if (runner->GetKey() == key) {
        return runner;
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
  public: Value Get(const Key key) const {
    auto const slot = FindSlot(key);
    return slot->HasEntry() ? slot->GetValue() : static_cast<Value>(0);
  }

  // [I]
  public: bool IsEmpty() const { return Count() == 0; }

  // [R]
  private: void Rehash() {
    DEBUG_PRINTF("count=%d/%d\n", count_, capacity_);
    auto const old_slots = slots_;
    auto const old_size = capacity_;
    capacity_ = capacity_ * 3 / 2;
    slots_ = new(zone_) Slot[capacity_];

    auto rest = count_;
    count_ = 0;
    for (
        auto runner = old_slots;
        runner < old_slots + old_size;
        runner++) {
      if (runner->HasEntry()) {
       Add(runner->GetKey(), runner->GetValue());
       --rest;
       if (!rest) {
         break;
       }
      }
    }

    delete[] old_slots;
  }

  public: bool Remove(const Key key) {
    auto const slot = FindSlot(key);
    if (slot->HasEntry()) {
        slot->SetRemoved();
        --count_;
        ASSERT(count_ >= 0);
        return true;
    }
    return false;
  }

  DISALLOW_COPY_AND_ASSIGN(HashMap_);
}; // HashMap_

}
}

#endif // !defined(INCLUDE_Common_Collections_HashMap_h)
