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

    static float fmul(float x, float y)
    {
        return x * y;
    }
    static float fdiv(float x, float y)
    {
        return x / y;
    }
    static float fsqrt(float x)
    {
        return std::sqrt(x);
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

