#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeUnifierTest
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../AbstractCompilerTest.h"

#include "Common.h"
#include "Common/Collections.h"
#include "Compiler/Tasks/TypeUnifier.h"
#include "Il.h"

namespace CompilerTest {

using namespace Common::Collections;
using namespace Compiler;
using namespace Il::Ir;

class TypeUnifierTest : public AbstractCompilerTest {
  protected: typedef HashMap_<const TypeVar*, const Type*> TypeVarMap;
  protected: TypeUnifier unifier_;
  protected: TypeVarMap tyvar_map_;

  protected: TypeUnifierTest() 
        : unifier_(session(), 
          SourceInfo("unittest", 1, 2)) {}

  protected: const TypeVarMap& GetTypeVarMap() {
    foreach (TypeUnifier::EnumTypeVar, tyvars, unifier_) {
      tyvar_map_.Add(&tyvars.Get(), &tyvars.GetBoundType());
    }
    return tyvar_map_;
  }
};

// String == String
TEST_F(TypeUnifierTest, Fixed) {
  ASSERT_TRUE(unifier_.Unify(*Ty_String, *Ty_String));
}

// Enumerable<T> == String[] where T = String
TEST_F(TypeUnifierTest, Enumerable) {
  auto& tyvar = *new TypeVar();
  auto& ty1 = Ty_Enumerable->Construct(tyvar);
  auto& ty2 = ArrayType::Intern(*Ty_String);
  ASSERT_TRUE(unifier_.Unify(ty1, ty2));
  auto& tyvars = GetTypeVarMap();
  EXPECT_EQ(1, tyvars.Count());
  EXPECT_EQ(*Ty_String, *tyvars.Get(&tyvar));
}

// T == String
TEST_F(TypeUnifierTest, Simple) {
  auto& tyvar = *new TypeVar();
  ASSERT_TRUE(unifier_.Unify(tyvar, *Ty_String));
  auto& tyvars = GetTypeVarMap();
  EXPECT_EQ(1, tyvars.Count());
  EXPECT_EQ(Ty_String, tyvars.Get(&tyvar));
}

}  // CompilerTest
