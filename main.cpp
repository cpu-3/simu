#include <iostream>
#include <stdarg.h>
#include <cmath>
#include <fstream>
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

    void addr_alignment_check(uint32_t addr) {
        if (addr % 4 != 0) {
            error_dump("メモリアドレスのアラインメントがおかしいです: %x", addr);
        }
    }

    void data_mem_check(uint32_t addr, uint8_t size)
    {
        addr_alignment_check(addr);
        if (addr + size >= memory_lim || addr + size <= inst_mem_lim)
        {
            error_dump("多分不正なアドレスに書き込もうとしました: %x", addr);
        }
    }

    void inst_mem_check(uint32_t addr) {
        addr_alignment_check(addr);
        if (addr + 4 > inst_mem_lim) {
            error_dump("多分不正なアドレスに書き込もうとしました: %x", addr);
        }
    }

    void map_mem_check(uint32_t addr, uint32_t size) {
        if ((addr + size) >= memory_lim) {
            error_dump("多分不正なアドレスに書き込もうとしました: %x", addr);
        }
    }
    public:
    void write_mem(uint32_t addr, uint8_t val)
    {
        data_mem_check(addr, 1);
        memory[addr] = val;
    }

    void write_mem(uint32_t addr, uint16_t val)
    {
        data_mem_check(addr, 2);
        uint16_t *m = (uint16_t *)memory;
        m[addr / 2] = val;
    }

    void write_mem(uint32_t addr, uint32_t val)
    {
        data_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        m[addr / 4] = val;
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
        return m[addr / 2];
    }

    uint32_t read_mem_4(uint32_t addr)
    {
        data_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    uint32_t get_inst(uint32_t addr) {
        inst_mem_check(addr);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    void mmap(uint32_t addr, uint8_t *data, uint32_t length) {
        addr_alignment_check(addr);
        addr_alignment_check(length);
        for (int i = 0; i < length; i++) {
            memory[addr + i] = data[i];
        }
    }
};

class ALU {
    public:
    static uint32_t add(uint32_t x, uint32_t y) {
        return x + y;
    }
    static uint32_t sub(uint32_t x, uint32_t y) {
        return x - y;
    }
    static uint32_t sll(uint32_t x, uint32_t y) {
        return x << (y & 0b11111);
    }
    static uint32_t srl(uint32_t x, uint32_t y) {
        return x >> (y & 0b11111);
    }
    static uint32_t sra(uint32_t x, uint32_t y) {
        int32_t a = (int32_t)x;
        return (uint32_t)(x >> (y & 0b11111));
    }
    static uint32_t slt(uint32_t x, uint32_t y) {
        int32_t a = (int32_t)x;
        int32_t b = (int32_t)y;
        return a > b;
    }
    static uint32_t sltu(uint32_t x, uint32_t y) {
        return x > y;
    }
    static uint32_t and_(uint32_t x, uint32_t y) {
        return x & y;
    }
    static uint32_t or_(uint32_t x, uint32_t y) {
        return x | y;
    }
    static uint32_t xor_(uint32_t x, uint32_t y) {
        return x ^ y;
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
    void info() {
        for (int i = 0; i < ireg_size; i++) {
            std::cout << "x" << i << ": " << i_registers[i] << std::endl;
        }
    }
};

class Decoder {
    // get val's [l, r) bit value
    uint32_t bit_range(uint32_t val, uint8_t l, uint8_t r) {
        static const uint32_t masks[] = {
            1 << 0,
            (1u << 1) - 1,
            (1 << 2) - 1,
            (1 << 3) - 1,
            (1 << 4) - 1,
            (1 << 5) - 1,
            (1 << 6) - 1,
            (1 << 7) - 1,
            (1 << 8) - 1,
            (1 << 9) - 1,
            (1 << 10) - 1,
            (1 << 11) - 1,
            (1 << 12) - 1,
            (1 << 13) - 1,
            (1 << 14) - 1,
            (1 << 15) - 1,
            (1 << 16) - 1,
            (1 << 17) - 1,
            (1 << 18) - 1,
            (1 << 19) - 1,
            (1 << 20) - 1,
            (1 << 21) - 1,
            (1 << 22) - 1,
            (1 << 23) - 1,
            (1 << 24) - 1,
            (1 << 25) - 1,
            (1 << 26) - 1,
            (1 << 27) - 1,
            (1 << 28) - 1,
            (1 << 29) - 1,
            (1 << 30) - 1,
            (1u << 31) - 1,
            (1ull << 32) - 1,
        };

        val >>= (32 - r);
        val &= (masks[r - l]);
        std::cout << val << std::endl;
        return val;
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
    const uint32_t instruction_load_address = 0;
    Memory *m;
    Register *r;

    void add(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::add(x, y));
    }
    void sub(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sub(x, y));
    }
    void sll(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sll(x, y));
    }
    void slt(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::slt(x, y));
    }
    void sltu(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sltu(x, y));
    }
    void xor_(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::xor_(x, y));
    }
    void or_(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::or_(x, y));
    }
    void and_(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::and_(x, y));
    }
    void sra(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sra(x, y));
    }
    void srl(Decoder *d) {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::srl(x, y));
    }

    void sr(Decoder *d) {
        switch (static_cast<ALU_SR_Inst>(d->funct7())) {
            case ALU_SR_Inst::SRA:
                sra(d);
                break;
            case ALU_SR_Inst::SRL:
                srl(d);
                break;
            default:
                error_dump("対応していないfunct7が使用されました: %x", d->funct7());
        }
    }

    void add_sub(Decoder *d) {
        switch (static_cast<ALU_ADD_SUB_Inst>(d->funct7())) {
            case ALU_ADD_SUB_Inst::ADD:
                add(d);
                break;
            case ALU_ADD_SUB_Inst::SUB:
                sub(d);
                break;
            default:
                error_dump("対応していないfunct7が使用されました: %x", d->funct7());
        }
    }


    void alu(Decoder *d) {
        switch (static_cast<ALU_Inst>(d->funct3())) {
            case ALU_Inst::ADD_SUB:
                add_sub(d);
                break;
            case ALU_Inst::SLL:
                sll(d);
                break;
            case ALU_Inst::SLT:
                slt(d);
                break;
            case ALU_Inst::SLTU:
                sltu(d);
                break;
            case ALU_Inst::XOR:
                xor_(d);
                break;
            case ALU_Inst::SR:
                sr(d);
                break;
            case ALU_Inst::OR:
                or_(d);
                break;
            case ALU_Inst::AND:
                and_(d);
                break;
            default:
                error_dump("対応していないopcodeが使用されました: %x", d->opcode());
                break;
        }
    }

    void run(Decoder *d) {
        switch (static_cast<Inst>(d->opcode())) {
            case Inst::ALU:
                alu(d);
                break;
            default:
                error_dump("対応していないopcodeが使用されました: %x", d->opcode());
                break;
        }
    }

    public:
    Core(std::string filename) {
        r = new Register;
        m = new Memory;

        char buf[512];
        std::ifstream ifs(filename);
        uint32_t addr = instruction_load_address;
        while(!ifs.eof()) {
            ifs.read(buf, 512);
            int read_bytes = ifs.gcount();
            m->mmap(addr, (uint8_t*)buf, read_bytes);
            addr += read_bytes;
        }
    }
    ~Core() {
        delete r;
        delete m;
    }
    void main_loop() {
        while(1) {
            uint32_t ip = r->ip;
            if (ip > 12) {
                break;
            }
            Decoder d = Decoder(m->get_inst(ip));
            r->ip += 4;
            run(&d);
        }
    }
};

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " program file" << std::endl;
        return 0;
    }
    Core core((std::string(argv[1])));
    core.main_loop();
    return 0;
}
