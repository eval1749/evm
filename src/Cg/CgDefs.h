// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_CgDefs_h)
#define INCLUDE_Il_Cg_CgDefs_h

#include "../Il.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

interface CgSession;

struct RegDesc;
struct RegSet;
struct RegGroup;

class CgInstruction;
class CgImmediate;
class CgOutput;
class CgPseudoOutput;
class ClosedMarker;
class CopyInstruction;
class FunLit;
class MemSlot;
class Physical;
class StackSlot;
class ThreadSlot;
class VarHome;

// TODO 2010-01-08 yosi@msn.com We should not use "Int".
typedef int Int;

struct RegDesc {
    RegClass    m_eRegClass;
    uint        m_nId;
    uint        m_nSize;
    uint        m_nIndex;
    const char* m_pszName;
}; // RegDesc

struct RegSet {
    RegClass m_eRegClass;
    int m_c;
    const RegDesc* const* m_prgpReg;

    // [E]
    class Enum {
        private: int m_i;
        private: const RegSet* m_p;

        public: Enum(const RegSet* p) :
            m_p(p), m_i(0) {}

        public: bool AtEnd() const
            { return m_i >= m_p->m_c; }

        public: const RegDesc* Get() const
        {
            ASSERT(! AtEnd());
            return m_p->m_prgpReg[m_i];
        } // Get

        public: int GetIndex() const
        {
            ASSERT(! AtEnd());
            return m_i;
        } // GetIndex

        public: void Next()
            { ASSERT(! AtEnd()); m_i += 1; }
    }; // Enum
}; // RegSet

struct RegGroup {
    const char*     m_pszName;
    RegClass        m_eRegClass;
    uint            m_cbWidth;
    const RegDesc*  m_pRn;
    const RegDesc*  m_pRsp;
    const RegDesc*  m_pRtcb;
    const RegSet*   m_pAll;
    const RegSet*   m_pAllocable;
    const RegSet*   m_pFree;
    const RegSet*   m_pArgs;
    const RegSet*   m_pCalleeSave;
    const RegSet*   m_pCallerSave;
}; // RegGroup

#define DEFCGINSTRUCTION(mp_name, mp_arity, mp_family, mp_format) \
  class mp_name ## I;

#include "./Instructions/CgInstructions.inc"

extern void LibExport CgInit();
extern void LibExport CgTargetInit();

} // Cg
} // Il

#include "./CgTarget.h"
#include "./CgFunctor.h"
#include "./CgOperands.h"
#include "./CgInstructions.h"
#include "./Tasks/CgSession.h"

#endif // !defined(INCLUDE_Il_Cg_CgDefs_h)
