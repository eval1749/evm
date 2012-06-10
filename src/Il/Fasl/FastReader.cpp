#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Reader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FastReader.h"

#include "./FaslErrorInfo.h"
#include "./Name64.h"

#include "../Ir.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;

namespace {

static const char* const k_FaslOp[] = {
    #define DEFOP(mp_name) # mp_name,
    #include "./FaslOp.inc"
};

} // namespace

FastReader::FastReader(Session& session, const String& file_name, 
                       Callback& callback)
    : Tasklet("FastReader", session),
      callback_(callback),
      counter_(0),
      fasl_op_(FaslOp_Invalid),
      file_name_(file_name),
      intacc_(0),
      state_(State_FaslOp) {}

FastReader::~FastReader() {
  DEBUG_PRINTF("%p\n", this);
}

// [C]
void FastReader::CallRef() {
  auto const ref_id = ExpectInt32();
  if (!HasError()) {
    callback_.HandleRef(ref_id);
  }
}

// [E]
void FastReader::Error(const FaslErrorInfo& error) {
  DEBUG_FORMAT("%s", error);
  callback_.HandleError(error);
  state_ = State_Error;
}

int FastReader::ExpectInt32() {
  if (intacc_ >= Int32::MinValue && intacc_ <= Int32::MaxValue) {
    return static_cast<int32>(intacc_);
  }
  Error(FaslErrorInfo(GetSourceInfo(), FaslError_Expect_Int32, intacc_));
  return 0;
}

// [F]
void FastReader::Feed(const void* const pv, uint const size) {
  bit_reader_.Feed(pv, size);
  for (;;) {
    switch (state_) {
      case State_Char: {
         auto const bits = bit_reader_.Read(8);
         if (bits < 0) {
           return;
         }
         intacc_ <<= 7;
         intacc_ |= bits & 0x7F;
         if ((bits & 0x80) == 0) {
           Process();
         }
         break;
      }

      case State_Error:
        return;

      case State_FaslOp: {
        auto const bits = bit_reader_.Read(FaslOp_NumBits);
        if (bits < 0) {
          return;
        }

        intacc_ <<= 4;
        intacc_ |= bits & 0xF;

        if ((bits & 0x10) == 0) {
          fasl_op_ = static_cast<FaslOp>(intacc_);
          #ifndef NDEBUG
            if (fasl_op_ >= 0 && fasl_op_ < ARRAYSIZE(k_FaslOp)) {
              DEBUG_PRINTF("%d: FaslOp_%hs(%d)\n",
                  bit_reader_.bit_position(),
                  k_FaslOp[fasl_op_],
                  fasl_op_);
            } else {
              DEBUG_PRINTF("FaslOp_Invalid_%d", fasl_op_);
            }
          #endif
          intacc_ = 0;
          Process();
        }
        break;
      }

      case State_I4: {
        auto const bits = bit_reader_.Read(4);
        if (bits < 0) {
          return;
        }
        intacc_ <<= 4;
        intacc_ |= bits;
        Process();
        break;
      }

      case State_I4_I4: {
        auto const bits = bit_reader_.Read(4);
        if (bits < 0) {
          return;
        }
        intacc_ <<= 4;
        intacc_ |= bits;
        state_ = State_I4;
        break;
      }

      case State_I4_I4_I4: {
        auto const bits = bit_reader_.Read(4);
        if (bits < 0) {
          return;
        }
        intacc_ <<= 4;
        intacc_ |= bits;
        state_ = State_I4_I4;
        break;
      }

      case State_I5: {
        auto const bits = bit_reader_.Read(5);
        if (bits < 0) {
          return;
        }
        intacc_ = bits;
        Process();
        break;
      }

     case State_NameChar:
       intacc_ = bit_reader_.Read(6);
       if (intacc_ < 0) {
         return;
       }
       Process();
       break;

     case State_V4:
       if (!ProcessVn(4)) {
         return;
       }
       break;

     case State_V5:
       if (!ProcessVn(5)) {
         return;
       }
       break;

     case State_V6:
       if (!ProcessVn(6)) {
         return;
       }
       break;

     case State_V7:
       if (!ProcessVn(7)) {
         return;
       }
       break;

     case State_V8:
       if (!ProcessVn(8)) {
         return;
       }
       break;

     default:
       // TODO(yosin) Throw BrokenFaslFile instead of abort.
       CAN_NOT_HAPPEN();
    } // switch state_
  } // for
}

