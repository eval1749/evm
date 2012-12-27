// -*- Mode: C++ -*-
//
// TinyCl - Common Utitlity Classes and Functions
// tinycl/z_util.h
//
// Copyright (C) 2007-2008 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evedit2/mainline/tinycl/z_util.h#10 $
//
#if !defined(INCLUDE_z_util_h)
#define INCLUDE_z_util_h

namespace Common {

class AsInt {
  public: intptr_t ToInt() const {
   return reinterpret_cast<intptr_t>(this);
  }

  public: template<typename T> static T* LibExport FromInt(
      intptr_t const iVal) {
    return reinterpret_cast<T*>(iVal);
  }
}; // AsInt

template<class Item_, class TParent>
class DoubleLinkedList_;

class GlobalObject {};
class DummyParent {};

template<class Item, class Parent = DummyParent>
class DoubleLinkedItem_ {
  friend class DoubleLinkedList_<Item, Parent>;

  private: Item* m_pNext;
  private: Item* m_pPrev;

  public: DoubleLinkedItem_()
      : m_pNext(nullptr),
        m_pPrev(nullptr) {
  }

  public: Item* GetNext() const { return m_pNext; }
  public: Item* GetPrev() const { return m_pPrev; }
}; // DoubleLinkedItem

template<class Item_, class TParent = DummyParent>
class DoubleLinkedList_ {
  protected: typedef DoubleLinkedList_<Item_, TParent> List_;
  private: typedef DoubleLinkedItem_<Item_, TParent> Cons_;
  public: typedef Cons_ Item;

  public: class Iterator {
    private: Item_* item_;
    public: Iterator(Item_* item) : item_(item) {}

    public: Item_& operator*() const { return *item_; }

    public: bool operator ==(const Iterator& another) const {
      return item_ == another.item_;
    }

    public: bool operator !=(const Iterator& another) const {
      return !operator==(another);
    }

    public: Iterator& operator++() {
      item_= static_cast<Cons_*>(item_)->m_pNext;
      return *this;
    }
  };

  private: Item_* m_pFirst;
  private: Item_* m_pLast;

  public: DoubleLinkedList_()
      : m_pFirst(nullptr),
        m_pLast(nullptr) {}

  public: ~DoubleLinkedList_() {
    DeleteAll();
  }

  public: Iterator begin() const { return Iterator(m_pFirst); }
  public: Iterator end() const { return Iterator(nullptr); }

  // [A]
  public: Item_* Append(Item_* const pItem) {
    auto const pCons = static_cast<Cons_*>(pItem);
    ASSERT(!pCons->m_pNext);
    ASSERT(!pCons->m_pPrev);

    pCons->m_pNext = nullptr;
    pCons->m_pPrev = m_pLast;

    if (m_pFirst == nullptr) {
      m_pFirst = pItem;
    }

    if (m_pLast != nullptr) {
      static_cast<Cons_*>(m_pLast)->m_pNext = pItem;
    }

    return m_pLast = pItem;
  }

  // [C]
  public: int Count() const {
    auto n = 0;
    foreach (Enum, oEnum, this) {
      n += 1;
    }
    return n;
  }

  // [D]
  public: Item_* Delete(Item_* const pItem) {
    auto const pCons = static_cast<Cons_*>(pItem);

    Item_* pNext = pCons->m_pNext;
    Item_* pPrev = pCons->m_pPrev;
    if (pNext == nullptr) {
      m_pLast = pPrev;

    } else {
      static_cast<Cons_*>(pNext)->m_pPrev = pPrev;
    }

    if (pPrev == nullptr) {
      m_pFirst = pNext;

    } else {
      static_cast<Cons_*>(pPrev)->m_pNext = pNext;
    }

    pCons->m_pNext = nullptr;
    pCons->m_pPrev = nullptr;

    return pItem;
  }

  public: void DeleteAll() {
    while (auto const pItem = GetFirst()) {
      Delete(pItem);
    }
  }

  // [E]
  public: class Enum {
    private: Item_* cur_;
    public: Enum(const List_& r) : cur_(r.m_pFirst) {}
    public: Enum(const List_* p) : cur_(p->m_pFirst) {}
    public: Item_& operator *() const { return *Get(); }
    public: Item_* operator ->() const { return Get(); }
    public: bool AtEnd() const { return cur_ == nullptr; }
    public: Item_* Get() const { ASSERT(!AtEnd()); return cur_; }
    public: void Next() { 
      ASSERT(!AtEnd());
      cur_ = static_cast<Cons_*>(cur_)->m_pNext;
    }
  };

