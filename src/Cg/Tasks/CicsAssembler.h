// @(#)$Id$
//
// Evita Ee - Cg
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_CicsAssembler_h)
#define INCLUDE_Il_Cg_CicsAssembler_h

#include "./CgAssembler.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

class CicsAssembler : public Assembler {
    protected: class CodeBuf {
        enum Limits
        {
            BufferSize = 1024 * 16,
        }; // Limits

        protected: uint8*  m_prgbBuf;
        public: int m_cb;
        public: int m_nAddr;
        protected: Spans m_oSpans;

        // ctor
        public: CodeBuf() {
            m_prgbBuf = new uint8[BufferSize];
            Reset();
        } // CodeBuf

        // [A]
        public: void AddSpan(Span* pSpan)
            { m_oSpans.Append(pSpan); }

        // [E]
        public: void EmitU16(uint16 u16) {
            ASSERT(m_cb + 2 < BufferSize);
            // TODO yosi@msn.com 2007-10-08 How do we implement
            // bigendian?
            m_prgbBuf[m_cb + 0] = static_cast<uint8>((u16 >>  0) & 0xff);
            m_prgbBuf[m_cb + 1] = static_cast<uint8>((u16 >>  8) & 0xff);
            m_cb += 2;
            m_nAddr += 2;
        }

        public: void EmitU32(uint32 u32)
        {
            ASSERT(m_cb + 4 < BufferSize);
            // TODO yosi@msn.com 2007-10-08 How do we implement
            // bigendian?
            m_prgbBuf[m_cb + 0] = static_cast<uint8>((u32 >>  0) & 0xff);
            m_prgbBuf[m_cb + 1] = static_cast<uint8>((u32 >>  8) & 0xff);
            m_prgbBuf[m_cb + 2] = static_cast<uint8>((u32 >> 16) & 0xff);
            m_prgbBuf[m_cb + 3] = static_cast<uint8>((u32 >> 24) & 0xff);
            m_cb += 4;
            m_nAddr += 4;
        } // EmitU32

        public: void EmitU64(uint64 u64) {
          EmitU32(static_cast<uint32>((u64 >> 0) & 0xFFFFFFFFul));
          EmitU32(static_cast<uint32>((u64 >> 32) & 0xFFFFFFFFul));
        }

        public: void EmitU8(uint8 u8)
        {
            ASSERT(m_cb < BufferSize);
            m_prgbBuf[m_cb] = u8;
            m_cb += 1;
            m_nAddr += 1;
        } // EmitU8

        public: class EnumSpan : public Spans::Enum
        {
            public: EnumSpan(const CodeBuf* p) :
                Spans::Enum(&p->m_oSpans) {}
        }; // EnumSpan

        // [G]
        public: const uint8* GetCode(int ofs) const
        {
            ASSERT(ofs <= m_cb);
            return m_prgbBuf + ofs;
        } // GetCode

        public: int GetAddr()   const { return m_nAddr; }
        public: int GetOffset() const { return m_cb; }

        // [P]
        public: void PatchS32(int ofs, int32 s32)
        {
            ASSERT(ofs + 4 < m_cb);
            // TODO yosi@msn.com 2007-10-08 How do we implement
            // bigendian?
            m_prgbBuf[ofs + 0] = static_cast<uint8>((s32 >>  0) & 0xff);
            m_prgbBuf[ofs + 1] = static_cast<uint8>((s32 >>  8) & 0xff);
            m_prgbBuf[ofs + 2] = static_cast<uint8>((s32 >> 16) & 0xff);
            m_prgbBuf[ofs + 3] = static_cast<uint8>((s32 >> 24) & 0xff);
        } // PatchU8

        public: void PatchU8(int ofs, uint8 u8)
        {
            ASSERT(ofs < m_cb);
            ASSERT(ofs < BufferSize);
            m_prgbBuf[ofs] = u8;
        } // PatchU8

        // [R]
        public: void Reset()
        {
            m_cb    = 0;
            m_nAddr = 0;
            m_oSpans.DeleteAll();
        } // Reset

        public: void Reserve(int n)
            { m_cb += n; }
    }; // CodeBuf

    protected: CodeBuf  m_oCodeBuf;

    protected: struct JumpSpan : public Span_<JumpSpan> {
        CASTABLE_CLASS(JumpSpan);

        public: bool    m_fLongForm;
        public: int     m_opLong;
        public: int     m_opShort;
        public: BBlock* m_pTargetBB;

        // ctor
        public: JumpSpan(
            int     iAddr,
            int     ofs,
            bool    fLongForm,
            int     opShort,
            int     opLong,
            BBlock*     pTargetBB) :
                m_fLongForm(fLongForm),
                m_opLong(opLong),
                m_opShort(opShort),
                m_pTargetBB(pTargetBB),
                Base(iAddr, ofs) {}

        // [G]
        public: int GetTarget() const
            { return m_pTargetBB->GetWork<BBlockExt>()->m_nAddr; }

        // [I]
        public: bool IsCrossing(int nAddr) const {
            if (GetAddr() < nAddr) {
                //      JUMP L1
                //      -- iAddr --
                //  L1: ...
                return GetTarget() >= nAddr;
            } else {
                // L1:  ...
                //      -- iAddr --
                //      JUMP L1
                return GetTarget() < nAddr;
            }
        } // IsCrossing
        DISALLOW_COPY_AND_ASSIGN(JumpSpan);
    }; // JumpSpan

    // ctor
    protected: CicsAssembler() {}

    // [A]
    protected: void Annotate(
        Annot::Kind const eKind,
        intptr_t const iDatum);

    // [E]
    protected: void emitJump(
        int const opShort,
        int const opLong,
        BBlock* const pTargetBB)
    {
        bool fLongForm = false;

        if (BBlockExt* const pExt = pTargetBB->GetWork<BBlockExt>()) {
            if (0 != pExt->GetAddr()) {
                int const iRel = pExt->GetAddr() - (m_oCodeBuf.GetAddr() + 2);
                fLongForm = !isS8(iRel);
            }
        } // if

        // We always allocate 6 byte for jump instruction
        if (fLongForm) {
            emitOp(opLong);
            emitU32(0);
            if (opLong <= 0xFF) {
                m_oCodeBuf.Reserve(1);
            }

        } else {
            emitOp(opShort);
            emitU8(0);
            m_oCodeBuf.Reserve(4);
        }

        m_oCodeBuf.AddSpan(
            new(this) JumpSpan(
                m_oCodeBuf.GetAddr(),
                m_oCodeBuf.GetOffset(),
                fLongForm,
                opShort,
                opLong,
                pTargetBB));
    } // emitJump

    protected: void emitOp(int const opcode) {
        if (opcode > 0xFFFF) {
            emitU8(static_cast<uint8>((opcode >> 16) & 0xff));
        }

        if (opcode > 0xFF) {
            emitU8(static_cast<uint8>((opcode >> 8) & 0xff));
        }

        emitU8(static_cast<uint8>(opcode & 0xff));
    } // emitOp

    protected: void emitU32(uint32 u32)
        { m_oCodeBuf.EmitU32(u32); }

    protected: void emitU8(uint8 u8)
        { m_oCodeBuf.EmitU8(u8); }

    // [F]
    protected: void FixSpans();

    // [S]
    protected: void SerializeCode(void* const pvCode);

    DISALLOW_COPY_AND_ASSIGN(CicsAssembler);
}; // CicsAssembler

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Cg_CicsAssembler_h)
