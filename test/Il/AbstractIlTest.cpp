#include "precomp.h"
// @(#)$Id$
//
// Evita Test Helper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./AbstractIlTest.h"

namespace IlTest {

AbstractIlTest::AbstractIlTest()
    : global_ns_(Namespace::CreateGlobalNamespace()) {
}

AbstractIlTest::~AbstractIlTest() {
  delete &global_ns_;
}

// [L]
Literal& AbstractIlTest::LitFalse() {
  return *new(zone_) BooleanLiteral(false);
}

Literal& AbstractIlTest::LitTrue() {
  return *new(zone_) BooleanLiteral(true);
}

// [N]
Class& AbstractIlTest::NewClass(const char* const name) {
  return NewClass(name, CollectionV_<const Class*>(Ty_Object));
}

Class& AbstractIlTest::NewClass(const char* const name, const Class& a) {
  return NewClass(name, CollectionV_<const Class*>(&a));
}

Class& AbstractIlTest::NewClass(
    const char* const name,
    const Class& a,
    const Interface& b) {
  ASSERT(b.IsInterface());
  return NewClass(name, CollectionV_<const Class*>(&a, &b));
}

Class& AbstractIlTest::NewClass(
    const char* const name,
    const Class& a,
    const Interface& b,
    const Interface& c) {
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  return NewClass(name, CollectionV_<const Class*>(&a, &b, &c));
}

Class& AbstractIlTest::NewClass(
    const char* const name,
    const Collection_<const Class*>& base_specs) {
  auto& clazz = *new Class(global_ns_, Modifier_Public, Name::Intern(name));
  clazz.RealizeClass(base_specs);
  return clazz;
}

Class& AbstractIlTest::NewClass(
    const Class& outer,
    const char* const name) {
  auto& clazz = *new Class(outer, Modifier_Public, Name::Intern(name));
  clazz.RealizeClass(CollectionV_<const Class*>());
  return clazz;
}

const BoolOutput& AbstractIlTest::NewBoolOutput() {
 return module().NewBoolOutput();
}

BBlock& AbstractIlTest::NewBBlock(int const name) {
  auto& bblock = module().NewBBlock();
  bblock.SetName(name);
  return bblock;
}

Function& AbstractIlTest::NewFunction(const char* const name) {
  return module().NewFunction(
    nullptr,
    Function::Flavor_Named,
    Name::Intern(name));
}

Interface& AbstractIlTest::NewInterface(const char* const name) {
  return NewInterface(name, CollectionV_<const Interface*>());
}

Interface& AbstractIlTest::NewInterface(
    const char* const name,
    const Interface& a) {
  ASSERT(a.IsInterface());
  return NewInterface(name, CollectionV_<const Interface*>(&a));
}

Interface& AbstractIlTest::NewInterface(
    const char* const name,
    const Interface& a,
    const Interface& b) {
  ASSERT(a.IsInterface());
  ASSERT(b.IsInterface());
  return NewInterface(name, CollectionV_<const Interface*>(&a, &b));
}

Interface& AbstractIlTest::NewInterface(
    const char* const name,
    const Interface& a,
    const Interface& b,
    const Interface& c) {
  ASSERT(a.IsInterface());
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  return NewInterface(name, CollectionV_<const Interface*>(&a, &b, &c));
}

Interface& AbstractIlTest::NewInterface(
    const char* const name,
    const Collection_<const Class*>& base_specs) {
  auto& clazz = *new Class(
      global_ns_, 
      Modifier_Public | Modifier_ClassVariation_Interface,
      Name::Intern(name));
  clazz.RealizeClass(base_specs);
  return clazz;
}

const Register& AbstractIlTest::NewRegister() {
 return module_.NewRegister();
}

} // IlTest
