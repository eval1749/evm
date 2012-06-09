// -*- Mode: C++; -*-
//////////////////////////////////////////////////////////////////////////////
// evcl - compiler - cg - x86x64 - Instruction Format
// arch/x86x86/x86x64_opcode.inc
//
// Copyright (C) 1996-2006 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//
// @(#)$Id: //proj/evcl3/mainline/arch/x86x64/x86x64_opcode.inc#8 $
//

// Syntax for instruction format definition:
//
//  DEFFORMAT_0 opcode mnemonic
//      Defines instruction without operand
//  DEFFORMAT_1 opcode mnemonic format
//      Defines one operand instruction
//  DEFFORMAT_2 opcode mnemonic format format
//      Defines two operands instruction
//  DEFFORMAT_3 opcode mnemonic format format format
//      Defines three operands instruction

//  DEFFORMAT_X opcode extension
//      Defines first byte of opcode. extension is either "ModRm" or "2".

//  DEFFORMAT_EXT_1 opcode opext mnemonic format
//      Defines one operand instruction using opext field of ModRm byte.
//  DEFFORMAT_EXT_2 opcode opext mnemonic format
//      Defines two operands instruction using opext field of ModRm byte.

#if !defined(DEFFORMAT_0)
    #define DEFFORMAT_0(mp_opcode, mp_mnemonic)
#endif // !defined(DEFFORMAT_0)

#if !defined(DEFFORMAT_1)
    #define DEFFORMAT_1(mp_opcode, mp_mnemonic, mp_1)
#endif // !defined(DEFFORMAT_1)

#if !defined(DEFFORMAT_X)
    #define DEFFORMAT_X(mp_opcode, mp_extend)
#endif // !defined(DEFFORMAT_X)

#if !defined(DEFFORMAT_EXT_1)
    #define DEFFORMAT_EXT_1(mp_opcode, mp_opext, mp_mnemonic, mp_1)
#endif // !defined(DEFFORMAT_EXT_1)

#if !defined(DEFFORMAT_2)
    #define DEFFORMAT_2(mp_opcode, mp_mnemonic, mp_1, mp_2)
#endif // !defined(DEFFORMAT_2)

#if !defined(DEFFORMAT_EXT_2)
    #define DEFFORMAT_EXT_2(mp_opcode, mp_opext, mp_mnemonic, mp_1, mp_2)
#endif // !defined(DEFFORMAT_EXT_2)

#if !defined(DEFFORMAT_3)
    #define DEFFORMAT_3(mp_opcode, mp_mnemonic, mp_1, mp_2, mp_3)
#endif // !defined(DEFFORMAT_3)

#if !defined(DEFFORMAT_EXT_3)
    #define DEFFORMAT_EXT_3(mp_opcode, mp_opext, mp_mnemonic, mp_1, mp_2, mp_3)
#endif // !defined(DEFFORMAT_EXT_3)

#if !defined(DEFFORMAT_P)
  #define DEFFORMAT_P(code,  mnemonic)
#endif


//////////////////////////////////////////////////////////////////////
//
// 0x00
//
DEFFORMAT_2(0x00, ADD, Eb, Gb)
DEFFORMAT_2(0x01, ADD, Ev, Gv)
DEFFORMAT_2(0x02, ADD, Gb, Eb)
DEFFORMAT_2(0x03, ADD, Gv, Ev)
DEFFORMAT_2(0x04, ADD, AL, Ib)
DEFFORMAT_2(0x05, ADD, eAX, Iz)
DEFFORMAT_1(0x06, PUSH, ES)
DEFFORMAT_1(0x07, POP,  ES)
DEFFORMAT_2(0x08, OR, Eb, Gb)
DEFFORMAT_2(0x09, OR, Ev, Gv)
DEFFORMAT_2(0x0A, OR, Gb, Eb)
DEFFORMAT_2(0x0B, OR, Gv, Ev)
DEFFORMAT_2(0x0C, OR, AL, Ib)
DEFFORMAT_2(0x0D, OR, eAX, Iz)
DEFFORMAT_1(0x0E, PUSH, CS)
DEFFORMAT_X(0x0F, TwoByte)
DEFFORMAT_X(0x0F38, ModRm)

//////////////////////////////////////////////////////////////////////
//
// 0x10
//
DEFFORMAT_2(0x10, ADC, Eb, Gb)
DEFFORMAT_2(0x11, ADC, Ev, Gv)
DEFFORMAT_2(0x12, ADC, Gb, Eb)
DEFFORMAT_2(0x13, ADC, Gv, Ev)
DEFFORMAT_2(0x14, ADC, AL, Ib)
DEFFORMAT_2(0x15, ADC, eAX, Iz)
DEFFORMAT_1(0x16, PUSH, SS)
DEFFORMAT_1(0x17, POP, SS)
DEFFORMAT_2(0x18, SBB, Eb, Gb)
DEFFORMAT_2(0x19, SBB, Ev, Gv)
DEFFORMAT_2(0x1A, SBB, Gb, Eb)
DEFFORMAT_2(0x1B, SBB, Gv, Ev)
DEFFORMAT_2(0x1C, SBB, AL, Ib)
DEFFORMAT_2(0x1D, SBB, eAX, Iz)
DEFFORMAT_1(0x1E, PUSH, DS)
DEFFORMAT_1(0x1F, POP, DS)

//////////////////////////////////////////////////////////////////////
//
// 0x20
//
DEFFORMAT_2(0x20, AND, Eb, Gb)
DEFFORMAT_2(0x21, AND, Ev, Gv)
DEFFORMAT_2(0x22, AND, Gb, Eb)
DEFFORMAT_2(0x23, AND, Gv, Ev)
DEFFORMAT_2(0x24, AND, AL, Ib)
DEFFORMAT_2(0x25, AND, eAX, Iz)
DEFFORMAT_P(0x26, ES)
DEFFORMAT_0(0x27, DAA)
DEFFORMAT_2(0x28, SUB, Eb, Gb)
DEFFORMAT_2(0x29, SUB, Ev, Gv)
DEFFORMAT_2(0x2A, SUB, Gb, Eb)
DEFFORMAT_2(0x2B, SUB, Gv, Ev)
DEFFORMAT_2(0x2C, SUB, AL, Ib)
DEFFORMAT_2(0x2D, SUB, eAX, Iz)
DEFFORMAT_P(0x2E, CS)
DEFFORMAT_0(0x2F, DAS)

//////////////////////////////////////////////////////////////////////
//
// 0x30
//
DEFFORMAT_2(0x30, XOR, Eb, Gb)
DEFFORMAT_2(0x31, XOR, Ev, Gv)
DEFFORMAT_2(0x32, XOR, Gb, Eb)
DEFFORMAT_2(0x33, XOR, Gv, Ev)
DEFFORMAT_2(0x34, XOR, AL, Ib)
DEFFORMAT_2(0x35, XOR, eAX, Iz)
DEFFORMAT_P(0x36, SS)
DEFFORMAT_0(0x37, AAA)
DEFFORMAT_2(0x38, CMP, Eb, Gb)
DEFFORMAT_2(0x39, CMP, Ev, Gv)
DEFFORMAT_2(0x3A, CMP, Gb, Eb)
DEFFORMAT_2(0x3B, CMP, Gv, Ev)
DEFFORMAT_2(0x3C, CMP, AL, Ib)
DEFFORMAT_2(0x3D, CMP, eAX, Iz)
DEFFORMAT_P(0x3E, DS)
DEFFORMAT_0(0x3F, AAS)

