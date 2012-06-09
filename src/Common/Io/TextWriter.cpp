#include "precomp.h"
// @(#)$Id$
//
// Evita Common.Io.TextWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./TextWriter.h"

#include "../Collections/Array_.h"
#include "../Collections/ArrayToString.h"
#include "../Collections/ArrayListToString.h"
#include "../Collections/CollectionToString.h"

namespace Common {
namespace Io {

using Common::Collections::CollectionV_;

namespace {
class Formatter {
  private: struct Desc {
    bool is_long_;
    bool is_right_;
    bool is_sign_;
    char padding_char_;
    int width_;

    Desc() { Reset(); }

    void Format(
        TextWriter& writer,
        const Box& box,
        int const base = 10,
        bool const sign = true,
        char const ten = 'A') {
      switch (box.type()) {
        case Box::Type_I32:
          Format(writer, box.i32(), base, sign, ten);
          break;

        case Box::Type_I64:
          Format(writer, box.i64(), base, sign, ten);
          break;

        case Box::Type_U32:
          Format(writer, box.u32(), base, false, ten);
          break;

        case Box::Type_U64:
          Format(writer, box.u64(), base, false, ten);
          break;

        case Box::Type_Null:
        case Box::Type_ObjectPtr:
        case Box::Type_String:
        case Box::Type_VoidPtr:
        default:
          CAN_NOT_HAPPEN();
      }
    }

    void Format(
        TextWriter& writer,
        int64 const i64,
        int const base,
        bool const sign,
        char const ten) {
      ASSERT(base >= 2 && base <= 36);
      uint64 u64 = i64;
      bool is_minus = sign && i64 < 0;
      if (is_minus) {
        u64 = -i64;
      }

      char16 wsz[64 + 2];
      auto const end = wsz + ARRAYSIZE(wsz);
      auto p = end;
      *--p = 0;
      if (u64 == 0) {
        *--p = '0';
      } else {
        while (u64) {
          auto const mod = u64 % base;
          u64 /= base;
          *--p = static_cast<char16>(mod >= 10 ? mod + ten - 10 : mod + '0');
        }
      }

      if (is_minus) {
        *--p = '-';
      } else if (is_sign_) {
        *--p = '+';
      }

      FormatField(writer, p);
    }

    void FormatField(TextWriter& writer, const String& str) {
      if (!is_right_) {
        writer.Write(str);
      }

      for (int i = str.length(); i < width_; ++i) {
        writer.Write(padding_char_);
      }

      if (is_right_) {
        writer.Write(str);
      }
    }

    void Reset() {
      is_long_ = false;
      is_right_ = false;
      is_sign_ = false;
      padding_char_ = ' ';
      width_ = 0;
    }
  };

  private: const String& format_;
  private: Collection_<Box>::Enum params_;
  private: TextWriter& writer_;

  public: Formatter(
    TextWriter& writer,
    const String& format,
    const Collection_<Box>& params)
    : format_(format), params_(params), writer_(writer) {}

  private: Box NextParam() {
    auto box = params_.Get();
    params_.Next();
    return box;
  }

