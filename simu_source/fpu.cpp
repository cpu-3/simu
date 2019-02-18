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

void char_uint_cpy(uint32_t *c, char *x, int bit_size)
{
    int i = 0;
    while (x[i] != '\0')
    {
        if (x[i] == '1')
        {
            *c |= 1 << (bit_size - 1 - i);
        }
        i++;
    }
}
/*
uint32_t map(uint32_t mh)
{
    switch (mh)
    {
    case 0b00000:
        return 0b01111110;
    case 0b00001:
        return 0b01111010;
    case 0b00010:
        return 0b01110111;
    case 0b00011:
        return 0b01110011;
    case 0b00100:
        return 0b01110000;
    case 0b00101:
        return 0b01101101;
    case 0b00110:
        return 0b01101010;
    case 0b00111:
        return 0b01101000;
    case 0b01000:
        return 0b01100101;
    case 0b01001:
        return 0b01100011;
    case 0b01010:
        return 0b01100000;
    case 0b01011:
        return 0b01011110;
    case 0b01100:
        return 0b01011100;
    case 0b01101:
        return 0b01011010;
    case 0b01110:
        return 0b01011000;
    case 0b01111:
        return 0b01010110;
    case 0b10000:
        return 0b01010100;
    case 0b10001:
        return 0b01010011;
    case 0b10010:
        return 0b01010001;
    case 0b10011:
        return 0b01010000;
    case 0b10100:
        return 0b01001110;
    case 0b10101:
        return 0b01001101;
    case 0b10110:
        return 0b01001011;
    case 0b10111:
        return 0b01001010;
    case 0b11000:
        return 0b01001000;
    case 0b11001:
        return 0b01000111;
    case 0b11010:
        return 0b01000110;
    case 0b11011:
        return 0b01000101;
    case 0b11100:
        return 0b01000100;
    case 0b11101:
        return 0b01000011;
    case 0b11110:
        return 0b01000010;
    case 0b11111:
        return 0b01000001;
    default:
        return 0b0;
    }
}
*/

class FPU
{
  public:
    static uint32_t fadd(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1); //23bit
        uint32_t m2 = bit_range(x2, 23, 1); //23bit

        uint32_t em1 = bit_range(x1, 31, 1);
        uint32_t em2 = bit_range(x2, 31, 1);

        uint32_t b = (em1 > em2);

        uint32_t es, ei; //8bit
        uint32_t ms, mi; //23bit
        if(b){
            es = e1;
            ei = e2;
            ms = m1;
            mi = m2;
        }else{
            es = e2;
            ei = e1;
            ms = m2;
            mi = m1;
        }

        uint32_t ediff = es - ei; //8bit
        uint32_t shift = bit_range(ediff, 5, 1); //5bit

        uint32_t mia = ((1 << 25) + (mi << 2)) >> shift; //27bit

        uint32_t ssr = b ? s1 : s2; //1bit
        uint32_t sub = s1 ^ s2; //8bit
        uint32_t inonzero = (ei != 0) & (bit_range(ediff, 8, 6) == 0); //1bit

        //esr = es;
        //msr = ms;
        //mir = mia;

        uint32_t calc; //27bit
        if(sub){
            calc = bit_range((1 << 25) + (ms << 2) - mia, 27, 1);
        }
        else{
            calc = bit_range((1 << 25) + (ms << 2) + mia, 27, 1);
        }

