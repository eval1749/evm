// @(#)$Id$
//
// Evita Il - FastError
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Il_Fasl_FaslErrorInfo_h)
#define INCLUDE_evm_Il_Fasl_FaslErrorInfo_h

#include "./FaslError.h"
#include "../Ir/ErrorInfo.h"

namespace Il {
namespace Fasl {

using Il::Ir::ErrorInfo_;
using Il::Ir::SourceInfo;

class FaslErrorInfo : public ErrorInfo_<FaslErrorInfo> {
  public: FaslErrorInfo(const SourceInfo&, FaslError);
  public: FaslErrorInfo(const SourceInfo&, FaslError, Box);
  public: FaslErrorInfo(const SourceInfo&, FaslError, Box, Box);
  public: FaslErrorInfo(const SourceInfo&, FaslError, Box, Box, Box);

  private: FaslErrorInfo(
    const SourceInfo&,
    FaslError,
    const Collection_<Box>&);

  public: virtual ErrorInfo& Clone() const override;
  public: virtual String ToString() const override;
  DISALLOW_COPY_AND_ASSIGN(FaslErrorInfo);
};

} // Fasl
} // Il

#endif // !defined(INCLUDE_evm_Il_Fasl_FaslErrorInfo_h)
