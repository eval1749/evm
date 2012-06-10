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
    144, 67, 50, 18, 204, 151, 96, 34, 201, 157, 1, 111, 0, 192, 153, 192, 36,
    102, 11, 2, 71, 14, 200, 113, 82, 204, 151, 224, 34, 201, 157, 0, 60, 20,
    144, 226, 177, 10, 204, 224, 110, 10, 176, 30, 20, 21, 146, 192, 152, 47,
    17, 15, 48, 30, 74, 18, 96, 200, 241, 8, 11, 5, 230, 161, 65, 8, 5, 24,
    52, 49, 72, 194, 120, 32, 84, 165, 96, 188, 156, 42, 192, 120, 101, 0
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
    89, 253, 50, 4, 222, 9, 158, 8, 1, 104, 103, 152, 11, 120, 6, 4, 206, 1,
    35, 48, 100, 0, 216, 77, 131, 32, 134, 228, 41, 157, 0, 52, 191, 0, 138,
    104, 0
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