//////////////////////////////////////////////////////////////////////
//
// 0x40
//
DEFFORMAT_1(0x40, INC, eAX)
DEFFORMAT_1(0x41, INC, eCX)
DEFFORMAT_1(0x42, INC, eDX)
DEFFORMAT_1(0x43, INC, eBX)
DEFFORMAT_1(0x44, INC, eSP)
DEFFORMAT_1(0x45, INC, eBP)
DEFFORMAT_1(0x46, INC, eSI)
DEFFORMAT_1(0x47, INC, eDI)
DEFFORMAT_1(0x48, DEC, eAX)
DEFFORMAT_1(0x49, DEC, eCX)
DEFFORMAT_1(0x4A, DEC, eDX)
DEFFORMAT_1(0x4B, DEC, eBX)
DEFFORMAT_1(0x4C, DEC, eSP)
DEFFORMAT_1(0x4D, DEC, eBP)
DEFFORMAT_1(0x4E, DEC, eSI)
DEFFORMAT_1(0x4F, DEC, eDI)

//////////////////////////////////////////////////////////////////////
//
// 0x50
//
DEFFORMAT_1(0x50, PUSH, rAX)
DEFFORMAT_1(0x51, PUSH, rCX)
DEFFORMAT_1(0x52, PUSH, rDX)
DEFFORMAT_1(0x53, PUSH, rBX)
DEFFORMAT_1(0x54, PUSH, rSP)
DEFFORMAT_1(0x55, PUSH, rBP)
DEFFORMAT_1(0x56, PUSH, rSI)
DEFFORMAT_1(0x57, PUSH, rDI)
DEFFORMAT_1(0x58, POP,  rAX)
DEFFORMAT_1(0x59, POP,  rDX)
DEFFORMAT_1(0x5A, POP,  rCX)
DEFFORMAT_1(0x5B, POP,  rBX)
DEFFORMAT_1(0x5C, POP,  rSP)
DEFFORMAT_1(0x5D, POP,  rBP)
DEFFORMAT_1(0x5E, POP,  rSI)
DEFFORMAT_1(0x5F, POP,  rDI)

//////////////////////////////////////////////////////////////////////
//
// 0x60
//
DEFFORMAT_0(0x60,   PUSHAD)
DEFFORMAT_0(0x6660, PUSHA)
DEFFORMAT_0(0x61,   POPAD)
DEFFORMAT_0(0x6661, POPA)
//DEFFORMAT_2(0x62,   BOUND, Gv Ma)
//DEFFORMAT_2(0x63,   ARPL, Ew Gw)   :processor 286 :privilege 0)
DEFFORMAT_P(0x64,   FS)
DEFFORMAT_P(0x65,   GS)
DEFFORMAT_0(0x66,   OPDSIZ)
//DEFFORMAT_2(0x67,   ADDRSIZ)
DEFFORMAT_1(0x68,   PUSH, Iz)
DEFFORMAT_3(0x69,   IMUL, Gv, Ev, Iz)
DEFFORMAT_1(0x6A,   PUSH, Ib)
DEFFORMAT_3(0x6B,   IMUL, Gv, Ev, Is)
//DEFFORMAT_2(0x6C,   INSB  ()   ;, Yb DX)
//DEFFORMAT_2(0x6D,   INSD  ()   ;, Yv DX)
//DEFFORMAT_2(0x66,6D INSW  ()   ; (Yv DX)
//DEFFORMAT_2(0x6E,   OUTSB ()   ;, DX Xb)
//DEFFORMAT_2(0x6F,   OUTSD ()   ;, DX Xd)
//DEFFORMAT_2(0x66,6F OUTSW () ; (DX Xw)

// x64 only
DEFFORMAT_2(0x63, MOVSXD, Gv, Ev)

//////////////////////////////////////////////////////////////////////
//
// 0x70
//
DEFFORMAT_1(0x70, Jcc, Jb)

DEFFORMAT_1(0x70, JO,  Jb)  // alias Jcc
DEFFORMAT_1(0x71, JNO, Jb)
DEFFORMAT_1(0x72, JB,  Jb)  // alias JC JNAE
DEFFORMAT_1(0x73, JAE, Jb)  // alias JNB JNC
DEFFORMAT_1(0x74, JE,  Jb)  // alias JZ
DEFFORMAT_1(0x75, JNE, Jb)  // alias JNZ
DEFFORMAT_1(0x76, JBE, Jb)  // alias JNA
DEFFORMAT_1(0x77, JA, Jb)   // alias JNBE
DEFFORMAT_1(0x78, JS, Jb)
DEFFORMAT_1(0x79, JNS, Jb)
DEFFORMAT_1(0x7A, JPE, Jb)  // alias JP
DEFFORMAT_1(0x7B, JPO, Jb)  // alias JNP
DEFFORMAT_1(0x7C, JL, Jb)   // alias JNGE
DEFFORMAT_1(0x7D, JGE, Jb)  // alias JNL
DEFFORMAT_1(0x7E, JLE, Jb)  // alias JNG
DEFFORMAT_1(0x7F, JG, Jb)   // alias JNLE

//////////////////////////////////////////////////////////////////////
//
// 0x80
//
DEFFORMAT_X(0x80, ModRm)    // Grp1, Eb, Ib
DEFFORMAT_X(0x81, ModRm)    // Grp1, Ev, Iv
DEFFORMAT_X(0x82, ModRm)    // Grp1, Eb, Ib
DEFFORMAT_X(0x83, ModRm)    // Grp1, Ev, Ib
DEFFORMAT_2(0x84, TEST, Eb, Gb)
DEFFORMAT_2(0x85, TEST, Ev, Gv)
DEFFORMAT_2(0x86, XCHG, Eb, Gb)
DEFFORMAT_2(0x87, XCHG, Ev, Gv)
DEFFORMAT_2(0x88, MOV, Eb, Gb)
DEFFORMAT_2(0x89, MOV, Ev, Gv)
DEFFORMAT_2(0x8A, MOV, Gb, Eb)
DEFFORMAT_2(0x8B, MOV, Gv, Ev)
DEFFORMAT_2(0x8C, MOV, Ew, Sw)
DEFFORMAT_2(0x8D, LEA, Gv, M)
DEFFORMAT_2(0x8E, MOV, Sw, Ew)
DEFFORMAT_X(0x8F, ModRm)
DEFFORMAT_EXT_1(0x8F, 0, POP, Ev)

