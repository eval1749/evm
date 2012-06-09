// @(#)$Id$
//
// Evita Compiler - ErrorInfo
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_ErrorInfo_h)
#define INCLUDE_Il_Ir_ErrorInfo_h

namespace Il {
namespace Ir {

using Common::Box;
using Common::Collections::Collection_;
class SourceInfo;

class ErrorInfo
    : public Object_<ErrorInfo>,
      public DoubleLinkedItem_<ErrorInfo> {
  CASTABLE_CLASS(ErrorInfo);

  public: typedef DoubleLinkedList_<ErrorInfo> List;

  public: class EnumParam : public Collection_<Box>::Enum {
    public: EnumParam(const ErrorInfo& r)
        : Collection_<Box>::Enum(r.params_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumParam);
  };

  private: SourceInfo const source_info_;
  private: int const error_code_;
  private: Collection_<Box> const params_;

  // ctor
  protected: ErrorInfo(const SourceInfo&, int, const Collection_<Box>&);
  public:  virtual ~ErrorInfo() {}

  // operators
  public: bool operator==(const ErrorInfo&) const;
  public: bool operator!=(const ErrorInfo&) const;
  public: bool operator<(const ErrorInfo&) const;
  public: bool operator<=(const ErrorInfo&) const;
  public: bool operator>(const ErrorInfo&) const;
  public: bool operator>=(const ErrorInfo&) const;

  // properties
  public: int error_code() const { return error_code_; }
  public: const Collection_<Box> params() const { return params_; }
  public: const SourceInfo& source_info() const { return source_info_; }

  // [C]
  public: int ComputeHashCode() const;
  public: virtual ErrorInfo& Clone() const = 0;

  DISALLOW_COPY_AND_ASSIGN(ErrorInfo);
};

template<class TSelf, class TParent = ErrorInfo>
class ErrorInfo_ : public WithCastable3_<TSelf, TParent, const SourceInfo&, 
                                         int, const Collection_<Box>&> {
  protected: typedef ErrorInfo_ Base;
  protected: ErrorInfo_(
    const SourceInfo& si,
    int const ec,
    const Collection_<Box>& param)
    : WithCastable3_(si, ec, param) {}
  DISALLOW_COPY_AND_ASSIGN(ErrorInfo_);
};

} // Ir
} // Il

#endif // !defined(INCLUDE_Il_Ir_ErrorInfo_h)
