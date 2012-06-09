// @(#)$Id$
//
// Evita Executor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_ExecuteErrorInfo_h)
#define INCLUDE_evc_ExecuteErrorInfo_h

#include "../Il/Ir/ErrorInfo.h"

namespace Executor {

using Common::Box;
using Common::Collections::Collection_;
using namespace Il::Ir;

class ExecuteSession;

enum ExecuteError {
  ExecuteError_Zero,
  #define DEFERROR(category, name) ExecuteError_ ## category ## _ ## name,
  #include "./ExecuteError.inc"
  ExecuteError_Max_1,
};

class ExecuteErrorInfo : public ErrorInfo_<ExecuteErrorInfo> {
  public: ExecuteErrorInfo(
      const SourceInfo&, ExecuteError, const Collection_<Box>&);
  public: virtual ErrorInfo& Clone() const override;
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(ExecuteErrorInfo);
};

} // Executor

#endif // !defined(INCLUDE_evc_ExecuteErrorInfo_h)
