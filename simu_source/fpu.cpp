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

    static float fadd(float x, float y)
    {
        return x + y;
    }
    static float fsub(float x, float y)
    {
        return x - y;
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

