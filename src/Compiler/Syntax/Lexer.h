// @(#)$Id$
//
// Evita Compiler - Lexer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Lexer_h)
#define INCLUDE_Evita_Compiler_Lexer_h

namespace Compiler {

interface TokenProcessor;

// Lexer Token Types:
//  o Comments
//      - Comments aren't token. Lexer treats them as whitespaces.
//      - Block comment /* ... */
//      - Line comment //
//  o Literal in Il/Ir/Literal.h
//      - Character 'x'
//      - Number    12345 0xABCD, 123.45, 12e+10.
//      - String    "string" @"string"
//      - Note: Lexer doesn't handler number with sign.
//      - Note: Lexer returns keyword rather than literal for literal
//              names, e.g. false, null, true.
//  o Keyword in Compiler/Syntax/Keyword.h
//      - Context keywords, e.g. var, aren't Keyword.
//      - Keyword names are listed in Keywords.inc.
//      - K_abstract, K_as, ...
//  o Name in Il/Ir/Name.h
//      - Keywords followed by "@", e.g. @class, are Name.
//      - Well-known names are referred as Q_xxx in C++ source code.
//  o Operator in Compiler/Syntax/Operator.h
//      - Operators are referred as Op__xxx in C++ source code listed in
//        Operators.inc.
//  o Rank specification in Compiler/Syntax/Token.h
//      - rank([]) == 1
//      - rank([,]) == 2
//      - rank([,,]) == 3
class Lexer {
  private: static int const k_nMaxToken = 1024;

  private: enum State {
      // Verbatim Identifier @class
      // Verbatim String Literal @"\s+"
      State_At,
      State_At_DoubleQuote,
      State_At_DoubleQuote_DoubleQuote,
      State_At_Identifier,

    // /* ... */
      State_BlockComment,
      State_BlockComment_Asterisk,

    // Rank Specifier
      State_Bracket,
      State_Bracket_Comma,

    // Integral Liteal \d+ [LUlu]
      // Real Literal \d+ . \d+ E [+-] \d+ [Dd]
      State_Digit,
      State_Digit_Exponent,
      State_Digit_Exponent_Digit,
      State_Digit_Exponent_Minus,
      State_Digit_Exponent_Plus,
      State_Digit_Dot,
      State_Digit_Dot_Digit,
      State_Digit_L,
      State_Digit_U,

    // String literal
      State_DoubleQuote_BackSlash,
      State_DoubleQuote_Char,
      State_DoubleQuote_Hex,
      State_DoubleQuote_Start,
      State_DoubleQuote_Unicode,

    State_Error,

    State_HexDigit,

    State_Identifier,

    State_LineComment,
      State_LineStart,

    State_Operator,
      State_Operator_Double,

    State_Sharp,

    State_SingleQuote_BackSlash,
      State_SingleQuote_Char,
      State_SingleQuote_Hex,
      State_SingleQuote_Start,
      State_SingleQuote_Unicode,
      State_SingleQuote_Unicode_4,

    State_Slash_Start,

    State_Start,

    State_Zero_Start,
  };

  private: struct IntBuf {
      public: char m_ch;
      private: int m_cDigits;
      private: uint64 m_u64;

    public: void Accumlate(int const iBase, int const iDigit) {
      // TODO 2010-12-20 yosi@ Detect integer overflow
      // TODO 2010-12-20 yosi@ Detect float overflow
      m_cDigits += 1;
      m_u64 *= iBase;
      m_u64 += iDigit;
    }

    public: int GetInt() const { return static_cast<int32>(m_u64); }
    public: int32 GetInt32() const;
    public: int64 GetInt64() const;
    public: int GetLength() const { return m_cDigits; }
    public: uint32 GetUInt32() const;
    public: uint64 GetUInt64() const;
    public: bool IsInt64() const;

    public: void Start(uint const nAcc = 0) {
      m_cDigits = 0;
      m_u64 = nAcc;
    }
  };

  private: struct RealBuf {
    uint m_cDecimals;
    int m_iExponent;
    uint64 m_nDecimalPart;
    uint64 m_nIntPart;

    float32 GetFloat32() const;
    float64 GetFloat64() const;

    void Start(uint64 nIntPart) {
      m_cDecimals = 0;
      m_iExponent = 0;
      m_nDecimalPart = 0;
      m_nIntPart = nIntPart;
    }
  };


