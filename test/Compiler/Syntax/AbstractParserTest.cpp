#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - AbstractParseTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractParserTest.h"

#include "Compiler/CompilationUnit.h"
#include "Compiler/CompileSession.h"
#include "Compiler/Ir.h"
#include "Compiler/Syntax/Parser.h"

namespace CompilerTest {

using namespace Compiler;

// ctor
AbstractParserTest::AbstractParserTest()
    : parser_(*new Parser(session(), compilation_unit())) {}

AbstractParserTest::~AbstractParserTest() {}

// [F]
ClassDef* AbstractParserTest::FindClassDef(const String& name1) const {
  auto& name = Name::Intern(name1);
  foreach (CompileSession::EnumCompilationUnit, units, session()) {
    if (auto const present = units->namespace_body().Find(name)) {
      return present->DynamicCast<ClassDef>();
    }
  }
  return nullptr;
}

MethodDef* AbstractParserTest::FindMethodDef(
    const ClassDef& class_def,
    const String& name) const {
  auto const group = class_def.Find(Name::Intern(name))
      ->DynamicCast<MethodGroupDef>();
  if (!group) {
    return nullptr;
  }
  MethodGroupDef::EnumMethodDef methods(*group);
  return methods.AtEnd() ? nullptr : methods.Get();
}

// [P]
void AbstractParserTest::Parse(const char* const first, ...) {
  StringBuilder builder;
  builder.Append(first);
  builder.Append("\n");
  va_list args;
  va_start(args, first);
  while (auto const line = va_arg(args, const char*)) {
    builder.Append(line);
    builder.Append("\n");
  }
  va_end(args);
  auto source = builder.ToString();
  parser_.Parse(source.value(), source.length());
}

} // CompilerTest
