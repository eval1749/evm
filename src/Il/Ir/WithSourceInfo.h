// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_WithSourceInfo_h)
#define INCLUDE_Il_Ir_WithSourceInfo_h

#include "./SourceInfo.h"

namespace Il {
namespace Ir {

class WithSourceInfo {
  private: mutable SourceInfo source_info_;

  protected: WithSourceInfo(const SourceInfo* const source_info = nullptr) {
    if (source_info) {
      source_info_ = *source_info;
    }
  }

  public: const SourceInfo& source_info() const { return source_info_; }
  public: void set_source_info(const SourceInfo& r) { source_info_ = r; }

  // [G]
  public: const SourceInfo* GetSourceInfo() const { 
    return &source_info_;
  }

  // [H]
  public: bool HasSourceInfo() const {
    return source_info_.HasInfo();
  }

  // [S]
  public: void SetSourceInfo(const SourceInfo* const source_info) const {
    // Object may have source info from another way, e.g.
    // instruction takes source info from output.
    if (source_info) {
      source_info_ = *source_info;
    }
  }
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_WithSourceInfo_h)