// [G]
SourceInfo FastReader::GetSourceInfo() const {
  return SourceInfo(file_name_, 0, bit_reader_.bit_position());
}

// [H]
bool FastReader::HasError() const {
  return state_ == State_Error;
}

// [I]
void FastReader::InvalidState() {
  Error(FaslErrorInfo(GetSourceInfo(), FaslError_Invalid_State, state_));
}

// [P]
void FastReader::Process() {
  #define case_FaslOp_(Name, ...) \
    case FaslOp_ ## Name: \
      callback_.Handle ## Name(__VA_ARGS__); \
      StartFaslOp(); \
      break

  #define case_FaslOp2_(Name, Handler, ...) \
    case FaslOp_ ## Name: \
      callback_.Handle ## Handler(__VA_ARGS__); \
      StartFaslOp(); \
      break

  switch (fasl_op_) {
    case FaslOp_Array:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V4;
          break;

        case State_V4: {
          auto const length = ExpectInt32();
          if (HasError()) return;
          callback_.HandleArray(length);
          intacc_ = 0;
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
      }
      break;

    case_FaslOp_(ArrayType);

    case FaslOp_Char:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_Char;
          break;

        case State_Char:
          callback_.HandleLiteral(*new CharLiteral(
              static_cast<char16>(intacc_)));
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
        }
        break;

    case_FaslOp_(Class);
    case_FaslOp_(ClassContext);
    case_FaslOp_(ClassRef);
    case_FaslOp_(ConstructedClassRef);
    case_FaslOp_(ConstructedMethodRef);
    case_FaslOp2_(False, Literal, *new BooleanLiteral(false));
    case_FaslOp_(Field, source_info_);
    case_FaslOp_(FileMagic);
    case_FaslOp_(Function);
    case_FaslOp_(FunctionBody);
    case_FaslOp_(FunctionType);
    case_FaslOp_(GenericClass);
    case_FaslOp_(GenericMethod);

    case FaslOp_Instruction:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          if (intacc_ < 0 || intacc_ >= Op_Limit) {
            Error(FaslErrorInfo(GetSourceInfo(),
                                FaslError_Invalid_Instruction, intacc_));
            return;
          }
          callback_.HandleInstruction(
              static_cast<Op>(intacc_),
              source_info_);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_Int16:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new Int16Literal(
              static_cast<int16>(ExpectInt32()));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_Int32:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new Int32Literal(ExpectInt32());
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_Int64:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new Int64Literal(intacc_);
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_Int8:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new Int8Literal(
              static_cast<int8>(ExpectInt32()));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_Label:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          if (intacc_ < 0) {
            Error(FaslErrorInfo(GetSourceInfo(), FaslError_Invalid_Label,
                                intacc_));
            return;
          }
          callback_.HandleLabel(static_cast<int32>(intacc_));
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_LineAdd:
    case FaslOp_LineColAdd:
    case FaslOp_LineColSub:
    case FaslOp_LineInfo:
    case FaslOp_LineSub:
      // TODO(yosi) 2012-02-05 NYI: SourceInfo related FASL operation.
      break;

    case_FaslOp_(Method);
    case_FaslOp_(MethodGroup);
    case_FaslOp_(MethodRef);

    case FaslOp_Name:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8:
          counter_ = ExpectInt32();
          intacc_ = 0;
          state_ = State_Char;
          break;

        case State_Char:
          char_sink_.Append(static_cast<char16>(intacc_));
          --counter_;
          if (counter_ == 0) {
            ProcessName();
            StartFaslOp();
          }
          intacc_ = 0;
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_Name64:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_NameChar;
          break;

        case State_NameChar:
          if (intacc_ == 0) {
            ProcessName();
            StartFaslOp();

          } else {
            auto const code = ExpectInt32();
            char_sink_.Append(Name64::FromIndex(code));
            intacc_ = 0;
          }
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case_FaslOp_(Namespace);

    case FaslOp_NegInt:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_V8;
          break;

        case State_V8:
          callback_.HandleInt(-intacc_);
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case_FaslOp_(PatchPhi);
    case_FaslOp_(PointerType);

    case FaslOp_PosInt:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_V8;
          break;

        case State_V8:
          callback_.HandleInt(intacc_);
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case_FaslOp_(Property);
    case_FaslOp_(PropertyMember);
    case_FaslOp_(RealizeClass);
    case_FaslOp_(RealizeTypeParam);

    case FaslOp_Ref1:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_I4;
          break;

        case State_I4:
          CallRef();
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_Ref2:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_I4_I4;
          break;

        case State_I4:
          CallRef();
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_Ref3:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_I4_I4_I4;
          break;

        case State_I4:
          CallRef();
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_RefV:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_V4;
          break;

        case State_V4:
          CallRef();
          StartFaslOp();
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case FaslOp_Reg1:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_I5;
          break;

         case State_I5:
           callback_.HandleOutput(static_cast<uint8>(intacc_) + 1);
           StartFaslOp();
           break;

         default:
           InvalidState();
           break;
      }
      break;

    case FaslOp_Reg2:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_I5;
          break;

         case State_I5:
           callback_.HandleOutput(static_cast<uint8>(intacc_) + 34);
           StartFaslOp();
           break;

         default:
           InvalidState();
           break;
      }
      break;

    case FaslOp_RegV:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          state_ = State_V8;
          break;

         case State_V8:
           callback_.HandleOutput(static_cast<uint8>(intacc_) + 67);
           StartFaslOp();
           break;

         default:
           InvalidState();
           break;
      }
      break;

    case FaslOp_Reset:
      bit_reader_.Align();
      break;

    case FaslOp_String:
      switch (state_) {
        case State_FaslOp:
          ASSERT(intacc_ == 0);
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8:
          counter_ = ExpectInt32();
          if (HasError()) {
            return;
          }

          if (!counter_) {
            // TODO(yosi) 2012-03-20 Where should we allocate temporary
            // literal?
            callback_.HandleLiteral(* new StringLiteral(""));
            StartFaslOp();
            break;;
          }

          intacc_ = 0;
          state_ = State_Char;
          break;

        case State_Char:
          char_sink_.Append(static_cast<char16>(intacc_ & 0xFFFF));
          --counter_;
          if (counter_ == 0) {
            auto const str = char_sink_.ToString();
            char_sink_.Clear();
            // TODO(yosi) 2012-03-20 Where should we allocate temporary
            // literal?
            const auto& lit = *new StringLiteral(str);
            callback_.HandleLiteral(lit);
            StartFaslOp();
          }
          intacc_ = 0;
          break;

        default:
          InvalidState();
          break;
      }
      break;

    case_FaslOp2_(True, Literal, *new BooleanLiteral(true));
    case_FaslOp_(TypeParam);

    case FaslOp_UInt16:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new UInt16Literal(
              static_cast<uint16>(ExpectInt32()));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_UInt32:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new UInt32Literal(
              static_cast<uint32>(ExpectInt32()));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_UInt64:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new UInt64Literal(
              static_cast<uint64>(intacc_));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case FaslOp_UInt8:
      switch (state_) {
        case State_FaslOp:
          intacc_ = 0;
          state_ = State_V8;
          break;

        case State_V8: {
          const auto& lit = *new UInt8Literal(
              static_cast<uint8>(ExpectInt32()));
          callback_.HandleLiteral(lit);
          StartFaslOp();
          break;
        }

        default:
          InvalidState();
          break;
        }
        break;

    case_FaslOp_(ValuesType);
    case_FaslOp_(Variable);
    case_FaslOp_(Zero);

    default:
      Error(FaslErrorInfo(GetSourceInfo(), FaslError_Invalid_FaslOp,
                          fasl_op_));
      break;
  } // switch fasl_op_
}

void FastReader::ProcessName() {
  if (char_sink_.GetLength() == 0) {
   Error(FaslErrorInfo(GetSourceInfo(), FaslError_Expect_Name));
   return;
  }

  callback_.HandleName(char_sink_.GetString());
  char_sink_.Clear();
}

bool FastReader::ProcessVn(int const num_bits) {
  ASSERT(num_bits >= 4);
  ASSERT(num_bits <= 8);

  auto const bits = bit_reader_.Read(num_bits);
  if (bits < 0) {
   return false;
  }

  auto const continue_bit = 1 << (num_bits - 1);
  auto const value_mask = continue_bit - 1;

  intacc_ <<= (num_bits - 1);
  intacc_ |= bits & value_mask;
  if ((bits & continue_bit) == 0) {
    Process();
  }

  return true;
}

// [S]
void FastReader::StartFaslOp() {
  if (state_ == State_Error) {
    return;
  }
  intacc_ = 0;
  state_ = State_FaslOp;
}

} // Fasl
} // Il
