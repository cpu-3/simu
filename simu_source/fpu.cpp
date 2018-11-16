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
    while(x[i] != '\0'){
        if (x[i] == '1'){
            *c |= 1 << (bit_size - 1 - i);
        }
        i++;
    }
}
uint32_t map(uint32_t mh)
{
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

        uint32_t m1a, m2a; //25bit
        uint32_t e1a, e2a; //8bit
        if (e1 == 0){
            m1a = m1;
            e1a = 1;
        }else{
            m1a = (1 << 23) + m1;
            e1a = e1;
        }
        if (e2 == 0){
            m2a = m2;
            e2a = 1;
        }else{
            m2a = (1 << 23) + m2;
            e2a = e2;
        }

        uint32_t te = e1a + bit_reverse(e2a, 8); //9bit
        uint32_t ce = bit_range(~(bit_range(te,9,9)),1,1); //1bit
        uint32_t tei = 1 + te; //9bit
        uint32_t tde; //8bit
        uint32_t de; //5bit
        uint32_t sel; //1bit
        if(ce){
            tde = bit_reverse(bit_range(te,8,1), 8);
        }else{
            tde = bit_range(tei,8,1);
        }
        if (bit_range(tde,8,6) != 0){
            de = 31;
        }else{
            de = bit_range(tde,5,1);
        }
        if(de == 0){
            sel = (m1a <= m2a);
        }
        else{
            sel = ce;
        }

        uint32_t ms, mi; //25bit
        uint32_t es, ei; //8bit
        uint32_t ss; //1bit
        if(sel){
            ms = m2a;
            mi = m1a;
            es = e2a;
            ei = e1a;
            ss = s2;
        }else{
            ms = m1a;
            mi = m2a;
            es = e1a;
            ei = e2a;
            ss = s1;
        }
     
        uint64_t mie = (uint64_t)mi << 31; //56bit
        uint64_t mia = mie >> de; //56bit

        uint32_t tstck = bit_range(mia,29,1) != 0; //1bit

        uint32_t mye; //27bit
        if(s1 == s2){
            mye = bit_range((ms << 2) + (uint32_t)bit_range64(mia,56,30),27,1);
        }else{
            mye = bit_range((ms << 2) - (uint32_t)bit_range64(mia,56,30),27,1);
        }

        uint32_t esi = bit_range(es+1,8,1); //8bit
        uint32_t ovf1 = (bit_range(mye,27,27)) && (esi == 255); //1bit

        uint32_t eyd; //8bit
        uint32_t myd; //27bit
        uint32_t stck; //1bit
        if(bit_range(mye,27,27)){
            esi == 255 ? eyd = 255 : eyd = esi;
            esi == 255 ? myd = 1 << 25 : myd = mye >> 1;
            stck = tstck | bit_range(mye,1,1);
        }else{
            eyd = es;
            myd = mye;
            stck = tstck;
        }

        uint32_t se = 26; //5bit
        for (int i = 0; i < 26; i++){
            if(bit_range(myd,26-i,26-i)){
                se = i;
                break;
            }
        }
        
        int32_t eyf = (int32_t)eyd - (int32_t)se; //9bit
        uint32_t eyr; //8bit
        uint32_t myf; //27bit
        if(eyf > 0){
            eyr = bit_range(eyf,8,1);
            myf = bit_range(myd << se,27,1);
        }else{
            eyr = 0;
            myf = bit_range(myd << (bit_range(eyd,5,1)-1), 27,1);
        }

        uint32_t myr; //27bit
        if((bit_range(myf,3,1) == 0b110 && stck == 0) ||
        (bit_range(myf,2,1) == 0b10 && s1 == s2 && stck == 1) ||
        (bit_range(myf,2,1) == 0b11)){
            myr = bit_range(myf,27,3) + 1;
        }else{
            myr = bit_range(myf,27,3);
        }

        uint32_t eyri = bit_range(eyr + 1,8,1); //8bit
        uint32_t ey; //8bit
        uint32_t my; //23bit
        uint32_t ovf2; //1bit
        if(bit_range(myr,25,25)){
            ey = eyri;
            my = 0;
        }else if(bit_range(myr,24,1) != 0){
            ey = eyr;
            my = bit_range(myr,23,1);
        }else{
            ey = 0;
            my = 0;
        }

        if(e1 != 255 && e2 != 255 && bit_range(myr,25,25) && eyri == 255){
            ovf2 = 1;
        }else{
            ovf2 = 0;
        }

        uint32_t sy; //1bit
        (ey == 0 && my == 0) ? sy = (s1 & s2) : sy = ss;

        uint32_t nzm1 = m1 != 0; //1bit
        uint32_t nzm2 = m2 != 0; //1bit

        uint32_t y;
        if(e1 == 255 && e2 != 255){
            y = (s1 << 31) + (255 << 23) + (nzm1 << 22) + bit_range(m1,22,1);
        }else if(e2 == 255 && e1 != 255){
            y = (s2 << 31) + (255 << 23) + (nzm2 << 22) + bit_range(m2,22,1);
        }else if(e1 == 255 && e2 == 255 && nzm2){
            y = (s2 << 31) + (255 << 23) + (1 << 22) + bit_range(m2,22,1);
        }else if(e1 == 255 && e2 == 255 && nzm1){
            y = (s1 << 31) + (255 << 23) + (1 << 22) + bit_range(m1,22,1);
        }else if(e1 == 255 && e2 == 255 && s1 == s2){
            y = (s1 << 31) + (255 << 23);
        }else if(e1 == 255 && e2 == 255){
            y = (1 << 31) + (255 << 23) + (1 << 22);
        }else{
            y = (sy << 31) + (ey << 23) + bit_range(my,23,1);
        }
        uint32_t ovf = ovf1 | ovf2;
        
        return y;
    }

    static uint32_t fsub(uint32_t x1, uint32_t x2){
        uint32_t s2 = bit_range(~x2, 32, 32); //1bit
        uint32_t x2s = (s2 << 31) + bit_range(x2,31,1);
        return (fadd(x1, x2s));
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

    static uint32_t fdiv(uint32_t x1, uint32_t x2){
        return fmul(x1, finv(x2));
    }

    static uint32_t fsqrt(uint32_t x)
    {
        using namespace std;
        uint32_t s = bit_range(x, 32, 32); //1bit
        uint32_t e = bit_range(x, 31, 25); //7bit
        uint32_t index = bit_range(x, 24, 15); //10bit
        uint32_t a = bit_range(x, 14, 1); //14bit
        uint32_t d = bit_reverse(bit_range(index,10,10),1); //1bit

        char outfile[] = "simu_source/sqrt_v4.bin";
        
        ifstream ifs(outfile);
         if (!ifs){
             cout << "ファイルが開けません(sqrt)" << endl;
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

        uint32_t e1 = e - d; //7bit
        uint32_t ey = (bit_range(e1,7,7) << 7) + (bit_reverse(bit_range(e1,7,7),1) << 6) + bit_range(e1,6,1); //8bit

        uint64_t calc; //38bit
        if(d){
            calc = ((uint64_t)c << 15) + (uint64_t)((1 << 13) + g)*a*2;
        }else{
            calc = ((uint64_t)c << 15) + (uint64_t)((1 << 13) + g)*a;
        }

        uint32_t y = (s << 31) + (ey << 23) + bit_range64(calc,38,16) + bit_range64(calc,15,15);

        return y;
    }

    static uint32_t feq(float x, float y)
    {
        if (x == y) return 1;
        else return 0;
    }
    static uint32_t flt(float x, float y)
    {
        if (x < y) return 1;
        else return 0;
    }
    static uint32_t fle(float x, float y)
    {
        if (x <= y) return 1;
        else return 0;
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

