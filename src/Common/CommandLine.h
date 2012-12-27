// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_CommandLine)
#define INCLUDE_Common_CommandLine

namespace Common {

class CommandLine {
  private: const char16* command_line_;

  public: class Iterator {
    private: const char16* current_;
    private: StringBuilder builder_;

    public: Iterator(const char16* string) : current_(string) {
      ExtractWord();
    }

    public: Iterator(const Iterator& another)
      : current_(another.current_) {
    }

    public: Iterator& operator=(const Iterator& another) {
      current_ = another.current_;
      builder_.Clear();
      return *this;
    }

    public: bool operator==(const Iterator& another) const {
      return current_ == another.current_ && builder_.IsEmpty();
    }

    public: bool operator!=(const Iterator& another) const {
      return !operator==(another);
    }

    public: String operator*() const {
      ASSERT(!builder_.IsEmpty());
      return builder_.ToString();
    }

    public: Iterator& operator++() {
      ExtractWord();
      return *this;
    }

    private: static bool IsSpace(char16 const wch) {
      return wch == ' ' || wch == '\t';
    }

    private: void ExtractWord() {
      builder_.Clear();

      enum State {
        State_DoubleQuote,
        State_Start,
        State_Word,
      } state = State_Start;

      for (;;) {
        auto const ch = *current_;
        if (!ch)
          return;

        current_++;

        switch (state) {
          case State_DoubleQuote:
            if (ch == '"')
              return;
            builder_.Append(ch);
            break;

          case State_Start:
            if (ch == '"') {
              state = State_DoubleQuote;
            } else if (!IsSpace(ch)) {
              builder_.Append(ch);
              state = State_Word;
            }
            break;

         case State_Word:
           if (IsSpace(ch))
             return;
           builder_.Append(ch);
           break;

         default:
           CAN_NOT_HAPPEN();
        }
      }
    }
  };

  public: CommandLine() : command_line_(::GetCommandLineW()) {}
  public: Iterator begin() { return Iterator(command_line_); }

  public: Iterator end() {
    return Iterator(command_line_ + ::lstrlenW(command_line_));
  }

  DISALLOW_COPY_AND_ASSIGN(CommandLine);
};

} // Common

#endif // !defined(INCLUDE_Common_CommandLine)
