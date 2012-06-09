#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Writer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FaslErrorInfo.h"

namespace Il {
namespace Fasl {

using Common::Box;
using Common::Collections::CollectionV_;

namespace {
static const char* const k_fasl_error_name[] = {
  "None",
  #define DEFERROR(category, subcategory) #category "_" #subcategory,
  #include "./FaslError.inc"
};

static String GetErrorName(int const ec) {
  return ec >= 0 && ec < ARRAYSIZE(k_fasl_error_name)
      ? k_fasl_error_name[ec]
      : String::Format("Invalid(%d)", ec);
}
} // namespace

FaslErrorInfo::FaslErrorInfo(const SourceInfo& si, FaslError const ec)
    : Base(si, ec, Collection_<Box>()) {}

FaslErrorInfo::FaslErrorInfo(const SourceInfo& si, FaslError const ec,
                             Box a)
    : Base(si, ec, CollectionV_<Box>(a)) {}

FaslErrorInfo::FaslErrorInfo(const SourceInfo& si, FaslError const ec,
                             Box a, Box b)
    : Base(si, ec, CollectionV_<Box>(a, b)) {}

FaslErrorInfo::FaslErrorInfo(const SourceInfo& si, FaslError const ec,
                             Box a, Box b, Box c)
    : Base(si, ec, CollectionV_<Box>(a, b, c)) {}

FaslErrorInfo::FaslErrorInfo(const SourceInfo& si, FaslError const ec,
                             const Collection_<Box>& params)
    : Base(si, ec, params) {}

Il::Ir::ErrorInfo& FaslErrorInfo::Clone() const {
  return *new FaslErrorInfo(
      source_info(), 
      static_cast<FaslError>(error_code()), 
      params());
}

String FaslErrorInfo::ToString() const {
  return String::Format("%s(%s:%s) %s %s",
    source_info().file_name(),
    source_info().line() + 1,
    source_info().column(),
    GetErrorName(error_code()),
    params());
}

} // Fasl
} // Il