DEFFORMAT_0(0x90,   NOP)
DEFFORMAT_2(0x90,   XCHG, eAX, eAX)
DEFFORMAT_2(0x91,   XCHG, eAX, eCX)
DEFFORMAT_2(0x92,   XCHG, eAX, eDX)
DEFFORMAT_2(0x93,   XCHG, eAX, eBX)
DEFFORMAT_2(0x94,   XCHG, eAX, eSP)
DEFFORMAT_2(0x95,   XCHG, eAX, eBP)
DEFFORMAT_2(0x96,   XCHG, eAX, eSI)
DEFFORMAT_2(0x97,   XCHG, eAX, eDI)
DEFFORMAT_0(0x6698, CBW)
DEFFORMAT_0(0x98,   CWDE)
DEFFORMAT_0(0x99,   CDQ)
DEFFORMAT_0(0x6699, CWD)
//DEFFORMAT_2(0x9A,   CALLF, Ap)
DEFFORMAT_0(0x9B,   WAIT)   // alias FWAIT
DEFFORMAT_0(0x9C,   PUSHFD)
DEFFORMAT_0(0x669C, PUSHF)
DEFFORMAT_0(0x9D,   POPFD)
DEFFORMAT_0(0x669D, POPF)
DEFFORMAT_0(0x9E,   SAHF)
DEFFORMAT_0(0x9F,   LAHF)

//////////////////////////////////////////////////////////////////////
//
// 0xA0
//
DEFFORMAT_2(0xA0,   MOV, AL,  Ob)
DEFFORMAT_2(0xA1,   MOV, eAX, Ov)
DEFFORMAT_2(0xA2,   MOV, Ob,  AL)
DEFFORMAT_2(0xA3,   MOV, Ov,  eAX)
DEFFORMAT_0(0xA4,   MOVSB)

DEFFORMAT_0(0xA5,   MOVSD)
DEFFORMAT_0(0x66A5, MOVSW)
#if MACH == MACH_x64
DEFFORMAT_0(0x48A5, MOVSQ)
#endif // MACH == MACH_x64



DEFFORMAT_0(0xA6,   CMPSB)
DEFFORMAT_0(0xA7,   CMPSD)
DEFFORMAT_0(0x66A7, CMPSW)
DEFFORMAT_2(0xA8,   TEST, AL, Ib)
DEFFORMAT_2(0xA9,   TEST, eAX, Iz)
DEFFORMAT_0(0xAA,   STOSB)
DEFFORMAT_0(0xAB,   STOSD)
DEFFORMAT_0(0x66AB, STOSW)
DEFFORMAT_0(0xAC,   LODSB)
DEFFORMAT_0(0xAD,   LODSD)
DEFFORMAT_0(0x66AD, LODSW)
DEFFORMAT_0(0xAE,   SCASB)
DEFFORMAT_0(0xAF,   SCASD)
DEFFORMAT_0(0x66AF, SCASW)

//////////////////////////////////////////////////////////////////////
//
// 0xB0
//
DEFFORMAT_2(0xB0, MOV, AL, Ib)
DEFFORMAT_2(0xB1, MOV, CL, Ib)
DEFFORMAT_2(0xB2, MOV, DL, Ib)
DEFFORMAT_2(0xB3, MOV, BL, Ib)
DEFFORMAT_2(0xB4, MOV, AH, Ib)
DEFFORMAT_2(0xB5, MOV, CH, Ib)
DEFFORMAT_2(0xB6, MOV, DH, Ib)
DEFFORMAT_2(0xB7, MOV, BH, Ib)

DEFFORMAT_2(0xB8, MOV, eAX, Iv)
DEFFORMAT_2(0xB9, MOV, eCX, Iv)
DEFFORMAT_2(0xBA, MOV, eDX, Iv)
DEFFORMAT_2(0xBB, MOV, eBX, Iv)
DEFFORMAT_2(0xBC, MOV, eSP, Iv)
DEFFORMAT_2(0xBD, MOV, eBP, Iv)
DEFFORMAT_2(0xBE, MOV, eSI, Iv)
DEFFORMAT_2(0xBF, MOV, eDI, Iv)

//////////////////////////////////////////////////////////////////////
//
// 0xC0
//
DEFFORMAT_X(0xC0, ModRm)    // Grp2, Eb, Ib
DEFFORMAT_X(0xC1, ModRm)    // Grp2, Ev, Ib
DEFFORMAT_1(0xC2, RET, Iw)
DEFFORMAT_0(0xC3, RET)
DEFFORMAT_2(0xC4, LES, Gv, Mp)
DEFFORMAT_2(0xC5, LDS, Gv, Mp)
DEFFORMAT_X(0xC6, ModRm)    // Grp11, Eb, Ib    MOV_Eb_Ib
DEFFORMAT_X(0xC7, ModRm)    // Grp11, Ev, Iv    MOV_Ev_Iz
DEFFORMAT_2(0xC8, ENTER, Iw, Ib)
DEFFORMAT_0(0xC9, LEAVE)
DEFFORMAT_1(0xCA, RETF, Iw)
DEFFORMAT_0(0xCB, RETF)
DEFFORMAT_0(0xCC, INT3)
DEFFORMAT_1(0xCD, INT, Ib)
DEFFORMAT_0(0xCE, INTO)
DEFFORMAT_0(0xCF, IRET)

//////////////////////////////////////////////////////////////////////
//
// 0xD0
//

DEFFORMAT_X(0xD0, ModRm)    // Grp2,  Eb, 1
DEFFORMAT_X(0xD1, ModRm)    // Grp2,  Ev, 1
DEFFORMAT_X(0xD2, ModRm)    // Grp2,  Eb, CL
DEFFORMAT_X(0xD3, ModRm)    // Grp2,  Ev, CL
DEFFORMAT_0(0xD40A, AAM)
DEFFORMAT_0(0xD50A, AAD)
DEFFORMAT_0(0xD6, UNDEF)
DEFFORMAT_0(0xD7, XLATB)
DEFFORMAT_0(0xD7, XLAT)
DEFFORMAT_0(0xD8, ESC_D8)
DEFFORMAT_0(0xD9, ESC_D9)
DEFFORMAT_0(0xDA, ESC_DA)
DEFFORMAT_0(0xDB, ESC_DB)
DEFFORMAT_0(0xDC, ESC_DC)
DEFFORMAT_0(0xDD, ESC_DD)
DEFFORMAT_0(0xDE, ESC_DE)
DEFFORMAT_0(0xDF, ESC_DF)

//////////////////////////////////////////////////////////////////////
//
// 0xE0
//
DEFFORMAT_1(0xE0, LOOPNE,   Jb)   // alias LOOPNZ
DEFFORMAT_1(0xE1, LOOPE,    Jb)   // alias LOOPZ
DEFFORMAT_1(0xE2, LOOP,     Jb)
DEFFORMAT_1(0xE3, JECXZ,    Jb)
DEFFORMAT_2(0xE4, IN,       AL,  Ib)
DEFFORMAT_2(0xE5, IN,       eAX, Ib)
DEFFORMAT_2(0xE6, OUT,      Ib,  AL)
DEFFORMAT_2(0xE7, OUT,      Ib, eAX)
DEFFORMAT_1(0xE8, CALL,     Jv)
DEFFORMAT_1(0xE9, JMP,      Jv)
//DEFFORMAT_1(0xEA, JMP,      Ap)
DEFFORMAT_1(0xEB, JMP,      Jb)
DEFFORMAT_2(0xEC, IN,       AL, DX)
DEFFORMAT_2(0xED, IN,       eAX, DX)
DEFFORMAT_2(0xEE, OUT,      DX, AL)
DEFFORMAT_2(0xEF, OUT,      DX, eAX)


