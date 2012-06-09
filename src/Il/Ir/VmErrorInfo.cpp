#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Vm - Fast Writer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./VmErrorInfo.h"

namespace Il {
namespace Ir {

using Common::Box;
using Common::Collections::CollectionV_;

namespace {
static const char* const k_vm_error_name[] = {
  "None",
  #define DEFERROR(category, subcategory) #category "_" #subcategory,
  #include "../VmError.inc"
};

static String GetErrorName(int const ec) {
  auto i = ec - VmError_Base;
  return ec >= 0 && ec < ARRAYSIZE(k_vm_error_name)
      ? k_vm_error_name[i]
      : String::Format("Invalid(%d)", ec);
}
} // namespace

VmErrorInfo::VmErrorInfo(const SourceInfo& si, VmError const ec)
    : Base(si, ec, Collection_<Box>()) {}

VmErrorInfo::VmErrorInfo(const SourceInfo& si, VmError const ec,
                         Box a)
    : Base(si, ec, CollectionV_<Box>(a)) {}

VmErrorInfo::VmErrorInfo(const SourceInfo& si, VmError const ec,
                         Box a, Box b)
    : Base(si, ec, CollectionV_<Box>(a, b)) {}

VmErrorInfo::VmErrorInfo(const SourceInfo& si, VmError const ec,
                         Box a, Box b, Box c)
    : Base(si, ec, CollectionV_<Box>(a, b, c)) {}

VmErrorInfo::VmErrorInfo(const SourceInfo& si, VmError const ec,
                 const Collection_<Box>& params)
    : Base(si, ec, params) {}

ErrorInfo& VmErrorInfo::Clone() const {
  return *new VmErrorInfo(
      source_info(), 
      static_cast<VmError>(error_code()), 
      params());
}

String VmErrorInfo::ToString() const {
  return String::Format("%s(%s:%s) %s %s",
    source_info().file_name(),
    source_info().line() + 1,
    source_info().column(),
    GetErrorName(error_code()),
    params());
}

} // Ir
} // Il
