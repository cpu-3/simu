class Decoder
{
    // get val's [l, r] bit value
    // ex) bit_range(00010011, 7, 1) -> 00010011
    uint32_t bit_range(uint32_t val, uint8_t l, uint8_t r)
    {
        val <<= 32 - l;
        val >>= 31 - (l - r);
        return val;
    }

  public:
    uint32_t code;
    Decoder(uint32_t c)
    {
        code = c;
    }
    uint8_t opcode()
    {
        return bit_range(code, 7, 1);
    }
    uint8_t rd()
    {
        return bit_range(code, 12, 8);
    }
    uint8_t rm()
    {
        return bit_range(code, 15, 13);
    }
    uint8_t rs1()
    {
        return bit_range(code, 20, 16);
    }
    uint8_t rs2()
    {
        return bit_range(code, 25, 21);
    }
    uint8_t funct3()
    {
        return bit_range(code, 15, 13);
    }
    uint8_t funct5_fmt()
    {
        return bit_range(code, 32, 26);
    }
    uint16_t funct7()
    {
        return bit_range(code, 32, 26);
    }
    uint32_t s_type_imm()
    {
        int32_t ret = (bit_range(code, 32, 26) << 5) | (bit_range(code, 12, 8));
        ret <<= 20;
        ret >>= 20;
        return ret;
    }
    uint32_t u_type_imm()
    {
        return bit_range(code, 32, 13) << 12;
    }
    uint32_t i_type_imm()
    {
        int32_t ret = bit_range(code, 32, 21);
        ret <<= 20;
        ret >>= 20;
        return ret;
    }
    int32_t b_type_imm()
    {
        int32_t ret = (bit_range(code, 32, 32) << 12) +
                      (bit_range(code, 8, 8) << 11) +
                      (bit_range(code, 31, 26) << 5) +
                      (bit_range(code, 12, 9) << 1);
        ret <<= 20;
        ret >>= 20;
        return ret;
    }
    int32_t jal_imm()
    {
        // sign extended
        int32_t ret = (bit_range(code, 32, 32) << 20) +
                      (bit_range(code, 20, 13) << 12) +
                      (bit_range(code, 21, 21) << 11) +
                      (bit_range(code, 31, 22) << 1);
        ret <<= 11;
        ret >>= 11;
        return ret;
    }
};