//////////////////////////////////////////////////////////////////////
//
// 0xF0
//
DEFFORMAT_P(0xF0, LOCK)         // prefix
DEFFORMAT_0(0xF1, UD1)          // UD1 (undocumented)
DEFFORMAT_P(0xF2, REPNE)        // prefix
DEFFORMAT_P(0xF3, REP)          // prefix
DEFFORMAT_0(0xF390, PAUSE)
DEFFORMAT_0(0xF4, HLT)
DEFFORMAT_0(0xF5, CMC)
DEFFORMAT_X(0xF6, ModRm)
DEFFORMAT_X(0xF7, ModRm)
DEFFORMAT_0(0xF8, CLC)
DEFFORMAT_0(0xF9, STC)
DEFFORMAT_0(0xFA, CLI)
DEFFORMAT_0(0xFB, STI)
DEFFORMAT_0(0xFC, CLD)
DEFFORMAT_0(0xFD, STD)
DEFFORMAT_X(0xFE, ModRm)    // Group 4
DEFFORMAT_X(0xFF, ModRm)    // Group 5


//////////////////////////////////////////////////////////////////////
//
// Two Byte Opcode
//

////////////////////////////////////////////////////////////
//
// 0x0F10
//
DEFFORMAT_2(0x0F10, MOVUPS,   Vps, Wps)
DEFFORMAT_2(0x0F11, MOVUPS,   Wps, Vps)
DEFFORMAT_2(0x0F12, MOVLPS,   Vq,  Mq)
DEFFORMAT_2(0x0F13, MOVLPS,   Mq,  Vq)
DEFFORMAT_2(0x0F14, UNPCKLPS, Vps, Wq)
DEFFORMAT_2(0x0F15, UNPCKHPS, Vps, Wq)
DEFFORMAT_2(0x0F16, MOVHPS,   Vq,  Mq)
DEFFORMAT_2(0x0F17, MOVHPS,   Mq,  Vq)

DEFFORMAT_2(0x660F10, MOVUPD,   Vps, Wps)
DEFFORMAT_2(0x660F11, MOVUPD,   Wps, Vps)
DEFFORMAT_2(0x660F12, MOVLPD,   Vq,  Mq)
DEFFORMAT_2(0x660F13, MOVLPD,   Mq,  Vq)
DEFFORMAT_2(0x660F14, UNPCKLPD, Vpd, Wq)
DEFFORMAT_2(0x660F15, UNPCKHPD, Vpd, Wq)
DEFFORMAT_2(0x660F16, MOVHPD,   Vq,  Mq)
DEFFORMAT_2(0x660F17, MOVHPD,   Mq,  Vq)

DEFFORMAT_2(0xF30F10, MOVSS,    Vss, Wss)
DEFFORMAT_2(0xF30F11, MOVSS,    Wss, Vss)
DEFFORMAT_2(0xF30F12, MOVSLDUP, Vq,  Wq)
DEFFORMAT_2(0xF30F16, MOVSHDUP, Vq,  Wq)

DEFFORMAT_2(0xF20F10, MOVSD,    Vsd, Wsd)
DEFFORMAT_2(0xF20F11, MOVSD,    Wsd, Vsd)
DEFFORMAT_2(0xF20F12, MOVDDUP,  Vq,  Wq)


////////////////////////////////////////////////////////////
//
// 0x0F20
//
DEFFORMAT_2(0x0F28, MOVAPS,    Vps, Wps)
DEFFORMAT_2(0x0F29, MOVAPS,    Wps, Vps)
DEFFORMAT_2(0x0F2A, CVTPI2PS,  Vps, Qq)
DEFFORMAT_2(0x0F2B, MOVNTPS,   Mps, Vps)
DEFFORMAT_2(0x0F2C, CVTTPS2PI, Qq,  Wps)
DEFFORMAT_2(0x0F2D, CVTPS2PI,  Qq,  Wps)
DEFFORMAT_2(0x0F2E, UCOMISS,   Vss, Wss)
DEFFORMAT_2(0x0F2F, COMISS,    Vss, Wss)

DEFFORMAT_2(0x660F28, MOVAPD,    Vpd, Wpd)
DEFFORMAT_2(0x660F29, MOVAPD,    Wpd, Vpd)
DEFFORMAT_2(0x660F2A, CVTPI2PD,  Vpd, Qq)
DEFFORMAT_2(0x660F2B, MOVNTPD,   Mpd, Vpd)
DEFFORMAT_2(0x660F2C, CVTTPD2PI, Qdq, Wpd)
DEFFORMAT_2(0x660F2D, CVTPD2PI,  Qdq, Wpd)
DEFFORMAT_2(0x660F2E, UCOMISD,   Vsd, Wsd)
DEFFORMAT_2(0x660F2F, COMISD,    Vsd, Wsd)

DEFFORMAT_2(0xF20F2A, CVTSI2SD,  Vsd, Ed)
DEFFORMAT_2(0xF20F2C, CVTTSD2SI, Gd,  Wsd)
DEFFORMAT_2(0xF20F2C, CVTSD2SI,  Gd,  Wsd)

DEFFORMAT_2(0xF30F2A, CVTSI2SS,  Vss, Ed)
DEFFORMAT_2(0xF30F2C, CVTTSS2SI, Gd,  Wss)
DEFFORMAT_2(0xF20F2D, CVTSS2SI,  Gd,  Wss)


////////////////////////////////////////////////////////////
//
// 0x0F40
//
DEFFORMAT_2(0x0F40, CMOVcc,    Gv, Ev)

DEFFORMAT_2(0x0F40, CMOVO,     Gv, Ev)    // alias CMOVcc
DEFFORMAT_2(0x0F41, CMOVNO,    Gv, Ev)    // :processor 686
DEFFORMAT_2(0x0F42, CMOVC,     Gv, Ev)    // alias CMOVC CMOVNAE
DEFFORMAT_2(0x0F42, CMOVB,     Gv, Ev)    // alias CMOVC CMOVNAE
DEFFORMAT_2(0x0F42, CMOVNAE,   Gv, Ev)    // alias CMOVC CMOVNAE
DEFFORMAT_2(0x0F43, CMOVNC,    Gv, Ev)    // alias CMOVNB CMOVNC
DEFFORMAT_2(0x0F43, CMOVAE,    Gv, Ev)    // alias CMOVNB CMOVNC
DEFFORMAT_2(0x0F43, CMOVNB,    Gv, Ev)    // alias CMOVNB CMOVNC
DEFFORMAT_2(0x0F44, CMOVE,     Gv, Ev)    // alias CMOVZ
DEFFORMAT_2(0x0F45, CMOVNE,    Gv, Ev)    // alias CMOVNZ
DEFFORMAT_2(0x0F46, CMOVBE,    Gv, Ev)    // alias CMOVNA
DEFFORMAT_2(0x0F47, CMOVA,     Gv, Ev)    // alias CMOVNBE
DEFFORMAT_2(0x0F48, CMOVS,     Gv, Ev)
DEFFORMAT_2(0x0F49, CMOVNS,    Gv, Ev)
DEFFORMAT_2(0x0F4A, CMOVPE,    Gv, Ev)    // alias CMOVP
DEFFORMAT_2(0x0F4B, CMOVPO,    Gv, Ev)    // alias CMOVNP
DEFFORMAT_2(0x0F4C, CMOVL,     Gv, Ev)    // alias CMOVNGE
DEFFORMAT_2(0x0F4D, CMOVGE,    Gv, Ev)    // alias CMOVNL
DEFFORMAT_2(0x0F4E, CMOVLE,    Gv, Ev)    // alias CMOVNG
DEFFORMAT_2(0x0F4F, CMOVG,     Gv, Ev)    // alias CMOVNLE

