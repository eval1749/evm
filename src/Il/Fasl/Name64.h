// @(#)$Id$
//
// Evita Il - Name64
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Fasl_Name64_h)
#define INCLUDE_Il_Fasl_Name64_h

namespace Il {
namespace Fasl {

//   1 = '0'     2 = '1'     3 = '2'     4 = '3'     5 = '4'
//   6 = '5'     7 = '6'     8 = '7'     9 = '8'    10 = '9'
//  11 = 'A'    12 = 'B'    13 = 'C'    14 = 'D'    15 = 'E'
//  16 = 'F'    17 = 'G'    18 = 'H'    19 = 'I'    20 = 'J'
//  21 = 'K'    22 = 'L'    23 = 'M'    24 = 'N'    25 = 'O'
//  26 = 'P'    27 = 'Q'    28 = 'R'    29 = 'S'    30 = 'T'
//  31 = 'U'    32 = 'V'    33 = 'W'    34 = 'X'    35 = 'Y'
//  36 = 'Z'    37 = 'a'    38 = 'b'    39 = 'c'    40 = 'd'
//  41 = 'e'    42 = 'f'    43 = 'g'    44 = 'h'    45 = 'i'
//  46 = 'j'    47 = 'k'    48 = 'l'    49 = 'm'    50 = 'n'
//  51 = 'o'    52 = 'p'    53 = 'q'    54 = 'r'    55 = 's'
//  56 = 't'    57 = 'u'    58 = 'v'    59 = 'w'    60 = 'x'
//  61 = 'y'    62 = 'z'    63 = '_'
class Name64 {
    public: static int FromChar(char16 const cwch);
    public: static char16 FromIndex(int const iIndex);
}; // Name64

} // Fasl
} // Il

#endif // !defined(INCLUDE_Il_Fasl_Name64_h)
