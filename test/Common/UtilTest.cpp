#include "precomp.h"
// @(#)$Id$
//
// Util Test
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "gtest/gtest.h"

using namespace Common;

class UtilTest : public ::testing::Test {
};

class Base : public Castable_<Base> {
};

class Foo : public WithCastable_<Foo, Base> {
public: static const char* Kind_() { return "Foo"; }
};

class Bar : public WithCastable_<Bar, Foo> {
public: static const char* Kind_() { return "Bar"; }
};

class Baz : public WithCastable_<Baz, Bar> {
public: static const char* Kind_() { return "Baz"; }
};


TEST_F(UtilTest, Castable) {
  Foo foo;
  Bar bar;
  Baz baz;

  EXPECT_TRUE(bar.DynamicCast<Foo>() != nullptr);

  EXPECT_TRUE(bar.DynamicCast<Bar>() != nullptr);
  EXPECT_TRUE(bar.DynamicCast<Foo>() != nullptr);

  EXPECT_TRUE(baz.DynamicCast<Baz>() != nullptr);
  EXPECT_TRUE(baz.DynamicCast<Bar>() != nullptr);
  EXPECT_TRUE(baz.DynamicCast<Foo>() != nullptr);
}

class Node;
class Parent : public ChildList_<Node, Parent> {
};

class Node : public ChildItem_<Node, Parent> {
  private: const char* name_;
  public: Node(const char* name) : name_(name) {
  }

  public: const char* name() const { return name_; }
};

TEST_F(UtilTest, ChildList) {
  Node a("a");
  Node b("b");
  Node c("c");
  Parent nodes;

  EXPECT_TRUE(nodes.IsEmpty());
  EXPECT_EQ(nullptr, nodes.GetFirst());
  EXPECT_EQ(nullptr, nodes.GetLast());
  EXPECT_EQ(0, nodes.Count());

  nodes.Append(&a);
  EXPECT_FALSE(nodes.IsEmpty());
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&a, nodes.GetLast());
  EXPECT_EQ(1, nodes.Count());

  nodes.InsertAfter(&b, &a);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());

  nodes.InsertBefore(&c, &b);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(&c, b.GetPrev());
  EXPECT_EQ(3, nodes.Count());

  EXPECT_EQ(&a, nodes.Pop());
  EXPECT_EQ(&c, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());

  nodes.Prepend(&a);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(3, nodes.Count());

  nodes.Delete(&b);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&c, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());
  EXPECT_EQ(nullptr, b.GetNext());
  EXPECT_EQ(nullptr, b.GetPrev());

  nodes.Replace(&b, &c);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());
  EXPECT_EQ(nullptr, c.GetNext());
  EXPECT_EQ(nullptr, c.GetPrev());

  nodes.Append(&c);

  Node* const nodev[] = { &a, &b, &c };
  ChildList_<Node, Parent>::Enum oEnum(&nodes);
  for (auto p = nodev; p < nodev + lengthof(nodev); p++) {
    auto const node = *p;
    ASSERT_FALSE(oEnum.AtEnd());
    EXPECT_EQ(node, oEnum.Get());
    oEnum.Next();
  }
}

class DlNode : public DoubleLinkedItem_<DlNode> {
  private: const char* name_;
  public: DlNode(const char* name) : name_(name) {}
  public: const char* name() const { return name_; }
};

TEST_F(UtilTest, DoubleLinkedList) {
  DlNode a("a");
  DlNode b("b");
  DlNode c("c");
  DoubleLinkedList_<DlNode> nodes;

  EXPECT_TRUE(nodes.IsEmpty());
  EXPECT_EQ(nullptr, nodes.GetFirst());
  EXPECT_EQ(nullptr, nodes.GetLast());
  EXPECT_EQ(0, nodes.Count());

  nodes.Append(&a);
  EXPECT_FALSE(nodes.IsEmpty());
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&a, nodes.GetLast());
  EXPECT_EQ(1, nodes.Count());

  nodes.InsertAfter(&b, &a);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());

  nodes.InsertBefore(&c, &b);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(&c, b.GetPrev());
  EXPECT_EQ(3, nodes.Count());

  EXPECT_EQ(&a, nodes.Pop());
  EXPECT_EQ(&c, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());

  nodes.Prepend(&a);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(3, nodes.Count());

  nodes.Delete(&b);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&c, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());
  EXPECT_EQ(nullptr, b.GetNext());
  EXPECT_EQ(nullptr, b.GetPrev());

  nodes.Replace(&b, &c);
  EXPECT_EQ(&a, nodes.GetFirst());
  EXPECT_EQ(&b, nodes.GetLast());
  EXPECT_EQ(2, nodes.Count());
  EXPECT_EQ(nullptr, c.GetNext());
  EXPECT_EQ(nullptr, c.GetPrev());

  nodes.Append(&c);

  DlNode* const nodev[] = { &a, &b, &c };
  DoubleLinkedList_<DlNode>::Enum oEnum(&nodes);
  for (auto p = nodev; p < nodev + lengthof(nodev); p++) {
    auto const node = *p;
    ASSERT_FALSE(oEnum.AtEnd());
    EXPECT_EQ(node, oEnum.Get());
    oEnum.Next();
  }
}

