// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Cg_Tasks_CgSession_h)
#define INCLUDE_Evita_Cg_Tasks_CgSession_h

#include "../../Il/Tasks/Session.h"

namespace Il {
namespace Cg {

interface CgSession : public Il::Tasks::Session {
  public: virtual const CgTarget& target() const = 0;
};

} // Cg
} // Il

#endif // !defined(INCLUDE_Evita_Cg_Tasks_CgSession_h)
