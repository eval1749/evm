// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_NameRef_h)
#define INCLUDE_evc_NameRef_h

#include "./Operand.h"
#include "../WithSourceInfo.h"

namespace Il {
namespace Ir {

using Common::Collections::Vector_;

/// <summary>
//   Represents name reference. Instanes are used for part of qualified name.
/// </summary>
class NameRef : public Operand_<NameRef>, public WithSourceInfo {
  CASTABLE_CLASS(NameRef)

  public: class Item : public Object {
    CASTABLE_CLASS_(Item, Object);
    private: const Name& name_;
    private: const SourceInfo source_info_;
    protected: Item(const Name&, const SourceInfo&);
    public: virtual ~Item() {}
    public: bool operator ==(const Item& r) const { return Equals(r); }
    public: bool operator !=(const Item& r) const { return !operator ==(r); }
    public: virtual const NameRef& arg(int) const { CAN_NOT_HAPPEN(); }
    public: virtual int number_of_args() const { return 0; }
    public: const Name& name() const { return name_; }
    public: const SourceInfo& source_info() const { return source_info_; }
    public: virtual int ComputeHashCode() const = 0;
    protected: virtual bool Equals(const Item&) const = 0;
    DISALLOW_COPY_AND_ASSIGN(Item);
  };

  public: class CompositeName : public Item {
    CASTABLE_CLASS_(CompositeName, Item);
    public: class Enum : public Vector_<const NameRef*>::Enum {
      private: typedef Vector_<const NameRef*>::Enum Base;
      public: Enum(const CompositeName& r) : Base(r.args_) {}
      public: const NameRef& operator *() const { return *Base::Get(); }
      DISALLOW_COPY_AND_ASSIGN(Enum);
    };

    private: const Vector_<const NameRef*> args_;

    public: CompositeName(
        const Name& name,
        const Vector_<const NameRef*>& args,
        const SourceInfo& source_info)
        : Item(name, source_info), args_(args) {}

    public: virtual const NameRef& arg(int index) const override {
      return *args_[index];
    }

    public: virtual int number_of_args() const override {
      return int(args_.size());
    }

    public: virtual int ComputeHashCode() const override;
    protected: virtual bool Equals(const Item&) const override;
    public: virtual String ToString() const;
    DISALLOW_COPY_AND_ASSIGN(CompositeName);
  };

  public: class SimpleName : public Item {
    CASTABLE_CLASS_(SimpleName, Item);
    public: SimpleName(const Name& name, const SourceInfo& source_info)
        : Item(name, source_info) {}
    public: virtual int ComputeHashCode() const override;
    protected: virtual bool Equals(const Item&) const override;
    public: virtual String ToString() const;
    DISALLOW_COPY_AND_ASSIGN(SimpleName);
  };

  public: class Enum : public Vector_<const Item*>::Enum {
    typedef Vector_<const Item*>::Enum Base;
    public: Enum(const NameRef& name_ref) : Base(name_ref.items_) {}
    public: const Item& operator *() const { return *Base::Get(); }
    public: const Item* operator ->() const { return Base::Get(); }
    DISALLOW_COPY_AND_ASSIGN(Enum);
  };

  public: class ArgIterator {
    private: int index_;
    private: const NameRef& name_ref_;

    public: ArgIterator(const ArgIterator& another)
        : index_(another.index_), name_ref_(another.name_ref_) {
      ASSERT(index_ >= 0);
    }

    public: ArgIterator(const NameRef& name_ref, int index)
        : index_(index), name_ref_(name_ref) {
      ASSERT(index_ >= 0);
    }

    public: ArgIterator& operator =(const ArgIterator& another) {
      ASSERT(&name_ref_ == &another.name_ref_);
      index_ = another.index_;
      return *this;
    }

    public: const NameRef& operator *() const {
      return name_ref_.arg(index_);
    }

    public: const NameRef* operator ->() const {
      return &name_ref_.arg(index_);
    }

    public: bool operator ==(const ArgIterator& another) const {
      ASSERT(&name_ref_ == &another.name_ref_);
      return index_ == another.index_;
    }

    public: bool operator !=(const ArgIterator& another) const {
      return !operator ==(another);
    }

    public: ArgIterator& operator ++() {
      ++index_;
      return *this;
    }
  };

  public: class RankIterator {
    private: int index_;
    private: const NameRef& name_ref_;

    public: RankIterator(const RankIterator& another)
        : index_(another.index_), name_ref_(another.name_ref_) {}

    public: RankIterator(const NameRef& name_ref, int index)
        : index_(index), name_ref_(name_ref) {}

    public: RankIterator& operator =(const RankIterator& another) {
      ASSERT(&name_ref_ == &another.name_ref_);
      index_ = another.index_;
      return *this;
    }

    public: int operator *() const { return name_ref_.rank(index_); }

    public: bool operator ==(const RankIterator& another) const {
      ASSERT(&name_ref_ == &another.name_ref_);
      return index_ == another.index_;
    }

    public: bool operator !=(const RankIterator& another) const {
      return !operator ==(another);
    }

    public: RankIterator& operator ++() {
      --index_;
      return *this;
    }
  };

  private: const Vector_<const Item*> items_;
  private: const Vector_<int> ranks_;

  // ctor
  public: NameRef(const Name&, const SourceInfo&);
  public: explicit NameRef(const Vector_<const Item*>&);
  public: NameRef(const Vector_<const Item*>&, const Vector_<int>&);
  public: virtual ~NameRef();

  public: bool operator ==(const NameRef&) const;
  public: bool operator !=(const NameRef&) const;

  // properties
  public: const NameRef& arg(int) const;

  public: ArgIterator arg_begin() const {
    return ArgIterator(*this, 0);
  }

  public: ArgIterator arg_end() const {
    return ArgIterator(*this, items_[items_.size() - 1]->number_of_args());
  }

  public: const Name& name() const;
  public: const NameRef& qualifier() const;
  public: int rank(int const index) const { return ranks_[index]; }

  public: RankIterator rank_rbegin() const {
    return RankIterator(*this, ranks_.size() - 1);
  }

  public: RankIterator rank_rend() const {
    return RankIterator(*this, -1);
  }

  // [C]
  public: virtual int ComputeHashCode() const override;

  // [E]
  public: virtual bool Equals(const Operand&) const;

  // [G]
  public: const SimpleName* GetSimpleName() const;

  // [H]
  public: bool HasQualifier() const { return items_.size() > 1; }

  // [I]
  public: bool IsArray() const { return ranks_.size() > 0; }

  // [S]
  public: bool StartsWith(const NameRef&) const;

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(NameRef);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_evc_NameRef_h)
