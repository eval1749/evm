#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./MockLogWriter.h"

#include "Common/Io/TextWriter.h"

namespace IlTest {

MockLogWriter::MockLogWriter() : LogWriter(&text_writer_) {}

}  // IlTest
