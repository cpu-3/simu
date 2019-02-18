#include <iostream>
#include <fstream>
#include <bitset>
#include <stdarg.h>
#include <cmath>
#include <fstream>
#include <climits>


#include "fpu_const.h"

using namespace std;

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


int main() {

    for (int index = 0; index < 1024; index++) {
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

        uint32_t c2 = fpu_inv_c[index];
        uint32_t g2 = fpu_inv_g[index];
        if (c2 != c || g2 != g) {
            printf("diff: %d\n", index);
            printf("%x != %x\n", c);
        }


        ifs.close();
        free(c_char);
        free(g_char);
    }
    return 0;
}
