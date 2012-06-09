// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Io_XhtmlWriter_h)
#define INCLUDE_Il_Io_XhtmlWriter_h

namespace Common {
namespace Io {
    class TextWriter;
} // Io
} // Common

namespace Il {
namespace Io {

using Common::Io::TextWriter;

class XhtmlWriter :
        public Object_<XhtmlWriter> {

    public: static const char* Kind_() { return "XhtmlWriter"; }

    private: struct NameValuePair {
        const char* m_pszName;
        const char16* m_pwszValue;
    }; // NameValuePair

    private: struct NameValuePairA {
        const char* m_pszName;
        const char* m_pszValue;
    }; // NameValuePairA

    private: TextWriter* const m_pTextWriter;

    // ctor
    public: XhtmlWriter(TextWriter* const pTextWriter);
    public: virtual ~XhtmlWriter() {}

    // [C]
    public: void Close();

    // [F]
    public: void Flush();

    // [G]
    public: TextWriter* GetTextWriter() const { return m_pTextWriter; }

    // [E]
    public: void EndElement(const char* pszName);

    // [P]
    public: void Printf(const char* pszFormat, ...);
    public: void PrintfV(const char* pszFormat, va_list args);

    // [S]
    public: void StartElement(const char* pszName);

    public: void StartElement(
        const char* pszElementName,
        const char* pszName1,
        const char* const pszValue1);

    public: void StartElement(
        const char* pszElementName,
        const char* pszName1,
        const char* const pszValue1,
        const char* pszName2,
        const char* const pszValue2);

    public: void StartElement(
        const char* pszElementName,
        const char* pszName1,
        const char* const pszValue1,
        const char* pszName2,
        const char* const pszValue2,
        const char* pszName3,
        const char* const pszValue3);

    private: void StartElement(
        const char* pszElementName,
        const NameValuePairA* prgoNameValuePairA,
        size_t cNameValuePaires);

    public: void StartElement(
        const char* pszElementName,
        const char* pszName1,
        const char16* const pwszAttValue1);

    public: void StartElement(
        const char* pszElementName,
        const char* pszName1,
        const char16* const pwszAttValue1,
        const char* pszName2,
        const char16* const pwszAttValue2);

    private: void StartElement(
        const char* pszElementName,
        const NameValuePair* prgoNameValuePair,
        size_t cNameValuePaires);

    // [W]
    public: void Write(char const ch)
        { this->Write(static_cast<char16>(ch)); }

    public: void Write(char16 const wch);
    public: void WriteElement(const char* pszName, const char16* pwszContent);

    public: void WriteLine() { this->Write('\n'); }

    public: void WriteString(const char* pszString) {
        this->WriteString(pszString, ::lstrlenA(pszString));
    } // WriteString

    public: void WriteString(const char* pchString, size_t const cch);

    public: void WriteString(const char16* pwszString) {
        this->WriteString(pwszString, ::lstrlenW(pwszString));
    } // WriteString

    public: void WriteString(const char16* pwchString, size_t const cwch);

    DISALLOW_COPY_AND_ASSIGN(XhtmlWriter);
}; // XhtmlWriter

} // Io
} // Il

#endif // !defined(INCLUDE_Il_Io_XhtmlWriter_h)
