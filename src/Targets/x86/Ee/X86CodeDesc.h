// @(#)$Id$
//
// Evita Ee - CodeDesc
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Targets_X86_Ee_X86CodeDesc_h)
#define INCLUDE_Targets_X86_Ee_X86CodeDesc_h

#include "../../../Ee/CodeDesc.h"

namespace Ee {

class X86CodeDesc : public CodeDesc {
  public: X86CodeDesc(
      const Function&,
      const Collection_<Annotation>&,
      void*,
      int32);
  DISALLOW_COPY_AND_ASSIGN(X86CodeDesc);
};

} // Ee

#endif // !defined(INCLUDE_Targets_X86_Ee_X86CodeDesc_h)
