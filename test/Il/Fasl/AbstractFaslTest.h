// @(#)$Id$
//
// Evita Lexer Checker
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_AbstractFaslTest_h)
#define INCLUDE_test_AbstractFaslTest_h

#include "../AbstractIlTest.h"
#include "Common/Io.h"

std::ostream& operator<<(
    std::ostream& os,
    const Common::Collections::Array_<uint8>& arr);

namespace IlTest {

using namespace Common::Io;
using namespace Il::Ir;

typedef Array_<uint8> ByteArray;

class AbstractFaslTest : public AbstractIlTest {
    // [C]
  private: Method& CreateMethod(
      Class&, int, const Name&, const FunctionType&);

  // [M]
  protected: void MakeSampleClass(MemoryStream&);
  protected: void MakeSampleConstructedClass(MemoryStream&);
  protected: void MakeSampleConstructedMethod(MemoryStream&);
  protected: void MakeSampleField(MemoryStream&);
  protected: void MakeSampleGenericClass(MemoryStream&);
  protected: void MakeSampleGenericMethod(MemoryStream&);
  protected: void MakeSampleMethod(MemoryStream&);
  protected: void MakeSampleMethodBody(MemoryStream&);
  protected: void MakeSampleProperty(MemoryStream&);

  // [T]
  protected: static ByteArray* ToByteArray(MemoryStream&);
}; // AbstractTest

} // AbstractFaslTest

#endif // !defined(INCLUDE_test_AbstractFaslTest_h)
