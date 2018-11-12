#include <iostream>
#include <stdarg.h>
#include <cmath>
#include <fstream>
#include <climits>

typedef union {
    uint32_t i;
    float f;
} float_int;

// print bitraw
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

static uint32_t fsqrt(uint32_t x)
{
    //TODO implement fdiv
}

int main(){
    float_int data;

    float_int result;
    float_int seikai;
    srand((unsigned) time(NULL));
    
    for(int j = 0; j < 1000; j++){
        data.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        result.i = fsqrt(data.i);
        seikai.f = sqrt(data.f);
/*
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fsqrt結果:\t%f\n", result.f);
        printf("理論値:\t%f\n", seikai.f);
*/
        //誤差が生じた場合に出力
        if(result.f-seikai.f != 0){
            printf("残念！\ndata:%f\n", data.f);
            printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printb(result.i);
            printf("\n");
            printb(seikai.i);
            printf("\n");
        }
    }

    return 0;
}
