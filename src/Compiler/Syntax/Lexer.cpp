#include "precomp.h"
// @(#)$Id$
//
// Evita Lexer - FileLexer
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Lexer.h"

#include "./Keyword.h"
#include "./Operator.h"
#include "./Token.h"
#include "./TokenProcessor.h"

#include "../CompileErrorInfo.h"
#include "../CompilerGlobal.h"
#include "../CompileSession.h"

namespace Compiler {

class Keyword;

enum CharType {
  CharType_Space      = 1 << 0,
  CharType_IdentPart  = 1 << 1,
  CharType_IdentStart = 1 << 2,
  CharType_Digit      = 1 << 3,
  CharType_Newline    = 1 << 4,

  CharType_OpSingle       = 1 << 5,   // %
  CharType_OpDouble       = 1 << 6,   // ++
  CharType_OpEqual        = 1 << 7,   // +=
  CharType_OpDoubleEqual  = 1 << 7,   // <<=
}; // CharType

// U+2028 Line Separator
// U+2029 Paragraph Separator
static uint const k_rgnCharType[0x128] = {
    0,  // U+0000
    0,  // U+0001
    0,  // U+0002
    0,  // U+0003
    0,  // U+0004
    0,  // U+0005
    0,  // U+0006
    0,  // U+0007
    0,  // U+0008
    CharType_Space,     // U+0009 Horizonal Tab
    CharType_Newline,   // U+000A
    CharType_Space,     // U+000B Vertical Tab
    CharType_Space,     // U+000C Form-Feed
    CharType_Newline,   // U+000D
    0,  // U+000E
    0,  // U+000F

    0,  // U+0010
    0,  // U+0011
    0,  // U+0012
    0,  // U+0013
    0,  // U+0014
    0,  // U+0015
    0,  // U+0016
    0,  // U+0017
    0,  // U+0018
    0,  // U+0019
    0,  // U+001A
    0,  // U+001B
    0,  // U+001C
    0,  // U+001D
    0,  // U+001E
    0,  // U+001F

    CharType_Space,                             // U+0020
    CharType_OpEqual,                           // U+0021 ! !=
    0,                                          // U+0022
    0,                                          // U+0023
    0,                                          // U+0024
    CharType_OpEqual,                           // U+0025 % %=
    CharType_OpDouble | CharType_OpEqual,       // U+0026 & && &=
    0,                                          // U+0027
    CharType_OpSingle,                          // U+0028
    CharType_OpSingle,                          // U+0029
    CharType_OpEqual,                           // U+002A * *=
    CharType_OpDouble | CharType_OpEqual,       // U+002B + ++ +=
    CharType_OpSingle,                          // U+002C ,
    CharType_OpDouble | CharType_OpEqual,       // U+002D - -- -=
    CharType_OpSingle,                          // U+002E . ...
    CharType_OpSingle | CharType_OpEqual,       // U+002F / /=

    CharType_Digit | CharType_IdentPart,        // U+0030
    CharType_Digit | CharType_IdentPart,        // U+0031
    CharType_Digit | CharType_IdentPart,        // U+0032
    CharType_Digit | CharType_IdentPart,        // U+0033
    CharType_Digit | CharType_IdentPart,        // U+0034
    CharType_Digit | CharType_IdentPart,        // U+0035
    CharType_Digit | CharType_IdentPart,        // U+0036
    CharType_Digit | CharType_IdentPart,        // U+0037
    CharType_Digit | CharType_IdentPart,        // U+0038
    CharType_Digit | CharType_IdentPart,        // U+0039
    CharType_OpSingle,                          // U+003A :
    CharType_OpSingle,                          // U+003B ;
    CharType_OpDouble                           // U+003C < << <= <<=
        | CharType_OpDoubleEqual
        | CharType_OpEqual,
    CharType_OpEqual,                           // U+003D = ==
    CharType_OpDouble                           // U+003C > >> >= >>=
        | CharType_OpDoubleEqual
        | CharType_OpEqual,
    CharType_OpDouble,                          // U+003F ? ??

    0,                                          // U+0040 @keyword
    CharType_IdentPart | CharType_IdentStart,   // U+0041
    CharType_IdentPart | CharType_IdentStart,   // U+0042
    CharType_IdentPart | CharType_IdentStart,   // U+0043
    CharType_IdentPart | CharType_IdentStart,   // U+0044
    CharType_IdentPart | CharType_IdentStart,   // U+0045
    CharType_IdentPart | CharType_IdentStart,   // U+0046
    CharType_IdentPart | CharType_IdentStart,   // U+0047
    CharType_IdentPart | CharType_IdentStart,   // U+0048
    CharType_IdentPart | CharType_IdentStart,   // U+0049
    CharType_IdentPart | CharType_IdentStart,   // U+004A
    CharType_IdentPart | CharType_IdentStart,   // U+004B
    CharType_IdentPart | CharType_IdentStart,   // U+004C
    CharType_IdentPart | CharType_IdentStart,   // U+004D
    CharType_IdentPart | CharType_IdentStart,   // U+004E
    CharType_IdentPart | CharType_IdentStart,   // U+004F

    CharType_IdentPart | CharType_IdentStart,   // U+0050
    CharType_IdentPart | CharType_IdentStart,   // U+0051
    CharType_IdentPart | CharType_IdentStart,   // U+0052
    CharType_IdentPart | CharType_IdentStart,   // U+0053
    CharType_IdentPart | CharType_IdentStart,   // U+0054
    CharType_IdentPart | CharType_IdentStart,   // U+0055
    CharType_IdentPart | CharType_IdentStart,   // U+0056
    CharType_IdentPart | CharType_IdentStart,   // U+0057
    CharType_IdentPart | CharType_IdentStart,   // U+0058
    CharType_IdentPart | CharType_IdentStart,   // U+0059
    CharType_IdentPart | CharType_IdentStart,   // U+005A
    CharType_OpSingle,                          // U+005B [
    0,                                          // U+005C
    CharType_OpSingle,                          // U+005D ]
    CharType_OpEqual,                           // U+005E ^ ^=
    CharType_IdentPart | CharType_IdentStart,   // U+005F

    0,                                          // U+0060
    CharType_IdentPart | CharType_IdentStart,   // U+0061
    CharType_IdentPart | CharType_IdentStart,   // U+0062
    CharType_IdentPart | CharType_IdentStart,   // U+0063
    CharType_IdentPart | CharType_IdentStart,   // U+0064
    CharType_IdentPart | CharType_IdentStart,   // U+0065
    CharType_IdentPart | CharType_IdentStart,   // U+0066
    CharType_IdentPart | CharType_IdentStart,   // U+0067
    CharType_IdentPart | CharType_IdentStart,   // U+0068
    CharType_IdentPart | CharType_IdentStart,   // U+0069
    CharType_IdentPart | CharType_IdentStart,   // U+006A
    CharType_IdentPart | CharType_IdentStart,   // U+006B
    CharType_IdentPart | CharType_IdentStart,   // U+006C
    CharType_IdentPart | CharType_IdentStart,   // U+006D
    CharType_IdentPart | CharType_IdentStart,   // U+006E
    CharType_IdentPart | CharType_IdentStart,   // U+006F

    CharType_IdentPart | CharType_IdentStart,   // U+0070
    CharType_IdentPart | CharType_IdentStart,   // U+0071
    CharType_IdentPart | CharType_IdentStart,   // U+0072
    CharType_IdentPart | CharType_IdentStart,   // U+0073
    CharType_IdentPart | CharType_IdentStart,   // U+0074
    CharType_IdentPart | CharType_IdentStart,   // U+0075
    CharType_IdentPart | CharType_IdentStart,   // U+0076
    CharType_IdentPart | CharType_IdentStart,   // U+0077
    CharType_IdentPart | CharType_IdentStart,   // U+0078
    CharType_IdentPart | CharType_IdentStart,   // U+0079
    CharType_IdentPart | CharType_IdentStart,   // U+007A
    CharType_OpSingle,                          // U+007B {
    CharType_OpDouble | CharType_OpEqual,       // U+007C | || |=
    CharType_OpSingle,                          // U+007D }
    CharType_OpSingle,                          // U+007E ~
    0,  // U+007F
}; // k_rgnCharType

const char kBackSlashCharMap[128] = {
    1,  // U+0000
    0,  // U+0001
    0,  // U+0002
    0,  // U+0003
    0,  // U+0004
    0,  // U+0005
    0,  // U+0006
    0,  // U+0007
    0,  // U+0008
    0,  // U+0009
    0,  // U+000A
    0,  // U+000B
    0,  // U+000C
    0,  // U+000D
    0,  // U+000E
    0,  // U+000F

    0,  // U+0010
    0,  // U+0011
    0,  // U+0012
    0,  // U+0013
    0,  // U+0014
    0,  // U+0015
    0,  // U+0016
    0,  // U+0017
    0,  // U+0018
    0,  // U+0019
    0,  // U+001A
    0,  // U+001B
    0,  // U+001C
    0,  // U+001D
    0,  // U+001E
    0,  // U+001F

    0,  // U+0020
    0,  // U+0021
    '"',  // U+0022
    0,  // U+0023
    0,  // U+0024
    0,  // U+0025
    0,  // U+0026
    '\'',  // U+0027
    0,  // U+0028
    0,  // U+0029
    0,  // U+002A
    0,  // U+002B
    0,  // U+002C
    0,  // U+002D
    0,  // U+002E
    0,  // U+002F

    0,  // U+0030
    0,  // U+0031
    0,  // U+0032
    0,  // U+0033
    0,  // U+0034
    0,  // U+0035
    0,  // U+0036
    0,  // U+0037
    0,  // U+0038
    0,  // U+0039
    0,  // U+003A
    0,  // U+003B
    0,  // U+003C
    0,  // U+003D
    0,  // U+003E
    0,  // U+003F

    0,  // U+0040
    0,  // U+0041
    0,  // U+0042
    0,  // U+0043
    0,  // U+0044
    0,  // U+0045
    0,  // U+0046
    0,  // U+0047
    0,  // U+0048
    0,  // U+0049
    0,  // U+004A
    0,  // U+004B
    0,  // U+004C
    0,  // U+004D
    0,  // U+004E
    0,  // U+004F

    0,  // U+0050
    0,  // U+0051
    0,  // U+0052
    0,  // U+0053
    0,  // U+0054
    'u',  // U+0055
    0,  // U+0056
    0,  // U+0057
    0,  // U+0058
    0,  // U+0059
    0,  // U+005A
    0,  // U+005B
    '\\',  // U+005C
    0,  // U+005D
    0,  // U+005E
    0,  // U+005F

    0,  // U+0060
    7,  // U+0061 Alert
    8,  // U+0062 Backspace
    0,  // U+0063
    0,  // U+0064
    0,  // U+0065
    0x0C,  // U+0066 Formfeed
    0,  // U+0067
    0,  // U+0068
    0,  // U+0069
    0,  // U+006A
    0,  // U+006B
    0,  // U+006C
    0,  // U+006D
    0x0A,  // U+006E Newline
    0,  // U+006F

    0,  // U+0070
    0,  // U+0071
    0x0D,  // U+0072 Carriage Return
    0,  // U+0073
    0x09,  // U+0074 Horizontal Tab
    'u',  // U+0075
    0x0B,  // U+0076 Vertical Tab
    0,  // U+0077
    'x',  // U+0078
    0,  // U+0079 y
    0,  // U+007A z
    0,  // U+007B {
    0,  // U+007C |
    0,  // U+007D |
    0,  // U+007E ~
    0,  // U+007F
}; // kBackSlashCharMap

static int fromHexDigit(char16 const wch) {
  return
    wch >= '0' && wch <= '9'
    ? wch - '0'
    : wch >= 'A' && wch <= 'F'
    ? wch - 'A' + 10
    : wch >= 'a' && wch <= 'f'
    ? wch - 'a' + 10
    : -1;
}

static bool isDigit(char16 const wch) {
  return wch < lengthof(k_rgnCharType)
    ? 0 != (k_rgnCharType[wch] & CharType_Digit)
    : false;
}

static bool isIdentPart(char16 const wch) {
  return wch < lengthof(k_rgnCharType)
    ? 0 != (k_rgnCharType[wch] & CharType_IdentPart)
    // TOOD 2010-12-19 yosi@ Support full identifier part chars.
    : false;
}

static bool isIdentStart(char16 const wch) {
  return wch < lengthof(k_rgnCharType)
    ? 0 != (k_rgnCharType[wch] & CharType_IdentStart)
    // TOOD 2010-12-19 yosi@ Support full identifier part chars.
    : false;
}

static bool isNewlineChar(char16 const wch) {
  return wch < lengthof(k_rgnCharType)
    ? 0 != (k_rgnCharType[wch] & CharType_Newline)
    : 0x2028 == wch || 0x2029 == wch;
}

static bool isSpace(char16 const wch) {
  auto const category = Char::GetUnicodeCategory(wch);

  if (category == UnicodeCategory_Control) {
      return wch < lengthof(k_rgnCharType)
          ? 0 != (k_rgnCharType[wch] & CharType_Space)
          : false;
  }
    
  return category == UnicodeCategory_SpaceSeparator;
}

int32 Lexer::IntBuf::GetInt32() const {
  return static_cast<int32>(m_u64);
}

int64 Lexer::IntBuf::GetInt64() const {
  return static_cast<int64>(m_u64);
}

bool Lexer::IntBuf::IsInt64() const {
  return Literal::NeedInt64(m_u64);
}

uint32 Lexer::IntBuf::GetUInt32() const {
  return static_cast<uint32>(m_u64);
}

uint64 Lexer::IntBuf::GetUInt64() const {
  return static_cast<uint64>(m_u64);
}

// warning C4738: storing 32-bit float result in memory, possible loss of performance
#pragma warning(push)
#pragma warning(disable: 4738)
static float32 expt32(uint k) {
  auto f = static_cast<float32>(1);
  while (k > 0) {
    f *= 10;
    --k;
  }
  return f;
}
#pragma warning(pop)

// warning C4738: storing 32-bit float result in memory, possible loss of performance
#pragma warning(push)
#pragma warning(disable: 4738)
float32 Lexer::RealBuf::GetFloat32() const {
  auto f32 = static_cast<float32>(m_nIntPart)
    + (static_cast<float32>(m_nDecimalPart) / expt32(m_cDecimals));
  return m_iExponent >= 0
    ? f32 * expt32(m_iExponent)
    : f32 / expt32(-m_iExponent);
}
#pragma warning(pop)

static float64 expt64(uint k) {
  auto f = static_cast<float64>(1);
  while (k > 0) {
    f *= 10;
    --k;
  }
  return f;
}

float64 Lexer::RealBuf::GetFloat64() const {
  auto f64 = static_cast<float64>(m_nIntPart)
    + (static_cast<float64>(m_nDecimalPart) / expt64(m_cDecimals));
  return m_iExponent >= 0
    ? f64 * expt64(m_iExponent)
    : f64 / expt64(-m_iExponent);
}

String Lexer::StrBuf::GetString() {
  EndString();
  return String(m_prgwch, m_cwch);
}

const Name& Lexer::StrBuf::name() {
  ASSERT(GetLength() > 0);
  EndString();
  return Name::Intern(m_prgwch);
}

// ctor
Lexer::Lexer(
    CompileSession& session,
    const String& file_name,
    TokenProcessor& processor)
    : file_name_(file_name),
      processor_(processor),
      session_(session),
      state_(State_LineStart) {
  DEBUG_FORMAT("ctor: %s", file_name);
}

// [A]
void Lexer::AddError(CompileError const error_code, Box a) {
  session_.AddErrorInfo(
    CompileErrorInfo(
        SourceInfo(file_name_, curr_loc_.line(), curr_loc_.column()),
        error_code,
        CollectionV_<Box>(a)));
  state_ = State_Error;
}

void Lexer::AddError(CompileError const error_code, Box a, Box b) {
  session_.AddErrorInfo(
    CompileErrorInfo(
        SourceInfo(file_name_, curr_loc_.line(), curr_loc_.column()),
        error_code,
        CollectionV_<Box>(a, b)));
  state_ = State_Error;
}

void Lexer::AddError(CompileError const error_code, char16 const ch) {
  char16 str[2] = { ch, 0 };
  AddError(error_code, str);
}

// [F]
void Lexer::Feed(
    const char16* const pwchSource,
    uint const cwchSource) {
  ASSERT(pwchSource != nullptr);
  ASSERT(cwchSource > 0);
  Source source(curr_loc_, pwchSource, cwchSource);
  while (!source.AtEnd()) {
    Process(source);
    if (session_.HasError()) {
      return;
    }
  }
}

void Lexer::Finish() {
  if (state_ == State_Start) {
    return;
  }

  switch (state_) {
    case State_At:
      UnexpectedEof();
      break;

    case State_At_DoubleQuote:
      AddError(
          CompileError_Lexer_Unclosed_DoubleQuote,
          token_loc_.line(),
          token_loc_.column());
      break;

    case State_At_DoubleQuote_DoubleQuote:
      SetLiteral(str_buf_.GetString());
      break;

    case State_At_Identifier:
      str_buf_.EndString();
      SetToken(*new NameToken(GetTokenLocation(), str_buf_.name()));
      break;

    // /* ... */
    case State_BlockComment:
    case State_BlockComment_Asterisk:
      AddError(
          CompileError_Lexer_Unclosed_BlockComment,
          token_loc_.line(),
          token_loc_.column());
      break;

    // Rank Specifier
    case State_Bracket:
    case State_Bracket_Comma:
      AddError(
          CompileError_Lexer_Unclosed_RankSpec,
          token_loc_.line(),
          token_loc_.column());
      break;

    // Integral Liteal \d+ [LUlu]
    // Real Literal \d+ . \d+ E [+-] \d+ [Dd]
    case State_Digit:
      SetLiteral(int_buf_.GetInt32());
      break;

    case State_Digit_Exponent:
      UnexpectedEof();
      break;

    case State_Digit_Exponent_Digit:
      SetLiteral(real_buf_.GetFloat64());
      break;

    case State_Digit_Exponent_Minus:
    case State_Digit_Exponent_Plus:
    case State_Digit_Dot:
      UnexpectedEof();
      break;

    case State_Digit_Dot_Digit:
      real_buf_.m_cDecimals = int_buf_.GetLength();
      real_buf_.m_nDecimalPart = int_buf_.GetUInt64();
      SetLiteral(real_buf_.GetFloat64());
      break;

    case State_Digit_L:
      SetLiteral(int_buf_.GetInt64());
      break;

    case State_Digit_U:
      SetLiteral(int_buf_.GetUInt32());
      break;

    // String literal
    case State_DoubleQuote_BackSlash:
    case State_DoubleQuote_Char:
    case State_DoubleQuote_Hex:
    case State_DoubleQuote_Start:
    case State_DoubleQuote_Unicode:
      AddError(
          CompileError_Lexer_Unclosed_DoubleQuote,
          token_loc_.line(),
          token_loc_.column());
      break;

    case State_Error:
      break;

    case State_HexDigit:
      // TODO(yosi) 2012-03-18 Check integer overflow.
      SetLiteral(int_buf_.GetInt32());
      break;

    case State_Identifier:
      FinishIdentifier();
      break;

    case State_LineComment:
    case State_LineStart:
      break;

    case State_Operator:
    case State_Operator_Double:
      FinishOperator();
      break;

    case State_Sharp:
      break;

    case State_SingleQuote_BackSlash:
    case State_SingleQuote_Char:
    case State_SingleQuote_Hex:
    case State_SingleQuote_Start:
    case State_SingleQuote_Unicode:
    case State_SingleQuote_Unicode_4:
      AddError(
          CompileError_Lexer_Unclosed_SingleQuote,
          token_loc_.line(),
          token_loc_.column());
      break;

    case State_Slash_Start:
      SetOperator(*Op__Div);
      break;

    case State_Start:
      break;

    case State_Zero_Start:
      SetLiteral(0);
      break;

    default:
      CAN_NOT_HAPPEN();
  }
}

// Note: To implment operator false and operator true, we don't return
// Literal for false and true.
void Lexer::FinishIdentifier() {
  auto& name = str_buf_.name();
  if (auto const keyword = Keyword::Find(name)) {
    DEBUG_FORMAT("keyword %d:%d %s",
        token_loc_.line(),
        token_loc_.column(),
        name);
    SetToken(*new KeywordToken(GetTokenLocation(), *keyword));
    return;
  }

  DEBUG_FORMAT(" %d:%d name |%s|",
      token_loc_.line(),
      token_loc_.column(),
      name);
  SetToken(*new NameToken(GetTokenLocation(), name));
}

void Lexer::FinishOperator() {
  auto& name = str_buf_.name();
  if (auto const op = Operator::Find(name)) {
    SetOperator(*op);

  } else {
    state_ = State_Error;
    session_.AddErrorInfo(
        CompileErrorInfo(
            GetTokenLocation(),
            CompileError_Lexer_Invalid_Name,
            CollectionV_<Box>(name)));
  }
}

// [G]
SourceInfo Lexer::GetTokenLocation() const {
  return SourceInfo(
    file_name_,
    token_loc_.line(),
    token_loc_.column());
}

// [P]
void Lexer::Process(Source& source) {
  while (!source.AtEnd()) {
    auto const wch = source.Get();

    if (state_ == State_LineStart || state_ == State_Start) {
      token_loc_ = curr_loc_;
      DEBUG_PRINTF("Token location: %d:%d ch='%c' \n",
          token_loc_.line(),
          token_loc_.column(),
          wch < 0x20 ? 0x20 : wch);
    }

    tryAgain:
    switch (state_) {
      //
      // At
      //    @"...." raw string
      //    @name   raw identidier
      case State_At:
        if ('"' == wch) {
          state_ = State_At_DoubleQuote;
          break;
        }

        if (isIdentStart(wch)) {
          str_buf_.Add(wch);
          state_ = State_At_Identifier;
          break;
        }
        AddError(CompileError_Lexer_Expect_IdStart, wch);
        return;

      case State_At_DoubleQuote:
        if ('"' == wch) {
          state_ = State_At_DoubleQuote_DoubleQuote;
          break;
        }

        str_buf_.Add(wch);
        break;

      case State_At_DoubleQuote_DoubleQuote:
        if ('"' == wch) {
          str_buf_.Add('"');
          state_ = State_At_DoubleQuote;
          break;
        }

        SetLiteral(str_buf_.GetString());
        return;

      case State_At_Identifier:
        if (isIdentPart(wch)) {
          str_buf_.Add(wch);
          break;
        }

        source.Unget(wch);
        str_buf_.EndString();
        SetToken(*new NameToken(GetTokenLocation(), str_buf_.name()));
        return;

      //
      // BlockComment - No nesting
      //    /* ... */
      //
      case State_BlockComment:
          if ('*' == wch) {
              state_ = State_BlockComment_Asterisk;
          }
          break;

      case State_BlockComment_Asterisk:
          switch (wch) {
          case '*':
              break;

          case '/':
              state_ = State_Start;
              break;

          default:
              state_ = State_BlockComment;
              break;
          }
          break;

      //
      // Bracket
      //    '['
      //    [,,,]
      //
      case State_Bracket:
        switch (wch) {
          case ',':
            str_buf_.Add(',');
            state_ = State_Bracket_Comma;
            break;

          case ']':
            SetToken(*new RankSpecToken(GetTokenLocation(), 1));
            return;

          default:
            source.Unget(wch);
            SetOperator(*Op__OpenBracket);
            return;
        }
        break;

      case State_Bracket_Comma:
        switch (wch) {
          case ',':
            str_buf_.Add(',');
            break;

          case ']':
            SetToken(
                *new RankSpecToken(GetTokenLocation(), str_buf_.GetLength()));
            return;

          default:
            AddError(CompileError_Lexer_Bad_RankSpec, wch);
            return;
        }
        break;

      //
      // Digit
      //    1234 [luLU]{0,2}           int, uint
      //    1234 [eE] [+-]? 1234 [df]?   float32, float64
      case State_Digit:
        switch (wch) {
          case '0': case '1': case '2': case '3': case '4': 
          case '5': case '6': case '7': case '8': case '9':
            int_buf_.Accumlate(10, wch - '0');
            break;

          case '.':
            state_ = State_Digit_Dot;
            real_buf_.Start(int_buf_.GetInt());
            int_buf_.Start();
            break;

          case 'D': case 'd':
            // 1234d == (float64) 1234
            real_buf_.Start(int_buf_.GetInt());
            SetLiteral(real_buf_.GetFloat64());
            return;

          case 'E': case 'e':
            state_ = State_Digit_Exponent;
            break;

          case 'F': case 'f':
            // 1234f == (float32) 1234
            real_buf_.Start(int_buf_.GetInt());
            SetLiteral(real_buf_.GetFloat32());
            return;

          case 'L': case 'l':
            state_ = State_Digit_L;
            break;

          case 'U': case 'u':
            state_ = State_Digit_U;
            break;

          default:
            source.Unget(wch);
            // TODO(yosi) 2011-12-31 NYI: Check int32 overflow
            // Note: It is illegal to write "-0x80000000" since
            // 0x80000000 is greater than MAX_INT32.
            SetLiteral(int_buf_.GetInt32());
            return;
        }
        break;

      case State_Digit_Dot:
        switch (wch) {
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            int_buf_.Accumlate(10, wch - '0');
            state_ = State_Digit_Dot_Digit;
            break;

          default:
            AddError(CompileError_Lexer_Expect_Digit, wch);
            return;
        }
        break;

      case State_Digit_Dot_Digit:
        switch (wch) {
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            int_buf_.Accumlate(10, wch - '0');
            break;

          case 'D': case 'd':
            real_buf_.m_cDecimals = int_buf_.GetLength();
            real_buf_.m_nDecimalPart = int_buf_.GetUInt64();
            SetLiteral(real_buf_.GetFloat64());
            return;

          case 'E': case 'e':
            real_buf_.m_cDecimals = int_buf_.GetLength();
            real_buf_.m_nDecimalPart = int_buf_.GetUInt64();
            state_ = State_Digit_Exponent;
            break;

          case 'F': case 'f':
            real_buf_.m_cDecimals = int_buf_.GetLength();
            real_buf_.m_nDecimalPart = int_buf_.GetUInt64();
            SetLiteral(real_buf_.GetFloat32());
            return;

          default:
            source.Unget(wch);
            real_buf_.m_cDecimals = int_buf_.GetLength();
            real_buf_.m_nDecimalPart = int_buf_.GetUInt64();
            SetLiteral(real_buf_.GetFloat64());
            return;
        }
        break;

      case State_Digit_Exponent:
        switch (wch) {
          case '+':
            state_ = State_Digit_Exponent_Plus;
            break;

          case '-':
            state_ = State_Digit_Exponent_Minus;
            break;

          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            real_buf_.m_iExponent = wch - '0';
            state_ = State_Digit_Exponent_Digit;
            break;

          default:
            AddError(CompileError_Lexer_Bad_Real, wch);
            return;
         }
         break;

      case State_Digit_Exponent_Digit:
        switch (wch) {
          case '0': case '1': case '2': case '3': case '4': 
          case '5': case '6': case '7': case '8': case '9':
            // TOOD 2010-12-20 yosi@ Detect exponent overflow.
            real_buf_.m_iExponent *= 10;
            real_buf_.m_iExponent += wch - '0';
            break;

          case 'D': case 'd':
            SetLiteral(real_buf_.GetFloat64());
            return;

          case 'F': case 'f':
            SetLiteral(real_buf_.GetFloat32());
            return;

          default:
            source.Unget(wch);
            SetLiteral(real_buf_.GetFloat64());
            return;
        }
        break;

      case State_Digit_Exponent_Minus:
        if (wch < '0' || wch > '9') {
          AddError(CompileError_Lexer_Bad_Real, wch);
          return;
        }
        real_buf_.m_iExponent = -(wch - '0');
        state_ = State_Digit_Exponent_Digit;
        break;

      case State_Digit_Exponent_Plus:
        if (wch < '0' || wch > '9') {
          AddError(CompileError_Lexer_Bad_Real, wch);
          return;
        }
        real_buf_.m_iExponent = wch - '0';
        state_ = State_Digit_Exponent_Digit;
        break;

      case State_Digit_L:
        if (wch == 'U' || wch == 'u') {
          // TODO(yosi) 2011-12-31 NYI: Check uint32 overflow
          SetLiteral(int_buf_.GetUInt64());
        } else {
          source.Unget(wch);
          // TODO(yosi) 2011-12-31 NYI: Check int32 overflow
          SetLiteral(int_buf_.GetInt64());
        }
        return;

      case State_Digit_U:
        if (wch == 'L' || wch == 'l') {
          // TODO(yosi) 2011-12-31 NYI: Check uint64 overflow
          SetLiteral(int_buf_.GetUInt64());
        } else {
          source.Unget(wch);
          // TODO(yosi) 2011-12-31 NYI: Check int64 overflow
          SetLiteral(int_buf_.GetUInt32());
        }
        return;

      //
      // DoubleQuote
      //    "..."
      //
      case State_DoubleQuote_BackSlash:
        if (wch > ARRAYSIZE(kBackSlashCharMap)) {
          AddError(CompileError_Lexer_Bad_BackSlash, wch);
          return;
        }

        {
          auto const chEscaped = kBackSlashCharMap[wch];
          switch (chEscaped) {
            case 0:
              AddError(CompileError_Lexer_Bad_BackSlash, wch);
              return;

            case 1:
              state_ = State_DoubleQuote_Char;
              str_buf_.Add(0);
              break;

            case 'u':
              int_buf_.Start();
              state_ = State_DoubleQuote_Unicode;
              break;

            case 'x':
              int_buf_.Start();
              state_ = State_DoubleQuote_Hex;
              break;

            default:
              state_ = State_DoubleQuote_Char;
              str_buf_.Add(chEscaped);
              break;
          }
        }
        break;

      case State_DoubleQuote_Char:
      case State_DoubleQuote_Start:
        switch (wch) {
          case '"':
            SetLiteral(str_buf_.GetString());
            return;

          case '\\':
            state_ = State_DoubleQuote_BackSlash;
            break;

          default:
            if (isNewlineChar(wch)) {
              AddError(CompileError_Lexer_Unexpected_Newline,
                token_loc_.line(),
                token_loc_.column());
              return;
            }
            str_buf_.Add(wch);
            break;
        }
        break;

      case State_DoubleQuote_Hex: { // Xxxxx
        auto const iDigit = fromHexDigit(wch);
        if (iDigit < 0) {
          source.Unget(wch);
          str_buf_.EndString();
          SetLiteral(str_buf_.GetString());
          return;
        }

        int_buf_.Accumlate(16, iDigit);
        if (int_buf_.GetLength() == 4) {
          str_buf_.Add(static_cast<char16>(int_buf_.GetInt()));
          state_ = State_DoubleQuote_Char;
        }
        break;
      }

      case State_DoubleQuote_Unicode: { // Uxxxx
       auto const iDigit = fromHexDigit(wch);
       if (iDigit < 0) {
         AddError(CompileError_Lexer_Bad_BackSlash, wch);
         return;
       }

       int_buf_.Accumlate(16, iDigit);
       if (int_buf_.GetLength() == 4) {
         str_buf_.Add(static_cast<char16>(int_buf_.GetInt()));
         state_ = State_DoubleQuote_Char;
       }
       break;
      }

      // HexDigit
      //    0xXXXX [LUlu]{0,2}
      //
      case State_HexDigit:
        switch (wch) {
          case 'l': case 'L':
            state_ = State_Digit_L;
            break;

          case 'u': case 'U':
            state_ = State_Digit_U;
            break;

          default: {
            auto const iDigit = fromHexDigit(wch);
            if (iDigit < 0) {
              source.Unget(wch);
              // TODO(yosi) 2012-03-18 Check integer overflow.
              SetLiteral(int_buf_.GetInt32());
              return;
            }
            int_buf_.Accumlate(16, iDigit);
            break;
          }
        }
        break;

      case State_Identifier:
        if (!isIdentPart(wch)) {
          source.Unget(wch);
          FinishIdentifier();
          return;
        }
        str_buf_.Add(wch);
        break;

      // LineComment
      //    //.....\n
      //
      case State_LineComment:
        if (isNewlineChar(wch)) {
          state_ = State_LineStart;
        }
        break;

      case State_LineStart:
        if ('#' == wch) {
          state_ = State_Sharp;
          break;
        }

        if (isSpace(wch)) {
          break;
        }

        if (isNewlineChar(wch)) {
          break;
        }

        state_ = State_Start;
        goto tryAgain;

      ////////////////////////////////////////////////////////////
      //
      // Operator
      //
      case State_Operator: {
          auto const nType = k_rgnCharType[str_buf_.GetChar()];

          if ('=' == wch) {
              if (nType & CharType_OpEqual) {
                  str_buf_.Add(wch);
              } else {
                source.Unget(wch);
              }

          } else if (str_buf_.GetChar() == wch) {
              if (nType & CharType_OpDouble) {
                  str_buf_.Add(wch);

                  if (nType & CharType_OpEqual) {
                      state_ = State_Operator_Double;
                      break;
                  }

              } else {
                source.Unget(wch);
              }
          } else {
            source.Unget(wch);
          }

          FinishOperator();
          return;
      }

      case State_Operator_Double: // >>=
          if ('=' == wch) {
            str_buf_.Add(wch);
          } else {
            source.Unget(wch);
          }

          FinishOperator();
          return;

      ////////////////////////////////////////////////////////////
      //
      // Sharp
      //
      case State_Sharp:
          if (isNewlineChar(wch)) {
              state_ = State_LineStart;
          }
          break;

      ////////////////////////////////////////////////////////////
      //
      // SingleQuote
      //
      case State_SingleQuote_BackSlash:
          if (wch > lengthof(kBackSlashCharMap)) {
              AddError(CompileError_Lexer_Bad_BackSlash, wch);
              return;
          }

          {
              auto const chEscaped = kBackSlashCharMap[wch];
              switch (chEscaped) {
              case 0:
                  AddError(CompileError_Lexer_Bad_BackSlash, wch);
                  return;

              case 1:
                  state_ = State_SingleQuote_Char;
                  break;

              case 'u':
                  state_ = State_SingleQuote_Unicode;
                  break;

              case 'x':
                  state_ = State_SingleQuote_Hex;
                  break;

              default:
                  state_ = State_SingleQuote_Char;
                  int_buf_.Start(wch);
                  break;
              }
          }
          break;

      case State_SingleQuote_Char:
          if ('\'' == wch) {
            SetCharLiteral(int_buf_.GetInt());
          } else {
            AddError(CompileError_Lexer_Expect_SingleQuote, wch);
          }
          return;

      case State_SingleQuote_Hex: {
          int iDigit = fromHexDigit(wch);
          if (iDigit < 0) {
              source.Unget(wch);
              SetCharLiteral(int_buf_.GetInt());
              return;
          }

          int_buf_.Accumlate(16, iDigit);

          if (4 == int_buf_.GetLength()) {
              SetCharLiteral(int_buf_.GetInt());
              return;
          }

          break;
      }

      case State_SingleQuote_Start:
          switch (wch) {
          case '\'':
              AddError(CompileError_Lexer_Bad_CharLiteral, wch);
              return;

          case '\\':
              state_ = State_SingleQuote_BackSlash;
              break;

          default:
              state_ = State_SingleQuote_Char;
              int_buf_.Start(wch);
              break;
          }
          break;

      case State_SingleQuote_Unicode: {
          int iDigit = fromHexDigit(wch);
          if (iDigit < 0) {
              AddError(CompileError_Lexer_Bad_BackSlash, wch);
              return;
          }

          int_buf_.Accumlate(16, iDigit);

          if (int_buf_.GetLength() == 4) {
              state_ = State_SingleQuote_Unicode_4;
              return;
          }

          break;
      }

      case State_SingleQuote_Unicode_4:
        if (wch == '\'') {
          SetCharLiteral(int_buf_.GetInt());
          return;
        }

        AddError(CompileError_Lexer_Expect_SingleQuote, wch);
        return;

      ////////////////////////////////////////////////////////////
      //
      // Slash
      //
      case State_Slash_Start:
          switch (wch) {
          case '=':
              SetOperator(*Op__DivEq);
              return;

          case '*':
              state_ = State_BlockComment;
              break;

          case '/':
              state_ = State_LineComment;
              break;

          default:
              source.Unget(wch);
              SetOperator(*Op__Div);
              return;
          }
          break;

      ////////////////////////////////////////////////////////////
      //
      // Start
      //
      case State_Start:
        int_buf_.Start();
        str_buf_.Start();

        if ('0' == wch) {
            state_ = State_Zero_Start;
            break;
        }

        if (isDigit(wch)) {
            int_buf_.Accumlate(10, wch - '0');
            state_ = State_Digit;
            break;
        }

        if (isIdentStart(wch)) {
            str_buf_.Add(wch);
            state_ = State_Identifier;
            break;
        }

        if (isNewlineChar(wch)) {
            state_ = State_LineStart;
            break;
        }

        if (isSpace(wch)) {
            break;
        }

        switch (wch) {
          case '"':
            state_ = State_DoubleQuote_Start;
            break;

          case '\'':
            state_ = State_SingleQuote_Start;
            break;

          case '/':
            state_ = State_Slash_Start;
            break;

          case '@':
            state_ = State_At;
            break;

          case '[':
            str_buf_.Start();
            str_buf_.Add('A');
            state_ = State_Bracket;
            break;

          default: {
            auto const nType = wch < lengthof(k_rgnCharType)
                ? k_rgnCharType[wch]
                : 0;

            if (nType) {
                str_buf_.Add(wch);

                if (nType & CharType_OpSingle) {
                    FinishOperator();
                    return;
                }

                state_ = State_Operator;
                break;
            }

            AddError(CompileError_Lexer_Invalid_Char, wch);
            return;
          }
        }
      break;

      // Zero
      //    01234
      //    0x12ABC
      //    0
      case State_Zero_Start:
        if (isDigit(wch)) {
          // Note: We don't support octal notation. (^_^)
          int_buf_.Accumlate(10, wch - '0');
          state_ = State_Digit;
          break;
        }

        if ('x' == wch || 'X' == wch) {
          state_ = State_HexDigit;
          break;
        }

        source.Unget(wch);
        SetLiteral(0);
        return;

      case State_Error:
      default:
        CAN_NOT_HAPPEN();
    }
  }
}

void Lexer::SetCharLiteral(uint64 const code) {
  if (code >= 0xFFFF) {
    AddError(CompileError_Lexer_Bad_CharLiteral, Box(code));
    return;
  }

  SetToken(*new CharToken(GetTokenLocation(), static_cast<char16>(code)));
}

void Lexer::SetLiteral(const String& str) {
  SetToken(*new StringToken(GetTokenLocation(), str));
}

void Lexer::SetLiteral(float32 const f32) {
  SetToken(*new Float32Token(GetTokenLocation(), f32));
}

void Lexer::SetLiteral(float64 const f64) {
  SetToken(*new Float64Token(GetTokenLocation(), f64));
}

void Lexer::SetLiteral(int32 const i32) {
  SetToken(*new IntToken(GetTokenLocation(), *Ty_Int32, i32));
}

void Lexer::SetLiteral(int64 const i64) {
  SetToken(*new IntToken(GetTokenLocation(), *Ty_Int64, i64));
}

void Lexer::SetLiteral(uint32 const u32) {
  SetToken(*new IntToken(GetTokenLocation(), *Ty_UInt32, u32));
}

void Lexer::SetLiteral(uint64 const u64) {
  SetToken(*new IntToken(GetTokenLocation(), *Ty_UInt64, u64));
}

void Lexer::SetOperator(const Operator& op) {
  SetToken(*new OperatorToken(GetTokenLocation(), op));
}

void Lexer::SetToken(const Token& token) {
  state_ = State_Start;
  ScopedRefCount_<const Token> scope(token);
  processor_.ProcessToken(token);
}

// [U]
void Lexer::UnexpectedEof() {
  AddError(
      CompileError_Lexer_Unexpected_Eof,
      token_loc_.line(),
      token_loc_.column());
}

} // Compiler
