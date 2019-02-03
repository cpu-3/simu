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
    else if((mabs >> 27) != 0){
        my = (0b10011010 << 23) + bit_range(mabs, 27, 5);
    }
    else if((mabs >> 26) != 0){
        my = (0b10011001 << 23) + bit_range(mabs, 26, 4);
    }
    else if((mabs >> 25) != 0){
        my = (0b10011000 << 23) + bit_range(mabs, 25, 3);
    }
    else if((mabs >> 24) != 0){
        my = (0b10010111 << 23) + bit_range(mabs, 24, 2);
    }
    else if((mabs >> 23) != 0){
        my = (0b10010110 << 23) + bit_range(mabs, 23, 1);
    }
    else if((mabs >> 22) != 0){
        my = (0b10010101 << 23) + (bit_range(mabs, 22, 1) << 1);
    }
    else if((mabs >> 21) != 0){
        my = (0b10010100 << 23) + (bit_range(mabs, 21, 1) << 2);
    }
    else if((mabs >> 20) != 0){
        my = (0b10010011 << 23) + (bit_range(mabs, 20, 1) << 3);
    }
    else if((mabs >> 19) != 0){
        my = (0b10010010 << 23) + (bit_range(mabs, 19, 1) << 4);
    }
    else if((mabs >> 18) != 0){
        my = (0b10010001 << 23) + (bit_range(mabs, 18, 1) << 5);
    }
    else if((mabs >> 17) != 0){
        my = (0b10010000 << 23) + (bit_range(mabs, 17, 1) << 6);
    }
    else if((mabs >> 16) != 0){
        my = (0b10001111 << 23) + (bit_range(mabs, 16, 1) << 7);
    }
    else if((mabs >> 15) != 0){
        my = (0b10001110 << 23) + (bit_range(mabs, 15, 1) << 8);
    }
    else if((mabs >> 14) != 0){
        my = (0b10001101 << 23) + (bit_range(mabs, 14, 1) << 9);
    }
    else if((mabs >> 13) != 0){
        my = (0b10001100 << 23) + (bit_range(mabs, 13, 1) << 10);
    }
    else if((mabs >> 12) != 0){
        my = (0b10001011 << 23) + (bit_range(mabs, 12, 1) << 11);
    }
    else if((mabs >> 11) != 0){
        my = (0b10001010 << 23) + (bit_range(mabs, 11, 1) << 12);
    }
    else if((mabs >> 10) != 0){
        my = (0b10001001 << 23) + (bit_range(mabs, 10, 1) << 13);
    }
    else if((mabs >> 9) != 0){
        my = (0b10001000 << 23) + (bit_range(mabs, 9, 1) << 14);
    }
    else if((mabs >> 8) != 0){
        my = (0b10000111 << 23) + (bit_range(mabs, 8, 1) << 15);
    }
    else if((mabs >> 7) != 0){
        my = (0b10000110 << 23) + (bit_range(mabs, 7, 1) << 16);
    }
    else if((mabs >> 6) != 0){
        my = (0b10000101 << 23) + (bit_range(mabs, 6, 1) << 17);
    }
    else if((mabs >> 5) != 0){
        my = (0b10000100 << 23) + (bit_range(mabs, 5, 1) << 18);
    }
    else if((mabs >> 4) != 0){
        my = (0b10000011 << 23) + (bit_range(mabs, 4, 1) << 19);
    }
    else if((mabs >> 3) != 0){
        my = (0b10000010 << 23) + (bit_range(mabs, 3, 1) << 20);
    }
    else if((mabs >> 2) != 0){
        my = (0b10000001 << 23) + (bit_range(mabs, 2, 1) << 21);
    }
    else if((mabs >> 1) != 0){
        my = (0b10000000 << 23) + (bit_range(mabs, 1, 1) << 22);
    }
    else if((mabs >> 0) != 0){
        my = 0b01111111 << 23;
    }
    else{
        my = 0b10011110 << 23;
    }

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
    
    for(int j = 0; j < 1000; j++){

        data.i = (int)rand() / 10000;
        result.i = itof(data.i);

        printf("data:\t%d\n", data.i);
        printf("itof:\t%f\n", result.f);
        
        /*
        //誤差が生じた場合に出力
        if(fabs(result.f-data.i) > 0.1){
            printf("\n残念！\ndata:%d", data.i);
            printf("\nitof:%f", result.f);
        }
        */
    }

    return 0;
}
