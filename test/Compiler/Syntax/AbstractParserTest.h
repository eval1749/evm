// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Syntax_AbstractParserTest_h)
#define INCLUDE_test_Compiler_Syntax_AbstractParserTest_h

#include "../AbstractCompilerTest.h"
#include "Il/Ir/Name.h"

namespace Compiler {
class ClassDef;
class MethodDef;
class Parser;
}

namespace CompilerTest {

using namespace Compiler;

class AbstractParserTest : public AbstractCompilerTest {
  private: Parser& parser_;

  // ctor
  protected: AbstractParserTest();
  public: virtual ~AbstractParserTest();

  // properties
  protected: Parser& parser() const { return parser_; }

  // [F]
  protected: ClassDef* FindClassDef(const String&) const;
  protected: MethodDef* FindMethodDef(const ClassDef&, const String&) const;

  // [P]
  protected: void Parse(const char*, ...);

  DISALLOW_COPY_AND_ASSIGN(AbstractParserTest);
};

#define RUN_PARSER(...) { \
  Parse(__VA_ARGS__, nullptr); \
  ASSERT_FALSE(session().HasError()) << GetErrors(); \
  ASSERT_FALSE(parser().NeedMoreFeed()) << "Need more token!"; \
}

} // CompilerTest

#endif // !defined(INCLUDE_test_Compiler_Syntax_AbstractParserTest_h)
