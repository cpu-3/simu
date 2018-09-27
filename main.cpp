#include <iostream>
#include <stdarg.h>
#include <cmath>
#include "inst.hpp"

void error_dump(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    // ここに Dump情報詳細を追加する

    exit(-1);
}

void warn_dump(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

class Memory {
    /* Current Memory Map 
    0x0      --------
               Inst
    0x7ffff  --------
               Data
    0x3fffff --------
    */
    static const uint32_t memory_size = 0xf4240;
    static const uint32_t inst_mem_lim = 0xffff;
    static const uint32_t memory_base = 0;
    static const uint32_t memory_lim = memory_base + memory_size;
    uint8_t memory[memory_size];

    void data_mem_check(uint32_t addr, uint8_t size)
    {
        if (addr % 4 != 0) {
            error_dump("メモリアドレスのアラインメントがおかしいです: %x", addr);
        }
        if (addr + size >= memory_lim || addr + size <= inst_mem_lim)
        {
            error_dump("多分不正なアドレスに書き込もうとしました: %x", addr);
        }
    }

    void inst_mem_check(uint32_t addr) {
        if (addr % 4 != 0) {
            error_dump("メモリアドレスのアラインメントがおかしいです: %x", addr);
        }
        if (addr + 4 > inst_mem_lim) {
            error_dump("多分不正なアドレスに書き込もうとしました: %x", addr);
        }
    }

    void write_mem(uint32_t addr, uint8_t val)
    {
        data_mem_check(addr, 1);
        memory[addr] = val;
    }

    void write_mem(uint32_t addr, uint16_t val)
    {
        data_mem_check(addr, 2);
        uint16_t *m = (uint16_t *)memory;
        m[addr] = val;
    }

    void write_mem(uint32_t addr, uint32_t val)
    {
        data_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        m[addr] = val;
    }

    uint8_t read_mem_1(uint32_t addr)
    {
        data_mem_check(addr, 1);
        return memory[addr];
    }

    uint16_t read_mem_2(uint32_t addr)
    {
        data_mem_check(addr, 2);
        uint16_t *m = (uint16_t *)memory;
        return m[addr];
    }

    uint32_t read_mem_4(uint32_t addr)
    {
        data_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        return m[addr];
    }

    uint32_t get_inst(uint32_t addr) {
        inst_mem_check(addr);
        uint32_t *m = (uint32_t *)memory;
        return m[addr];
    }
};

class ALU {
    static uint32_t add(uint32_t x, uint32_t y) {
        return x + y;
    }
};

class FPU {
    static float add(float x, float y) {
        return x + y;
    }
    static float sub(float x, float y) {
        return x - y;
    }
    static float mul(float x, float y) {
        return x * y;
    }
    static float div(float x, float y) {
        return x / y;
    }
    static float sqrt(float x) {
        return std::sqrt(x);
    }
};

class Register {
    static const int ireg_size = 32;
    static const int freg_size = 32;
    uint32_t i_registers[ireg_size] = {0};
    float f_registers[freg_size] = {0.0f};

    static void check_ireg_name(int name, int write) {
        if (name < 0 || name > ireg_size) {
            error_dump("レジスタの番号が不正です: %d", name);
        }

        if (write && name == 0) {
            warn_dump("レジスタ0に書き込もうとしていますが");
        }
    }
    static void check_freg_name(int name) {
        if (name < 0 || name > freg_size) {
            error_dump("レジスタの番号が不正です: %d", name);
        }
    }

    public:
    uint32_t ip;
    Register() : ip(0){}
    void set_ireg(int name, uint32_t val) {
        check_ireg_name(name, 1);

        if (name == 0) {
            return;
        }
        i_registers[name] = val;
    }
    void set_freg(int name, float val) {
        check_freg_name(name);

        f_registers[name] = val;
    }
    uint32_t get_ireg(int name) {
        check_ireg_name(name, 0);
        if (name == 0) {
            return 0;
        }
        return i_registers[name];
    }
};


class Decoder {
    // get val's [l, r) bit value
    uint32_t bit_range(uint8_t val, uint8_t l, uint8_t r) {
        static const int masks[] = {
            ~(1 << 0),
            ~(1 << 1),
            ~(1 << 2),
            ~(1 << 3),
            ~(1 << 4),
            ~(1 << 5),
            ~(1 << 6),
            ~(1 << 7),
            ~(1 << 8),
            ~(1 << 9),
            ~(1 << 10),
            ~(1 << 11),
            ~(1 << 12),
            ~(1 << 13),
            ~(1 << 14),
            ~(1 << 15),
            ~(1 << 16),
            ~(1 << 17),
            ~(1 << 18),
            ~(1 << 19),
            ~(1 << 20),
            ~(1 << 21),
            ~(1 << 22),
            ~(1 << 23),
            ~(1 << 24),
            ~(1 << 25),
            ~(1 << 26),
            ~(1 << 27),
            ~(1 << 28),
            ~(1 << 29),
            ~(1 << 30),
            ~(1 << 31),
        };

        val &= masks[32 - r];
        return val >> l;
    }
    public:
    uint32_t code;
    Decoder(uint32_t c) {
        code = c;
    }
    uint8_t opcode() {
        return bit_range(code, 0, 7);
    }
    uint8_t rd() {
        return bit_range(code, 7, 12);
    }
    uint8_t rs1() {
        return bit_range(code, 15, 20);
    }
    uint8_t rs2() {
        return bit_range(code, 20, 25);
    }
    uint8_t funct3() {
        return bit_range(code, 12, 15);
    }
    uint16_t funct7() {
        return bit_range(code, 25, 31);
    }
    uint16_t s_type_imm() {
        return (bit_range(code, 25, 32) << 5) | (bit_range(code, 7, 12));
    }
    uint32_t u_type_imm() {
        return bit_range(code, 12, 32);
    }
};

class Core {
    Memory *m;
    Register *r;

    void run(Decoder d) {
        switch (d.opcode) {
            case Inst::ADD:
                break;
            default:
                break;
        }
    }

    public:
    void main_loop() {
        while(1) {
            uint32_t ip = r->ip;
            Decoder d = Decoder(ip);
            run(d);
        }
    }
};

int main(void) {
    return 0;
}