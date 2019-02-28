#include "fpu_const.h"

// get val's [l, r] bit value
// ex) bit_range(00010011, 7, 1) -> 00010011
uint32_t bit_range(uint32_t val, int l, int r)
{
    val <<= 32 - l;
    val >>= 31 - (l - r);
    return val;
}
uint64_t bit_range64(uint64_t val, int l, int r)
{
    val <<= 64 - l;
    val >>= 63 - (l - r);
    return val;
}
uint32_t bit_reverse(uint32_t val, int bitsize)
{
    val = ~val;
    return bit_range(val, bitsize, 1);
}

class FPU
{
  public:
    static uint32_t fadd(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1);  //23bit
        uint32_t m2 = bit_range(x2, 23, 1);  //23bit

        uint32_t em1 = bit_range(x1, 31, 1);
        uint32_t em2 = bit_range(x2, 31, 1);

        uint32_t b = (em1 > em2);

        uint32_t es, ei; //8bit
        uint32_t ms, mi; //23bit
        if (b)
        {
            es = e1;
            ei = e2;
            ms = m1;
            mi = m2;
        }
        else
        {
            es = e2;
            ei = e1;
            ms = m2;
            mi = m1;
        }

        uint32_t ediff = es - ei;                //8bit
        uint32_t shift = bit_range(ediff, 5, 1); //5bit

        uint32_t mia = ((1 << 25) + (mi << 2)) >> shift; //27bit

        uint32_t ssr = b ? s1 : s2;                                    //1bit
        uint32_t sub = s1 ^ s2;                                        //8bit
        uint32_t inonzero = (ei != 0) & (bit_range(ediff, 8, 6) == 0); //1bit

        //esr = es; msr = ms; mir = mia;

        uint32_t calc; //27bit
        if (sub)
        {
            calc = bit_range((1 << 25) + (ms << 2) - mia, 27, 1);
        }
        else
        {
            calc = bit_range((1 << 25) + (ms << 2) + mia, 27, 1);
        }

        uint32_t ketaoti; //5bit
        if ((calc >> 26) != 0)
        {
            ketaoti = 0b00000;
        }
        else if ((calc >> 25) != 0)
        {
            ketaoti = 0b00001;
        }
        else if ((calc >> 24) != 0)
        {
            ketaoti = 0b00010;
        }
        else if ((calc >> 23) != 0)
        {
            ketaoti = 0b00011;
        }
        else if ((calc >> 22) != 0)
        {
            ketaoti = 0b00100;
        }
        else if ((calc >> 21) != 0)
        {
            ketaoti = 0b00101;
        }
        else if ((calc >> 20) != 0)
        {
            ketaoti = 0b00110;
        }
        else if ((calc >> 19) != 0)
        {
            ketaoti = 0b00111;
        }
        else if ((calc >> 18) != 0)
        {
            ketaoti = 0b01000;
        }
        else if ((calc >> 17) != 0)
        {
            ketaoti = 0b01001;
        }
        else if ((calc >> 16) != 0)
        {
            ketaoti = 0b01010;
        }
        else if ((calc >> 15) != 0)
        {
            ketaoti = 0b01011;
        }
        else if ((calc >> 14) != 0)
        {
            ketaoti = 0b01100;
        }
        else if ((calc >> 13) != 0)
        {
            ketaoti = 0b01101;
        }
        else if ((calc >> 12) != 0)
        {
            ketaoti = 0b01110;
        }
        else if ((calc >> 11) != 0)
        {
            ketaoti = 0b01111;
        }
        else if ((calc >> 10) != 0)
        {
            ketaoti = 0b10000;
        }
        else if ((calc >> 9) != 0)
        {
            ketaoti = 0b10001;
        }
        else if ((calc >> 8) != 0)
        {
            ketaoti = 0b10010;
        }
        else if ((calc >> 7) != 0)
        {
            ketaoti = 0b10011;
        }
        else if ((calc >> 6) != 0)
        {
            ketaoti = 0b10100;
        }
        else if ((calc >> 5) != 0)
        {
            ketaoti = 0b10101;
        }
        else if ((calc >> 4) != 0)
        {
            ketaoti = 0b10110;
        }
        else if ((calc >> 3) != 0)
        {
            ketaoti = 0b10111;
        }
        else if ((calc >> 2) != 0)
        {
            ketaoti = 0b11000;
        }
        else if ((calc >> 1) != 0)
        {
            ketaoti = 0b11001;
        }
        else if ((calc >> 0) != 0)
        {
            ketaoti = 0b11010;
        }
        else
        {
            ketaoti = 0b11111;
        }

