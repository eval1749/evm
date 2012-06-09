// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_WithChangeCount_h)
#define INCLUDE_Il_Tasks_WithChangeCount_h

namespace Il {
namespace Tasks {

// TODO 2011-01-05 yosi@msn.com Should we use WithChangeCount?
class WithChangeCount {
    private: int m_cChanges;

    protected: WithChangeCount() :
        m_cChanges(0) {}

    // [G]
    public: int GetChangeCount() const { return m_cChanges; }

    // [I]
    public: bool IsChanged() const { return m_cChanges > 0; }

    // [R]
    public: void ResetChanged() { m_cChanges = 0; }

    // [S]
    public: void SetChanged() { m_cChanges++; }
}; // Tasklet

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_WithChangeCount_h)