////////////////////////////////////////////////////////////
//
// 0x0F50
//
DEFFORMAT_2(0x0F50, MOVMSKPS,   Gd,  Ups)
DEFFORMAT_2(0x0F51, SQRTPS,     Vps, Wps)
DEFFORMAT_2(0x0F52, RSQRTPS,    Vps, Wps)
DEFFORMAT_2(0x0F53, RCPPS,      Vps, Wps)
DEFFORMAT_2(0x0F54, ANDPS,      Vps, Wps)
DEFFORMAT_2(0x0F55, ANDNPS,     Vps, Wps)
DEFFORMAT_2(0x0F56, ORPS,       Vps, Wps)
DEFFORMAT_2(0x0F57, XORPS,      Vps, Wps)
DEFFORMAT_2(0x0F58, ADDPS,      Vps, Wps)
DEFFORMAT_2(0x0F59, MULPS,      Vps, Wps)
DEFFORMAT_2(0x0F5A, CVTPS2PD,   Vpd, Wpd)
DEFFORMAT_2(0x0F5B, CVTDQ2PS,   Vps, Wdq)
DEFFORMAT_2(0x0F5C, SUBPS,      Vps, Wps)
DEFFORMAT_2(0x0F5D, MINPS,      Vps, Wps)
DEFFORMAT_2(0x0F5E, DIVPS,      Vps, Wps)
DEFFORMAT_2(0x0F5F, MAXPS,      Vps, Wps)

DEFFORMAT_2(0x660F50, MOVMSKPD,   Gd,  Upd)
DEFFORMAT_2(0x660F51, SQRTPD,     Vpd, Wpd)
DEFFORMAT_2(0x660F52, RSQRTPD,    Vpd, Wpd)
DEFFORMAT_2(0x660F53, RCPPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F54, ANDPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F55, ANDNPD,     Vpd, Wpd)
DEFFORMAT_2(0x660F56, ORPD,       Vpd, Wpd)
DEFFORMAT_2(0x660F57, XORPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F58, ADDPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F59, MULPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F5A, CVTPD2PD,   Vpd, Wpd)
DEFFORMAT_2(0x660F5B, CVTDQ2PD,   Vpd, Wdq)
DEFFORMAT_2(0x660F5C, SUBPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F5D, MINPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F5E, DIVPD,      Vpd, Wpd)
DEFFORMAT_2(0x660F5F, MAXPD,      Vpd, Wpd)

DEFFORMAT_2(0xF30F51, SQRTSS,     Vss, Wss)
DEFFORMAT_2(0xF30F58, ADDSS,      Vss, Wss)
DEFFORMAT_2(0xF30F59, MULSS,      Vss, Wss)
DEFFORMAT_2(0xF30F5A, CVTSS2SD,   Vsd, Wss)
DEFFORMAT_2(0xF30F5B, CVTDQ2SS,   Vdq, Wps)
DEFFORMAT_2(0xF30F5C, SUBSS,      Vss, Wss)
DEFFORMAT_2(0xF30F5D, MINSS,      Vss, Wss)
DEFFORMAT_2(0xF30F5E, DIVSS,      Vss, Wss)
DEFFORMAT_2(0xF30F5F, MAXSS,      Vss, Wss)

DEFFORMAT_2(0xF20F51, SQRTSD,     Vsd, Wsd)
DEFFORMAT_2(0xF20F58, ADDSD,      Vsd, Wsd)
DEFFORMAT_2(0xF20F59, MULSD,      Vsd, Wsd)
DEFFORMAT_2(0xF20F5A, CVTSD2SS,   Vss, Wsd)
// 0x5B
DEFFORMAT_2(0xF20F5C, SUBSD,      Vsd, Wsd)
DEFFORMAT_2(0xF20F5D, MINSD,      Vsd, Wsd)
DEFFORMAT_2(0xF20F5E, DIVSD,      Vsd, Wsd)
DEFFORMAT_2(0xF20F5F, MAXSD,      Vsd, Wsd)

////////////////////////////////////////////////////////////
//
// 0x0F60
//
DEFFORMAT_2(0x0F60, PUNPCKLBW,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F61, PUNPCKLWD,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F62, PUNPCKLDQ,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F63, PCKSSWB,    Pq, Qd)     // MMX
DEFFORMAT_2(0x0F64, PCMPGTB,    Pq, Qd)     // MMX
DEFFORMAT_2(0x0F65, PCMPGTW,    Pq, Qd)     // MMX
DEFFORMAT_2(0x0F66, PCMPGTD,    Pq, Qd)     // MMX
DEFFORMAT_2(0x0F67, PACKUSWB,   Pq, Qd)     // MMX
DEFFORMAT_2(0x0F68, PUNPCKHBW,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F69, PUNPCKHWD,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F6A, PUNPCKHDQ,  Pq, Qd)     // MMX
DEFFORMAT_2(0x0F6B, PACKSSDW,   Pq, Qd)     // MMX
//          0x0F6C
//          0x0F6D
DEFFORMAT_2(0x0F6E, MOVD,       Pd, Ed)     // MMX
DEFFORMAT_2(0x0F6F, MOVQ,       Pq, Qq)     // MMX

DEFFORMAT_2(0x660F60, PUNPCKLBW,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F61, PUNPCKLWD,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F62, PUNPCKLDQ,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F63, PCKSSWB,        Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F64, PCMPGTB,        Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F65, PCMPGTW,        Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F66, PCMPGTD,        Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F67, PACKUSWB,       Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F68, PUNPCKHBW,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F69, PUNPCKHWD,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F6A, PUNPCKHDQ,      Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F6B, PACKSSDW,       Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F6C, PUNPCKLQDQ,     Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F6D, PUNPCKHQDQ,     Vdq, Wdq)     // SSE
DEFFORMAT_2(0x660F6E, MOVD,           Vdq, Ed)      // SSE
DEFFORMAT_2(0x660F6F, MOVDQA,         Vdq, Wdq)     // SSE

DEFFORMAT_2(0xF30F6F, MOVDQU,         Vdq, Wdq)     // SSE

////////////////////////////////////////////////////////////
//
// 0x0F70
//
DEFFORMAT_3(0x0F70, PSHUFW,     Pq, Qq, Ib) // MMX
// 0x0F71x  Grp 12  PSRLW, PSRAW, PSLLW
// 0x0F72x  Grp 13  PSRLD, PSRAD, PSLLD
// 0x0F73x  Grp 14  PSRLQ, PSRLDQ, PSLLQ, PSLLDQ
DEFFORMAT_2(0x0F74, PCMPEQB,    Pq, Qq)     // MMX
DEFFORMAT_2(0x0F75, PCMPEQW,    Pq, Qq)     // MMX
DEFFORMAT_2(0x0F76, PCMPEQD,    Pq, Qq)     // MMX
DEFFORMAT_0(0x0F77, EMMS)                   // MMX
DEFFORMAT_2(0x0F78, VMREAD,     Ed, Gd)     // VMX
DEFFORMAT_2(0x0F79, VMWRITE,    Gd, Ed)     // VMX
//          0x0F7A
//          0x0F7B
//          0x0F7C
//          0x0F7D
DEFFORMAT_2(0x0F7E, MOVD,       Pd, Pd)     // MMX
DEFFORMAT_2(0x0F7F, MOVQ,       Qq, Pq)     // MMX

