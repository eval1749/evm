// @(#)$Id$
//
// Evita Parser
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Syntax_Parser_h)
#define INCLUDE_Compiler_Syntax_Parser_h

#include "../../Common/Collections.h"

#include "./Builder.h"
#include "./Operator.h"
#include "./TokenProcessor.h"

#include "../CompileErrorInfo.h"

namespace Compiler {

using Common::Collections::Stack_;

using Il::Ir::Operand;
using Il::Ir::Output;

class KeywordToken;
class Lexer;

class AliasDef;
class CastLexer;
class CastOperator;
class ClassDef;
class CompilationUnit;
class CompileSession;
class LocalVarDef;
class MethodDef;
class MethodGroupDef;
class NameDef;
class ClassOrNamespaceDef;
class NameLexer;
class NamespaceBody;
class NamespaceDef;
class NameScope;
class Operator;
class PropertyDef;
class RankSpec;
class TypeArgList;
class TypeArgsLexer;
class TypeDef;

class Parser : public TokenProcessor {
  private: class Control : public Castable_<Control> {
    CASTABLE_CLASS(Control);
    protected: Control() {}
    DISALLOW_COPY_AND_ASSIGN(Control);
  }; // Control

  private: class EnumControl : public Stack_<Control*>::Enum {
      private: typedef Stack_<Control*>::Enum Base;

      public: EnumControl(const Parser* p) :
          Base(&const_cast<Parser*>(p)->control_stack_) {}

      DISALLOW_COPY_AND_ASSIGN(EnumControl);
  }; // EnumControl

  private: class WhileControl :
          public WithCastable_<WhileControl, Control> {

      CASTABLE_CLASS(WhileControl);

      public: BBlock* const m_pBreakBB;
      public: BBlock* const m_pContinueBB;
      public: FrameReg* const frame_reg_;

      public: WhileControl(
          FrameReg* const pFrameReg,
          BBlock* const pBreakBB,
          BBlock* const pContinueBB) :
              m_pBreakBB(pBreakBB),
              m_pContinueBB(pContinueBB),
              frame_reg_(pFrameReg) {
          ASSERT(m_pBreakBB != nullptr);
          ASSERT(m_pContinueBB != nullptr);
      } // WhileControl

      DISALLOW_COPY_AND_ASSIGN(WhileControl);
  }; // WhileControl

  private: class ForControl :
          public WithCastable3_<
              ForControl,
              WhileControl,
              FrameReg*,
              BBlock*,
              BBlock*> {

      CASTABLE_CLASS(ForControl);

      private: typedef WithCastable3_<
          ForControl,
          WhileControl,
          FrameReg*,
          BBlock*,
          BBlock*> Base;

      public: BBlock* const m_pLoopBB;
      public: BBlock* const m_pTestBB;

      public: ForControl(
          FrameReg* const pFrameReg,
          BBlock* const pLoopBB,
          BBlock* const pTestBB,
          BBlock* const pBreakBB,
          BBlock* const pContinueBB) :
              Base(pFrameReg, pBreakBB, pContinueBB),
              m_pLoopBB(pLoopBB),
              m_pTestBB(pTestBB) {
          ASSERT(m_pLoopBB != nullptr);
          ASSERT(m_pTestBB != nullptr);
      } // ForControl

      DISALLOW_COPY_AND_ASSIGN(ForControl);
  }; // ForControl

  private: class SwitchControl :
          public WithCastable_<SwitchControl, Control> {

      CASTABLE_CLASS(SwitchControl);

      public: bool m_fHasDefault;
      public: BBlock* const m_pDefaultBB;
      public: FrameReg* const frame_reg_;
      public: BBlock* const m_pLabelsBB;
      public: BBlock* const m_pSuccBB;
      public: SwitchI* const m_pSwitchI;

      public: SwitchControl(
          FrameReg* const pFrameReg,
          BBlock* const pLabelsBB,
          SwitchI* const pSwitchI,
          BBlock* const pDefaultBB,
          BBlock* const pSuccBB) :
              m_fHasDefault(false),
              m_pDefaultBB(pDefaultBB),
              frame_reg_(pFrameReg),
              m_pLabelsBB(pLabelsBB),
              m_pSuccBB(pSuccBB),
              m_pSwitchI(pSwitchI) {
          ASSERT(m_pDefaultBB != nullptr);
          ASSERT(m_pLabelsBB != nullptr);
          ASSERT(m_pSuccBB != nullptr);
          ASSERT(m_pSwitchI != nullptr);
      }

      public: SwitchOperandBox* FindCase(const Operand&) const;

      DISALLOW_COPY_AND_ASSIGN(SwitchControl);
  }; // SwitchControl