  public: void Run() {
    enum State {
      State_Normal,
      State_Percent,
    } state = State_Normal;

    Desc desc;

    foreach (String::EnumChar, chars, format_) {
      auto const wch = *chars;
      switch (state) {
        case State_Normal:
          if (wch == '%') {
            desc.Reset();
            state = State_Percent;
          } else {
            writer_.Write(wch);
          }
          break;

        case State_Percent:
          switch (wch) {
            case '%':
              writer_.Write('%');
              state = State_Normal;
              break;

            case '-':
              desc.is_right_ = true;
              break;

            case '+':
              desc.is_sign_ = true;
              break;

            case '0':
              desc.is_right_ = true;
              desc.padding_char_ = '0';
              break;

            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
              desc.width_ *= 10;
              desc.width_ += wch - '0';
              break;

            case 'b':
              desc.Format(writer_, NextParam(), 2, false);
              state = State_Normal;
              break;

            case 'c':
              writer_.Write(static_cast<char16>(NextParam().i32()));
              state = State_Normal;
              break;

            case 'd':
            case 'i':
              desc.Format(writer_, NextParam());
              state = State_Normal;
              break;

            case 'h':
              desc.is_long_ = false;
              break;

            case 'l':
              desc.is_long_ = true;
              break;

            case 'p': {
              auto param = NextParam();
              switch (param.type()) {
                case Box::Type_ObjectPtr: {
                  char buf[20];
                  ::wsprintfA(buf, "%p", param.obj());
                  desc.FormatField(writer_, buf);
                  break;
                }

                case Box::Type_String:
                  desc.FormatField(writer_, param.str());
                  break;

                case Box::Type_VoidPtr: {
                  char buf[20];
                  ::wsprintfA(buf, "%p", param.ptr());
                  desc.FormatField(writer_, buf);
                  break;
                }

                case Box::Type_I32:
                case Box::Type_I64:
                case Box::Type_Null:
                case Box::Type_U32:
                case Box::Type_U64:
                  desc.FormatField(writer_, param.ToString());
                  break;

                default:
                  CAN_NOT_HAPPEN();
              }
              state = State_Normal;
              break;
            }

            case 'r':
              desc.Format(writer_, NextParam(), 36, false, 'a');
              state = State_Normal;
              break;

            case 'R':
              desc.Format(writer_, NextParam(), 36, false, 'A');
              state = State_Normal;
              break;

            case 's': {
              auto param = NextParam();
              switch (param.type()) {
                case Box::Type_ObjectPtr:
                  if (param.obj() == nullptr) {
                    desc.FormatField(writer_, "nullptr");
                  } else {
                    desc.FormatField(writer_, param.obj()->ToString());
                  }
                  break;

                case Box::Type_String:
                  desc.FormatField(writer_, param.str());
                  break;

                case Box::Type_VoidPtr: {
                  char buf[20];
                  ::wsprintfA(buf, "%p", param.ptr());
                  desc.FormatField(writer_, buf);
                  break;
                }

                case Box::Type_I32:
                case Box::Type_I64:
                case Box::Type_Null:
                case Box::Type_U32:
                case Box::Type_U64:
                  desc.FormatField(writer_, param.ToString());
                  break;

                default:
                  CAN_NOT_HAPPEN();
              }
              state = State_Normal;
              break;
            }

            case 'u':
              desc.Format(writer_, NextParam(), 10, false);
              state = State_Normal;
              break;

            case 'x':
              desc.Format(writer_, NextParam(), 16, true, 'a');
              state = State_Normal;
              break;

            case 'X':
              desc.Format(writer_, NextParam(), 16, true, 'A');
              state = State_Normal;
              break;

            default:
              CAN_NOT_HAPPEN();
          }
          break;

        default:
          CAN_NOT_HAPPEN();
      }
    } // for
    ASSERT(state == State_Normal);
  }

  DISALLOW_COPY_AND_ASSIGN(Formatter);
};
}  // namespace

void TextWriter::Write(char const ch) {
  Write(static_cast<char16>(ch));
}

void TextWriter::Write(char16 const wch) {
  char16 rgwch[1];
  rgwch[0] = wch;
  WriteStringImpl(rgwch, 1);
}

void TextWriter::Write(const String& str) {
  WriteStringImpl(str.value(), str.length());
}

void TextWriter::Write(
    const String& fmt,
    const Collection_<Box>& params) {
  Formatter formatter(*this, fmt, params);
  formatter.Run();
}

void TextWriter::WriteLine() {
  Write('\n');
}

void TextWriter::WriteLine(const String& str) {
  WriteStringImpl(str.value(), str.length());
  WriteLine();
}

void TextWriter::WriteLine(
    const String& fmt,
    const Collection_<Box>& params) {
  Write(fmt, params);
  WriteLine();
}

#define REPEATED_MACRO(n) \
  void TextWriter::Write(const String& fmt, PARAMS_DECL_##n(Box)) { \
    Formatter formatter(*this, fmt, CollectionV_<Box>(PARAMS_##n)); \
    formatter.Run(); \
  } \
  void TextWriter::WriteLine(const String& fmt, PARAMS_DECL_##n(Box)) { \
    Write(fmt, CollectionV_<Box>(PARAMS_##n)); \
    WriteLine(); \
  }

REPEAT_MACRO_9

} // Io
} // Common
