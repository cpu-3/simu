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
static uint32_t finv(uint32_t x)
{
    using namespace std;
    uint32_t s = bit_range(x, 32, 32); //1bit
    uint32_t e = bit_range(x, 31, 24); //8bit
    uint32_t index = bit_range(x, 23, 14); //10bit
    uint32_t a = bit_range(x, 13, 1); //13bit

    char outfile[] = "inv_v4.bin";
    int k;
    ifstream ifs(outfile);
     if (!ifs){
         cout << "ファイルが開けません";
        exit(-1);
    }

    char *c_char;
    char *g_char;
    c_char = (char *)malloc(23*sizeof(char));
    g_char = (char *)malloc(13*sizeof(char));
    ifs.seekg ( 38*index*sizeof (char) );
    ifs.read ( c_char, 23*sizeof(char) );
    ifs.seekg ( 38*index*sizeof(char) + 24*sizeof(char) );
    ifs.read ( g_char, 13*sizeof(char) );
    
    uint32_t c = 0; //23bit
    uint32_t g = 0; //13bit
    char_uint_cpy(&c,c_char,23);
    char_uint_cpy(&g,g_char,13); 
        
    ifs.close();
    free(c_char);
    free(g_char);

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
        //誤差が生じた場合に出力
        if(result.f-seikai.f != 0){
            printf("残念！\ndata1:%f\ndata2:%f\n", data1.f, data2.f);
            printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printb(result.i);
            printf("\n");
            printb(seikai.i);
            printf("\n");
        }
    }
*/
    for(int j = 0; j < 1000; j++){

        data1.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        data2.f = ((float)rand() / (float)(RAND_MAX)) * 100000.0;
        result.i = fdiv(data1.i, data2.i);
        seikai.f = data1.f / data2.f;
        printf("data1:%f data2:%f\n", data1.f, data2.f);
        printf("fdiv結果:\t%f\n", result.f);
        printf("理論値:\t%f\n", seikai.f);
        //誤差が生じた場合に出力
        if(result.f-seikai.f != 0){
            printf("残念！\ndata1:%f\ndata2:%f\n", data1.f, data2.f);
            printf("result:%f\nseikai:%f\n", result.f, seikai.f);
            printb(result.i);
            printf("\n");
            printb(seikai.i);
            printf("\n");
        }
    }

    return 0;
}