  public: class EnumReverse {
    private: Item_* cur_;
    public: EnumReverse(const List_& r) : cur_(r.m_pLast) {}
    public: EnumReverse(const List_* p) : cur_(p->m_pLast) {}
    public: Item_& operator *() const { return *Get(); }
    public: Item_* operator ->() const { return Get(); }
    public: bool AtEnd() const { return cur_ == nullptr; }
    public: Item_* Get() const { return cur_; }
    public: void Next() {
      ASSERT(!AtEnd());
      cur_ = static_cast<Cons_*>(cur_)->m_pPrev;
    }
  };

  // [G]
  public: Item_* GetFirst() const { return m_pFirst; }
  public: Item_* GetLast() const { return m_pLast; }

  // [I]
  public: Item_* InsertAfter(Item_* pItem, Item_* pRefItem) {
    ASSERT(pItem != pRefItem);
    auto const pNext = static_cast<Cons_*>(pRefItem)->m_pNext;
    if (pNext == nullptr) {
      m_pLast = pItem;
    } else {
      static_cast<Cons_*>(pNext)->m_pPrev = pItem;
    }

    auto const pCons = static_cast<Cons_*>(pItem);
    ASSERT(!pCons->m_pNext);
    ASSERT(!pCons->m_pPrev);

    pCons->m_pPrev = pRefItem;
    pCons->m_pNext = pNext;
    static_cast<Cons_*>(pRefItem)->m_pNext = pItem;
    return pItem;
  }

  public: Item_* InsertBefore(Item_* const pItem, Item_* const pRefItem) {
    ASSERT(pItem != pRefItem);
    auto const  pPrev = static_cast<Cons_*>(pRefItem)->m_pPrev;
    if (pPrev == nullptr) {
      m_pFirst = pItem;
    } else {
      static_cast<Cons_*>(pPrev)->m_pNext = pItem;
    }

    auto const pCons = static_cast<Cons_*>(pItem);
    ASSERT(!pCons->m_pNext);
    ASSERT(!pCons->m_pPrev);

    pCons->m_pPrev = pPrev;
    pCons->m_pNext = pRefItem;
    static_cast<Cons_*>(pRefItem)->m_pPrev = pItem;
    return pItem;
  }

  public: bool IsEmpty() const {
    return m_pFirst == nullptr;
  }

  // [P]
  public: Item_* Pop() {
    if (auto const pItem = GetFirst()) {
      return Delete(pItem);
    }
    return nullptr;
  }

  public: Item_* Prepend(Item_* pItem) {
    auto const pCons = static_cast<Cons_*>(pItem);
    pCons->m_pNext = m_pFirst;
    pCons->m_pPrev = nullptr;

    if (m_pLast == nullptr) {
      m_pLast = pItem;
    }

    if (m_pFirst != nullptr) {
      static_cast<Cons_*>(m_pFirst)->m_pPrev = pItem;
    }

    return m_pFirst = pItem;
  }

  // [R]
  public: Item_* Replace(Item_* pNew, Item_* pOld) {
    ASSERT(pNew != pOld);
    InsertBefore(pNew, pOld);
    Delete(pOld);
    return pNew;
  }
};


template<class Item_, class TParent>
class ChildList_;

template<class Item_, class TParent>
class ChildItem_ : public DoubleLinkedItem_<Item_, TParent> {
    friend class ChildList_<Item_, TParent>;

    protected: TParent* m_pParent;

    public: ChildItem_(TParent* const p = nullptr) : m_pParent(p) {}

    public: TParent* GetParent() const { return m_pParnet; }
}; // ChildItem_


template<class Item_, class TParent>
class ChildList_ : public DoubleLinkedList_<Item_, TParent> {
    private: typedef DoubleLinkedList_<Item_, TParent> DoubleLinkedList;
    protected: typedef ChildList_<Item_, TParent> ChildList;
    protected: typedef ChildItem_<Item_, TParent> ChildItem;

    // [A]
    public: Item_* Append(Item_* const pItem) {
        DoubleLinkedList::Append(pItem);

        static_cast<ChildItem*>(pItem)->m_pParent =
            static_cast<TParent*>(this);

        return pItem;
    } // Append

    // [D]
    public: Item_* Delete(Item_* const pItem) {
        DoubleLinkedList::Delete(pItem);
        static_cast<ChildItem*>(pItem)->m_pParent = nullptr;
        return pItem;
    } // Delete