        uint32_t nonzero = (calc != 0); //1bit

        uint32_t my = calc << ketaoti;

        uint32_t ey = es - ketaoti + 1; //9bit

        uint32_t eya; //8bit
        if (bit_range(ey, 9, 9))
            eya = 0;
        else
            eya = bit_range(ey, 8, 1);

        uint32_t y;

        if (inonzero)
        {
            if (nonzero)
            {
                y = (ssr << 31) + (eya << 23) + bit_range(my, 26, 4) + bit_range(my, 3, 3);
            }
            else
            {
                y = (ssr << 31);
            }
        }
        else
        {
            y = (ssr << 31) + (es << 23) + ms;
        }

        return y;
    }

    static uint32_t fsub(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1);  //23bit
        uint32_t m2 = bit_range(x2, 23, 1);  //23bit

        s2 = bit_reverse(s2, 1);

        uint32_t em1 = bit_range(x1, 31, 1);
        uint32_t em2 = bit_range(x2, 31, 1);

        uint32_t b = (em1 > em2);

        uint32_t es, ei; //8bit
        uint32_t ms, mi; //23bit
        if (b)
        {
            es = e1;
            ei = e2;
            ms = m1;
            mi = m2;
        }
        else
        {
            es = e2;
            ei = e1;
            ms = m2;
            mi = m1;
        }

        uint32_t ediff = es - ei; //8bit
        uint32_t shift;           //5bit
        if (bit_range(ediff, 8, 6) != 0)
            shift = 31;
        else
            shift = bit_range(ediff, 5, 1);

        uint32_t mia = ((1 << 25) + (mi << 2)) >> shift; //27bit

        uint32_t ssr = b ? s1 : s2; //1bit
        //esr = es;
        uint32_t msr; //25bit
        if (es != 0)
            msr = (1 << 23) + ms;
        else
            msr = ms;

        uint32_t mir; //27bit
        if (s1 == s2)
            mir = mia;
        else
            mir = bit_reverse(mia, 27) + 1;

        uint32_t inonzero = (ei != 0); //1bit

        uint32_t calc = bit_range((msr << 2) + mir, 27, 1); //27bit

        uint32_t ketaoti; //5bit
        if ((calc >> 26) != 0)
        {
            ketaoti = 0b00000;
        }
        else if ((calc >> 25) != 0)
        {
            ketaoti = 0b00001;
        }
        else if ((calc >> 24) != 0)
        {
            ketaoti = 0b00010;
        }
        else if ((calc >> 23) != 0)
        {
            ketaoti = 0b00011;
        }
        else if ((calc >> 22) != 0)
        {
            ketaoti = 0b00100;
        }
        else if ((calc >> 21) != 0)
        {
            ketaoti = 0b00101;
        }
        else if ((calc >> 20) != 0)
        {
            ketaoti = 0b00110;
        }
        else if ((calc >> 19) != 0)
        {
            ketaoti = 0b00111;
        }
        else if ((calc >> 18) != 0)
        {
            ketaoti = 0b01000;
        }
        else if ((calc >> 17) != 0)
        {
            ketaoti = 0b01001;
        }
        else if ((calc >> 16) != 0)
        {
            ketaoti = 0b01010;
        }
        else if ((calc >> 15) != 0)
        {
            ketaoti = 0b01011;
        }
        else if ((calc >> 14) != 0)
        {
            ketaoti = 0b01100;
        }
        else if ((calc >> 13) != 0)
        {
            ketaoti = 0b01101;
        }
        else if ((calc >> 12) != 0)
        {
            ketaoti = 0b01110;
        }
        else if ((calc >> 11) != 0)
        {
            ketaoti = 0b01111;
        }
        else if ((calc >> 10) != 0)
        {
            ketaoti = 0b10000;
        }
        else if ((calc >> 9) != 0)
        {
            ketaoti = 0b10001;
        }
        else if ((calc >> 8) != 0)
        {
            ketaoti = 0b10010;
        }
        else if ((calc >> 7) != 0)
        {
            ketaoti = 0b10011;
        }
        else if ((calc >> 6) != 0)
        {
            ketaoti = 0b10100;
        }
        else if ((calc >> 5) != 0)
        {
            ketaoti = 0b10101;
        }
        else if ((calc >> 4) != 0)
        {
            ketaoti = 0b10110;
        }
        else if ((calc >> 3) != 0)
        {
            ketaoti = 0b10111;
        }
        else if ((calc >> 2) != 0)
        {
            ketaoti = 0b11000;
        }
        else if ((calc >> 1) != 0)
        {
            ketaoti = 0b11001;
        }
        else if ((calc >> 0) != 0)
        {
            ketaoti = 0b11010;
        }
        else
        {
            ketaoti = 0b11111;
        }

        uint32_t zero = (ketaoti == 0b11111); //1bit

        uint32_t my = calc << ketaoti; //27bit

        uint32_t ey = es - ketaoti + 1; //9bit

        uint32_t eya; //8bit
        if (bit_range(ey, 9, 9))
            eya = 0;
        else
            eya = bit_range(ey, 8, 1);

        uint32_t y;

        if (inonzero)
        {
            if (zero)
            {
                y = (ssr << 31);
            }
            else
            {
                y = (ssr << 31) + (eya << 23) + bit_range(my, 26, 4) + bit_range(my, 3, 3);
            }
        }
        else
        {
            y = (ssr << 31) + (es << 23) + bit_range(msr, 23, 1);
        }

        return y;
    }

    static uint32_t fmul(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1);  //23bit
        uint32_t m2 = bit_range(x2, 23, 1);  //23bit

        uint64_t calc; //48bit
        calc = (uint64_t)((1 << 23) + m1) * (uint64_t)((1 << 23) + m2);

        uint32_t sy = s1 ^ s2;                                //1bit
        uint32_t esum = bit_range(e1 + e2 - 0b1111111, 9, 1); //9bit
        uint32_t calcr = bit_range64(calc, 48, 23);           //26bit
        uint32_t nz = ((e1 != 0) & (e2 != 0));                //1bit

        uint32_t mketa; //24bit
        uint32_t my;    //23bit

        if (bit_range(calcr, 26, 26))
            mketa = bit_range(calcr, 25, 2);
        else
            mketa = bit_range(calcr, 24, 1);

        my = bit_range(bit_range(mketa, 24, 2) + bit_range(mketa, 1, 1), 23, 1);

        uint32_t ey = esum;
        if ((bit_range(calcr, 26, 26) != 0) | (bit_range(calcr, 24, 1) == 0b111111111111111111111111))
            ey++;

        uint32_t y;
        if (nz)
            y = (sy << 31) + (bit_range(ey, 8, 1) << 23) + my;
        else
            y = (sy << 31);

        return y;
    }

    static uint32_t finv(uint32_t x)
    {
        uint32_t s = bit_range(x, 32, 32);     //1bit
        uint32_t e = bit_range(x, 31, 24);     //8bit
        uint32_t index = bit_range(x, 23, 14); //10bit
        uint32_t a = bit_range(x, 13, 1);      //13bit

        uint32_t c = fpu_inv_c[index]; //23bit
        uint32_t g = fpu_inv_g[index]; //13bit

        uint32_t ey; //8bit
        if (index != 0 || a != 0)
        {
            ey = bit_reverse(e + 2, 8);
        }
        else
        {
            ey = bit_reverse(e + 1, 8);
        }

        uint64_t calc = (1UL << 35) + ((uint64_t)c << 12) - (uint64_t)(g * a); //36bit

        uint32_t y = (s << 31) + (ey << 23) + bit_range64(calc, 35, 13);

        return y;
    }

    static uint32_t fdiv(uint32_t x1, uint32_t x2)
    {
        return fmul(x1, finv(x2));
    }

    //fpu/fsqrt (not fsqrt2)
    //not used
    static uint32_t fsqrt(uint32_t x)
    {
        using namespace std;
        uint32_t s = bit_range(x, 32, 32);                     //1bit
        uint32_t e = bit_range(x, 31, 25);                     //7bit
        uint32_t index = bit_range(x, 24, 15);                 //10bit
        uint32_t a = bit_range(x, 14, 1);                      //14bit
        uint32_t d = bit_reverse(bit_range(index, 10, 10), 1); //1bit

        uint32_t c = fpu_sqrt_c[index];                                                                                //23bit
        uint32_t g = fpu_sqrt_g[index];                                                                                //13bit
        uint32_t em = e - bit_reverse(bit_range(index, 10, 10), 1);                                                    //7bit
        uint32_t e1 = (bit_range(em, 7, 7) << 7) + (bit_reverse(bit_range(em, 7, 7), 1) << 6) + (bit_range(em, 6, 1)); //8bit

        uint64_t calc; //37bit
        calc = ((uint64_t)c << 14) + g * a;

        uint32_t y;
        if (e1 == 0b10111111)
        {
            y = s << 31;
        }
        else
        {
            y = (s << 31) + (e1 << 23) + bit_range64(calc, 37, 15) + bit_range64(calc, 14, 14);
        }

        return y;
    }

    static uint32_t feq(uint32_t x, uint32_t y)
    {
        uint32_t x1 = bit_range(x, 31, 1);
        uint32_t x2 = bit_range(y, 31, 1);
        return x == y ? 1 : (x1 == 0) && (x2 == 0) ? 1 : 0;
    }
    static uint32_t flt(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32);
        uint32_t s2 = bit_range(x2, 32, 32);
        uint32_t e1 = bit_range(x1, 31, 24);
        uint32_t e2 = bit_range(x2, 31, 24);
        uint32_t m1 = bit_range(x1, 23, 1);
        uint32_t m2 = bit_range(x2, 23, 1);

        uint32_t t1 = bit_range(x1, 31, 1);
        uint32_t t2 = bit_range(x2, 31, 1);
        if ((t1 == 0) && (t2 == 0))
            return 0;

        return (s1 == 1) && (s2 == 0) ? 1 : (s1 == 0) && (s2 == 1) ? 0 : (s1 == 0) && (e1 < e2) ? 1 : (s1 == 0) && (e1 == e2) && (m1 < m2) ? 1 : (s1 == 1) && (e1 > e2) ? 1 : (s1 == 1) && (e1 == e2) && (m1 > m2) ? 1 : 0;
    }
    static uint32_t fle(uint32_t x, uint32_t y)
    {
        return flt(y, x) ? 0 : 1;
    }
    static uint32_t fsgnj(uint32_t x1, uint32_t x2)
    {
        //uint32_t s1 = bit_range(x1, 32, 32);
        uint32_t s2 = bit_range(x2, 32, 32);
        uint32_t x = bit_range(x1, 31, 1);
        return (s2 << 31) | x;
    }
    static uint32_t fsgnjn(uint32_t x1, uint32_t x2)
    {
        uint32_t s2 = bit_range(x2, 32, 32);
        uint32_t x = bit_range(x1, 31, 1);
        return ((s2 ^ 1) << 31) | x;
    }
    static uint32_t fsgnjx(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32);
        uint32_t s2 = bit_range(x2, 32, 32);
        uint32_t x = bit_range(x1, 31, 1);
        return ((s1 ^ s2) << 31) | x;
    }
    static uint32_t float2int(uint32_t x)
    {
        using namespace std;
        uint32_t s = bit_range(x, 32, 32); //1bit
        uint32_t e = bit_range(x, 31, 24); //8bit
        uint32_t m = bit_range(x, 23, 1);  //23bit

        uint32_t shift = 158 - e;                     //8bit
        uint32_t ma = (1 << 31) + (m << 8);           //32bit
        uint32_t mabs = ma >> bit_range(shift, 5, 1); //32bit
        uint32_t my;                                  //32bit

        if (s)
            my = bit_reverse(mabs, 32) + 1;
        else
            my = mabs;

        bool nonzero = e > 126;

        uint32_t y;

        if (nonzero)
            y = my;
        else
            y = 0;

        return y;
    }

  static uint32_t int2float(uint32_t x)
    {
        using namespace std;
        uint32_t s = bit_range(x, 32, 32); //1bit
        uint32_t m = bit_range(x, 31, 1);  //31bit
        uint32_t mabs;                     //31bit
        if (s)
            mabs = bit_reverse(m, 31) + 1;
        else
            mabs = m;

        bool nonzero = s || (m != 0);

        uint32_t my; //31bit
        if ((mabs >> 30) != 0)
        {
            my = (0b10011101 << 23) + bit_range(mabs, 30, 8);
        }
        else if ((mabs >> 29) != 0)
        {
            my = (0b10011100 << 23) + bit_range(mabs, 29, 7);
        }
        else if ((mabs >> 28) != 0)
        {
            my = (0b10011011 << 23) + bit_range(mabs, 28, 6);
        }
        else if ((mabs >> 27) != 0)
        {
            my = (0b10011010 << 23) + bit_range(mabs, 27, 5);
        }
        else if ((mabs >> 26) != 0)
        {
            my = (0b10011001 << 23) + bit_range(mabs, 26, 4);
        }
        else if ((mabs >> 25) != 0)
        {
            my = (0b10011000 << 23) + bit_range(mabs, 25, 3);
        }
        else if ((mabs >> 24) != 0)
        {
            my = (0b10010111 << 23) + bit_range(mabs, 24, 2);
        }
        else if ((mabs >> 23) != 0)
        {
            my = (0b10010110 << 23) + bit_range(mabs, 23, 1);
        }
        else if ((mabs >> 22) != 0)
        {
            my = (0b10010101 << 23) + (bit_range(mabs, 22, 1) << 1);
        }
        else if ((mabs >> 21) != 0)
        {
            my = (0b10010100 << 23) + (bit_range(mabs, 21, 1) << 2);
        }
        else if ((mabs >> 20) != 0)
        {
            my = (0b10010011 << 23) + (bit_range(mabs, 20, 1) << 3);
        }
        else if ((mabs >> 19) != 0)
        {
            my = (0b10010010 << 23) + (bit_range(mabs, 19, 1) << 4);
        }
        else if ((mabs >> 18) != 0)
        {
            my = (0b10010001 << 23) + (bit_range(mabs, 18, 1) << 5);
        }
        else if ((mabs >> 17) != 0)
        {
            my = (0b10010000 << 23) + (bit_range(mabs, 17, 1) << 6);
        }
        else if ((mabs >> 16) != 0)
        {
            my = (0b10001111 << 23) + (bit_range(mabs, 16, 1) << 7);
        }
        else if ((mabs >> 15) != 0)
        {
            my = (0b10001110 << 23) + (bit_range(mabs, 15, 1) << 8);
        }
        else if ((mabs >> 14) != 0)
        {
            my = (0b10001101 << 23) + (bit_range(mabs, 14, 1) << 9);
        }
        else if ((mabs >> 13) != 0)
        {
            my = (0b10001100 << 23) + (bit_range(mabs, 13, 1) << 10);
        }
        else if ((mabs >> 12) != 0)
        {
            my = (0b10001011 << 23) + (bit_range(mabs, 12, 1) << 11);
        }
        else if ((mabs >> 11) != 0)
        {
            my = (0b10001010 << 23) + (bit_range(mabs, 11, 1) << 12);
        }
        else if ((mabs >> 10) != 0)
        {
            my = (0b10001001 << 23) + (bit_range(mabs, 10, 1) << 13);
        }
        else if ((mabs >> 9) != 0)
        {
            my = (0b10001000 << 23) + (bit_range(mabs, 9, 1) << 14);
        }
        else if ((mabs >> 8) != 0)
        {
            my = (0b10000111 << 23) + (bit_range(mabs, 8, 1) << 15);
        }
        else if ((mabs >> 7) != 0)
        {
            my = (0b10000110 << 23) + (bit_range(mabs, 7, 1) << 16);
        }
        else if ((mabs >> 6) != 0)
        {
            my = (0b10000101 << 23) + (bit_range(mabs, 6, 1) << 17);
        }
        else if ((mabs >> 5) != 0)
        {
            my = (0b10000100 << 23) + (bit_range(mabs, 5, 1) << 18);
        }
        else if ((mabs >> 4) != 0)
        {
            my = (0b10000011 << 23) + (bit_range(mabs, 4, 1) << 19);
        }
        else if ((mabs >> 3) != 0)
        {
            my = (0b10000010 << 23) + (bit_range(mabs, 3, 1) << 20);
        }
        else if ((mabs >> 2) != 0)
        {
            my = (0b10000001 << 23) + (bit_range(mabs, 2, 1) << 21);
        }
        else if ((mabs >> 1) != 0)
        {
            my = (0b10000000 << 23) + (bit_range(mabs, 1, 1) << 22);
        }
        else if ((mabs >> 0) != 0)
        {
            my = 0b01111111 << 23;
        }
        else
        {
            my = 0b10011110 << 23;
        }

        uint32_t y;

        if (nonzero)
            y = (s << 31) + my;
        else
            y = 0;

        return y;
    }
};