  private: class UsingControl :
          public WithCastable_<UsingControl, Control> {

      CASTABLE_CLASS(UsingControl);

      private: FrameReg* m_pCurrent;
      public: FrameReg* const frame_reg_;
      public: const Type* current_type_;

      public: UsingControl(FrameReg* const pFrameReg) :
          frame_reg_(pFrameReg),
          current_type_(Ty_Void) {}

      public: void Add(FrameReg* const pFrameReg) {
          ASSERT(pFrameReg != nullptr);
          m_pCurrent = pFrameReg;
      } // Add

      DISALLOW_COPY_AND_ASSIGN(UsingControl);
  }; // UsingControl

  private: enum Mask {
      Modifier_Mask_Access =
          Modifier_Internal
          | Modifier_Private
          | Modifier_Protected
          | Modifier_Public,

      Modifier_Mask_Class =
          Modifier_Mask_Access
          | Modifier_Abstract
          | Modifier_New
          | Modifier_Final
          | Modifier_Static,

      Modifier_Mask_Constant =
          Modifier_Mask_Access
          | Modifier_New,

      Modifier_Mask_Enum =
          Modifier_Mask_Access
          | Modifier_New,

      Modifier_Mask_Field =
          Modifier_Mask_Access
          | Modifier_Const
          | Modifier_New
          | Modifier_ReadOnly
          | Modifier_Static
          | Modifier_Volatile,

      Modifier_Mask_Indexer =
          Modifier_Mask_Access
          | Modifier_Abstract
          | Modifier_Extern
          | Modifier_New
          | Modifier_Override
          | Modifier_Final
          | Modifier_Virtual,

      Modifier_Mask_Interface =
          Modifier_Mask_Access
          | Modifier_New,

      Modifier_Mask_Method =
          Modifier_Mask_Access
          | Modifier_Abstract
          | Modifier_Extern
          | Modifier_New
          | Modifier_Override
          | Modifier_Final
          | Modifier_Static
          | Modifier_Virtual,

      Modifier_Mask_Property = Modifier_Mask_Method,

      Modifier_Mask_Struct =
          Modifier_Mask_Access
          | Modifier_New,
  }; // Mask

  private: enum IndexerOrProperty {
    IsIndexer,
    IsProperty,
  };

  private: enum State {
      #define DEFSTATE(mp_name) State_ ## mp_name,
      #include "./Parser_State.inc"
  };

  private: typedef HashMap_<const Name*, const KeywordToken*> ModifierMap;

  private: Builder* builder_;
  private: State state_;

  private: int modifiers_;
  private: ModifierMap modifierMap_;
  private: Stack_<const Token*> paren_stack_;
  private: ArrayList_<const NameRef*> name_refs_;

  private: int m_iUnique;
  private: Stack_<BBlock*> bblock_stack_;
  private: Stack_<Control*> control_stack_;
  private: Type::Stack expect_stack_;
  private: LocalMemoryZone local_zone_;
  private: Stack_<Instruction*> instruction_stack_;
  private: Stack_<const Operand*> operand_stack_;
  private: Stack_<State> state_stack_;

  private: ClassDef* class_def_;
  private: CompilationUnit& compilation_unit_;
  private: EnumFieldDef* enum_field_;
  private: const Operand* expression_;
  private: FrameReg* frame_reg_;
  private: MemoryZone& memory_zone_;
  private: EnumFieldDef* last_enum_field_;
  private: SourceInfo last_source_info_;
  private: MethodDef* method_def_;
  private: const NameRef* name_ref_;
  private: NameScope* name_scope_;
  private: NamespaceBody* namespace_body_;

  // Owner of namespace and type name.
  private: ClassOrNamespaceDef* outer_ns_member_;
  private: PropertyDef* property_def_;
  private: CompileSession& session_;
  private: const Type* current_type_;
  private: LocalVarDef* local_var_def_;
  private: TypeParamDef* typaram_def_;

  // Lexers
  // Note: Do not change order of member variables for lexers.
  // Initialization of them must be done in this order.
  //private: CastLexer* const m_pCastLexer;
  //private: TypeArgsLexer* const current_type_ArgsLexer;
  private: NameLexer& name_lexer_;
  private: Lexer& lexer_;

  // ctor
  public: Parser(CompileSession&, CompilationUnit&);
  public: virtual ~Parser();

  // [A]
  private: void AddClassDef(ClassDef&);

