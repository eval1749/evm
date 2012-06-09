// @(#)$Id$
//
// Common Collections Hashtable
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_Hashtable_h)
#define INCLUDE_Common_Collections_Hashtable_h

#include "../MemoryZone.h"

namespace Common {
namespace Collections {

template<
    typename TKey,
    typename TValue>
class Hashtable_ {
    private: typedef Hashtable_<TKey, TValue> Hashtable;

    public: struct Entry :
            public Allocable {

        private: TKey* m_pKey;
        private: TValue* m_pValue;

        public: Entry() :
            m_pKey(nullptr),
            m_pValue(nullptr) {}

        public: TKey* GetKey() const { return m_pKey; }
        public: TValue* GetValue() const { return m_pValue; }
        public: void SetKey(TKey* const pKey) { m_pKey = pKey; }
        public: void SetValue(TValue* const pValue) { m_pValue = pValue; }
    }; // Entry

    private: struct Slot :
            public Entry {

        bool HasKey() const { return !IsEmpty() && !IsRemoved(); }

        bool IsEmpty() const { return nullptr == this->GetKey(); }

        bool IsRemoved() const {
            return reinterpret_cast<TKey*>(1) == this->GetKey();
        } // IsRemoved

        void SetRemoved() {
            m_pKey = reinterpret_cast<TKey*>(1);
        } // SetRemoved
    }; // Slot

    public: class Enum {
        private: const Slot* m_pRunner;
        private: int m_iCount;

        public: Enum(const Hashtable* const pHashtable) :
                m_iCount(pHashtable->m_iCount),
                m_pRunner(pHashtable->m_prgoSlot) {
            if (!this->AtEnd()) {
                m_iCount++;
                m_pRunner--;
                Next();
            }
        } // Enum

        public: bool AtEnd() const { return m_iCount == 0; }

        public: const Entry* Get() const {
            ASSERT(!this->AtEnd());
            return m_pRunner;
        } // GetKey

        public: void Next() {
            ASSERT(!AtEnd());
            for (;;) {
                m_pRunner++;
                if (m_pRunner->HasKey()) {
                    break;
                } // if
            } // for
            m_iCount--;
        } // Next
    }; // Enum

    private: uint m_nCapacity;
    private: uint m_iCount;
    private: uint const m_nRehashThreshold;
    private: MemoryZone* const m_pMemoryZone;
    private: Slot* m_prgoSlot;

    // ctor
    public: Hashtable(
        MemoryZone* const pMemoryZone,
        uint const nCapacity = 103,
        uint const nRehashThreshold = 63) :
            m_iCount(0),
            m_nRehashThreshold(nRehashThreshold),
            m_nCapacity(nCapacity),
            m_pMemoryZone(pMemoryZone),
            m_prgoSlot(new(pMemoryZone) Slot[nCapacity]) {}

    private: Hashtable& operator =(Hashtable&);

    // [A]
    public: void Add(
        TKey* const pKey,
        TValue* const pValue) {

        ASSERT(nullptr != pKey);
        ASSERT(nullptr != pValue);

        auto const pStart = &m_prgoSlot[
            pKey->ComputeHashCode() % m_nCapacity];

        auto const pEnd = &m_prgoSlot[m_nCapacity];

        auto pRunner = pStart;

        for (;;) {
            if (pRunner->IsEmpty() || pRunner->IsRemoved()) {
                if (m_iCount * 100 > m_nCapacity * m_nRehashThreshold) {
                    this->Rehash();
                    return this->Add(pKey, pValue);
                } // if

                m_iCount += 1;
                pRunner->SetKey(pKey);
                pRunner->SetValue(pValue);
                return;
            }

            if (pRunner->GetKey()->Equals(pKey)) {
                pRunner->SetValue(pValue);
                return;
            }

            pRunner++;

            if (pRunner >= pEnd) {
                pRunner = &m_prgoSlot[0];
            } // if

            if (pRunner == pStart) {
                CAN_NOT_HAPPEN();
            }
        } // for
    } // Add

    // [C]
    public: int Count() const { return m_iCount; }

    // [G]
    public: TValue* Get(const TKey* pKey) const {
        const Slot* const pStart = &m_prgoSlot[
            pKey->ComputeHashCode() % m_nCapacity];

        const Slot* const pEnd = &m_prgoSlot[m_nCapacity];

        const Slot* pRunner = pStart;

        for (;;) {
            if (pRunner->IsEmpty()) {
                return nullptr;
            }

            if (!pRunner->IsRemoved() && pRunner->GetKey()->Equals(pKey)) {
                return pRunner->GetValue();
            }

            pRunner++;

            if (pRunner >= pEnd) {
                pRunner = &m_prgoSlot[0];
            } // if

            if (pRunner == pStart) {
                return nullptr;
            }
        } // for
    } // Get

    // [R]
    private: void Rehash() {
        auto const prgoOldSlot = m_prgoSlot;
        auto const nOldSize = m_nCapacity;
        m_nCapacity = m_nCapacity * 3 / 2;
        m_prgoSlot = new(m_pMemoryZone) Slot[m_nCapacity];

        for (
            auto pRunner = prgoOldSlot;
            pRunner < prgoOldSlot + nOldSize;
            pRunner++) {
            if (pRunner->HasKey()) {
                this->Add(pRunner->GetKey(), pRunner->GetValue());
            } // if
        } // for pRunner

        delete[] prgoOldSlot;
    } // Reahsh

    bool Removed(const TKey* pKey) {
        for (;;) {
            if (pRunner->IsEmpty()) {
                return false;
            }

            if (!pRunner->IsRemoved() && pRunner->GetKey()->Equals(pKey)) {
                pRunner->SetRemoved();
                return true;
            }

            pRunner++;

            if (pRunner >= pEnd) {
                pRunner = &m_prgoSlot[0];
            } // if

            if (pRunner == pStart) {
                return false;
            }
        } // for
    } // Removed
}; // Hashtable

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_Hashtable_h)
