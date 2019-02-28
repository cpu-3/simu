class ALU
{
  public:
    static uint32_t add(uint32_t x, uint32_t y)
    {
        return x + y;
    }
    static uint32_t sub(uint32_t x, uint32_t y)
    {
        return x - y;
    }
    static uint32_t sll(uint32_t x, uint32_t y)
    {
        return x << (y & 0b11111);
    }
    static uint32_t srl(uint32_t x, uint32_t y)
    {
        return x >> (y & 0b11111);
    }
    static uint32_t sra(uint32_t x, uint32_t y)
    {
        int32_t a = (int32_t)x;
        return (uint32_t)(x >> (y & 0b11111));
    }
    static uint32_t slt(uint32_t x, uint32_t y)
    {
        int32_t a = (int32_t)x;
        int32_t b = (int32_t)y;
        return a > b;
    }
    static uint32_t sltu(uint32_t x, uint32_t y)
    {
        return x > y;
    }
    static uint32_t and_(uint32_t x, uint32_t y)
    {
        return x & y;
    }
    static uint32_t or_(uint32_t x, uint32_t y)
    {
        return x | y;
    }
    static uint32_t xor_(uint32_t x, uint32_t y)
    {
        return x ^ y;
    }
};

