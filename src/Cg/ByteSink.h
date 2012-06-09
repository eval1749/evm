// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_ByteSink_h)
#define INCLUDE_Il_Cg_ByteSink_h

//#include "../../Common/LocalMemoryZone.h"

namespace Il {
namespace Cg {

class ByteSink : public Allocable {
    private: uint   m_cbAlloc;
    private: uint   m_cb;
    private: uint8* m_prgb;
    private: MemoryZone* const m_pZone;

    // ctor
    public: ByteSink(MemoryZone* pZone) :
        m_cbAlloc(0),
        m_cb(0),
        m_pZone(pZone) {}

    private: ByteSink& operator=(ByteSink&);

    // [C]
    public: void CopyTo(ByteSink* const that) const {
        if (m_cb + that->m_cb > that->m_cbAlloc) {
            that->enlarge(m_cb);
        }

        ::CopyMemory(that->m_prgb + that->m_cb, m_prgb, m_cb);

        that->m_cb += m_cb;
    } // CopyTo

    // [E]
    public: void EmitU16(uint16 u16) {
        EmitU8(static_cast<uint8>(u16 & 0xff));
        EmitU8(static_cast<uint8>((u16 >> 8) & 0xff));
    } // EmitU16

    public: void EmitU32(uint32 u32) {
        EmitU16(static_cast<uint16>(u32 & 0xffff));
        EmitU16(static_cast<uint16>((u32 >> 16) & 0xffff));
    } // EmitU16

    public: void EmitU8(uint8 u8) {
        if (m_cb + 1 > m_cbAlloc) {
            enlarge(1);
        }

        m_prgb[m_cb] = u8;
        m_cb += 1;
    } // EmitU8

    private: void enlarge(uint const cb) {
        m_cbAlloc = max((m_cbAlloc + cb) * 130 / 100, 256);
        uint8* prgb = m_prgb;
        m_prgb = reinterpret_cast<uint8*>(m_pZone->Alloc(m_cbAlloc));
        ::CopyMemory(m_prgb, prgb, m_cb);
    } // enlarge

    public: class Enum {
        uint8* m_pEnd;
        uint8* m_p;

        public: Enum(const ByteSink* const p) :
            m_pEnd(p->m_prgb + p->m_cb),
            m_p(p->m_prgb) {}

        public: bool AtEnd() const
            { return m_p >= m_pEnd; }

        public: uint8 Get() const
            { ASSERT(! AtEnd()); return *m_p; }

        public: void Next()
            { ASSERT(! AtEnd()); m_p++; }
    }; // Enum

    // [G]
    public: uint GetPosn() const
        { return m_cb; }

    // [S]
    public: void Serialize(void* const pv) {
        ::CopyMemory(pv, m_prgb, m_cb);
    } // Serialize
}; // ByteSink


} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_ByteSink_h)
