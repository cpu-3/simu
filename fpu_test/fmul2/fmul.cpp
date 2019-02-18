#include <iostream>
#include <stdarg.h>
#include <cmath>
#include <fstream>
#include <climits>
#include <bitset>

typedef union {
    uint32_t i;
    float f;
} float_int;

void printb(unsigned int v) {
  unsigned int mask = (int)1 << (sizeof(v) * CHAR_BIT - 1);
  do putchar(mask & v ? '1' : '0');
  while (mask >>= 1);
}

// get val's [l, r] bit value
// ex) bit_range(00010011, 7, 1) -> 0010011
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

    return y;
}

int main(){
    float_int data1;
    float_int data2;

    float_int result;
    float_int seikai;
    srand((unsigned) time(NULL));

    std::ofstream ofs("result.txt");

    for(int j = 0; j < 100000; j++){
        data1.f = ((float)rand() / (float)(RAND_MAX)) * 1000.0;
        data2.f = ((float)rand() / (float)(RAND_MAX)) * 1000.0;
        result.i = fmul(data1.i, data2.i);
        std::bitset<32> d1(data1.i);
        std::bitset<32> d2(data2.i);
        std::bitset<32> r(result.i);
        ofs << d1 << " " << d2 << " " << r << std::endl;

/*
        seikai.f = data1.f * data2.f;
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fmul結果:\t%f\n", result.f);
        printf("理論値:\t%f\n", seikai.f);
        if(fabs(result.f-seikai.f) > 0.1){
            printf("残念！\ndata1:%f\ndata2:%f\n", data1.f, data2.f);
            printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printb(result.i);
            printf("\n");
            printb(seikai.i);
            printf("\n");
        }
*/
    }

    return 0;
}
