
class RealValue {
    public: enum TypeCode {
        TypeCode_None,
        TypeCode_Int32,
    } // TypeCode

    private: TypeCode m_eTypeCode;
    private: int32 m_iInt32;
    private: uint32 m_iUInt32;

    public: RealValue(
        Dispatcher* pDispatcher,
        Operand* const pSx) :
            m_eTypeCode(TypeCode_None),
            m_iInt32(0),
            m_iUIn32(0) {
        ASSERT(nullptr != pDispatcher);
        ASSERT(nullptr != pSx);

        if (auto const pLx = pSx->DynamicCast<Literal>()) {
            auto const pValue = pLx->GetValue();
            if (auto const pInt32 = pValue->DynamicCast<Int32>()) {
                m_eTypeCode = TypeCode_Int32;
                m_iInt32 = pInt32->GetValue();
                return;
            } // if
        } // if
    } // RealValue

    public: int32 GetInt32() const { return m_iInt32; }
    public: TypeCode GetTypeCode() const { return m_eTypeCode; }
    public: uint32 GetUInt32() const { return m_iUInt32; }
}; // RealValue

class OperandEvaluator : public Visotr {
    public: Operand* override Process(Output* const pOutput) {
        return pOutput->GetDefI()->Apply(this);
    } // Process

    public: Operand* override Process(AddI* const pAddI) {
        ASSERT(nullptr != pAddI);

        RealValue oX(pAddI->GetSx()->Apply(this));
        RealValue oY(pAddI->GetSy()->Apply(this));

        if (oX.GetTypeCode() != oY.GetTypeCode()) {
            return nullptr;
        } // if

        switch (oX.GetTypeCode()) {
        case RealValue::TypeCode_Int32:
            // TODO 2011-01-05 yosi@msn.com Handling of Int32o overflow.
            return new Literal(
                Ty_Int32,
                new Int32(oX.GetInt32() + oY.GetInt32()));
        } // swtich

        return nullptr;
    } // Process AddI

    public: bool override Process(LoadI* const pLoadI) {
        ASSERT(nullptr != pLoadI);
        auto const pDefI = pLoadI->GetRx()->GetDefI();
        if (auto const pVarDefI = pDefI->DynamicCast<VarDefI>()) {
            auto const pVar = pVarDefI->GetSx()->StaticCast<Variable>();

            if (this->IsSSA(pVar)) {
                this->SetValue(pVarDefI->GetSy());
            } // if
        } // if

        return false;
    } // LoadI
}; // OperandEvaluator

bool SimplifyFunctor::Process(ArithmeticInstruction* const pI) {
    ASSERT(nullptr != pI);

    auto const pSx = pI->GetSx();
    auto const pSy = pI->GetSy();
    auto const pTx = pSx->GetTy();
    auto const pTy = pSy->GetTy();

    if (pTx == pTy) {
        this->SetOutputTy(pI, pTx);
        return true;
    } // if

    if (pTx == Ty_Float64 || pTy == Ty_Float64) {
        this->PromoteBinOp(pI, Ty_Float64);
        return true;
    } // if

    if (pTx == Ty_Float32 || pTy == Ty_Float32) {
        this->PromoteBinOp(pI, Ty_Float32);
        return true;
    } // if

    if (pTx == Ty_UInt64 || pTy == Ty_UInt64) {
        this->PromoteBinOp(pI, Ty_UInt64);
        return true;
    } // if

    if (pTy == Ty_Int64 || pTy == Ty_Int64) {
        this->PromoteBinOp(pI, Ty_Int64);
        return true;
    } // if

    if (pTx == Ty_UInt32 || pTy == Ty_UInt32) {
        this->PromoteBinOp(pI, Ty_UInt32);
        return true;
    } // if

    this->PromoteBinOp(pI, Ty_Int32);

    return true;
} // Process ArithmeticInstruction

bool SimplifyFunctor::Process(Insstruction* const pI) {
    OperandEvaluator oOperandEvaluator(this->session());

    foreach (Instruction::EnumOperand, oEnum, pI) {
        auto const pBox = oEnum.GetBox();
        auto const pSx = pBox->GetOperand();
        auto const pNx = pSx->Apply(&oOperandEvaluator);
        if (nullptr != pNx && !pSx->Equals(pNx)) {
            pBox->Replace(pNx);
            this->SetChanged();
        } // if
    } // for operand
} // Process Insstruction

