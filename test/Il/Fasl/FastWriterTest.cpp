#include "precomp.h"
// @(#)$Id$
//
// Evita Char Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractFaslTest.h"

namespace IlTest {

class FastWriterTest : public AbstractFaslTest {
};

// class A {}
// class B : A {}
TEST_F(FastWriterTest, Class) {
  MemoryStream mem;
  MakeSampleClass(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 101, 129, 23, 2, 103, 130, 0, 89, 128,
    69, 193, 144, 155, 6, 65, 13, 134, 88, 50, 8, 102, 66, 13, 200, 84, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

// class Foo { Array<String> x; }
TEST_F(FastWriterTest, ConstructedClass) {
  MemoryStream mem;
  MakeSampleConstructedClass(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 20, 206, 128, 5, 155, 158, 56, 231, 144, 17, 34, 203, 219,
    105, 125, 2, 76, 10, 48, 104, 37, 248, 4, 83, 64, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

TEST_F(FastWriterTest, ConstructedMethod) {
  MemoryStream mem;
  MakeSampleConstructedMethod(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 18, 204, 151, 96, 34, 201, 157, 1, 111, 1, 35, 7, 88, 19, 56,
    5, 18, 193, 96, 72, 225, 217, 14, 42, 89, 146, 252, 4, 89, 51, 160, 7,
    130, 146, 28, 86, 33, 89, 156, 13, 193, 86, 3, 194, 130, 178, 88, 19, 5,
    226, 33, 230, 3, 201, 66, 76, 25, 30, 33, 1, 96, 188, 212, 40, 33, 0, 163,
    6, 134, 41, 24, 79, 4, 10, 148, 172, 23, 147, 133, 88, 15, 12, 160, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

// class Foo { int x; }
TEST_F(FastWriterTest, Field) {
  MemoryStream mem;
  MakeSampleField(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 20, 206, 128, 18, 95, 128, 69, 52, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

// class Foo { int x; }
TEST_F(FastWriterTest, GenericClass) {
  MemoryStream mem;
  MakeSampleGenericClass(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 11, 120, 9, 24, 52, 192,
    153, 192, 40, 150, 12, 128, 27, 9, 176, 100, 16, 220, 133, 51, 160, 6,
    151, 224, 17, 77, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

// class Foo { extern int Bar(); }
TEST_F(FastWriterTest, Method) {
  MemoryStream mem;
  MakeSampleMethod(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 18, 204, 151, 96, 34, 201, 157, 17, 129, 35, 132, 164, 56,
    174, 128
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

// class Foo { int Bar() { ... } }
TEST_F(FastWriterTest, MethodBody) {
  MemoryStream mem;
  MakeSampleMethodBody(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 18, 204, 151, 96, 34, 201, 157, 1, 129, 35, 132, 164, 56,
    172, 63, 51, 129, 184, 42, 192, 120, 80, 86, 75, 2, 96, 188, 68, 56, 192,
    121, 40, 86, 33, 137, 82, 96, 188, 156, 42, 192, 120, 101, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

TEST_F(FastWriterTest, Property) {
  MemoryStream mem;
  MakeSampleProperty(mem);

  ScopedPtr_<ByteArray> actual_bytes(ToByteArray(mem));

  uint8 const expected_bytev[] = {
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 17, 112, 100, 38, 193,
    144, 67, 50, 18, 235, 167, 143, 221, 183, 234, 64, 139, 38, 120, 40, 32,
    6, 4, 142, 18, 144, 226, 185, 158, 8, 0, 36, 183, 109, 250, 144, 40, 117,
    215, 79, 0, 7, 168, 128, 252, 206, 6, 224, 171, 1, 225, 65, 89, 44, 9,
    130, 241, 16, 227, 1, 228, 161, 88, 134, 37, 73, 130, 242, 112, 171, 1,
    225, 148, 0
  };
  ByteArray expected_bytes(expected_bytev, sizeof(expected_bytev));
  EXPECT_EQ(expected_bytes, *actual_bytes);
}

}  // IlTest
