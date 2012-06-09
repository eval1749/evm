#include "precomp.h"
// @(#)$Id$
//
// Dom
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DomNode.h"

#include "Common.h"
#include "Common/Io.h"

namespace Dom {

Attribute::Attribute(
    Document* const doc,
    const String& name,
    const String& value)
    : Base(doc), name_(name), value_(value) {
}

void Attribute::ToWriter(TextWriter& writer) const {
  writer.Write(name());
  writer.Write("=\"");
  String::EnumChar chars(value());
  while (!chars.AtEnd()) {
    auto const ch = chars.Get();
    if (ch == '"') {
      writer.Write("&quot;");
    } else {
      writer.Write(ch);
    }
    chars.Next();
  }
  writer.Write("\"");
}

Document::Document()
    : ALLOW_THIS_IN_INITIALIZER_LIST(Base(this)) {
}

String Document::node_name() const {
  return String(L"#document");
}

Element* Document::CreateElement(const String& name) {
  return new(document()) Element(this, InternName(name));
}

Document* Document::New() {
  return ::new Document();
}

String Document::SaveString(const String& string) {
  return string;
}

// Element

Element::Element(Document* const doc, const String& name)
    : Base(doc), name_(name) {
}

Node* Element::first_child() const {
  auto p = first_child_;
  while (p && p->node_type() == ATTRIBUTE_NODE) {
    p = p->next_sibling_;
  }
  return p;
}

Node* Element::last_child() const {
  return last_child_ && last_child_->node_type() != ATTRIBUTE_NODE
      ? last_child_
      : nullptr;
}

Node* Element::AppendChild(Node* new_child) {
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  return AppendChildInternal(new_child);
}

String Element::GetAttribute(const String& name) const {
  if (auto const attr = GetAttributeNode(name)) {
    return attr->value();
  }
  return String();
}

Attribute* Element::GetAttributeNode(const String& name) const {
  for (auto p = first_child_; p; p = p->next_sibling_) {
    auto const attr = p->DynamicCast<Attribute>();
    if (attr == nullptr) return nullptr;
    if (attr->node_name() == name) {
      return attr;
    }
  }
  return nullptr;
}

Node* Element::InsertBefore(Node* new_child, Node* ref_child) {
  if (ref_child == nullptr) return AppendChild(new_child);
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  ASSERT(ref_child->node_type() != ATTRIBUTE_NODE);
  return InsertBeforeInternal(new_child, ref_child);
}

Attribute* Element::RemoveAttribute(const String& name) {
  if (auto attr = GetAttributeNode(name)) {
    RemoveChildInternal(attr);
    return attr;
  }
  return nullptr;
}

Node* Element::RemoveChild(Node* old_child) {
  ASSERT(old_child != nullptr);
  ASSERT(old_child->node_type() != ATTRIBUTE_NODE);
  return RemoveChildInternal(old_child);
}

Node* Element::ReplaceChild(Node* new_child, Node* old_child) {
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  ASSERT(old_child != nullptr);
  ASSERT(old_child->node_type() != ATTRIBUTE_NODE);
  return ReplaceChildInternal(new_child, old_child);
}

Element& Element::SetAttribute(const String& name, const String& value) {
  auto const doc = document();
  if (auto const attr = GetAttributeNode(name)) {
    auto const new_attr = new(doc) Attribute(
        doc, attr->node_name(), doc->SaveString(value));
    ReplaceChildInternal(new_attr, attr);

  } else {
    auto const new_attr = new(doc) Attribute(
        doc, doc->InternName(name), doc->InternName(value));
    InsertBeforeInternal(new_attr, first_child_);
  }

  return *this;
}

void Element::ToWriter(TextWriter& writer) const {
  writer.Write('<');
  writer.Write(name());
  foreach (EnumAttribute, attrs, this) {
    auto& attr = attrs.Get();
    writer.Write(' ');
    attr.ToWriter(writer);
  }

  if (first_child() == nullptr) {
    writer.Write("/>");
    return;
  }

  writer.Write('>');

  foreach (EnumChildNode, childs, this) {
    childs.Get().ToWriter(writer);
  }

  writer.Write("</");
  writer.Write(name());
  writer.Write('>');
}

void* Node::operator new(size_t const cb, Document* const doc) {
  return doc->Alloc(cb);
}

Node::Node(Document* doc)
  : flags_(FLAG_FLOATING),
    next_sibling_(nullptr),
    parent_node_(doc),
    previous_sibling_(nullptr) {
  ASSERT(parent_node_ != nullptr);
}

Document* Node::document() const {
  for (auto p = this; p; p = p->parent_node_) {
    if (auto const doc = p->DynamicCast<Document>()) {
      return doc;
    }
  }
  CAN_NOT_HAPPEN();
}

ParentNode* Node::parent_node() const {
  return IsFloating() ? nullptr : parent_node_;
}

bool Node::TreeEquals(const Node& node1, const Node& node2) {
  if (node1 == node2) {
    return true;
  }

  if (node1.node_type() != node2.node_type()) {
    return false;
  }

  if (auto const elem1 = node1.DynamicCast<Element>()) {
    auto const elem2 = node2.StaticCast<Element>();
    Element::EnumAttribute attrs2(elem2);
    foreach (Element::EnumAttribute, attrs1, elem1) {
      if (attrs2.AtEnd()) return false;
      auto& attr1 = attrs1.Get();
      if (attr1.value() != elem2->GetAttribute(attr1.name())) {
        return false;
      }
      attrs2.Next();
    }

    if (!attrs2.AtEnd()) {
      return false;
    }
  }

  if (auto const parent1 = node1.DynamicCast<ParentNode>()) {
    auto const parent2 = node2.StaticCast<ParentNode>();
    ParentNode::EnumChildNode enum1(parent1);
    ParentNode::EnumChildNode enum2(parent2);
    while (!enum1.AtEnd() && !enum2.AtEnd()) {
      if (!TreeEquals(enum1.Get(), enum2.Get())) {
        return false;
      }
      enum1.Next();
      enum2.Next();
    }
    return enum1.AtEnd() && enum2.AtEnd();
  }

  return false;
}

String Node::ToString() const {
  Io::StringWriter writer;
  ToWriter(writer);
  return writer.ToString();
}

// ParentNode

ParentNode::ParentNode(Document* const doc)
  : Base(doc),
    first_child_(nullptr),
    last_child_(nullptr) {
}

Node* ParentNode::AppendChild(Node* const new_child) {
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  return AppendChildInternal(new_child);
}

Node* ParentNode::AppendChildInternal(Node* const new_child) {
  ASSERT(new_child != nullptr);
  if (!new_child->IsFloating()) {
    new_child->parent_node_->RemoveChildInternal(new_child);
  }
  new_child->ClearFlag(FLAG_FLOATING);
  new_child->parent_node_ = this;
  new_child->next_sibling_ = nullptr;
  new_child->previous_sibling_ = last_child_;
  if (last_child_) {
    last_child_->next_sibling_ = new_child;
  } else {
    first_child_ = new_child;
  }
  last_child_ = new_child;
  return new_child;
}

Node* ParentNode::InsertBefore(Node* const new_child, Node* const ref_child) {
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  ASSERT(ref_child != nullptr);
  ASSERT(ref_child->node_type() != ATTRIBUTE_NODE);
  return InsertBeforeInternal(new_child, ref_child);
}

Node* ParentNode::InsertBeforeInternal(
    Node* const new_child,
    Node* const ref_child) {
  ASSERT(new_child != nullptr);

  if (ref_child == nullptr) {
    return AppendChildInternal(new_child);
  }

  ASSERT(ref_child != nullptr);
  ASSERT(!ref_child->IsFloating());
  ASSERT(ref_child->parent_node_ == this);
  ASSERT(new_child != ref_child);

  if (!new_child->IsFloating()) {
    RemoveChildInternal(new_child);
  }

  auto const prev = ref_child->previous_sibling_;

  new_child->ClearFlag(FLAG_FLOATING);
  new_child->parent_node_ = this;
  new_child->next_sibling_ = ref_child;
  new_child->previous_sibling_ = prev;

  if (prev) {
    prev->next_sibling_ = new_child;
  } else {
    ASSERT(first_child_ == ref_child);
    first_child_ = new_child;
  }

  return new_child;
}

Node* ParentNode::RemoveChild(Node* const old_child) {
  ASSERT(old_child != nullptr);
  ASSERT(old_child->node_type() != ATTRIBUTE_NODE);
  return RemoveChildInternal(old_child);
}

Node* ParentNode::RemoveChildInternal(Node* const old_child) {
  ASSERT(old_child != nullptr);
  ASSERT(old_child->parent_node_ == this);
  ASSERT(!old_child->IsFloating());

  auto const next = old_child->next_sibling_;
  auto const prev = old_child->previous_sibling_;

  if (next) {
    next->previous_sibling_ = prev;
  } else {
    last_child_ = prev;
  }

  if (prev) {
    prev->next_sibling_ = next;
  } else {
    first_child_ = next;
  }

  old_child->parent_node_ = document();
  old_child->SetFlag(FLAG_FLOATING);

  return old_child;
}

Node* ParentNode::ReplaceChild(Node* const new_child, Node* const ref_child) {
  ASSERT(new_child != nullptr);
  ASSERT(new_child->node_type() != ATTRIBUTE_NODE);
  ASSERT(ref_child != nullptr);
  ASSERT(ref_child->node_type() != ATTRIBUTE_NODE);
  return ReplaceChildInternal(new_child, ref_child);
}

Node* ParentNode::ReplaceChildInternal(
    Node* const new_child,
    Node* const old_child) {
  ASSERT(new_child != nullptr);
  ASSERT(old_child != nullptr);
  ASSERT(!old_child->IsFloating());
  ASSERT(old_child->parent_node_ == this);
  ASSERT(old_child != new_child);

  if (!new_child->IsFloating()) {
    RemoveChildInternal(new_child);
  }

  auto const next = old_child->next_sibling_;
  auto const prev = old_child->previous_sibling_;

  if (next) {
    next->previous_sibling_ = new_child;
  } else {
    last_child_ = new_child;
  }

  if (prev) {
    prev->next_sibling_ = new_child;
  } else {
    first_child_ = new_child;
  }

  old_child->next_sibling_ = nullptr;
  old_child->previous_sibling_ = nullptr;
  old_child->parent_node_ = document();
  old_child->SetFlag(FLAG_FLOATING);

  new_child->next_sibling_ = next;
  new_child->previous_sibling_ = prev;
  new_child->parent_node_ = this;
  new_child->ClearFlag(FLAG_FLOATING);

  return new_child;
}

// [T]
void ParentNode::ToWriter(TextWriter& writer) const {
  foreach (EnumChildNode, childs, this) {
    childs.Get().ToWriter(writer);
  }
}

::std::ostream& operator<<(::std::ostream& os, const Node& node) {
  return os << node.ToString();
}

} // Dom