  private: class SourceTracker {
    private: int column_;
    private: int line_;

    public: SourceTracker() : column_(0), line_(0) {}

    public: SourceTracker(const SourceTracker& r)
        : column_(r.column_), line_(r.line_) {}

    // properties
    public: int column() const { return column_; }
    public: int line() const { return line_; }

    // [A]
    public: void AdvanceColumn() { column_ += 1; }
    public: void AdvanceLine() { column_ = 0; line_ += 1; }
  };

  // Source code buffer for recording source code location.
  private: class Source {
    private: const char16* const end_;
    private: const char16* cur_;
    private: SourceTracker& tracker_;
    private: int unget_char_;

    public: Source(
        SourceTracker& tracker,
        const char16* const text,
        int const text_len)
        : end_(text + text_len),
          cur_(text),
          tracker_(tracker),
          unget_char_(-1) {
    }

    public: bool AtEnd() const {
      ASSERT(cur_ <= end_);
      return unget_char_ == -1 && cur_ == end_;
   }

    public: char16 Get() {
      ASSERT(!AtEnd());

      if (unget_char_ >= 0) {
        auto const wch = static_cast<char16>(unget_char_);
        unget_char_ = -1;
        return wch;
      }

      auto const wch = *cur_++;
      if (wch == 0x0A) {
        tracker_.AdvanceLine();
      } else {
        tracker_.AdvanceColumn();
      }
      return wch;
    }

    public: void Unget(char16 wch) {
      ASSERT(unget_char_ == -1);
      unget_char_ = wch;
    }

    DISALLOW_COPY_AND_ASSIGN(Source);
  };

  // String buffer.
  private: struct StrBuf {
    int m_cwch;
    int m_cwchBuffer;
    char16* m_prgwch;

    StrBuf() :
          m_cwch(0),
          m_cwchBuffer(100),
          m_prgwch(new char16[m_cwchBuffer]) {}

    ~StrBuf() {
          delete[] m_prgwch;
      }

    void Add(char16 const wch) {
          if (m_cwch >= m_cwchBuffer) {
              int const cwch = m_cwchBuffer * 3 / 2;
              char16* const prgwch = new char16[cwch];

            ::CopyMemory(
                  prgwch,
                  m_prgwch,
                  sizeof(char16) * m_cwchBuffer);

            delete[] m_prgwch;
              m_prgwch = prgwch;
              m_cwchBuffer = cwch;
          }

        ASSERT(m_cwch < m_cwchBuffer);
          m_prgwch[m_cwch++] = wch;
      }

    void EndString() {
          Add(0);
          m_cwch -= 1;
      }

    char16 GetChar() const {
          ASSERT(m_cwch >= 1);
          return m_prgwch[0];
      }

    uint GetLength() const { return m_cwch; }
    const Name& name();
    String GetString();

    void Start() {
          m_cwch = 0;
      }
  };

  private: SourceTracker curr_loc_;
  private: String const file_name_;
  private: IntBuf int_buf_;
  private: TokenProcessor& processor_;
  private: RealBuf real_buf_;
  private: CompileSession& session_;
  private: StrBuf str_buf_;
  private: State state_;
  private: SourceTracker token_loc_;

  // ctor
  public: Lexer(CompileSession&, const String&, TokenProcessor&);

  // [A]
  private: void AddError(CompileError, Box);
  private: void AddError(CompileError, Box, Box);
  private: void AddError(CompileError, char16);

  // [F]
  public: void Feed(const char16*, uint);
  public: void Finish();
  private: void FinishIdentifier();
  private: void FinishOperator();

  // [G]
  private: SourceInfo GetTokenLocation() const;

  // [P]
  private: void Process(Source&);

  // [S]
  private: void SetCharLiteral(uint64);
  private: void SetLiteral(const String&);
  private: void SetLiteral(float32);
  private: void SetLiteral(float64);
  private: void SetLiteral(int32);
  private: void SetLiteral(int64);
  private: void SetLiteral(uint32);
  private: void SetLiteral(uint64);
  private: void SetOperator(const Operator&);
  private: void SetToken(const Token&);

  // [U]
  private: void UnexpectedEof();

  DISALLOW_COPY_AND_ASSIGN(Lexer);
};

}

#endif // !defined(INCLUDE_Evita_Compiler_Lexer_h)