DEFFORMAT_3(0x660F70, PSHUFD, Vdq, Wdq, Ib)
DEFFORMAT_X(0x660F71, ModRm)
DEFFORMAT_X(0x660F72, ModRm)
DEFFORMAT_X(0x660F73, ModRm)
DEFFORMAT_2(0x660F74, PCMPEQB, Vdq, Wdq)
DEFFORMAT_2(0x660F75, PCMPEQW, Vdq, Wdq)
DEFFORMAT_2(0x660F76, PCMPEQD, Vdq, Wdq)
//          0x660F77
//          0x660F78
//          0x660F79
//          0x660F7A
//          0x660F7B
DEFFORMAT_2(0x660F7C, HADDPD,  Vpd, Wpd)
DEFFORMAT_2(0x660F7D, HSUBPD,  Vpd, Wpd)
DEFFORMAT_2(0x660F7E, MOVD,    Ed,  Vdq)
DEFFORMAT_2(0x660F7F, MOVDQA,  Wdq, Vdq)

DEFFORMAT_3(0xF20F70, PSHUFHW,  Vdq, Wdq, Ib)
DEFFORMAT_2(0xF20F7C, HADDPS,   Vps, Wps)
DEFFORMAT_2(0xF20F7D, HSUBPS,   Vps, Wps)

DEFFORMAT_3(0xF30F70, PSHUFLW,  Vdq, Wdq, Ib)
DEFFORMAT_2(0xF30F7E, MOVQ,     Vq,  Wq)
DEFFORMAT_2(0xF30F7F, MOVDQU,   Wdq, Vdq)


////////////////////////////////////////////////////////////
//
// 0x0F80
//
DEFFORMAT_1(0x0F80, Jcc, Jv)

DEFFORMAT_1(0x0F80, JO,  Jv)
DEFFORMAT_1(0x0F81, JNO, Jv)
DEFFORMAT_1(0x0F82, JB,  Jv)    // alias JC JNAE
DEFFORMAT_1(0x0F83, JAE, Jv)    // alias JNB JNC
DEFFORMAT_1(0x0F84, JE,  Jv)    // alias JZ
DEFFORMAT_1(0x0F85, JNE, Jv)    // alias JNZ
DEFFORMAT_1(0x0F86, JBE, Jv)    // alias JNA
DEFFORMAT_1(0x0F87, JA,  Jv)    // alias JNBE
DEFFORMAT_1(0x0F88, JS,  Jv)
DEFFORMAT_1(0x0F89, JNS, Jv)
DEFFORMAT_1(0x0F8A, JPE, Jv)    // alias JP
DEFFORMAT_1(0x0F8B, JPO, Jv)    // alias JNP
DEFFORMAT_1(0x0F8C, JL,  Jv)    // alias JNGE
DEFFORMAT_1(0x0F8D, JGE, Jv)    // alias JNL
DEFFORMAT_1(0x0F8E, JLE, Jv)    // alias JNG
DEFFORMAT_1(0x0F8F, JG,  Jv)    // alias JNLE

//////////////////////////////////////////////////////////////////////
//
// 0x0FA2
//
DEFFORMAT_0(0x0FA2, CPUID)

//////////////////////////////////////////////////////////////////////
//
// 0x0FB0
//
DEFFORMAT_2(0x0FB6, MOVZX, Gv, Eb)
DEFFORMAT_2(0x0FB7, MOVZX, Gv, Ew)
DEFFORMAT_2(0x0FBE, MOVSX, Gv, Eb)
DEFFORMAT_2(0x0FBF, MOVSX, Gv, Ew)

//////////////////////////////////////////////////////////////////////
//
// 0x0FC0
//
DEFFORMAT_2(0x0FC0, XADD, Eb, Gb)
DEFFORMAT_2(0x0FC1, XADD, Ev, Gv)
DEFFORMAT_3(0x0FC2, CMPSS, Vss, Wss, Ib)
DEFFORMAT_2(0x0FC3, MOVNTI, Md, Gd)
DEFFORMAT_3(0x0FC4, PINSRW, Pq, Ew, Ib)
DEFFORMAT_3(0x0FC5, PEXTRW, Gd, Nq, Ib)
DEFFORMAT_3(0x0FC6, SHUFPS, Pq, Ew, Ib)
// 0x0FC7 Grp 9

DEFFORMAT_3(0xF30FC2, CMPPS, Vps, Wps, Ib)
DEFFORMAT_3(0x660FC2, CMPPD, Vpd, Wpd, Ib)
DEFFORMAT_3(0xF20FC2, CMPSD, Vsd, Wsd, Ib)

//////////////////////////////////////////////////////////////////////
//
// 0x0FD0
//
DEFFORMAT_2(0x660FD0, ADDSUBPD, Vpd, Wpd)   // SSE3
DEFFORMAT_2(0x660FD1, PSRLW,    Vdq, Wdq)   // SSE2
DEFFORMAT_2(0x660FD2, PSRLD,    Vdq, Wdq)   // SSE2
DEFFORMAT_2(0x660FD3, PSRLQ,    Vdq, Wdq)   // SSE2
DEFFORMAT_2(0x660FD4, PADDQ,    Vdq, Wdq)   // SSE2
DEFFORMAT_2(0x660FD5, PMULLW,   Vdq, Wdq)   // SSE2
DEFFORMAT_2(0x660FD6, MOVQ,     Wq,  Vq)
//DEFFORMAT_2(0x660FD7, PMOVMKSB, Gd,  Udq)
DEFFORMAT_2(0x660FD8, PSUBUSB,  Vdq, Wdq)
DEFFORMAT_2(0x660FD9, PSUBUSW,  Vdq, Wdq)
DEFFORMAT_2(0x660FDA, PMINUB,   Vdq, Wdq)
DEFFORMAT_2(0x660FDB, PAND,     Vdq, Wdq)
DEFFORMAT_2(0x660FDC, PADDSUB,  Vdq, Wdq)
DEFFORMAT_2(0x660FDD, PADDUBW,  Vdq, Wdq)
DEFFORMAT_2(0x660FDE, PMAXUB,   Vdq, Wdq)
DEFFORMAT_2(0x660FDF, PANDN,    Vdq, Wdq)

