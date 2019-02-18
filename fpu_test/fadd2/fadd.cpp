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
        data1.f = ((float)rand() / (float)(RAND_MAX));
        data2.f = ((float)rand() / (float)(RAND_MAX));
        result.i = fadd(data1.i, data2.i);
        std::bitset<32> d1(data1.i);
        std::bitset<32> d2(data2.i);
        std::bitset<32> r(result.i);
        ofs << d1 << " " << d2 << " " << r << std::endl;
/*
        seikai.f = data1.f + data2.f;
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fadd結果:\t%f\n", result.f);
        printf("理論値:\t%f\n", seikai.f);
        if(fabs(result.f-seikai.f) > 0.0001){
            printf("残念！\ndata1:%f\ndata2:%f\n", data1.f, data2.f);
            printf("data1 bin: ");
            printb(data1.i);
            printf("\n");
            printf("data2 bin: ");
            printb(data2.i);
            printf("\n");
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