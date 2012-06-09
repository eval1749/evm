//////////////////////////////////////////////////////////////////////////////
//
// evm - evc- pre-compiled header
// precomp.h
//
// Copyright (C) 1996-2006 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: /proj/evcl3/boot/precomp.h 13 2006-07-29 01:55:00 yosi $
//
#if !defined(INCLUDE_Om_precomp_h)
#define INCLUDE_Om_precomp_h

#pragma once

// warning C4668: '__midl' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable: 4668)

// warning C4820: '<unnamed-tag>' : '3' bytes padding added after data member '<unnamed-tag>::Data'
#pragma warning(disable: 4820)

// Requires at least Windows XP
#define _WIN32_WINNT    0x501

#define STRICT
//#define INC_OLE2
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stddef.h> // ptrdiff_t

#include "../util/z_defs.h"
#include "../util/z_debug.h"
#include "../util/z_util.h"
#include "../util/vcsupport.h"

#include "../Common.h"
using namespace Common;

#include "./OmDefs.h"

#endif //!defined(INCLUDE_Om_precomp_h)