//////////////////////////////////////////////////////////////////////
//
// 0x0FE0
//
DEFFORMAT_2(0x660FE0, PAVGB,        Vdq, Wdq)
DEFFORMAT_2(0x660FE1, PSRAW,        Vdq, Wdq)
DEFFORMAT_2(0x660FE2, PSRAD,        Vdq, Wdq)
DEFFORMAT_2(0x660FE3, PAVGW,        Vdq, Wdq)
DEFFORMAT_2(0x660FE4, PMULHUW,      Vdq, Wdq)
DEFFORMAT_2(0x660FE5, PMULHW,       Vdq, Wdq)
DEFFORMAT_2(0x660FE6, CVTTPD2DQ,    Vdq, Wdq)
//DEFFORMAT_2(0x660FE7, MOVNTDQ,    Mdq, Wdq)
DEFFORMAT_2(0x660FE8, PSUBSB,       Vdq, Wdq)
DEFFORMAT_2(0x660FE9, PSUBSW,       Vdq, Wdq)
DEFFORMAT_2(0x660FEA, PMINSW,       Vdq, Wdq)
DEFFORMAT_2(0x660FEB, POR,          Vdq, Wdq)
DEFFORMAT_2(0x660FEC, PADDSB,       Vdq, Wdq)
DEFFORMAT_2(0x660FED, PADDSW,       Vdq, Wdq)
DEFFORMAT_2(0x660FEE, PMAXSW,       Vdq, Wdq)
DEFFORMAT_2(0x660FEF, PXOR,         Vdq, Wdq)

//////////////////////////////////////////////////////////////////////
//
// 0x0FF0
//
// 0x660FF0
DEFFORMAT_2(0x660FF1, PSLLW,        Vdq, Wdq)
DEFFORMAT_2(0x660FF2, PSLLD,        Vdq, Wdq)
DEFFORMAT_2(0x660FF3, PSLLQ,        Vdq, Wdq)
DEFFORMAT_2(0x660FF4, PMULUDQ,      Vdq, Wdq)
DEFFORMAT_2(0x660FF5, PMADDWD,      Vdq, Wdq)
DEFFORMAT_2(0x660FF6, PSADBW,       Vdq, Wdq)
//DEFFORMAT_2(0x660FF7, MASKMOVDQU, Mdq, Wdq)
DEFFORMAT_2(0x660FF8, PSUBB,        Vdq, Wdq)
DEFFORMAT_2(0x660FF9, PSUBW,        Vdq, Wdq)
DEFFORMAT_2(0x660FFA, PSUBD,        Vdq, Wdq)
DEFFORMAT_2(0x660FFB, PSUBQ,        Vdq, Wdq)
DEFFORMAT_2(0x660FFC, PADDB,        Vdq, Wdq)
DEFFORMAT_2(0x660FFD, PADDW,        Vdq, Wdq)
DEFFORMAT_2(0x660FFE, PADD,         Vdq, Wdq)
// 0x660FFF


// Three-byte Opcode (First Two Bytes are 0F 38)

// 0F3800
DEFFORMAT_2(0x660F3800, PSHUFB,     Vdq, Wdq)
DEFFORMAT_2(0x660F3801, PHADDW,     Vdq, Wdq)
DEFFORMAT_2(0x660F3802, PHADDD,     Vdq, Wdq)
DEFFORMAT_2(0x660F3803, PHADDSW,    Vdq, Wdq)
DEFFORMAT_2(0x660F3804, PMADDSUBSW, Vdq, Wdq)
DEFFORMAT_2(0x660F3805, PHSUBW,     Vdq, Wdq)
DEFFORMAT_2(0x660F3806, PHSUBD,     Vdq, Wdq)
DEFFORMAT_2(0x660F3807, PHSUBSW,    Vdq, Wdq)
DEFFORMAT_2(0x660F3808, PSIGNB,     Vdq, Wdq)
DEFFORMAT_2(0x660F3809, PSIGNW,     Vdq, Wdq)
DEFFORMAT_2(0x660F380A, PSIGND,     Vdq, Wdq)
DEFFORMAT_2(0x660F380B, PMULHRSW,   Vdq, Wdq)

// 0F3810
DEFFORMAT_2(0x660F381C, PABSB,  Vdq, Wdq)
DEFFORMAT_2(0x660F381D, PABSW,  Vdq, Wdq)
DEFFORMAT_2(0x660F381E, PABSD,  Vdq, Wdq)




//////////////////////////////////////////////////////////////////////
//
// Group 1
//
DEFFORMAT_EXT_2(0x80, 0, ADD, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 1, OR,  Eb, Ib)
DEFFORMAT_EXT_2(0x80, 2, ADC, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 3, SBB, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 4, AND, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 5, SUB, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 6, XOR, Eb, Ib)
DEFFORMAT_EXT_2(0x80, 7, CMP, Eb, Ib)

DEFFORMAT_EXT_2(0x81, 0, ADD, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 1, OR,  Ev, Iz)
DEFFORMAT_EXT_2(0x81, 2, ADC, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 3, SBB, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 4, AND, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 5, SUB, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 6, XOR, Ev, Iz)
DEFFORMAT_EXT_2(0x81, 7, CMP, Ev, Iz)

DEFFORMAT_EXT_2(0x83, 0, ADD, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 1, OR,  Ev, Ib)
DEFFORMAT_EXT_2(0x83, 2, ADC, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 3, SBB, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 4, AND, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 5, SUB, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 6, XOR, Ev, Ib)
DEFFORMAT_EXT_2(0x83, 7, CMP, Ev, Ib)

DEFFORMAT_EXT_2(0xC0, 0, ROL, Eb, Ib)
DEFFORMAT_EXT_2(0xC0, 1, ROR, Eb, Ib)
DEFFORMAT_EXT_2(0xC0, 2, RCL, Eb, Ib)
DEFFORMAT_EXT_2(0xC0, 3, RCR, Eb, Ib)
DEFFORMAT_EXT_2(0xC0, 4, SHL, Eb, Ib) DEFFORMAT_EXT_2(0xC0, 4, SAL, Eb, Ib)
DEFFORMAT_EXT_2(0xC0, 5, SHR, Eb, Ib)
// 6
DEFFORMAT_EXT_2(0xC0, 7, SAR, Eb, Ib)

DEFFORMAT_EXT_2(0xC1, 0, ROL, Ev, Ib)
DEFFORMAT_EXT_2(0xC1, 1, ROR, Ev, Ib)
DEFFORMAT_EXT_2(0xC1, 2, RCL, Ev, Ib)
DEFFORMAT_EXT_2(0xC1, 3, RCR, Ev, Ib)
DEFFORMAT_EXT_2(0xC1, 4, SHL, Ev, Ib) DEFFORMAT_EXT_2(0xC1, 4, SAL, Ev, Ib)
DEFFORMAT_EXT_2(0xC1, 5, SHR, Ev, Ib)
// 6
DEFFORMAT_EXT_2(0xC1, 7, SAR, Ev, Ib)

DEFFORMAT_EXT_2(0xC6, 0, MOV, Eb, Ib)
DEFFORMAT_EXT_2(0xC7, 0, MOV, Ev, Iz)

DEFFORMAT_EXT_2(0xD0, 0, ROL, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 1, ROR, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 2, RCL, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 3, RCR, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 4, SHL, Eb, 1) DEFFORMAT_EXT_2(0xD0, 4, SAL, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 5, SHR, Eb, 1)
DEFFORMAT_EXT_2(0xD0, 6, SAR, Eb, 1)