    // [I]
    public: Item_* InsertAfter(Item_* const pItem, Item_* const pRefItem) {
        DoubleLinkedList::InsertAfter(pItem, pRefItem);

        static_cast<ChildItem*>(pItem)->m_pParent =
            static_cast<ChildItem*>(pRefItem)->m_pParent;

        return pItem;
    } // InsertAfter

    public: Item_* InsertBefore(Item_* const pItem, Item_* const pRefItem) {
        DoubleLinkedList::InsertBefore(pItem, pRefItem);

        static_cast<ChildItem*>(pItem)->m_pParent =
            static_cast<ChildItem*>(pRefItem)->m_pParent;

        return pItem;
    } // InsertBefore

    // [P]
    public: Item_* Prepend(Item_* const pItem) {
        DoubleLinkedList::Prepend(pItem);

        static_cast<ChildItem*>(pItem)->m_pParent =
            static_cast<TParent*>(this);

        return pItem;
    } // Prepend

    // [R]
    public: Item_* Replace(Item_* const pNew, Item_* const pOld) {
        InsertBefore(pNew, pOld);
        Delete(pOld);
        return pNew;
    } // Replace
};  // ChiildList_

/// <remark>
///   C-String enumerator.
/// </remark>
class EnumChar {
    /// <summary>
    ///   Argument for EnumChar
    /// </summary>
    public: struct Arg {
        const char16* m_pwch;
        const char16* m_pwchEnd;

        Arg(
            const char16* pwch,
            int  cwch ) :
            m_pwch(pwch),
            m_pwchEnd(pwch + cwch) {}
    }; // Arg

    private: const char16*   m_pwch;
    private: const char16*   m_pwchEnd;

    /// <summary>
    ///   Construct C-String enumerator.
    /// </summary>
    public: EnumChar(Arg oArg) :
            m_pwch(oArg.m_pwch),
            m_pwchEnd(oArg.m_pwchEnd) {}

    /// <summary>
    ///  Check enumereator at end.
    /// </summary>
    public: bool AtEnd() const {
        return m_pwch >= m_pwchEnd;
    } // AtEnd

    /// <summary>
    ///  Returns current character
    /// </summary>
    public: char16 Get() const {
        ASSERT(!AtEnd());
        return *m_pwch;
    } // Get

    /// <summary>
    ///  Advance current position
    /// </summary>
    public: void Next() {
        ASSERT(!AtEnd());
        m_pwch++;
    } // AtEnd
}; // EnumChar


template<typename Element>
class EnumArray_ {
  private: Element const* const end_;
  private: Element const* runner_;

  public: EnumArray_(Element const* const start, size_t const num_entries)
      : end_(start + num_entries),
        runner_(start) {
  }
  public: Element operator*() const { return Get(); }
  public: Element* operator->() const { return &Get(); }
  public: bool AtEnd() const { return runner_ >= end_; }
  public: Element Get() const { return *GetPtr(); }

  public: Element* GetPtr() const {
    ASSERT(!AtEnd());
    return const_cast<Element*>(runner_);
  }

  public: void Next() { ASSERT(!AtEnd()); runner_++; }

  DISALLOW_COPY_AND_ASSIGN(EnumArray_);
};

template<typename Element>
class Enumerator_ {
  protected: Enumerator_() {}
  public: Element operator*() const { return Get(); }
  public: virtual bool AtEnd() const = 0;
  public: virtual Element Get() const = 0;
  public: virtual void Next() = 0;
  DISALLOW_COPY_AND_ASSIGN(Enumerator_);
};

template<typename Element>
class Enum_ {
  private: Enumerator_<Element>* enum_;
  public: Enum_(Enumerator_<Element>& enm) : enum_(&enm) {}
  public: Element operator*() const { return Get(); }
  public: bool AtEnd() const { return enum_->AtEnd(); }
  public: Element Get() const { return enum_->Get(); }
  public: void Next() { enum_->Next(); }
  DISALLOW_COPY_AND_ASSIGN(Enum_);
};

#define DEFINE_ENUMERATOR(Container, Element) \
  public: class Enumerator : public Enumerator_<Element> { \
    private: Enum enum_; \
    public: Enumerator(const Container& obj) : enum_(obj) {} \
    public: Enumerator(const Container* obj) : enum_(obj) {} \
    public: virtual bool AtEnd() const override { return enum_.AtEnd(); } \
    public: virtual Element Get() const override { return enum_.Get(); } \
    public: virtual void Next() override { enum_.Next(); } \
    DISALLOW_COPY_AND_ASSIGN(Enumerator); \
  }

template<typename T>
class RefCounted_ {
  private: mutable int ref_count_;
  protected: RefCounted_() : ref_count_(0) {}
  public: int ref_count() const { return ref_count_; }
  public: void AddRef() const { ++ref_count_; }

