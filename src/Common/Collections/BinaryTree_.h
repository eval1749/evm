// @(#)$Id$
//
// Common Collections BinaryTree_
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Collections_BinaryTree_h)
#define INCLUDE_Common_Collections_BinaryTree_h

#include "./ArrayList_.h"

namespace Common {
namespace Collections {

// TValue must have following methods:
//  bool operator ==(const TKey);
//  bool operator ==(const TValue);
//  bool operator ==(cosnt TKey);
//  bool operator ==(cosnt TValue);
template<typename TKey, typename TValue>
class BinaryTree_ {
  private: class TreeNode {
    private: TreeNode* left_;
    private: int priority_;
    private: TreeNode* right_;
    private: TValue value_;

    // ctor
    public: TreeNode(const TValue& value)
        : left_(nullptr),
          priority_(Random()),
          right_(nullptr),
          value_(value) {}

    public: ~TreeNode() {
      DEBUG_PRINTF("%p\n", this);
      delete left_;
      delete right_;
    }

    public: TreeNode* left() { return left_; }
    public: TreeNode* right() { return right_; }
    public: TValue& value() { return value_; }

    // [D]
    public: TreeNode* Delete() {
      if (!left_) {
        auto const right = right_;
        left_ = nullptr;
        right_ = nullptr;
        delete this;
        return right;
      }

      if (!right_) {
        auto const left = left_;
        left_ = nullptr;
        right_ = nullptr;
        delete this;
        return left;
      }

      if (left_->priority_ < right_->priority_) {
        auto& temp = RotateRight();
        temp.right_ = Delete();
        return &temp;
      }

      auto& temp = RotateLeft();
      temp.left_ = Delete();
      return &temp;
    }

    public: TreeNode* Delete(const TKey& key) {
      if (!this) {
        return nullptr;
      }
  
      if (value_ == key) {
        return Delete();
      }
  
      if (value_ < key) {
        right_ = right_->Delete(key);
      } else {
        left_ = left_->Delete(key);
      }
  
      return this;
    }
  
    // [F]
    public: TreeNode* Find(const TKey& key) const {
      if (!this) {
        return nullptr;
      }

      if (value_ == key) {
        return const_cast<TreeNode*>(this);
      }

      return value_ < key
          ? right_->Find(key)
          : left_->Find(key);
    }

    public: TreeNode& Insert(const TValue& value) {
      if (!this) {
        return *new TreeNode(value);
      }
  
      if (value_ < value) {
        auto& right = right_->Insert(value);
        right_ = &right;
        return right.priority_ > priority_
            ? RotateLeft()
            : *this;
      }
  
      auto& left = left_->Insert(value);
      left_ = &left;
      return left.priority_ > priority_
          ? RotateRight()
          : *this;
    }
  
    // [R]
    private: static int Random() {
      #pragma warning(push)
      // warning C4640: 'seed' : construction of local static object is not
      // thread-safe
      #pragma warning(disable: 4640)
      static int32 seed = ::GetTickCount();
      #pragma warning(pop)
      seed = seed * 1664525 + 1013904223;
      auto const mask = (1 << 30) - 1;
      return seed & mask;
    }

    private: TreeNode& RotateLeft() {
      ASSERT(right_ != nullptr);
      auto const temp = right_;
      right_ = right_->left_;
      temp->left_ = this;
      return *temp;
    }

    private: TreeNode& RotateRight() {
      ASSERT(left_ != nullptr);
      auto const temp = left_;
      left_ = left_->right_;
      temp->right_ = this;
      return *temp;
    }
  };

  public: class Enum {
    private: ArrayList_<TreeNode*> nodes_;
    private: int curr_;

    public: Enum(const BinaryTree_& r) : curr_(0) {
      Populate(r.root_);
    }

    public: bool AtEnd() const { return curr_ == nodes_.Count(); }

    public: const TValue& Get() {
      ASSERT(!AtEnd());
      return nodes_.Get(curr_)->value();
    }

    public: void Next() {
      ASSERT(!AtEnd());
      ++curr_;
    }

    private: void Populate(TreeNode* node) {
      if (node) {
        Populate(node->left());
        nodes_.Add(node);
        Populate(node->right());
      }
    }
  };

  private: TreeNode* root_;

  // ctor
  public: BinaryTree_() : root_(nullptr) {}
  public: ~BinaryTree_() { delete root_; }

  // [D]
  public: void Delete(const TKey& key) {
    root_ = root_->Delete(key);
  }

  // [F]
  public: TValue Find(const TKey& key) const {
    auto const node = root_->Find(key);
    return node ? node->value() : TValue(0);
  }

  // [I]
  public: void Insert(const TValue& value) {
    root_ = &root_->Insert(value);
  }
};

} // Collections
} // Common

#endif // !defined(INCLUDE_Common_Collections_BinaryTree_h)
