#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - FileCompiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FileCompiler.h"

#include "../Common/FileHandle.h"
#include "../Common/LocalMemoryZone.h"
#include "../Common//MemoryZone.h"
#include "../Common/String.h"

#include "../Common/Io/StreamWriter.h"
#include "../Common/Io/StreamReader.h"

#include "./CompilationUnit.h"
#include "./CompileSession.h"

#include "./Syntax/Parser.h"

#include "../Il/Ir/Module.h"

namespace Compiler {

using Common::Io::StreamReader;
using Common::Io::StreamWriter;

using Il::Io::XhtmlWriter;

FileCompiler::FileCompiler(CompileSession& session) : session_(session) {}

void FileCompiler::CompileFile(const String& file_name) {
  ASSERT(!file_name.IsEmpty());

  FileHandle const shFile = ::CreateFileW(
      file_name.value(),
      GENERIC_READ,
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      0,
      nullptr);

  if (!shFile.IsValid()) {
      session_.AddError(
        *new SourceInfo(file_name, 0, 0),
        CompileError_Host_File_NotFound, 
        file_name);
      return;
  }

  auto& cm_unit = *new CompilationUnit(session_,  file_name);

  session_.Add(cm_unit);

  StreamReader oSource(shFile, CP_UTF8);

  Parser parser(session_, cm_unit);

  for (;;) {
    char16 buf[StreamReader::k_nBuffer];
    auto const buflen = oSource.Read(buf, ARRAYSIZE(buf));
    if (buflen == 0) {
        cm_unit.FinishSource();
        parser.Finish();
        break;
    }

    cm_unit.AddSource(buf, buflen);

    parser.Parse(buf, buflen);
    if (session_.HasError()) {
      break;
    }
 }
}

void FileCompiler::FatalError(const char* const format, ...) {
  va_list args;
  va_start(args, format);
  Debugger::Printf(format, args);
  va_end(args);
}

} // Compiler
