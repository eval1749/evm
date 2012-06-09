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
#if !defined(INCLUDE_Il_precomp_h)
#define INCLUDE_Il_precomp_h

#pragma once

// Requires at least Windows XP
#define _WIN32_WINNT    0x501

#define STRICT
//#define INC_OLE2
#define WIN32_LEAN_AND_MEAN

// warning C4061: enumerator 'State_V8' in switch of enum 'Il::Fasl::FastReader::State' is not explicitly handled by a case label
#pragma warning(disable: 4061)

// warning C4266: 'void Il::Ir::Functor::Process(Il::Ir::VoidOutput *)' : no override available for virtual member function from base 'Il::Ir::Functor'; function is hidden
#pragma warning(disable: 4266)

// warning C4365: 'argument' : conversion from 'const int' to 'const size_t', signed/unsigned mismatch
#pragma warning(disable: 4365)

// warning C4505: 'Common::Collections::ArrayList_<T>::ToString' : unreferenced local function has been removed
#pragma warning(disable: 4505)

// warning C4668: '__midl' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
#pragma warning(disable: 4668)

// warning C4820: '_SECURITY_QUALITY_OF_SERVICE' : '2' bytes padding added after data member '_SECURITY_QUALITY_OF_SERVICE::EffectiveOnly'
#pragma warning(disable: 4820)

// warning C4822: 'Il::Tasks::FunctionEditor::EliminateInfiniteLoop::DfsWalker::DfsWalker' : local class member function does not have a body
#pragma warning(disable: 4822)

#include <windows.h>
#include <stddef.h> // ptrdiff_t

#include "../util/z_defs.h"
#include "../util/z_debug.h"
#include "../util/z_util.h"
#include "../util/vcsupport.h"

#include "../Common.h"
using namespace Common;

#include "./VmDefs.h"

#endif //!defined(INCLUDE_Il_precomp_h)