void SimplifyFunctor::PromoteBinOp(Instruction* const pI, Type* pTy) {
    ASSERT(nullptr != pI);
    ASSERT(nullptr != pTy);

    this->PromoteOperand(pI, pTy, 0);
    this->PromoteOperand(pI, pTy, 1);
    this->SetOutputTy(pI, pTy);
} // PromoteBinOp

void SimplifyFunctor::PromoteOperand(
    Instruction* const pI,
    Type* pTy,
    int const iNth) {
    ASSERT(nullptr != pI);
    ASSERT(nullptr != pTy);
    ASSERT(iNth >= 0);

    auto const pSx = pI->GetOperand(iNth);

    if (pSx->GetTy() == pTy) {
        return;
    } // if

    auto const pRx = new Register(pSx);
    pI->GetBB()->InsertBeforeI(new RuntimeCastI(pTy, pRx, pSx), pI);
    pI->GetOperandBox(iNth)->Replace(pRx);

    this->SetChanged();
} // PromoteOperand

// [S]
void SimplifyFunctor::SetOutputTy(Instruction* const pI, Type* pTy) {
    ASSERT(nullptr != pI);
    ASSERT(nullptr != pTy);
    if (pI->GetTy() == pTy) {
        return;
    } // if

    pI->SetTy(pTy);
    this->SetChanged();
} // SetOutputTy


bool NormalizeTask::Process(VarDefI* const pI) {
    ASSERT(nullptr != pI);

    auto const pClass = pI->GetTy()->StaticCast<ConstructedClass>();
    ASSERT(pClass->GetGenericClass() == Ty_ClosedCell);

    auto const pType = pI->GetSx()->GetTy();
    if (pClass->GetTypeArg(0) != pType) {
        auto const pNewClass = Ty_ClosedCell->Construct(pType);
        this->UpdateOutputType(pI, pNewClass);
    } // if

#if 0
    {
        LogSection oSection(this, "Variable SSA");

        auto const pVar = pI->GetSx()->StaticCast<Variable>();

        if (IsSsaVariable(pVar) {
            auto const pSy = pI->GetSy();

            if (auto const pLy = pSy->DynamicCast<Literal>()) {
                LogSection oSection(this, "Fold constant Variable");

                foreach (Module::EnumFuction, oEnum, module) {
                    auto const pFun = oEnum.Get();
                    if (pVar->GetOwner() == pFun) {
                        this->ReplaceVarRefs(pI->GetRd(), pLy);

                    } else if (auto const pQd = pFun->FindUpVar(pVar)) {
                        this->ReplaceVarRefs(pQd, pLy);
                    } // if
                } // for

            } else if (pVar->GetOwner()->HasNonLocalExitPoint()) {
                this->WriteLog("Has nonlocal exit point");

            } else {
                LogSection oSection(this, "Fold SSA Variable");

                // Connect variable value.
                if (auto const pRy = pSy->DynamicCast<Register>()) {
                    if (nullptr == pRy->GetVar()) {
                        pRy->SetVar(pVar);
                    } // if
                } // if

                this->ReplaceVarRefs(pI->GetRd(), pSy);
            } // if
        } // if IsSsaVariable
    }
#endif

    return true;
} // Process VarDefI

#if 0
// [R]
void NormalizeTask::ReplaceVarRefs(
    SsaOutput* const pRcell,
    Operand* const pSx) {

    ASSERT(nullptr != pRcell);
    ASSERT(nullptr != pSx);

    foreach (SsaOutput::Enum, oEnum, pRcell) {
        auto const pFieldPtrI = oEnum.GetI()->DynamicCast<FieldPtrI>();
        if (nullptr == pFieldPtrI) {
            continue;
        } // if

        foreach (Register::EnumUser, oEnum, pFieldPtrI->GetRd()) {
            if (auto const pLoadI = oEnum.GetI()->DynamicCast<LoadI>()) {
                this->ReplaceAll(pLoadI->GetRd(), pSx);
            } // if
        } // for load
    } // for
} // ReplaceVarRefs
#endif