TEST_F(UtilTest, Enum) {
  int const v[] = { 1, 2, 3 };
  EnumArray_<int> e(v, lengthof(v));
  for (auto i = 0; i < lengthof(v); i++) {
    ASSERT_TRUE(!e.AtEnd());
    EXPECT_EQ(v[i], e.Get());
    e.Next();
  }
}

class SlNode {
  private: const char* name_;
  public: SlNode(const char* name) : name_(name) {}
  public: const char* name() const { return name_; }
};

TEST_F(UtilTest, SingleLinkedList) {
  typedef SingleLinkedList_<SlNode> List;
  typedef List::Cons Cons;

  SlNode a("a");
  SlNode b("b");
  SlNode c("c");
  Cons ac(&a);
  Cons bc(&b);
  Cons cc(&c);
  ac.m_pNext = &bc;
  bc.m_pNext = &cc;

  List list;
  EXPECT_TRUE(list.IsEmpty());

  list.SetFirst(&ac);
  EXPECT_TRUE(!list.IsEmpty());
  EXPECT_EQ(&ac, list.GetFirst());

  EXPECT_EQ(0, list.Position(&a));
  EXPECT_EQ(1, list.Position(&b));
  EXPECT_EQ(2, list.Position(&c));

  SlNode* const nodev[] = { &a, &b, &c };
  List::Enum oEnum(&list);
  for (auto p = nodev; p < nodev + lengthof(nodev); p++) {
    auto const node = *p;
    EXPECT_TRUE(!oEnum.AtEnd());
    if (oEnum.AtEnd()) break;
    EXPECT_EQ(node, oEnum.Get());
    oEnum.Next();
  }

  list.Reverse();
  EXPECT_EQ(2, list.Position(&a));
  EXPECT_EQ(1, list.Position(&b));
  EXPECT_EQ(0, list.Position(&c));
}

class WorkNode : public WorkListItem_<WorkNode> {
  private: const char* name_;
  public: WorkNode(const char* name) : name_(name) {}
  public: const char* name() const { return name_; }
};

class WorkNodeOrder {
  public: static bool GreaterThan(
  const WorkNode* const p, const WorkNode* const q) {
    return ::lstrcmpA(p->name(), q->name()) < 0;
  }
};

TEST_F(UtilTest, WorkList) {
  WorkList_<WorkNode> nodes;
  EXPECT_TRUE(nodes.IsEmpty());
  WorkNode a("a");
  WorkNode b("b");
  WorkNode c("c");

  nodes.Push(&a);
  EXPECT_TRUE(a.IsInList());
  EXPECT_TRUE(nodes.IsNotEmpty());
  EXPECT_EQ(&a, nodes.Get());

  nodes.Push(&b);
  EXPECT_TRUE(b.IsInList());
  EXPECT_EQ(&b, nodes.Get());

  EXPECT_EQ(&b, nodes.Pop());

  nodes.Insert<WorkNodeOrder>(&b);
  EXPECT_EQ(&a, nodes.Get());

  nodes.Delete(&a);
  EXPECT_EQ(&b, nodes.Pop());
  EXPECT_TRUE(nodes.IsEmpty());

  nodes.Push(&a);
  nodes.Push(&b);
  nodes.Push(&c);

  WorkNode* const nodev[] = { &c, &b, &a };
  WorkList_<WorkNode>::Enum oEnum(&nodes);
  for (auto p = nodev; p < nodev + lengthof(nodev); p++) {
    auto const node = *p;
    ASSERT_FALSE(oEnum.AtEnd());
    EXPECT_EQ(node, oEnum.Get());
    oEnum.Next();
  }

  nodes.MakeEmpty();
  EXPECT_TRUE(nodes.IsEmpty());
}

TEST_F(UtilTest, lstrchrWTest) {
  auto const kSample = L"foobar";
  EXPECT_EQ(nullptr, ::lstrchrW(kSample, 'x'));
  EXPECT_EQ(kSample + 1, ::lstrchrW(kSample, 'o'));
}

TEST_F(UtilTest, lstrrchrWTest) {
  auto const kSample = L"foobar";
  EXPECT_EQ(nullptr, ::lstrrchrW(kSample, 'x'));
  EXPECT_EQ(kSample + 2, ::lstrrchrW(kSample, 'o'));
}
