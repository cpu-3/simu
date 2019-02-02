#include <iostream>
#include <bitset>
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
uint32_t bit_reverse(uint32_t val, int bit_size)
{
    val = ~val;
    return bit_range(val, bit_size, 1);
}

void char_uint_cpy(uint32_t *c, char *x, int bit_size)
{
    int i = 0;
    while(x[i] != '\0'){
        if (x[i] == '1'){
            *c |= 1 << (bit_size - 1 - i);
        }
        i++;
    }
}
static uint32_t itof(uint32_t x)
{
    using namespace std;
    uint32_t s = bit_range(x, 32, 32); //1bit
    uint32_t m = bit_range(x, 31, 1); //31bit
    uint32_t mabs; //31bit
    if(s)
        mabs = bit_reverse(m, 31) + 1;
    else
        mabs = m;

    bool nonzero = s || (m != 0);

    uint32_t my; //31bit
    if((mabs >> 30) != 0){
        my = (0b10011101 << 23) + bit_range(mabs, 30, 8);
    }
    else if((mabs >> 29) != 0){
        my = (0b10011100 << 23) + bit_range(mabs, 29, 7);
    }
    else if((mabs >> 28) != 0){
        my = (0b10011011 << 23) + bit_range(mabs, 28, 6);
    }
    //TODO

    
        

    uint32_t y;

    if(nonzero)
        y = (s << 31) + my;
    else
        y = 0;

    return y;
}

int main(){
    float_int data;

    float_int result;
    uint32_t seikai;
    srand((unsigned) time(NULL));
    
    for(int j = 0; j < 50; j++){

        data.i = rand() / (RAND_MAX)) * 100000.0;
        result.i = itof(data.i);
        seikai = data.i;

        printf("data:%f\n", data.f);
        printf("itof結果:\t%d\n", result.i);
        printf("理論値:\t%d\n", seikai);

        //誤差が生じた場合に出力
        if(result.i-seikai != 0){
            printf("残念！\ndata:%f\n", data.f);
        }
    }

    return 0;
}
