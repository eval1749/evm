// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_VmDefs_h)
#define INCLUDE_Il_VmDefs_h

namespace Il {

void LibExport Init();

namespace Io {
    class LogWriter;
    class XhtmlWriter;
} // Io

} // Il

#include "./VmError.h"
#include "./Ir/IrDefs.h"
#include "./Functor.h"

#endif // !defined(INCLUDE_Il_VmDefs_h)