DEFFORMAT_EXT_2(0xD1, 0, ROL, Ev, 1)
DEFFORMAT_EXT_2(0xD1, 1, ROR, Ev, 1)
DEFFORMAT_EXT_2(0xD1, 2, RCL, Ev, 1)
DEFFORMAT_EXT_2(0xD1, 3, RCR, Ev, 1)
DEFFORMAT_EXT_2(0xD1, 4, SHL, Ev, 1) DEFFORMAT_EXT_2(0xD1, 4, SAL, Ev, 1)
DEFFORMAT_EXT_2(0xD1, 5, SHR, Ev, 1)
// 6
DEFFORMAT_EXT_2(0xD1, 7, SAR, Ev, 1)

DEFFORMAT_EXT_2(0xD2, 0, ROL, Eb, CL)
DEFFORMAT_EXT_2(0xD2, 1, ROR, Eb, CL)
DEFFORMAT_EXT_2(0xD2, 2, RCL, Eb, CL)
DEFFORMAT_EXT_2(0xD2, 3, RCR, Eb, CL)
DEFFORMAT_EXT_2(0xD2, 4, SHL, Eb, CL) DEFFORMAT_EXT_2(0xD2, 4, SAL, Eb, CL)
DEFFORMAT_EXT_2(0xD2, 5, SHR, Eb, CL)
// 6
DEFFORMAT_EXT_2(0xD2, 7, SAR, Eb, CL)

DEFFORMAT_EXT_2(0xD3, 0, ROL, Ev, CL)
DEFFORMAT_EXT_2(0xD3, 1, ROR, Ev, CL)
DEFFORMAT_EXT_2(0xD3, 2, RCL, Ev, CL)
DEFFORMAT_EXT_2(0xD3, 3, RCR, Ev, CL)
DEFFORMAT_EXT_2(0xD3, 4, SHL, Ev, CL) DEFFORMAT_EXT_2(0xD3, 4, SAL, Ev, CL)
DEFFORMAT_EXT_2(0xD3, 5, SHR, Ev, CL)
// 6
DEFFORMAT_EXT_2(0xD3, 7, SAR, Ev, CL)

//  Group 3 - F6, F7
DEFFORMAT_EXT_2(0xF6, 0, TEST, Eb, Ib)
DEFFORMAT_EXT_1(0xF6, 2, NOT,  Eb)
DEFFORMAT_EXT_1(0xF6, 3, NEG,  Eb)
DEFFORMAT_EXT_1(0xF6, 4, MUL,  Eb)
DEFFORMAT_EXT_1(0xF6, 5, IMUL, Eb)
DEFFORMAT_EXT_1(0xF6, 6, DIV,  Eb)
DEFFORMAT_EXT_1(0xF6, 7, IDIV, Eb)

DEFFORMAT_EXT_2(0xF7, 0, TEST, Ev, Iz)
DEFFORMAT_EXT_1(0xF7, 2, NOT,  Ev)
DEFFORMAT_EXT_1(0xF7, 3, NEG,  Ev)
DEFFORMAT_EXT_1(0xF7, 4, MUL,  Ev)      // vDX:eAX = eAX * Ev
DEFFORMAT_EXT_1(0xF7, 5, IMUL, Ev)      // vDX:eAX = eAX * Ev
DEFFORMAT_EXT_1(0xF7, 6, DIV, Ev)       // eAX, vDX = vDX:eAX / Ev
DEFFORMAT_EXT_1(0xF7, 7, IDIV, Ev)      // eAX, vDX = vDX:eAX / Ev

// Group 4
DEFFORMAT_EXT_1(0xFE, 0, INC, Eb)
DEFFORMAT_EXT_1(0xFE, 1, DEC, Eb)

// Group 5 0xFF
DEFFORMAT_EXT_1(0xFF, 0, INC,   Ev)
DEFFORMAT_EXT_1(0xFF, 1, DEC,   Ev)
DEFFORMAT_EXT_1(0xFF, 2, CALL,  EvF64)
//DEFFORMAT_EXT_1(0xFF, 3, CALLF, Ev)
DEFFORMAT_EXT_1(0xFF, 4, JMP, EvF64)
//DEFFORMAT_EXT_1(0xFF, 5, JMPF, Ev)
DEFFORMAT_EXT_1(0xFF, 6, PUSH, EvD64)


#define op_CALL_Ev  op_CALL_EvF64
#define op_JMP_Ev   op_JMP_EvF64
#define op_PUSH_Ev  op_PUSH_EvD64

#define opext_CALL_Ev  opext_CALL_EvF64
#define opext_JMP_Ev   opext_JMP_EvF64
#define opext_PUSH_Ev  opext_PUSH_EvD64

// Group 12
//              0x660F71, 0
//              0x660F71, 1
DEFFORMAT_EXT_2(0x660F71, 2, PSRLW,  Udq, Ib)
//              0x660F71, 3
DEFFORMAT_EXT_2(0x660F71, 4, PSRAW,  Udq, Ib)
//              0x660F71, 5
DEFFORMAT_EXT_2(0x660F71, 6, PSLLW,  Udq, Ib)
//              0x660F71, 7

// Group 13
//              0x660F72, 0
//              0x660F72, 1
DEFFORMAT_EXT_2(0x660F72, 2, PSRLD,  Udq, Ib)
//              0x660F72, 3
DEFFORMAT_EXT_2(0x660F72, 4, PSRAD,  Udq, Ib)
//              0x660F72, 5
DEFFORMAT_EXT_2(0x660F72, 6, PSLLD,  Udq, Ib)
//              0x660F72, 7

// Group 14
//              0x660F73, 0
//              0x660F73, 1
DEFFORMAT_EXT_2(0x660F73, 2, PSRLQ,  Udq, Ib)
DEFFORMAT_EXT_2(0x660F73, 3, PSRLDQ, Udq, Ib)
//              0x660F73, 4
//              0x660F73, 5
DEFFORMAT_EXT_2(0x660F73, 6, PSLLQ,  Udq, Ib)
DEFFORMAT_EXT_2(0x660F73, 7, PSLLDQ, Udq, Ib)


#if MACH == MACH_x64

DEFFORMAT_2(0x48F20F2C, CVTTSD2SI, Gq,  Wsd)
DEFFORMAT_2(0x48F20F2C, CVTSD2SI,  Gq,  Wsd)

DEFFORMAT_2(0x48F20F2A, CVTSI2SD,  Vsd, Eq)
DEFFORMAT_2(0x48F30F2A, CVTSI2SS,  Vss, Eq)

DEFFORMAT_2(0x48F30F2C, CVTTSS2SI, Gq,  Wss)
DEFFORMAT_2(0x48F20F2D, CVTSS2SI,  Gq,  Wss)

DEFFORMAT_2(0x48660F6E, MOVQ, Vdq, Eq)
DEFFORMAT_2(0x48660F7E, MOVQ, Eq, Vdq)

#endif // MACH == MACH_x64


#undef DEFFORMAT_0
#undef DEFFORMAT_1
#undef DEFFORMAT_P
#undef DEFFORMAT_X
#undef DEFFORMAT_EXT_1
#undef DEFFORMAT_2
#undef DEFFORMAT_EXT_2
#undef DEFFORMAT_3
#undef DEFFORMAT_EXT_3
