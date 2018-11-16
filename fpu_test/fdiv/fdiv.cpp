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

uint32_t map(uint32_t mh){
    switch(mh){
        case 0b00000: return 0b01111110;
        case 0b00001: return 0b01111010;
        case 0b00010: return 0b01110111;
        case 0b00011: return 0b01110011;
        case 0b00100: return 0b01110000;
        case 0b00101: return 0b01101101;
        case 0b00110: return 0b01101010;
        case 0b00111: return 0b01101000;
        case 0b01000: return 0b01100101;
        case 0b01001: return 0b01100011;
        case 0b01010: return 0b01100000;
        case 0b01011: return 0b01011110;
        case 0b01100: return 0b01011100;
        case 0b01101: return 0b01011010;
        case 0b01110: return 0b01011000;
        case 0b01111: return 0b01010110;
        case 0b10000: return 0b01010100;
        case 0b10001: return 0b01010011;
        case 0b10010: return 0b01010001;
        case 0b10011: return 0b01010000;
        case 0b10100: return 0b01001110;
        case 0b10101: return 0b01001101;
        case 0b10110: return 0b01001011;
        case 0b10111: return 0b01001010;
        case 0b11000: return 0b01001000;
        case 0b11001: return 0b01000111;
        case 0b11010: return 0b01000110;
        case 0b11011: return 0b01000101;
        case 0b11100: return 0b01000100;
        case 0b11101: return 0b01000011;
        case 0b11110: return 0b01000010;
        case 0b11111: return 0b01000001;
        default : return 0b0;
    }
}

static uint32_t finv(uint32_t x)
{
    using namespace std;
    uint32_t s = bit_range(x, 32, 32); //1bit
    uint32_t e = bit_range(x, 31, 24); //8bit
    uint32_t m = bit_range(x, 23, 1); //23bit

    uint32_t ma = (1 << 23) + m; //24bit
    uint32_t init = map(bit_range(m,23,19)); //8bit

    uint64_t calc1 = ((uint64_t)init << 31) - (uint64_t)init*(uint64_t)init*(uint64_t)ma; //39bit

    uint32_t x1 = bit_range64(calc1,38,27) + bit_range64(calc1,26,26); //12bit

    uint64_t calc2 = ((uint64_t)x1 << 35) - (uint64_t)x1*(uint64_t)x1*(uint64_t)ma; //47bit

    
    uint32_t ey; //8bit
    if(bit_range64(calc2,46,46)){
        ey = bit_reverse(e,8) - 1;
    }else{
        ey = bit_reverse(e,8) - 2;
    }

    uint32_t my; //23bit
    if(bit_range64(calc2,46,46)){
        my = bit_range64(calc2,45,23) + bit_range64(calc2,22,22);
    }else{
        my = bit_range64(calc2,44,22) + bit_range64(calc2,21,21);
    }
    
    uint32_t y = (s << 31) + (ey << 23) + my;

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

    uint32_t sy = bit_range(s1^s2,1,1); //1bit
    
    uint32_t m1a = (1 << 23) + m1; //24bit
    uint32_t m2a = (1 << 23) + m2; //24bit

    uint64_t mmul; //48bit
    uint64_t mketa; //47bit
    uint32_t my; //23bit
    mmul = (uint64_t)m1a*(uint64_t)m2a;

    if(bit_range64(mmul,48,48)){
        mketa = bit_range64(mmul,47,1);
    }else{
        mketa = bit_range64(mmul,46,1) << 1;
    }
 
    if((bit_range(mketa,23,1) == 0) && bit_range(mketa,24,24)){
        my = bit_range64(mketa,47,25)+bit_range(mketa,25,25);
    }else{
        my = bit_range64(mketa,47,25)+bit_range(mketa,24,24);
    }

    uint32_t eadd; //9bit 
    uint32_t eexp; //9bit 
    uint32_t ey; //8bit 
    if(bit_range64(mketa,47,24) == 0xFFFFFF){
        eadd = bit_range(e1+e2+bit_range64(mmul,48,48)+1,9,1);
    }else{
        eadd = bit_range(e1+e2+bit_range64(mmul,48,48),9,1);
    }
    if(bit_range(eadd,9,9) & bit_range(eadd,8,8)){
        eexp = 0b111111111;
    }else if(bit_range(eadd,9,9) | bit_range(eadd,8,8)){
        eexp = eadd - 0b001111111;
    }else{
        eexp = 0;
    }
    ey = bit_range(eexp,8,1);

    uint32_t ovf = bit_range(eadd,8,8) & bit_range(eadd,7,7);

    uint32_t y;
    if(e1 != 0 || e2 != 0 || ey != 0){
        y = (sy << 31) + (ey << 23) + bit_range(my,23,1);
    }else{
        y = sy << 31;
    }
    return y;
}

static uint32_t fdiv(uint32_t x1, uint32_t x2){
    return fmul(x1, finv(x2));
}

int main(){
    float_int data1;
    float_int data2;

    float_int result;
    float_int seikai;
    srand((unsigned) time(NULL));
    /*
    float_int data;
    for(int j = 0; j < 10; j++){
        data.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        result.i = finv(data.i);
        seikai.f = 1/data.f;
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fdiv結果:\t%f\n", result.f);
        printf("理論値:\t%f\n", seikai.f);
        誤差が生じた場合に出力
        if(result.f-seikai.f != 0){
            printf("残念！\n");
            std::cout << std::bitset<32>(data.i) << std::endl;
            printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printb(result.i);
            printf("\n");
            printb(seikai.i);
            printf("\n");
        }
    }
    */
    
    for(int j = 0; j < 100; j++){

        data1.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        data2.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        result.i = fdiv(data1.i, data2.i);
        seikai.f = data1.f / data2.f;
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fdiv結果:\t%f\n", result.f);
        printf("div理論値:\t%f\n", seikai.f);
        //誤差が生じた場合に出力
        if(result.f-seikai.f != 0){
            //printf("残念！\ndata1:%f\ndata2:%f\n", data1.f, data2.f);
            //printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printf("result:");
            std::cout << std::bitset<32>(result.i) << std::endl;
            printf("\nseikai:");
            std::cout << std::bitset<32>(seikai.i) << std::endl;
            printf("\n");
        }
    }
    
    return 0;
}