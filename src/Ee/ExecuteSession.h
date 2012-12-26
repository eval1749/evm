// @(#)$Id$
//
// Evita Executor
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_ExecuteSession_h)
#define INCLUDE_evc_ExecuteSession_h

#include "./ExecuteErrorInfo.h"

#include "../Cg/Tasks/CgSession.h"

namespace Executor {

class ExecuteSession;

class ExecuteSession : public Il::Cg::CgSession {
  private: LocalMemoryZone zone_;

  private: ExecuteErrorInfo::List errors_;
  private: const Il::Cg::CgTarget& target_;
  private: ExecuteErrorInfo::List warnings_;

  // ctor
  public: ExecuteSession(const Il::Cg::CgTarget&);
  public: virtual ~ExecuteSession();

  // properties
  public: const ExecuteErrorInfo::List errors() const { return errors_; }
  public: const CgTarget& target() const override { return target_; }
  public: const ExecuteErrorInfo::List warnings() const { return warnings_; }

  // [A]
  public: virtual void AddErrorInfo(const ErrorInfo&) override;
  private: void AddWarning(const ExecuteErrorInfo&);

  // [H]
  public: virtual bool HasError() const override { 
    return !errors_.IsEmpty(); 
  }

  DISALLOW_COPY_AND_ASSIGN(ExecuteSession);
}; // ExecuteSession

} // Executor

#endif // !defined(INCLUDE_evc_ExecuteSession_h)
