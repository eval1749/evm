// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_h)
#define INCLUDE_Common_h

#include "./Common/Object.h"

#include "./Common/Boolean.h"
#include "./Common/Box.h"
#include "./Common/Char.h"
#include "./Common/CharSequence.h"
#include "./Common/CommonDefs.h"
#include "./Common/DebugHelper.h"
#include "./Common/FileHandle.h"
#include "./Common/Float32.h"
#include "./Common/Float64.h"
#include "./Common/Formatter.h"
#include "./Common/GlobalMemoryZone.h"
#include "./Common/Int16.h"
#include "./Common/Int32.h"
#include "./Common/Int64.h"
#include "./Common/Int8.h"
#include "./Common/IntPtr.h"
#include "./Common/LocalMemoryZone.h"
#include "./Common/Lockable.h"
#include "./Common/MemoryZone.h"
#include "./Common/Pair_.h"
#include "./Common/String.h"
#include "./Common/StringBuilder.h"
#include "./Common/UInt16.h"
#include "./Common/UInt32.h"
#include "./Common/UInt64.h"
#include "./Common/UInt8.h"
#include "./Common/UnicodeCategory.h"

namespace Common {
void LibExport Init();
} // Common

#endif // !defined(INCLUDE_Common_h)