  public: void Release() const {
    ASSERT(ref_count_ > 0);
    --ref_count_;
    if (ref_count_ == 0) {
      delete static_cast<T*>(const_cast<RefCounted_*>(this));
    }
  }

  DISALLOW_COPY_AND_ASSIGN(RefCounted_);
};

template<typename T>
class ScopedArray_ {
  private: T* const p_;
  public: explicit ScopedArray_(T* const p) : p_(p) {}
  public: ~ScopedArray_() { delete[] p_; }
  public: operator T*() const { return p_; }
  public: T* Detach() { p_ = nullptr; }
  public: T* Get() const { return p_; }
  DISALLOW_COPY_AND_ASSIGN(ScopedArray_);
};

template<typename T>
class ScopedPtr_ {
  private: T* p_;
  public: explicit ScopedPtr_(T* const p) : p_(p) {}
  public: ~ScopedPtr_() { delete p_; }
  public: operator T*() const { return p_; }
  public: T* operator->() const { return p_; }
  public: T& operator*() const { return *p_; }
  public: T* Detach() { T* ret = p_; p_ = nullptr; return ret; }
  public: T* Get() const { return p_; }
  DISALLOW_COPY_AND_ASSIGN(ScopedPtr_);
};

// For ease of use, we don't specify "explicit" to ctor.
template<typename T>
class ScopedRefCount_ {
  private: T* p_;
  public: ScopedRefCount_(T* const p) : p_(p) { p_->AddRef(); }
  public: ScopedRefCount_(T& r) : p_(&r) { p_->AddRef(); }
  public: ~ScopedRefCount_() { if (p_) p_->Release(); }
  public: T* operator->() const { return p_; }
  public: T& operator*() const { return *p_; }
  public: T* Detach() { p_ = nullptr; }
  public: T* Get() const { return p_; }
  DISALLOW_COPY_AND_ASSIGN(ScopedRefCount_);
};

template<class Node_, class ConsBase_ = GlobalObject>
class SingleLinkedList_ {
    private: typedef SingleLinkedList_<Node_, ConsBase_> List;

    public: struct Cons : public ConsBase_ {
        Node_*  m_pNode;
        Cons*   m_pNext;

        Cons(Node_* p, Cons* q = nullptr) :
            m_pNode(p),
            m_pNext(q) {}
    }; // Cons

    private: Cons*  m_pFirst;

    // ctor
    public: SingleLinkedList_() : m_pFirst(nullptr) {}

    // [D]
    public: void DeleteAll()
        { m_pFirst = nullptr; }

    // [E]
    public: class Enum
    {
        private: Cons* m_pRunner;

        public: Enum(const List* pList) :
            m_pRunner(pList->GetFirst()) {}

        public: bool AtEnd() const { return m_pRunner == nullptr; }

        public: Node_* Get() const
        {
            ASSERT(!AtEnd());
            return m_pRunner->m_pNode;
        } // Get

        public: void Next()
        {
            ASSERT(!AtEnd());
            m_pRunner = m_pRunner->m_pNext;
        } // Next
    }; // Enum

    // [G]
    public: Cons* GetFirst() const
        { return m_pFirst; }

    // [I]
    public: bool IsEmpty() const
        { return m_pFirst == nullptr; }

    // [P]
    public: int Position(Node_* pNode) const
    {
        int iPosn = 0;
        foreach (Enum, oEnum, this)
        {
            when (oEnum.Get() == pNode) return iPosn;
            iPosn += 1;
        } // for each cons
        return -1;
    } // Position

    // [R]
    public: void Reverse()
    {
        Cons* pList = m_pFirst;
        Cons* pRunner = nullptr;
        while (pList != nullptr)
        {
            // (rotatef (cdr list) runner list)
            Cons* pTemp = pList->m_pNext;
            pList->m_pNext = pRunner;
            pRunner = pList;
            pList = pTemp;
        } // for
        m_pFirst = pRunner;
    } // Reverse

