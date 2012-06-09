// -*- Mode: C++; -*-
// @(#)$Id$
//
// Evita VM IL FASL Operation Code
//

#if !defined(DEFOP)
    #error "You must define DEFOP macro."
#endif // !defined(DEFOP)

DEFOP(Zero)    // 0 |=> null
DEFOP(Ref1)    // 1 I4 |=>  -- 0+1..15+1
DEFOP(Ref2)    // 2 I4 I4 |=>  -- 0+15+2..255+15+2
DEFOP(Ref3)    // 3 I4 I4 I4 |=> -- 0+255+15+3..8191+255+15+3
DEFOP(RefV)    // 4 V4+ |=> object

DEFOP(Reg1)    // 5 I4 |=> register -- 0+1..15+1
DEFOP(Reg2)    // 6 I8 |=> register -- 0+15+1=16..256+15+2
DEFOP(RegV)    // 7 V4+ |=> register

// RefCtx -- reference object set by DEFOP.
DEFOP(RefContext)   // 8
DEFOP(SetContext)   // 9 Ref(Class)
DEFOP(Name)         // 10 V4 Char+ => name
DEFOP(Name64)       // 11 N6+ I6(0) => name

DEFOP(Array)        // 12 V4 | Ref* => array
DEFOP(NotUsedFunction) // 13
DEFOP(FunctionBody) // 14
DEFOP(Instruction)  // 15 V8 | Output Type? Operand*
DEFOP(Int16)        // 16 | Int32 => Literal
DEFOP(Int32)        // 17 | Int32 => Literal
DEFOP(Int64)        // 18 | Int64 => Literal
DEFOP(Int8)         // 19 | Int32 => Literal
DEFOP(Namespace)    // 20 | Ref(Outer) Name => namespace
DEFOP(UInt16)       // 21 | UInt32 => Literal
DEFOP(UInt32)       // 22 | UInt32 => Literal
DEFOP(UInt64)       // 23 | UInt64 => Literal
DEFOP(UInt8)        // 24 | UInt32 => Literal

// Class Members

// Field
//  Ref(Class) PosInt(Modifiers) Ref(Type) Name => |
DEFOP(Field)

// Method
// Ref(MethodGroup)
// Ref(FunctionType)
// Name+(Params) Zero
// Function* Zero
DEFOP(GenericMethod)
DEFOP(Method)
DEFOP(MethodGroup) // Ref(Class) Name

// Types
// Class and GenericClass
//  Name
//  Ref(Outer)
//  V4(Modifiers)
//  Ref(Base)
//  Interface*
//  Zero
//  Name+ Zero -- For GenericClass type parameter names
//
DEFOP(ArrayType)           // Ref(ElementType) V4(Rank)
DEFOP(Class)               // See Notes
DEFOP(ConstructedClass)    // Ref(GenericClass) Array(Type+)
DEFOP(GenericClass)        // See Notes
DEFOP(RealizeClass)
DEFOP(ClassContext)
DEFOP(FunctionType)        // Ref(ReturnType) Ref(ValuesType)
DEFOP(NamedFunctionType)   // Name Ref(Outer) Ref(FunctionTYpe)
DEFOP(PointerType)         // Ref(Pointee)
DEFOP(TypeParam)           // Ref(Class) Name
DEFOP(ValuesType)          // Ref(Type)* Zero

// FunctionDef
//  Ref(Function)
//  Ref(FunctionType)
//  Ref(Variable)+
//  V4(BBlockCount)
//  Instructions+
DEFOP(Function)
DEFOP(FunctionRef) // Ref(Function) Name V4(Flavor)

// DEFOP(Instruction) There is no instruction FASL command) since
// instructions are never referenced. Zero opcode indicates end of
// instructions.
//  V4(OpCode)
//  Ref(Type)
//  V4(Output)
//  V4(Number of operands) only for varible length instruction)
//  Ref+(Operand+)
//  LineInfo or Zero
//
// Example: ADD Int32 %r1 <= %r2 %r3
//  1 V4   0001 Op_Add
//  2 Ref  0001 Common.Int32
//  4 Reg1 0101
//  5      0010 %r2
//  6 Reg1 0101
//  7      0011 %r3
//  Total: 7x4=28bit => 4byte
// Operands
DEFOP(Char)         // V8
DEFOP(Label)        // V8
DEFOP(ClassRef)     // Ref(Outer) Name
DEFOP(FieldRef)     // Ref(Class) Name
DEFOP(Literal)      // Ref(Type) Ref(Integer)
                    //  where Int8) Int16) UInt8) UInt16
DEFOP(MethodRef)    // Ref(Class) Ref(FunctionType) Name
DEFOP(MethodGroupRef)   // Ref(Class) Name
DEFOP(NegFloat32)   // Exponent Significand
DEFOP(NegFloat64)   // Exponent Significand
DEFOP(NegInt)       // V8
DEFOP(Null)         // no operand
DEFOP(PosFloat32)   // Exponent Significand
DEFOP(PosFloat64)   // Exponent Significand
DEFOP(PosInt)       // V8
DEFOP(String)       // V4 Char*
DEFOP(Variable)     // Name

// We assume lines of source code are less than 80 chars.
// one word for V7 represents up to 127 chars.
DEFOP(LineAdd)      // V4(Delta) V7(Column)
DEFOP(LineColAdd)   // V4(Delta)
DEFOP(LineColSub)   // V4(Delta)
DEFOP(LineInfo)     // Ref(FileName) V8(Line) V7(Column)
DEFOP(LineSub)      // V4(Delta) V7(Column)
DEFOP(FileMagic)
DEFOP(Reset)        // Reset reference and align read position to octet.
// TODO(yosi) 2012-01-19 We should remove unused FaslOp's.
DEFOP(NamespaceRef)
DEFOP(ConstructedClassRef)
DEFOP(False)
DEFOP(True)
DEFOP(PatchPhi) // Stack(Ref(PhiOutput), Ref(Output))
DEFOP(ConstructedMethodRef)
DEFOP(Property) // Modifier Type Name
DEFOP(PropertyMember) // name Method

#undef DEFOP
