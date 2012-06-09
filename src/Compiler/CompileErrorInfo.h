// @(#)$Id$
//
// Evita Parser
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_CompileErrorInfo_h)
#define INCLUDE_evc_CompileErrorInfo_h

#include "./CompileError.h"
#include "../Common.h"
#include "../Common/Collections.h"

namespace Compiler {

using Common::Box;
using Common::Collections::Collection_;
using Il::Ir::ErrorInfo_;

class CompileErrorInfo : public ErrorInfo_<CompileErrorInfo> {
  // ctor
  public: CompileErrorInfo(
      const SourceInfo&,
      CompileError const error_code,
      const Collection_<Box>&);

  // [C]
  public: virtual ErrorInfo& Clone() const override;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(CompileErrorInfo);
}; // CompileErrorInfo

} // Compiler

#endif // !defined(INCLUDE_evc_CompileErrorInfo_h)
