#include "precomp.h"
// @(#)$Id$
//
// Evita Om - Windows Mm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../../Om/MemoryManager.h"
#include "../../Om/StaticObjects.h"

namespace Om {

typedef Mm::Area Area;

class WinMm : public Mm {
    // [A]
    public: static Area* Alloc(size_t const cbArea) {
        #if DEBUG_ALLOC
            DEBUG_PRINTF("commit=%p cb=%d\n", sm_pCommit, cbArea);
        #endif

        Area* pNext = reinterpret_cast<Area*>(sm_pCommit->ToInt() + cbArea);

        if (pNext > sm_pEnd) {
            Debugger::Fail("Memory Exhausted.");
        }

        void* pvArea = ::VirtualAlloc(
            sm_pCommit,
            cbArea,
            MEM_COMMIT,
            PAGE_READWRITE);
        if (nullptr == pvArea) {
            auto const dwError = ::GetLastError();
            Debugger::Fail("VirtualAlloc %d", dwError);
        }

        sm_pCommit = pNext;

        return reinterpret_cast<Area*>(pvArea);
    } // Alloc
}; // WinMm

// [G]
/// <summary>
///   Allocate code area.
/// </summary>
Mm::Area* Mm::GetCodeArea(uint nFlags, size_t cbData)
{
    Area* pArea = GetDataArea(nFlags, cbData);

    DWORD dwOldProtect;
    BOOL fSucceeded = ::VirtualProtect(
        pArea,
        pArea->m_cbArea,
        PAGE_EXECUTE_READWRITE,
        &dwOldProtect );
    if (! fSucceeded)
    {
        Debugger::Fail("VirtualProtect PAGE_EXECUTE_READWRITE");
    }

    return pArea;
} // Mm::GetCodeArea

/// <summary>
///   Allocate data area.
/// </summary>
Mm::Area* Mm::GetDataArea(uint nFlags, size_t cbData) {
    size_t cbArea = RoundUp(
        size_t(cbData + sizeof(Area)), 
        size_t(Arch::AllocUnit));
    sm_cbAllocSoFar += cbArea;

    Area* pArea = sm_pFreeAreas->GetFirst();
    for (;;) {
        if (nullptr == pArea)
        {
            pArea = WinMm::Alloc(cbArea);

            #if DEBUG_GC
                DEBUG_PRINTF("new %p cb=%d sofar=%d\n",
                    pArea, cbArea, sm_cbAllocSoFar );
            #endif
            break;
        } // if

        if (pArea->m_cbArea == cbArea) {
            sm_pFreeAreas->Delete(pArea);
            #if DEBUG_GC
                DEBUG_PRINTF("free %p cb=%d sofar=%d\n",
                    pArea, cbArea, sm_cbAllocSoFar );
            #endif
            break;
        }

        if (pArea->m_cbArea > cbArea) {
            auto const pBelow = reinterpret_cast<Area*>(
                pArea->ToInt() + cbArea );

            new(pBelow) Area(
                Area::ScanType_None,
                pArea->m_cbArea - cbArea );

            sm_pFreeAreas->InsertBefore(pBelow, pArea);
            sm_pFreeAreas->Delete(pArea);

            #if DEBUG_GC
                DEBUG_PRINTF("extract %p cb=%d sofar=%d\n",
                    pArea, cbArea, sm_cbAllocSoFar );
            #endif
            break;
        } // if

        pArea = pArea->GetNext();
    } // for

    pArea = new(pArea) Area(nFlags, cbArea);
    return pArea;
} // Mm::GetDataArea

// [S]
void Mm::Start(const InitParam* const pParams) {
    StaticInit();

    sm_pStart = reinterpret_cast<Area*>(LISP_BASE);

    const int MegaByte = 1024 * 1024;
    const int k_cbTotal  = pParams->m_nTotalMb * MegaByte;

    // TODO 2008-01-12 yosi@msn.com We should not set WriteWatch for
    // Gen0.
    void* pvStart = ::VirtualAlloc(
        reinterpret_cast<void*>(LISP_BASE),
        size_t(k_cbTotal),
        MEM_RESERVE | MEM_WRITE_WATCH,
        PAGE_READWRITE);
    if (!pvStart) {
        auto const dwError = ::GetLastError();
        Debugger::Fail(
            "VirtualAlloc MEM_RESERVE @ 0x%lx %d",
            sm_pStart,
            dwError);
    } // if

    sm_pEnd = reinterpret_cast<Area*>(sm_pStart->ToInt() + k_cbTotal);

    auto const cbReco = static_cast<size_t>(
        reinterpret_cast<intptr_t>(pParams->m_pvStaticEnd) - RECO_BASE);

    auto const cbRecoArea = RoundUp(cbReco, Arch::AllocUnit);

    void* pvStatic = ::VirtualAlloc(
        pvStart,
        BINO_AREA_SIZE + cbRecoArea,
        MEM_COMMIT,
        PAGE_READWRITE);
    if (!pvStatic) {
        auto const dwError = ::GetLastError();

        Debugger::Fail(
            "VirtualAlloc MEM_COMMIT @ 0x%lx %d",
            sm_pStart,
            dwError);
    } // if

    Area* pRecoArea = new(reinterpret_cast<void*>(RECO_BASE)) Area(
        Area::ScanType_Record | Area::Age_Static,
        cbRecoArea);

    pRecoArea->m_ofsFree = cbReco;

    auto const pBinoArea = new(reinterpret_cast<void*>(BINO_BASE)) Area(
        Area::ScanType_BinObj | Area::Age_Static,
        BINO_AREA_SIZE);

    pBinoArea->m_ofsFree = sizeof(StaticBino);

    sm_pCommit = reinterpret_cast<Area*>(
        pRecoArea->ToInt() + pRecoArea->m_cbArea);
} // Start

} // Om
