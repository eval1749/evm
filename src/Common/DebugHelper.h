//////////////////////////////////////////////////////////////////////////////
//
// evm - DebugHelper.h
//
// Copyright (C) 1996-2006 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: /proj/evcl3/boot/DebugHelper.h 13 2006-07-29 01:55:00 yosi $
//
#if !defined(INCLUDE_Common_DebugHelper_h)
#define INCLUDE_Common_DebugHelper_h

//#include "./Box.h"

#if _DEBUG
#define DCHECK_OP(name, op, a, b) \
  ::Common::DebugHelper::Check##name( \
      (a), (b), #a, #b, #op, __FILE__, __LINE__, __FUNCTION__)

#define DCHECK(expr) \
  ::Common::DebugHelper::CheckTrue((expr), #expr, \
      __FILE__, __LINE__, __FUNCTION__)

#else
#define DCHECK_OP(name, op, a, b) __noop(a, b)
#define DCHECK(expr) __noop(expr)
#endif

#define DCHECK_EQ(a, b) DCHECK_OP(Eq, ==, a, b)
#define DCHECK_GE(a, b) DCHECK_OP(Ge, >=, a, b)
#define DCHECK_GT(a, b) DCHECK_OP(Gt, >, a, b)
#define DCHECK_LE(a, b) DCHECK_OP(Le, <=, a, b)
#define DCHECK_LT(a, b) DCHECK_OP(Lt, <, a, b)
#define DCHECK_NE(a, b) DCHECK_OP(Ne, !=, a, b)

namespace Common {

class Box;

namespace DebugHelper {

void __declspec(noreturn) __fastcall
    CheckFailed(
        Box a,
        Box b,
        const char* a_str,
        const char* b_str,
        const char* op,
        const char* filename, int linenum, const char* fname);

void __fastcall CheckTrue(
    bool expr,
    const char* message,
    const char* filename,
    int linenum,
    const char* fname);

#define DEFINE_CHECK_OP(name, op) \
  template<typename A, typename B> \
  inline void Check##name( \
      const A& a, const B& b, \
      const char* const a_str, \
      const char* const b_str, \
      const char* const op_str, \
      const char* const filename, \
      int const linenum, \
      const char* const fname) { \
    if (!(a op b)) \
      CheckFailed(a, b, a_str, b_str, op_str, filename, linenum, fname); \
  }

DEFINE_CHECK_OP(Eq, ==)
DEFINE_CHECK_OP(Ge, >=)
DEFINE_CHECK_OP(Gt, >)
DEFINE_CHECK_OP(Le, <=)
DEFINE_CHECK_OP(Lt, <)
DEFINE_CHECK_OP(Ne, !=)

} // DebugHelper
} // Common

#endif //!defined(INCLUDE_Common_DebugHelper_h)