    // [S]
    public: Cons* SetFirst(Cons* pCons)
        { return m_pFirst = pCons; }
}; // SingleLinkedList

template<class T>
class Castable_ {
  // [D]
  public: template<class T> T* DynamicCast() const {
    if (this == nullptr) { return nullptr; }
    auto p = static_cast<T*>(const_cast<Castable_*>(this));
    return p->Is_(T::Kind_()) ? reinterpret_cast<T*>(p) : nullptr;
  } // DynamicCast

  // [G]
  public: virtual const char* GetKind() const = 0;

  // [I]
  public: template<class T> bool Is() const {
    return this != nullptr && Is_(T::Kind_());
  } // Is

  public: virtual bool Is_(const char*) const {
    return false;
  }

  // [S]
  public: template<class T> T* StaticCast() const {
    auto p = DynamicCast<T>();
    ASSERT(p != nullptr);
    return p;
  } // StaticCast
}; // Castable_

#define CASTABLE_CLASS(mp_name) \
  public: static const char* LibExport Kind_() { return #mp_name; }

#define CASTABLE_CLASS_(self, base) \
  public: static const char* LibExport Kind_() { return #self; } \
  public: virtual const char* GetKind() const override { return Kind_(); } \
  public: virtual bool Is_(const char* name) const override { \
    return self::GetKind() == name || base::Is_(name); \
  }

template<class T, class B>
class WithCastable_ : public B {
  protected: typedef WithCastable_<T, B> Base;

  protected: WithCastable_() {}

  // [G]
  public: virtual const char* GetKind() const override {
    return T::Kind_();
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
   auto const pszT = T::Kind_();
   return pszT == psz || B::Is_(psz);
  } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable_);
}; // WithCastable_

template<class T, class B, typename P1>
class WithCastable1_ : public B {
  protected: typedef WithCastable1_<T, B, P1> Base;
  protected: WithCastable1_(P1 p1) : B(p1) {}

  // [G]
  public: virtual const char* GetKind() const override {
    return T::Kind_(); 
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
     auto const pszT = T::Kind_();
     return pszT == psz || B::Is_(psz);
  } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable1_);
}; // WithCastable1_

template<class T, class B, typename P1, typename P2>
class WithCastable2_ : public B {
  protected: typedef WithCastable2_<T, B, P1, P2> Base;

  protected: WithCastable2_(P1 p1, P2 p2)
      : B(p1, p2) {
  }

  // [G]
  public: virtual const char* GetKind() const override {
    return T::Kind_();
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
    auto const pszT = T::Kind_();
    return pszT == psz || B::Is_(psz);
  } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable2_);
}; // WithCastable2_

template<class T, class B, typename P1, typename P2, typename P3>
class WithCastable3_ : public B {
  protected: typedef WithCastable3_<T, B, P1, P2, P3> Base;

  protected: WithCastable3_(P1 p1, P2 p2, P3 p3)
      : B(p1, p2, p3) {
  }

  // [G]
  public: virtual const char* GetKind() const override {
    return T::Kind_();
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
        auto const pszT = T::Kind_();
        return pszT == psz || B::Is_(psz);
    } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable3_);
}; // WithCastable3_

template<class T, class B, typename P1, typename P2, typename P3, typename P4>
class WithCastable4_ : public B {
  protected: typedef WithCastable4_<T, B, P1, P2, P3, P4> Base;

  protected: WithCastable4_(P1 p1, P2 p2, P3 p3, P4 p4)
      : B(p1, p2, p3, p4) {
  }

  // [G]
  public: virtual const char* GetKind() const {
    return T::Kind_();
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
    auto const pszT = T::Kind_();
    return pszT == psz || B::Is_(psz);
  } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable4_);
}; // WithCastable4_

template<
    class T,
    class B,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5>
class WithCastable5_ : public B {
  protected: typedef WithCastable5_<T, B, P1, P2, P3, P4, P5> Base;

  protected: WithCastable5_(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
      : B(p1, p2, p3, p4, p5) {
  }

  // [G]
  public: virtual const char* GetKind() const override {
    return T::Kind_();
  }

  // [I]
  public: virtual bool Is_(const char* psz) const override {
    return T::Kind_() == psz || B::Is_(psz);
  } // Is_

  DISALLOW_COPY_AND_ASSIGN(WithCastable5_);
}; // WithCastable5_

template<class Node_>
class WorkListItem_;

template<class Item_>
class WorkList_ {
  protected: typedef WorkListItem_<Item_> Cons;

  protected: Item_* m_pHead;

  // cotr
  public: WorkList_() : m_pHead(nullptr) {}
  public: ~WorkList_() { MakeEmpty(); }

