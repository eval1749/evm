#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Name64
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Name64.h"

namespace Il {
namespace Fasl {

static char k_rgchMapIndexToChar[64];
static int k_rgiMapCharToIndex[128];

int Name64::FromChar(char16 const wch) {
    if (0 == k_rgiMapCharToIndex['A']) {
        auto iIndex = 1;

        for (auto ch = '0';  ch <= '9'; ch++) { // 1..10
            k_rgiMapCharToIndex[ch] = iIndex++;
        } // for ch

        for (auto ch = 'A';  ch <= 'Z'; ch++) { // 11..36
            k_rgiMapCharToIndex[ch] = iIndex++;
        } // for ch

        for (auto ch = 'a';  ch <= 'z'; ch++) { // 37..62
            k_rgiMapCharToIndex[ch] = iIndex++;
        } // for ch

        k_rgiMapCharToIndex['_'] = iIndex++;

        ASSERT(iIndex <= 64);
    } // if

    return static_cast<uint>(wch) > lengthof(k_rgiMapCharToIndex)
        ? -1
        : k_rgiMapCharToIndex[wch];
} // FromChar

char16 Name64::FromIndex(int const iIndex) {
    if (0 == k_rgchMapIndexToChar['A']) {
        for (auto ch = '0';  ch <= '9'; ch++) {
            k_rgchMapIndexToChar[FromChar(ch)] = ch;
        } // for ch

        for (auto ch = 'A';  ch <= 'Z'; ch++) {
            k_rgchMapIndexToChar[FromChar(ch)] = ch;
        } // for ch

        for (auto ch = 'a';  ch <= 'z'; ch++) {
            k_rgchMapIndexToChar[FromChar(ch)] = ch;
        } // for ch

        k_rgchMapIndexToChar[FromChar('_')] = '_';
    } // if

    return static_cast<uint>(iIndex) > lengthof(k_rgchMapIndexToChar)
        ? 0
        : k_rgchMapIndexToChar[iIndex];
} // FromChar

} // Fasl
} // Il