        uint32_t ketaoti; //5bit
        if((calc >> 26) != 0){
             ketaoti = 0b00000;
        }
        else if((calc >> 25) != 0){
             ketaoti = 0b00001;
        }
        else if((calc >> 24) != 0){
             ketaoti = 0b00010;
        }
        else if((calc >> 23) != 0){
             ketaoti = 0b00011;
        }
        else if((calc >> 22) != 0){
             ketaoti = 0b00100;
        }
        else if((calc >> 21) != 0){
             ketaoti = 0b00101;
        }
        else if((calc >> 20) != 0){
             ketaoti = 0b00110;
        }
        else if((calc >> 19) != 0){
             ketaoti = 0b00111;
        }
        else if((calc >> 18) != 0){
             ketaoti = 0b01000;
        }
        else if((calc >> 17) != 0){
             ketaoti = 0b01001;
        }
        else if((calc >> 16) != 0){
             ketaoti = 0b01010;
        }
        else if((calc >> 15) != 0){
             ketaoti = 0b01011;
        }
        else if((calc >> 14) != 0){
             ketaoti = 0b01100;
        }
        else if((calc >> 13) != 0){
             ketaoti = 0b01101;
        }
        else if((calc >> 12) != 0){
             ketaoti = 0b01110;
        }
        else if((calc >> 11) != 0){
             ketaoti = 0b01111;
        }
        else if((calc >> 10) != 0){
             ketaoti = 0b10000;
        }
        else if((calc >> 9) != 0){
             ketaoti = 0b10001;
        }
        else if((calc >> 8) != 0){
             ketaoti = 0b10010;
        }
        else if((calc >> 7) != 0){
             ketaoti = 0b10011;
        }
        else if((calc >> 6) != 0){
             ketaoti = 0b10100;
        }
        else if((calc >> 5) != 0){
             ketaoti = 0b10101;
        }
        else if((calc >> 4) != 0){
             ketaoti = 0b10110;
        }
        else if((calc >> 3) != 0){
             ketaoti = 0b10111;
        }
        else if((calc >> 2) != 0){
             ketaoti = 0b11000;
        }
        else if((calc >> 1) != 0){
             ketaoti = 0b11001;
        }
        else if((calc >> 0) != 0){
             ketaoti = 0b11010;
        }
        else{
             ketaoti = 0b11111;
        }

        uint32_t nonzero = (calc != 0); //1bit

        uint32_t my = calc << ketaoti;

        uint32_t ey = es - ketaoti + 1; //9bit

        uint32_t eya; //8bit
        if(bit_range(ey, 9, 9))
            eya = 0;
        else
            eya = bit_range(ey, 8, 1);

        uint32_t y;

        if(inonzero){
            if(nonzero){
                y = (ssr << 31) + (eya << 23) + bit_range(my, 26, 4) + bit_range(my, 3, 3);
            }
            else{
                y = (ssr << 31);
            }
        }
        else{
            y = (ssr << 31) + (es << 23) + ms;
        }

