// @(#)$Id$
//
// Simple DOM.
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_test_Compiler_Syntax_Dom_h)
#define INCLUDE_test_Compiler_Syntax_Dom_h

#include "../Il/IlTest.h"

namespace Common {
namespace Io {
class TextWriter;
} // Io
} // Common

namespace Dom {

enum NodeType {
  ATTRIBUTE_NODE,
  DOCUMENT_NODE,
  ELEMENT_NODE,
  TEXT_NODE,
};

class Attribute;
class Document;
class Element;
class ParentNode;
class Text;
using Common::Io::TextWriter;

// DOM Node abstract class
class Node : public WithCastable_<Node, Object> {
  CASTABLE_CLASS(Node);

  friend class Element; // for next_sibling_ of attribute.
  friend class ParentNode;

  protected: void* operator new(size_t, Document*);

  protected: enum Flag {
    FLAG_FLOATING = 1 << 0,
  };

  private: int flags_;
  private: Node* next_sibling_;
  private: ParentNode* parent_node_;
  private: Node* previous_sibling_;

  // ctor
  protected: Node(Document*);
  public: virtual ~Node() {}

  // properties
  public: virtual Document* document() const;
  public: virtual Node* next_sibling() const { return next_sibling_; }
  public: virtual String node_name() const = 0;
  public: virtual NodeType node_type() const = 0;
  public: virtual ParentNode* parent_node() const;
  public: virtual Node* previous_sibling() const { return previous_sibling_; }

  // [C]
  protected: void ClearFlag(int f) { flags_ &= ~f; }

  // [I]
  protected: bool IsFloating() const {
    return (flags_ & FLAG_FLOATING) != 0;
  }

  // [S]
  protected: void SetFlag(int f) { flags_ |= f; }

  // [T]
  public: virtual String ToString() const override;
  public: virtual void ToWriter(TextWriter&) const = 0;
  public: static bool TreeEquals(const Node&, const Node&);

  DISALLOW_COPY_AND_ASSIGN(Node);
}; // Node

// DOM Attribute class.
class Attribute : public WithCastable1_<Attribute, Node, Document*> {
  CASTABLE_CLASS(ParentNode);
  friend class Element;

  private: String const name_;
  private: String const value_;

  protected: Attribute(Document*, const String&, const String&);
  public: virtual ~Attribute() {}

  // properties
  public: String name() const { return name_; }
  public: virtual Node* next_sibling() const override { return nullptr; }
  public: virtual String node_name() const override { return name_; }
  public: virtual NodeType node_type() const override { return ATTRIBUTE_NODE; }
  public: virtual ParentNode* parent_node() const override { return nullptr; }
  public: virtual Node* previous_sibling() const override { return nullptr; }
  public: const String& value() const { return value_; }

  // [T]
  public: virtual void ToWriter(TextWriter&) const override;

  DISALLOW_COPY_AND_ASSIGN(Attribute);
}; // Attribute

// DOM Parent node abstract class.
class ParentNode : public WithCastable1_<ParentNode, Node, Document*> {
  CASTABLE_CLASS(ParentNode);
  friend class Element; // first_child_, last_child_

  public: class EnumChildNode {
    private: Node* node_;

    public: EnumChildNode(const ParentNode* parent)
        : node_(parent->first_child()) {
    }

    public: bool AtEnd() const {
      return node_ == nullptr;
    }

    public: Node& Get() const {
      ASSERT(!AtEnd());
      return *node_;
    }

    public: void Next() {
      node_ = node_->next_sibling();
    }
  }; // EnumChildNode

  private: Node* first_child_;
  private: Node* last_child_;

  protected: ParentNode(Document*);
  public: virtual ~ParentNode() {}

  public: virtual Node* first_child() const { return first_child_; }
  public: virtual Node* last_child() const { return last_child_; }

  public: virtual Node* AppendChild(Node*);
  protected: Node* AppendChildInternal(Node*);
  public: virtual Node* InsertBefore(Node*, Node*);
  protected: Node* InsertBeforeInternal(Node*, Node*);
  public: virtual Node* RemoveChild(Node*);
  protected: Node* RemoveChildInternal(Node*);
  public: virtual Node* ReplaceChild(Node*, Node*);
  protected: Node* ReplaceChildInternal(Node*, Node*);

  // [T]
  public: virtual void ToWriter(TextWriter&) const override;

  DISALLOW_COPY_AND_ASSIGN(ParentNode);
}; // ParentNode

// DOM Document node class.
class Document : public WithCastable1_<Document, ParentNode, Document*> {
  CASTABLE_CLASS(Document);

  private: LocalMemoryZone zone_;

  public: Document();
  public: virtual ~Document() {}

  public: virtual String node_name() const override;

  public: virtual NodeType node_type() const override {
    return DOCUMENT_NODE;
  }

  // [A]
  public: void* Alloc(size_t cb) { return zone_.Alloc(cb); }

  // [C]
  public: Element* CreateElement(const String&);

  // [N]
  public: static Document* New();

  // [S]
  public: String InternName(const String& s) { return SaveString(s); }
  public: String SaveString(const String&);

  DISALLOW_COPY_AND_ASSIGN(Document);
}; // Document

// DOM Element node class.
class Element : public WithCastable1_<Element, ParentNode, Document*> {
  CASTABLE_CLASS(Element);
  friend class Document;

  public: class EnumAttribute {
    private: Node* node_;

    public: EnumAttribute(const Element* elem)
        : node_(elem->first_child_) {
    }

    public: bool AtEnd() const {
      return node_ == nullptr || node_->node_type() != ATTRIBUTE_NODE;
    }

    public: Attribute& Get() const {
      ASSERT(!AtEnd());
      return *node_->StaticCast<Attribute>();
    }

    public: void Next() {
      node_ = node_->next_sibling_;
    }
  }; // EnumAttribute

  private: String const name_;

  protected: Element(Document*, const String&);
  public: virtual ~Element() {}

  // properties
  public: virtual Node* first_child() const override;
  public: String name() const { return name_; }
  public: virtual String node_name() const override { return name_; }
  public: virtual NodeType node_type() const override { return ELEMENT_NODE; }
  public: virtual Node* last_child() const override;

  // [A]
  public: virtual Node* AppendChild(Node*) override;

  // [G]
  public: String GetAttribute(const String&) const;

  // [I]
  public: Attribute* GetAttributeNode(const String&) const;
  public: virtual Node* InsertBefore(Node*, Node*) override;

  // [R]
  public: Attribute* RemoveAttribute(const String&);
  public: virtual Node* RemoveChild(Node*) override;
  public: virtual Node* ReplaceChild(Node*, Node*) override;

  // [S]
  public: Element& SetAttribute(const String&, const String&);

  // [T]
  public: virtual void ToWriter(TextWriter&) const override;

  DISALLOW_COPY_AND_ASSIGN(Element);
}; // Element

::std::ostream& operator<<(::std::ostream&, const Node&);

} // Dom

#endif // !defined(INCLUDE_test_Compiler_Syntax_Dom_h)