  // [D]
  public: void Delete(Item_* const pItem) {
    ASSERT(pItem != nullptr);
    ASSERT(pItem->IsInList());

    if (m_pHead == nullptr) {
        // This WorkList is empty.
        CAN_NOT_HAPPEN();
    }

    if (m_pHead == pItem) {
      this->Pop();
      return;
    }

    auto pLast = m_pHead;
    for (;;) {
      ASSERT(pLast != nullptr);

      auto const pLastNext = static_cast<Cons*>(pLast)->m_pNext;
      if (pLastNext == pLast) {
        // We reach at end of WorkList.
        CAN_NOT_HAPPEN();
      }

      if (pLastNext == pItem) {
        auto const pItemNext = static_cast<Cons*>(pItem)->m_pNext;
        if (pItemNext == pItem) {
          static_cast<Cons*>(pLast)->m_pNext = pLast;
        } else  {
          static_cast<Cons*>(pLast)->m_pNext = pItemNext;
        }
        static_cast<Cons*>(pItem)->m_pNext = nullptr;
        return;
      }

      pLast = pLastNext;
    } // for
  } // Delete

  // [E]
  public: class Enum {
    private: Item_* m_pRunner;

    public: Enum(const WorkList_<Item_>* pList)
        : m_pRunner(pList->m_pHead) {}

    public: bool AtEnd() const { return m_pRunner == nullptr; }
    public: Item_* Get() const { ASSERT(!AtEnd()); return m_pRunner; }

    public: void Next() {
      ASSERT(!AtEnd());
      auto pNext = static_cast<Cons*>(m_pRunner)->m_pNext;
      if (pNext == m_pRunner) {
        pNext = nullptr;
      }
      m_pRunner = pNext;
    } // Next
  }; // Enum

  // [G]
  public: Item_* Get() const { ASSERT(!IsEmpty()); return m_pHead; }

  // [I]
  public: template<typename T> Item_* Insert(Item_* const pItem) {
    auto ppRunner = &m_pHead;
    for (;;) {
      auto pRunner = *ppRunner;
      if (pRunner == nullptr) {
        static_cast<Cons*>(pItem)->m_pNext = pItem;
        break;
      }

      if (T::GreaterThan(pItem, pRunner)) {
        static_cast<Cons*>(pItem)->m_pNext = pRunner;
        break;
      }

      ppRunner = &static_cast<Cons*>(pRunner)->m_pNext;

      if (static_cast<Cons*>(pRunner)->m_pNext == pRunner) {
        static_cast<Cons*>(pItem)->m_pNext = pItem;
        break;
      }
    } // for

    *ppRunner = pItem;
    return pItem;
  } // Insert

  public: bool IsEmpty() const { return m_pHead == nullptr; }
  public: bool IsNotEmpty() const { return m_pHead != nullptr; }

  // [M]
  public: void MakeEmpty() {
    while (!IsEmpty()) {
      Pop();
    }
  } // MakeEmpty

  // [P]
  public: Item_* Pop() {
    Item_* pItem = m_pHead;
    ASSERT(pItem != nullptr);

    m_pHead = static_cast<Cons*>(pItem)->m_pNext;
    static_cast<Cons*>(pItem)->m_pNext = nullptr;
    if (m_pHead == pItem) {
      m_pHead = nullptr;
    }
    return pItem;
  } // Pop

  public: Item_* Push(Item_* const pItem) {
    ASSERT(pItem != nullptr);
    ASSERT(static_cast<Cons*>(pItem)->m_pNext == nullptr);

    if (m_pHead == nullptr) {
      static_cast<Cons*>(pItem)->m_pNext = pItem;
    } else {
      static_cast<Cons*>(pItem)->m_pNext = m_pHead;
    }

    return m_pHead = pItem;
  } // Push

  DISALLOW_COPY_AND_ASSIGN(WorkList_);
}; // WorkList_

template<class Node_>
class WorkListItem_ {
  friend class WorkList_<Node_>;

  private: Node_* m_pNext;

  public: WorkListItem_() : m_pNext(nullptr) {}

  // [I]
  public: bool IsInList() const { return m_pNext != nullptr; }

  DISALLOW_COPY_AND_ASSIGN(WorkListItem_);
}; // WorkListItem_

} // Common

char16* LibExport lstrchrW(const char16*, char16);
char16* LibExport lstrrchrW(const char16*, char16);

#endif //!defined(INCLUDE_z_util_h)
