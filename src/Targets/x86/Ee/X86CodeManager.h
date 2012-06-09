// @(#)$Id$
//
// Evita Ee - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Targets_X86_Ee_X86CodeManager_h)
#define INCLUDE_Targets_X86_Ee_X86CodeManager_h

#include "../../../Ee/CodeManager.h"

namespace Ee {

class X86CodeManager : public CodeManager {
  public: X86CodeManager();
  public: ~X86CodeManager();
  DISALLOW_COPY_AND_ASSIGN(X86CodeManager);
};

} // Ee

#endif // !defined(INCLUDE_Targets_X86_Ee_X86CodeManager_h)