        printf("fadd %x %x %x\n", x1, x2, y);
        return y;

    }

    static uint32_t fsub(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1); //23bit
        uint32_t m2 = bit_range(x2, 23, 1); //23bit

        s2 = bit_reverse(s2, 1);

        uint32_t em1 = bit_range(x1, 31, 1);
        uint32_t em2 = bit_range(x2, 31, 1);

        uint32_t b = (em1 > em2);

        uint32_t es, ei; //8bit
        uint32_t ms, mi; //23bit
        if(b){
            es = e1;
            ei = e2;
            ms = m1;
            mi = m2;
        }else{
            es = e2;
            ei = e1;
            ms = m2;
            mi = m1;
        }

        uint32_t ediff = es - ei; //8bit
        uint32_t shift; //5bit
        if(bit_range(ediff, 8, 6) != 0)
            shift = 31;
        else
            shift = bit_range(ediff, 5, 1);

        uint32_t mia = ((1 << 25) + (mi << 2)) >> shift; //27bit

        uint32_t ssr = b ? s1 : s2; //1bit
        //esr = es;
        uint32_t msr; //25bit
        if(es != 0)
            msr = (1 << 23) + ms;
        else
            msr = ms;

        uint32_t mir; //27bit
        if(s1 == s2)
            mir = mia;
        else
            mir = bit_reverse(mia, 27) + 1;

        uint32_t inonzero = (ei != 0); //1bit

        uint32_t calc = bit_range((msr << 2) + mir, 27, 1); //27bit

        uint32_t ketaoti; //5bit
        if((calc >> 26) != 0){
             ketaoti = 0b00000;
        }
        else if((calc >> 25) != 0){
             ketaoti = 0b00001;
        }
        else if((calc >> 24) != 0){
             ketaoti = 0b00010;
        }
        else if((calc >> 23) != 0){
             ketaoti = 0b00011;
        }
        else if((calc >> 22) != 0){
             ketaoti = 0b00100;
        }
        else if((calc >> 21) != 0){
             ketaoti = 0b00101;
        }
        else if((calc >> 20) != 0){
             ketaoti = 0b00110;
        }
        else if((calc >> 19) != 0){
             ketaoti = 0b00111;
        }
        else if((calc >> 18) != 0){
             ketaoti = 0b01000;
        }
        else if((calc >> 17) != 0){
             ketaoti = 0b01001;
        }
        else if((calc >> 16) != 0){
             ketaoti = 0b01010;
        }
        else if((calc >> 15) != 0){
             ketaoti = 0b01011;
        }
        else if((calc >> 14) != 0){
             ketaoti = 0b01100;
        }
        else if((calc >> 13) != 0){
             ketaoti = 0b01101;
        }
        else if((calc >> 12) != 0){
             ketaoti = 0b01110;
        }
        else if((calc >> 11) != 0){
             ketaoti = 0b01111;
        }
        else if((calc >> 10) != 0){
             ketaoti = 0b10000;
        }
        else if((calc >> 9) != 0){
             ketaoti = 0b10001;
        }
        else if((calc >> 8) != 0){
             ketaoti = 0b10010;
        }
        else if((calc >> 7) != 0){
             ketaoti = 0b10011;
        }
        else if((calc >> 6) != 0){
             ketaoti = 0b10100;
        }
        else if((calc >> 5) != 0){
             ketaoti = 0b10101;
        }
        else if((calc >> 4) != 0){
             ketaoti = 0b10110;
        }
        else if((calc >> 3) != 0){
             ketaoti = 0b10111;
        }
        else if((calc >> 2) != 0){
             ketaoti = 0b11000;
        }
        else if((calc >> 1) != 0){
             ketaoti = 0b11001;
        }
        else if((calc >> 0) != 0){
             ketaoti = 0b11010;
        }
        else{
             ketaoti = 0b11111;
        }

        uint32_t zero = (ketaoti == 0b11111); //1bit

        uint32_t my = calc << ketaoti; //27bit

        uint32_t ey = es - ketaoti + 1; //9bit

        uint32_t eya; //8bit
        if(bit_range(ey, 9, 9))
            eya = 0;
        else
            eya = bit_range(ey, 8, 1);

        uint32_t y;

        if(inonzero){
            if(zero){
                y = (ssr << 31);
            }
            else{
                y = (ssr << 31) + (eya << 23) + bit_range(my, 26, 4) + bit_range(my, 3, 3);
            }
        }
        else{
            y = (ssr << 31) + (es << 23) + bit_range(msr, 23, 1);
        }

        printf("fsub %x %x %x\n", x1, x2, y);
        return y;

    }

    static uint32_t fmul(uint32_t x1, uint32_t x2)
    {
        uint32_t s1 = bit_range(x1, 32, 32); //1bit
        uint32_t s2 = bit_range(x2, 32, 32); //1bit
        uint32_t e1 = bit_range(x1, 31, 24); //8bit
        uint32_t e2 = bit_range(x2, 31, 24); //8bit
        uint32_t m1 = bit_range(x1, 23, 1); //23bit
        uint32_t m2 = bit_range(x2, 23, 1); //23bit

        uint64_t calc; //48bit
        calc = (uint64_t)((1 << 23) + m1) * (uint64_t)((1 << 23) + m2);

        uint32_t sy = s1 ^ s2; //1bit
        uint32_t esum = bit_range(e1 + e2 - 0b1111111, 9, 1); //9bit
        uint32_t calcr = bit_range64(calc, 48, 23); //26bit
        uint32_t nz = ((e1 != 0) & (e2 != 0)); //1bit

        uint32_t mketa; //24bit
        uint32_t my; //23bit

        if(bit_range(calcr, 26, 26))
            mketa = bit_range(calcr, 25, 2);
        else
            mketa = bit_range(calcr, 24, 1);

        my = bit_range(bit_range(mketa, 24, 2) + bit_range(mketa, 1, 1), 23, 1);

        uint32_t ey = esum;
        if((bit_range(calcr, 26, 26) != 0) | (bit_range(calcr, 24, 1)  == 0b111111111111111111111111))
            ey++;

        uint32_t y;
        if(nz)
            y = (sy << 31) + (bit_range(ey, 8, 1) << 23) + my;
        else
            y = (sy << 31);

        printf("fmul %x %x %x\n", x1, x2, y);
        return y;

    }

    static uint32_t finv(uint32_t x)
    {
        uint32_t s = bit_range(x, 32, 32); //1bit
        uint32_t e = bit_range(x, 31, 24); //8bit
        uint32_t index = bit_range(x, 23, 14); //10bit
        uint32_t a = bit_range(x, 13, 1); //13bit

        uint32_t c = fpu_inv_c[index]; //23bit
        uint32_t g = fpu_inv_g[index]; //13bit

        uint32_t ey; //8bit
        if(index != 0 || a != 0){
            ey = bit_reverse(e+2,8);
        }else{
            ey = bit_reverse(e+1,8);
        }

        uint64_t calc = (1UL << 35) + ((uint64_t)c << 12) - (uint64_t)(g*a); //36bit

        uint32_t y = (s << 31) + (ey << 23) + bit_range64(calc,35,13);

        return y;

    }

    static uint32_t fdiv(uint32_t x1, uint32_t x2)
    {
        uint32_t y = fmul(x1, finv(x2));
        printf("fdiv %x %x %x\n", x1, x2, y);
        return y;
    }

    static uint32_t fsqrt(uint32_t x)
    {
        //TODO fsqrt -> fsqrt2
        uint32_t s = bit_range(x, 32, 32);                     //1bit
        uint32_t e = bit_range(x, 31, 25);                     //7bit
        uint32_t index = bit_range(x, 24, 15);                 //10bit
        uint32_t a = bit_range(x, 14, 1);                      //14bit
        uint32_t d = bit_reverse(bit_range(index, 10, 10), 1); //1bit

        uint32_t c = fpu_sqrt_c[index];
        uint32_t g = fpu_sqrt_g[index];

        uint32_t e1 = e - d;                                                                                         //7bit
        uint32_t ey = (bit_range(e1, 7, 7) << 7) + (bit_reverse(bit_range(e1, 7, 7), 1) << 6) + bit_range(e1, 6, 1); //8bit

        uint64_t calc; //38bit
        if (d)
        {
            calc = ((uint64_t)c << 15) + (uint64_t)((1 << 13) + g) * a * 2;
        }
        else
        {
            calc = ((uint64_t)c << 15) + (uint64_t)((1 << 13) + g) * a;
        }

        uint32_t y = (s << 31) + (ey << 23) + bit_range64(calc, 38, 16) + bit_range64(calc, 15, 15);

        return y;
    }

    static uint32_t feq(float x, float y)
    {
        if (x == y)
            return 1;
        else
            return 0;
    }
    static uint32_t flt(float x, float y)
    {
        if (x < y)
            return 1;
        else
            return 0;
    }
    static uint32_t fle(float x, float y)
    {
        if (x <= y)
            return 1;
        else
            return 0;
    }
    static uint32_t float2int(float x)
    {
        int y = int(x);
        return y;
    }
    static float int2float(int x)
    {
        float y = float(x);
        return y;
    }
};
