#include "precomp.h"
// @(#)$Id$
//
// Evita Ee - Cg
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CicsAssembler.h"

namespace Il {
namespace Cg {

// [A]
void CicsAssembler::Annotate(
    Annot::Kind const eKind,
    intptr_t const iDatum) {

    auto const pFunExt = m_pFun->GetWork<FunExt>();

    pFunExt->m_oAnnots.Append(
        new(this) Annot(
            eKind,
            m_oCodeBuf.GetAddr(),
            m_oCodeBuf.GetOffset(),
            iDatum));
} // Annotate

// [F]
void CicsAssembler::FixSpans() {
    class Resolver {
        private: CodeBuf*   m_pCodeBuf;
        private: Function*  m_pFun;

        // ctor
        private: Resolver(CodeBuf* pCodeBuf, Function* pFun) :
            m_pCodeBuf(pCodeBuf),
            m_pFun(pFun) {}

        // Entry point
        public: static void Run(CodeBuf* pCodeBuf, Function* pFun) {
            Resolver oResolver(pCodeBuf, pFun);
            oResolver.run();
        } // Run

        // [A]
        private: void addCrossing(
            WorkList_<Span>* const pWorkList,
            int const nAddr) {
            foreach (CodeBuf::EnumSpan, oEnum, m_pCodeBuf)
            {
                Span* const pSpan = oEnum.Get();
                if (pSpan->IsInList()) continue;

                if (pSpan->IsCrossing(nAddr))
                {
                    pWorkList->Push(pSpan);
                }
            } // for each span
        } // addCrossing

        // [C]
        private: void compute() {
            WorkList_<Span> oWorkList;
            foreach (CodeBuf::EnumSpan, oEnum, m_pCodeBuf) {
                oWorkList.Push(oEnum.Get());
            } // for each span

            while (!oWorkList.IsEmpty()) {
                auto const pSpan = oWorkList.Pop();
                if (auto const pJumpS = pSpan->DynamicCast<JumpSpan>()) {
                    this->computeJumpSpan(&oWorkList, pJumpS);
                } else {
                    COMPILER_INTERNAL_ERROR();
                    break;
                }
            } // while
        } // compute

        private: void computeJumpSpan(
            WorkList_<Span>* const pWorkList,
            JumpSpan* const pJumpS)
        {
            if (pJumpS->m_fLongForm) {
                return;
            }

            int const iRel = pJumpS->GetTarget() - pJumpS->GetAddr();
            if (!isS8(iRel))
            {
                addCrossing(pWorkList, pJumpS->GetAddr());
                auto const cb = pJumpS->m_opLong > 0xFF ? 4 : 3;
                pJumpS->m_fLongForm = true;
                updateLabels(pJumpS->GetAddr(), cb);
            }
        } // computeJumpSpan

        // [P]
        private: void processJumpSpan(JumpSpan* const pJumpS) {
            auto const nTarget = pJumpS->GetTarget();
                        auto const ofsInsn = pJumpS->GetOffset() - 6;

            if (!pJumpS->m_fLongForm) {
                // Short form
                m_pCodeBuf->PatchU8(
                    ofsInsn,
                    static_cast<uint8>(pJumpS->m_opShort & 0xff));

                m_pCodeBuf->PatchU8(
                    ofsInsn + 1,
                    static_cast<uint8>(nTarget - pJumpS->GetAddr()));

            } else if (pJumpS->m_opLong <= 0xFF) {
                // Long form op <= 0xFF
                m_pCodeBuf->PatchU8(
                    ofsInsn,
                    static_cast<uint8>(pJumpS->m_opLong & 0xff));

                m_pCodeBuf->PatchS32(
                    ofsInsn + 1,
                    nTarget - pJumpS->GetAddr());

            } else {
                // Long form op >= 0xFF
                m_pCodeBuf->PatchU8(
                    ofsInsn,
                    static_cast<uint8>((pJumpS->m_opLong >> 8) & 0xff));

                m_pCodeBuf->PatchU8(
                    ofsInsn + 1,
                    static_cast<uint8>(pJumpS->m_opLong & 0xff));

                m_pCodeBuf->PatchS32(
                    ofsInsn + 2,
                    nTarget - pJumpS->GetAddr());
            }
        } // processJumpSpan

        // [R]
        private: void run() {
            this->compute();

            foreach (CodeBuf::EnumSpan, oEnum, m_pCodeBuf) {
                auto const pSpan = oEnum.Get();

                if (auto const pJumpS = pSpan->DynamicCast<JumpSpan>()) {
                    processJumpSpan(pJumpS);

                } else {
                    COMPILER_INTERNAL_ERROR();
                }
            } // for each span
        } // run

        // [U]
        private: void updateLabels(int nAddr, int cb) {
            m_pCodeBuf->m_nAddr += cb;

            foreach (CodeBuf::EnumSpan, oEnum, m_pCodeBuf) {
                auto const pSpan = oEnum.Get();
                if (pSpan->GetAddr() >= nAddr) {
                    pSpan->m_nAddr += cb;
                }
            } // for each span

            foreach (Function::EnumBBlockReverse, oEnum, m_pFun) {
                auto const pExt = oEnum.Get()->GetWork<BBlockExt>();
                if (pExt->GetAddr() < nAddr) break;
                pExt->m_nAddr += cb;
            } // for each bblock

            foreach (Annots::EnumReverse, oEnum,
                        &m_pFun->GetWork<FunExt>()->m_oAnnots) {
                auto const pAnnot = oEnum.Get();
                if (pAnnot->GetAddr() < nAddr) break;
                pAnnot->m_nAddr += cb;
            } // for each annot
        } // updateLabels
    }; // Resolver

    Resolver::Run(&m_oCodeBuf, m_pFun);

    {
        auto const pPrologueI = m_pFun->GetPrologueI();
        if (auto const pJump =
                pPrologueI->GetNext()->DynamicCast<JumpI>()) {
            pJump->SetIndex(
                pJump->GetBB()->GetNext()->
                    GetWork<BBlockExt>()->GetAddr());
        } // if
    }

    for (auto& bblock: m_pFun->bblocks()) {
        auto const iDiff =
            bblock.GetWork<BBlockExt>()->GetAddr()
                - bblock.GetFirstI()->GetIndex();

        if (0 == iDiff) {
            continue;
        }

        for (auto& inst: bblock.instructions())
            inst.SetIndex(inst.GetIndex() + iDiff);
    } // for each bblock
} // FixSpans


// [S]
void CicsAssembler::SerializeCode(void* const pvCode) {
    auto nAddr = 0;
    auto ofsCode = 0;
    auto pbCode = reinterpret_cast<uint8*>(pvCode);

    foreach (CodeBuf::EnumSpan, oEnum, &m_oCodeBuf) {
        auto const pSpan = oEnum.Get();
        auto const cbSpan = pSpan->m_nAddr - nAddr;

        ::CopyMemory(
            pbCode,
            m_oCodeBuf.GetCode(ofsCode),
            cbSpan);

        pbCode += cbSpan;
        nAddr   = pSpan->GetAddr();
        ofsCode = pSpan->GetOffset();
    } // for each span

    ASSERT(m_oCodeBuf.GetAddr() >= nAddr);

    ::CopyMemory(
        pbCode,
        m_oCodeBuf.GetCode(ofsCode),
        m_oCodeBuf.GetAddr() - nAddr);
} // SerializeCode

} // Cg
} // Il
