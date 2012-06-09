// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_GcAnchor_h)
#define INCLUDE_Om_GcAnchor_h

#include "./Layout.h"

namespace Om {

class GcAnchor :
    public DoubleLinkedItem_<GcAnchor> {

    friend class Gc;

    public: typedef DoubleLinkedList_<GcAnchor> List;

    public: class Scope {
        private: GcAnchor* const m_p;

        public: Scope(GcAnchor* const p) : m_p(p) { m_p->Lock(); }
        public: ~Scope() { m_p->Unlock(); }
        DISALLOW_COPY_AND_ASSIGN(Scope);
    }; // Scope

    private: Object* m_value;

    // ctor
    public: GcAnchor(Object* const value) :
        m_value(value) {}

    // [C]
    public: static GcAnchor* CreateGcAnchor(Val);

    // [D]
    public: static void DestroyGcAnchor(GcAnchor*);

    // [G]
    public: static GcAnchor::List* Get();
    public: template<class T> T* Get();

    // [L]
    public: void Lock();

    // [S]
    public: static void StaticInit();

    // [U]
    public: void Unlock();
}; // GcAnchor

} // Om

#endif // !defined(INCLUDE_Om_GcAnchor_h)
