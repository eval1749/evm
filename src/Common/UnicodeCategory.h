// @(#)$Id$
//
// Evita UnicodeCategory
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_UnicodeCategory_h)
#define INCLUDE_Common_UnicodeCategory_h

namespace Common {

enum UnicodeCategory {
    UnicodeCategory_OtherNotAssigned =  0,     // CN
    UnicodeCategory_Control =  1,     // CC
    UnicodeCategory_OtherFormat =  2,     // CF
    UnicodeCategory_OtherPrivateUse =  3,     // CO
    UnicodeCategory_OtherSurrogate =  4,     // CS

    // NOTE: FOR \P{L&} =  WE ASSIGN CONSECUTIVE NUMBERS FOR LL =  LT =  LU.
    UnicodeCategory_LowercaseLetter =  5,     // LL
    UnicodeCategory_TitleccaseLetter =  6,     // LT
    UnicodeCategory_UppercaseLetter =  7,     // LU
    UnicodeCategory_ModifierLetter =  8,     // LM
    UnicodeCategory_OtherLetter =  9,     // LO

    UnicodeCategory_CombiningSpacingMark =  10,     // MC
    UnicodeCategory_EnclosingMark =  11,     // ME
    UnicodeCategory_NonSpacingMark =  12,     // MN

    UnicodeCategory_DecimalDigitNumber =  13,     // ND
    UnicodeCategory_LetterNumber =  14,     // NL
    UnicodeCategory_OtherNumber =  15,     // NO

    UnicodeCategory_ConnectorPunctuation =  16,     // PC
    UnicodeCategory_DashPunctuation =  17,     // PD
    UnicodeCategory_ClosePunctuation =  18,     // PE
    UnicodeCategory_FinalPunctuation =  19,     // PF
    UnicodeCategory_InitialPunctuation =  20,     // PI
    UnicodeCategory_OtherPunctuation =  21,     // PO
    UnicodeCategory_OpenPunctuation =  22,     // PS

    UnicodeCategory_CurrencyName =  23,     // SC
    UnicodeCategory_ModifierName =  24,     // SK
    UnicodeCategory_MathName =  25,     // SM
    UnicodeCategory_OtherName =  26,     // SO

    UnicodeCategory_LineSeparator =  27,     // ZL
    UnicodeCategory_ParagraphSeparator =  28,     // ZP
    UnicodeCategory_SpaceSeparator =  29,     // ZS
}; // UnicodeCategory

} // Common

#endif // !defined(INCLUDE_Common_UnicodeCategory_h)
