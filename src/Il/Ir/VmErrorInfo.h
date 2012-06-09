// @(#)$Id$
//
// Evita Il - FastError
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Il_Ir_VmErrorInfo_h)
#define INCLUDE_evm_Il_Ir_VmErrorInfo_h

#include "./ErrorInfo.h"
#include "../VmError.h"

namespace Il {
namespace Ir {

using Il::Ir::ErrorInfo;
using Il::Ir::SourceInfo;

class VmErrorInfo : public ErrorInfo_<VmErrorInfo> {
  public: VmErrorInfo(const SourceInfo&, VmError);
  public: VmErrorInfo(const SourceInfo&, VmError, Box);
  public: VmErrorInfo(const SourceInfo&, VmError, Box, Box);
  public: VmErrorInfo(const SourceInfo&, VmError, Box, Box, Box);
  private: VmErrorInfo(const SourceInfo&, VmError, const Collection_<Box>&);
  public: virtual ErrorInfo& Clone() const override;
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(VmErrorInfo);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_evm_Il_Ir_VmErrorInfo_h)
