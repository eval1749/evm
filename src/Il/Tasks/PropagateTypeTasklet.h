// @(#)$Id$
//
// Evita Il - Tasks - PropagateTypeTasklet
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Tasks_PropagateTypeTasklet_h)
#define INCLUDE_Il_Tasks_PropagateTypeTasklet_h

#include "./EditInstructionTasklet.h"

namespace Il {
namespace Tasks {

class PropagateTypeTasklet : public EditInstructionTasklet {
  public: PropagateTypeTasklet(Session&, const Module&);
  private: virtual void EditInstruction(Instruction*) override;
  private: void UpdateOutputType(Instruction&, const Type&);
  DISALLOW_COPY_AND_ASSIGN(PropagateTypeTasklet);
};

} // Tasks
} // Il

#endif // !defined(INCLUDE_Il_Tasks_PropagateTypeTasklet_h)
