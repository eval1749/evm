#include <stdio.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*a))

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned int uint;

// Map BCD < 1000 (0...999) to DPD(Densely Packed Decimal)
// http://speleotrove.com/decimal/DPDecimal.html
static uint16 kPackedBcd3ToDpd[0x999 + 1];

// Map DPB(10bit) to 3BCD(12bit)
static uint16 kDpdToPackedBcd3[1 << 12];

// Map DPD(12bit) to binary (0...999)
static uint16 kDpdTo999[1 << 10];

class Static {
  private: static int Mod10(int const x) {
    return x % 10;
  }

  public: static void DumpTable(const char* name, uint16* table, uint size) {
    printf("\n");
    printf("static uint16 const %s[%d] = {", name, size);
    auto const kColumns = 10;
    auto column = 0;
    for (auto i = 0; i < size; i++) {
      if (!(column % kColumns)) {
        printf("\n ");
        column = 0;
      }

      printf(" %d,", table[i]);
      ++column;
    }
    if (column % kColumns) {
      printf("\n");
    }
    printf("};\n");
  }

  public: static void Init() {
    // Compute (abcd) (efgh) (ijkm) to (pqr)(stu)(v)(wxy)
    for (auto num = 1; num <= 999; ++num) {
      auto const abcd = Mod10(num / 100);
      auto const efgh = Mod10(num / 10);
      auto const ijkm = Mod10(num);

      auto const a = abcd >> 3;
      auto const b = (abcd >> 2) & 1;
      auto const c = (abcd >> 1) & 1;
      auto const d = abcd & 1;

      auto const e = efgh >> 3;
      auto const f = (efgh >> 2) & 1;
      auto const g = (efgh >> 1) & 1;
      auto const h = efgh & 1;

      auto const i = ijkm >> 3;
      auto const j = (ijkm >> 2) & 1;
      auto const k = (ijkm >> 1) & 1;
      auto const m = ijkm & 1;

      auto const p = b | (a & j) | (a & f & i);
      auto const q = c | (a & k) | (a & g & i);
      auto const r = d;
      auto const s = (f & (~a | ~i)) | (~a & e & j) | (e & i);
      auto const t = g  | (~a & e &k) | (a & i);
      auto const u = h;
      auto const v = a | e | i;
      auto const w = a | (e & i) | (~e & j);
      auto const x = e | (a & i) | (~a & k);
      auto const y = m;

      auto const packed_bcd3 = (abcd << 8) | (efgh << 4) | ijkm;

      auto const dpd = uint16(
          (p << 9) | (q << 8) | (r << 7) | (s << 6) | (t << 5)
          | (u << 4) | (v << 3) | (w << 2) | (x << 1) | y);

      kPackedBcd3ToDpd[packed_bcd3] = uint16(dpd);
      kDpdToPackedBcd3[dpd] = uint16(packed_bcd3);
      kDpdTo999[dpd] = uint16(num);
    }
  }
};

void main() {
  Static::Init();
  #define DUMP_TABLE(name) Static::DumpTable(#name, name, ARRAYSIZE(name))
  DUMP_TABLE(kDpdTo999);
  DUMP_TABLE(kDpdToPackedBcd3);
  DUMP_TABLE(kPackedBcd3ToDpd);
}
