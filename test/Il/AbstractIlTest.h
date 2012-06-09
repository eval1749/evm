// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_test_Il_AbstractIlTest_h)
#define INCLUDE_evm_test_Il_AbstractIlTest_h

#include "./IlTest.h"
#include "./Tasks/MockSession.h"

namespace IlTest {

using namespace Il::Ir;

class AbstractIlTest : public ::testing::Test {
  private: LocalMemoryZone zone_;
  private: Module module_;
  private: MockSession session_;
  private: Namespace& global_ns_;

  // ctor
  protected: AbstractIlTest();
  protected: virtual ~AbstractIlTest();

  // properties
  protected: Module& module() { return module_; }
  protected: Namespace& global_ns() { return global_ns_; }
  protected: MockSession& session() { return session_; }
  protected: MemoryZone& zone() { return zone_; }

  // [L]
  protected: Literal& LitFalse();
  protected: Literal& LitTrue();

  // [N]
  protected: const BoolOutput& NewBoolOutput();
  protected: BBlock& NewBBlock(int);
  protected: Class& NewClass(const char*);
  protected: Class& NewClass(const char*, const Class&);
  protected: Class& NewClass(const char*, const Class&, const Interface&);
  protected: Class& NewClass(
      const char*, const Class&,
      const Interface&,
      const Interface&);
  protected: Class& NewClass(const char*, const Collection_<const Class*>&);

  protected: Class& NewClass(const Class&, const char*);

  protected: Function& NewFunction(const char*);

  protected: Interface& NewInterface(const char*);
  protected: Interface& NewInterface(const char*, const Interface&);
  protected: Interface& NewInterface(
      const char*, const Interface&,
      const Interface&);
  protected: Interface& NewInterface(
      const char*,
      const Interface&,
      const Interface&,
      const Interface&);
  protected: Interface& NewInterface(
      const char*,
      const Collection_<const Interface*>&);
  protected: const Register& NewRegister();
};

} // IlTest

#endif // !defined(INCLUDE_evm_test_Il_AbstractIlTest_h)
