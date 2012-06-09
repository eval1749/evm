#include "precomp.h"
// @(#)$Id$
//
// DomBuilder test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"

#include "./DomBuilder.h"
#include "./DomNode.h"

namespace {

class DomBuilderTest : public ::testing::Test {
};

TEST_F(DomBuilderTest, EmptyTag) {
  Dom::Builder builder;
  builder.StartElement("foo");
  builder.EndElement("foo");
  Dom::Document* doc = builder.Finish();
  EXPECT_STREQ(L"<foo/>", doc->ToCharPtr());
}

TEST_F(DomBuilderTest, EmptyTagWithAttrs) {
  Dom::Builder builder;
  builder.StartElement("foo");
  builder.SetAttribute("b", "2");
  builder.SetAttribute("a", "1");
  builder.EndElement("foo");
  Dom::Document* doc = builder.Finish();
  EXPECT_STREQ(L"<foo a=\"1\" b=\"2\"/>", doc->ToCharPtr());
}

TEST_F(DomBuilderTest, StartTag) {
  Dom::Builder builder;
  builder.StartElement("foo");
  builder.StartElement("bar");
  builder.EndElement("bar");
  builder.EndElement("foo");
  Dom::Document* doc = builder.Finish();
  EXPECT_STREQ(L"<foo><bar/></foo>", doc->ToCharPtr());
}

TEST_F(DomBuilderTest, StartTagWithAttrs) {
  Dom::Builder builder;
  builder.StartElement("foo");
  builder.SetAttribute("b", "2");
  builder.SetAttribute("a", "1");
  builder.StartElement("bar");
  builder.EndElement("bar");
  builder.EndElement("foo");
  Dom::Document* doc = builder.Finish();

  EXPECT_STREQ(
    L"<foo a=\"1\" b=\"2\"><bar/></foo>",
    doc->ToCharPtr());
}

}