  private: void AddError(const SourceInfo&, CompileError, Box);
  private: void AddError(const SourceInfo&, CompileError, Box, Box);
  private: void AddError(const SourceInfo&, CompileError, Box, Box, Box);

  private: void AddError(CompileError, const Token&);
  private: void AddError(CompileError, const Token&, Box);
  private: void AddError(CompileError, const Token&, Box, Box);

  private: void AddVarDef(const Name&, const SourceInfo&);

  private: void AddWarning(const SourceInfo&, CompileError, Box);
  private: void AddWarning(const SourceInfo&, CompileError, Box, Box);
  private: void AddWarning(const SourceInfo&, CompileError, Box, Box, Box);

  // [C]
  private: void CallExpr(const Type&, State eNextState);
  private: void CallExpr(const Type&, const NameRef&, State);
  private: void CallExpr(State eNextState);
  private: void CallState(State eNextState);
  private: void CallState(State eNextState, State eContinueState);

  private: Function& CloseFinally();
  private: const Operand& CoerceToBoolean(const Operand&);
  private: const BoolOutput& CoerceToBoolOutput(const Operand&);
  private: void ContinueState(State eNextState);

  // [E]
  private: const Operand& EmitCast(const Operand&, const Type&);
  private: const Register& EmitLoadNameRef(const Operand&, const NameRef&);
  private: void EmitUnaryOperator(const NameRef&);
  private: void EmitVarDefI(const Variable&, const Operand&);
  private: const Register& EmitVarPtr(const Variable&);
  private: void EndEnum();
  private: void EndMethod();
  private: void EndProperty(IndexerOrProperty);
  private: void EndUnary(const Operator&);
  private: bool Expect(const Operator&, const Token&);
  private: bool ExpectCloseParen(const Token&);

  // [F]
  private: NameDef* Find(const Name&) const;
  private: SwitchControl* FindSwitchControl() const;
  public: void Finish();

  // [G]
  private: template<typename T> T* GetControl() {
      return control_stack_.GetTop()->StaticCast<T>();
  }

  private: Instruction* GetGetterInstruction(const Operand&);
  private: const NameRef* GetNameRef(const Token&);
  private: const Name& GetSimpleName(const NameRef&, CompileError);
  private: const NameRef* GetTypeNameRef(const Token&);
  private: void GotoState(State eNextState);

  // [I]
  private: MethodGroupDef& InternMethodGroup(
      const Name&,
      const SourceInfo&);

  private: MethodGroupDef& InternMethodGroup(const NameRef&);

  private: NamespaceDef& InternNamespaceDef(
      NamespaceDef&,
      const Name&,
      const SourceInfo&);

  // [N]
  public: bool NeedMoreFeed() const;
  private: const Name& NewTempName(const char* psz);

  // [O]
  private: Function& OpenFinally();

  // [P]
  public: void Parse(const char16*, uint);

  private: template<typename T> T* PopControl() {
      return control_stack_.Pop()->StaticCast<T>();
  }

  private: void PopNameScope();
  private: void PopParenthesis(const Token&);
  private: void PopState();
  private: void ProcessDisposer(const Keyword&, const Register&);

  private: bool ProcessExprLeft(
      const Token&,
      Operator::Category,
      State eNextState,
      State eReturnState);

  private: void ProcessForEachOnArray(const Operand&);
  private: void ProcessForEachOnEnumerator(const Operand&);
  private: void ProcessForEachArray();
  private: bool ProcessExprRight(State eNextState);
  private: Operand* ProcessInvoke();
  private: virtual void ProcessToken(const Token&) override;
  private: const Type& ProcessTypeName(const NameRef&);
  private: const Variable& ProcessVarDef(const Operand&);

  private: void PushI(Instruction&);
  private: void PushNameScope();
  private: void PushParenthesis(const Token&);

  // [R]
  private: void ReleaseModifierMap();
  private: const Type& ResolveTypeName(const NameRef&);

  // [S]
  private: void StartAccessor(
      IndexerOrProperty,
      const Name&,
      const SourceInfo&);
  private: void StartClass(const NameRef&);
  private: void StartEnum(ClassDef&, const Type&);
  private: BBlock& StartFor();
  private: void StartIf(const BoolOutput&);
  private: void StartMethod(const NameRef&);
  private: void StartNamespace(const NameRef&);
  private: void StartProperty(
      IndexerOrProperty,
      const Type&,
      const NameRef&);

  // [V]
  private: int ValidateField(const SourceInfo&, const Name&);
  private: int ValidateModifiers(CompileError, int);

  DISALLOW_COPY_AND_ASSIGN(Parser);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_Syntax_Parser_h)
