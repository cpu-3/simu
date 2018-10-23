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

void warn_dump(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

class IO
{
    uint8_t led;

  public:
    void show_status()
    {
        printf("LED: %02x\n", led);
    }
    void write_led(uint8_t val)
    {
        led = val;
    }

    void transmit_uart(uint8_t val)
    {
        std::cout << std::hex << val << std::dec << std::endl;
    }

    uint8_t receive_uart()
    {
        uint8_t val;
        std::cin >> val;
        return val;
    }
};

class Memory
{
    /* Current Memory Map
    0x0     --------
              Inst
    0xffff  --------
              IO
    0x10fff --------
              Data
    0xf4240 --------
    */
    static const uint32_t memory_size = 0xf4240;
    static const uint32_t inst_mem_lim = 0xffff;
    static const uint32_t IO_mem_lim = 0x10fff;
    static const uint32_t memory_base = 0;
    static const uint32_t memory_lim = memory_base + memory_size;

    static const uint32_t uart_rx_addr = 0x10000;
    static const uint32_t uart_tx_addr = 0x10004;
    static const uint32_t led_addr = 0x10008;

    uint8_t memory[memory_size];
    IO *io;


    void addr_alignment_check(uint32_t addr)
    {
        if (addr % 4 != 0)
        {
            error_dump("メモリアドレスのアラインメントがおかしいです: %x\n", addr);
        }
    }

    void data_mem_check(uint32_t addr, uint8_t size)
    {
        addr_alignment_check(addr);
        if (addr + size >= memory_lim || addr + size <= IO_mem_lim)
        {
            error_dump("多分不正なデータアドレスに書き込もうとしました: %x\n", addr);
        }
    }

    void read_mem_check(uint32_t addr, uint8_t size)
    {
        addr_alignment_check(addr);
        if (addr + size >= memory_lim)
        {
            error_dump("多分不正なデータアドレスを読もうとしました: %x\n", addr);
        }
    }

    void inst_mem_check(uint32_t addr)
    {
        addr_alignment_check(addr);
        if (addr + 4 > inst_mem_lim)
        {
            error_dump("多分不正な命令アドレスに書き込もうとしました: %x\n", addr);
        }
    }

    void map_mem_check(uint32_t addr, uint32_t size)
    {
        if ((addr + size) >= memory_lim)
        {
            error_dump("多分不正なアドレスに書き込もうとしました: %x\n", addr);
        }
    }

    bool hook_io_write(uint32_t addr, uint8_t val)
    {
        if (addr == uart_rx_addr)
        {
            error_dump("uartの読み込みポートに書き込みを試みました");
            return true;
        }
        else if (addr == uart_tx_addr)
        {
            io->transmit_uart(val);
        }
        else if (addr == led_addr)
        {
            io->write_led(val);
        }
        else
        {
            return false;
        }
        return true;
    }

    bool hook_io_read(uint32_t addr, uint8_t *v)
    {
        if (addr == uart_rx_addr)
        {
            *v = io->receive_uart();
        }
        else if (addr == uart_tx_addr)
        {
            error_dump("uartの書き込みポートを読み込もうとしました");
        }
        else if (addr == led_addr)
        {
            error_dump("ledの値を読み取ろうとしました");
        }
        else
        {
            return false;
        }
        return true;
    }

  public:
    Memory(IO *io) {
        this->io = io;
    }

    void write_mem(uint32_t addr, uint8_t val)
    {
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 1);
            memory[addr] = val;
        }
    }

    void write_mem(uint32_t addr, uint16_t val)
    {
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 2);
            uint16_t *m = (uint16_t *)memory;
            m[addr / 2] = val;
        }
    }

    void write_mem(uint32_t addr, uint32_t val)
    {
        if (!hook_io_write(addr, val))
        {
            data_mem_check(addr, 4);
            uint32_t *m = (uint32_t *)memory;
            m[addr / 4] = val;
        }
    }

    uint8_t read_mem_1(uint32_t addr)
    {
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 1);
        return memory[addr];
    }

    uint16_t read_mem_2(uint32_t addr)
    {
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 2);
        uint16_t *m = (uint16_t *)memory;
        return m[addr / 2];
    }

    uint32_t read_mem_4(uint32_t addr)
    {
        uint8_t v;
        if (hook_io_read(addr, &v))
        {
            return v;
        }
        read_mem_check(addr, 4);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    uint32_t get_inst(uint32_t addr)
    {
        inst_mem_check(addr);
        uint32_t *m = (uint32_t *)memory;
        return m[addr / 4];
    }

    // set instructions to memory
    // inst_memが満杯になって死ぬとかないのかな(wakarazu)
    void mmap(uint32_t addr, uint8_t *data, uint32_t length)
    {
        addr_alignment_check(addr);
        addr_alignment_check(length);
        for (int i = 0; i < length; i++)
        {
            memory[addr + i] = data[i];
        }
    }

    void show_data(uint32_t addr, uint32_t length)
    {
        int cnt = 0;
        for (uint32_t ad = addr; ad < addr + length; ad += 4) {
            if (ad + 4 >= memory_lim) {
                break;
            }
            uint32_t v = read_mem_4(ad);
            printf("%08x: %08x\n", ad, v);
        }
    }
};

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

typedef union {
    uint32_t i;
    float f;
} float_int;

int f2i(float x) {
    float_int data;
    data.f = x;
    return data.i;
}

float i2f(int x) {
    float_int data;
    data.i = x;
    return data.f;
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
        printf("data: %f\n", x);
        int y = int(x);
        return y;
    }
    static float int2float(int x)
    {
        float y = float(x);
        return y;
    }
};

class Register
{
    static const int ireg_size = 32;
    static const int freg_size = 32;
    uint32_t i_registers[ireg_size] = {0};
    uint32_t f_registers[freg_size] = {0};

    static void check_ireg_name(int name, int write)
    {
        if (name < 0 || name > ireg_size)
        {
            error_dump("レジスタの番号が不正です: %d\n", name);
        }
/*
        if (write && name == 0)
        {
            warn_dump("レジスタ0に書き込もうとしていますが\n");
        }
        */
    }
    static void check_freg_name(int name)
    {
        if (name < 0 || name > freg_size)
        {
            error_dump("レジスタの番号が不正です: %d\n", name);
        }
    }

  public:
    uint32_t ip;
    Register() : ip(0) {}
    void set_ireg(int name, uint32_t val)
    {
        check_ireg_name(name, 1);

        if (name == 0)
        {
            return;
        }
        i_registers[name] = val;
    }
    void set_freg(int name, float val)
    {
        check_freg_name(name);

        f_registers[name] = f2i(val);
    }
    void set_freg_raw(int name, uint32_t val)
    {
        check_freg_name(name);

        f_registers[name] = val;
    }
    uint32_t get_ireg(int name)
    {
        check_ireg_name(name, 0);
        if (name == 0)
        {
            return 0;
        }
        return i_registers[name];
    }
    float get_freg(int name)
    {
        check_freg_name(name);
        return i2f(f_registers[name]);
    }
    uint32_t get_freg_raw(int name)
    {
        check_freg_name(name);
        return f_registers[name];
    }

    void info()
    {
        std::cout << std::hex;
        std::cout << "ip: " << ip << std::endl;
        for (int i = 0; i < ireg_size; i++)
        {
            std::cout << std::dec << "x" << i << std::hex << ": " << i_registers[i] << " ";
            if (i % 6 == 5)
            {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        for (int i = 0; i < freg_size; i++)
        {
            std::cout << std::dec << "f" << i << std::hex << ": " << i2f(f_registers[i]) << " ";
            if (i % 6 == 5)
            {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        std::cout << std::dec;
    }
};

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

class Settings
{
    public:
    bool step_execution;
    bool show_stack;
    bool show_registers;
    bool show_inst_value;
    bool show_io;

    Settings(const char *cmd_arg) {
        step_execution = false;
        show_stack = false;
        show_registers = false;
        show_inst_value = false;
        show_io = false;

        for (const char *c = &cmd_arg[0]; *c; c++) {
            switch (*c) {
            case 's':
                step_execution = true;
                break;
            case 't':
                show_stack = true;
                break;
            case 'r':
                show_registers = true;
                break;
            case 'i':
                show_inst_value = true;
                break;
            case 'o':
                show_io = true;
                break;
            case 'a':
                show_stack = true;
                show_registers = true;
                show_inst_value = true;
                show_io = true;
            }
        }
    }
};

class Core
{
    const uint32_t instruction_load_address = 0;
    const int default_stack_pointer = 2;
    const int default_stack_dump_size = 48;
    Memory *m;
    Register *r;
    IO *io;

    Settings *settings;

    void add(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::add(x, y));
    }
    void sub(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sub(x, y));
    }
    void sll(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sll(x, y));
    }
    void slt(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::slt(x, y));
    }
    void sltu(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sltu(x, y));
    }
    void xor_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::xor_(x, y));
    }
    void or_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::or_(x, y));
    }
    void and_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::and_(x, y));
    }
    void sra(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sra(x, y));
    }
    void srl(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::srl(x, y));
    }

    void lui(Decoder *d)
    {
        uint32_t val = d->u_type_imm();
        r->set_ireg(d->rd(), val);
    }
    void auipc(Decoder *d)
    {
        // sign extended
        int64_t val = d->u_type_imm() << 12;
        val <<= 32;
        val >>= 32;
        val += (int64_t)(r->ip);
        r->set_ireg(d->rd(), val);
    }
    void jal(Decoder *d)
    {
        int32_t imm = d->jal_imm();
        r->set_ireg(d->rd(), r->ip + 4);
        r->ip = (int32_t)r->ip + imm;
    }
    void branch_inner(Decoder *d, int flag)
    {
        if (flag)
        {
            r->ip = (int32_t)r->ip + d->b_type_imm();
        }
        else
        {
            r->ip += 4;
        }
    }
    void beq(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) == r->get_ireg(d->rs2()));
    }
    void bne(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) != r->get_ireg(d->rs2()));
    }
    void blt(Decoder *d)
    {
        branch_inner(d, (int32_t)r->get_ireg(d->rs1()) < (int32_t)r->get_ireg(d->rs2()));
    }
    void bge(Decoder *d)
    {
        branch_inner(d, (int64_t)r->get_ireg(d->rs1()) >= (int64_t)r->get_ireg(d->rs2()));
    }
    void bltu(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) < r->get_ireg(d->rs2()));
    }
    void bgeu(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) >= r->get_ireg(d->rs2()));
    }
    void jalr(Decoder *d)
    {
        // sign extended
        int32_t imm = d->i_type_imm();
        int32_t s = r->get_ireg(d->rs1());
        r->set_ireg(d->rd(), r->ip + 4);
        r->ip = s + imm;
    }
    void addi(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::add(x, y));
    }
    void slti(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::slt(x, y));
    }
    void srli(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::srl(x, y));
    }
    void sltiu(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::sltu(x, y));
    }
    void xori(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::xor_(x, y));
    }
    void ori(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::or_(x, y));
    }
    void andi(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::and_(x, y));
    }
    void slli(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::sll(x, y));
    }

    void sr(Decoder *d)
    {
        switch (static_cast<ALU_SR_Inst>(d->funct7()))
        {
        case ALU_SR_Inst::SRA:
            sra(d);
            break;
        case ALU_SR_Inst::SRL:
            srl(d);
            break;
        default:
            error_dump("対応していないfunct7が使用されました: %x\n", d->funct7());
        }
    }

    void add_sub(Decoder *d)
    {
        switch (static_cast<ALU_ADD_SUB_Inst>(d->funct7()))
        {
        case ALU_ADD_SUB_Inst::ADD:
            add(d);
            break;
        case ALU_ADD_SUB_Inst::SUB:
            sub(d);
            break;
        default:
            error_dump("対応していないfunct7が使用されました: %x\n", d->funct7());
        }
    }

    void alui(Decoder *d)
    {
        switch (static_cast<ALUI_Inst>(d->funct3()))
        {
        case ALUI_Inst::ADDI:
            addi(d);
            break;
        case ALUI_Inst::SLTI:
            slti(d);
            break;
        case ALUI_Inst::SLTIU:
            sltiu(d);
            break;
        case ALUI_Inst::XORI:
            xori(d);
            break;
        case ALUI_Inst::ORI:
            ori(d);
            break;
        case ALUI_Inst::ANDI:
            andi(d);
            break;
        case ALUI_Inst::SLLI:
            slli(d);
            break;
        case ALUI_Inst::SRLI:
            srli(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void alu(Decoder *d)
    {
        switch (static_cast<ALU_Inst>(d->funct3()))
        {
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
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void branch(Decoder *d)
    {
        switch (static_cast<Branch_Inst>(d->funct3()))
        {
        case Branch_Inst::BEQ:
            beq(d);
            break;
        case Branch_Inst::BNE:
            bne(d);
            break;
        case Branch_Inst::BLT:
            blt(d);
            break;
        case Branch_Inst::BGE:
            bge(d);
            break;
        case Branch_Inst::BLTU:
            bltu(d);
            break;
        case Branch_Inst::BGEU:
            bgeu(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void lb(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        int32_t offset = d->i_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_1(addr);
        r->set_ireg(d->rd(), val);
    }
    void lh(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        int32_t offset = d->i_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_2(addr);
        r->set_ireg(d->rd(), val);
    }
    void lw(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        int32_t offset = d->i_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_4(addr);
        r->set_ireg(d->rd(), val);
    }
    void lbu(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t offset = d->i_type_imm();
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_1(addr);
        r->set_ireg(d->rd(), val);
    }
    void lhu(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t offset = d->i_type_imm();
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_2(addr);
        r->set_ireg(d->rd(), val);
    }

    void load(Decoder *d)
    {
        switch (static_cast<Load_Inst>(d->funct3()))
        {
        case Load_Inst::LB:
            lb(d);
            break;
        case Load_Inst::LH:
            lh(d);
            break;
        case Load_Inst::LW:
            lw(d);
            break;
        case Load_Inst::LBU:
            lbu(d);
            break;
        case Load_Inst::LHU:
            lhu(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
            r->ip += 4;
            break;
        }
    }

    void sb(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t src = r->get_ireg(d->rs2());
        src <<= 24;
        src >>= 24;
        int32_t offset = d->s_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        m->write_mem(addr, src);
    }

    void sh(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t src = r->get_ireg(d->rs2());
        src <<= 16;
        src >>= 16;
        int32_t offset = d->s_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        m->write_mem(addr, src);
    }

    void sw(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t src = r->get_ireg(d->rs2());
        int32_t offset = d->s_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        m->write_mem(addr, src);
    }

    void store(Decoder *d)
    {
        switch (static_cast<Store_Inst>(d->funct3()))
        {
        case Store_Inst::SB:
            sb(d);
            break;
        case Store_Inst::SH:
            sh(d);
            break;
        case Store_Inst::SW:
            sw(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
            r->ip += 4;
            break;
        }
    }

    void flw(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        int32_t offset = d->i_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_4(addr);
        r->set_freg_raw(d->rd(), val);
    }

    void fload(Decoder *d)
    {
        switch (static_cast<FLoad_Inst>(d->funct3()))
        {
        case FLoad_Inst::FLW:
            flw(d);
            break;
        default:
            error_dump("widthがおかしいです(仕様書p112): %x\n", d->funct3());
            r->ip += 4;
            break;
        }
    }

    void fsw(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t src = r->get_freg_raw(d->rs2());
        int32_t offset = d->s_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        m->write_mem(addr, src);
    }

    void fstore(Decoder *d)
    {
        switch (static_cast<FStore_Inst>(d->funct3()))
        {
        case FStore_Inst::FSW:
            fsw(d);
            break;
        default:
            error_dump("widthがおかしいです(仕様書p112): %x\n", d->funct3());
            r->ip += 4;
            break;
        }
    }

    void fadd(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_freg(d->rd(), FPU::fadd(x, y));
    }

    void fsub(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_freg(d->rd(), FPU::fsub(x, y));
    }

    void fmul(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_freg(d->rd(), FPU::fmul(x, y));
    }

    void fdiv(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_freg(d->rd(), FPU::fdiv(x, y));
    }

    void fsqrt(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        if(d->rs2() != 0){
          error_dump("命令フォーマットがおかしいです(fsqrtではrs2()は0になる)\n");
        }
        float x = r->get_freg(d->rs1());
        r->set_freg(d->rd(), FPU::fsqrt(x));
    }

    void fcvt_w_s(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        if(d->rs2() != 0){
          error_dump("命令フォーマットがおかしいです(fcvt_w_sではrs2()は0になる)\n");
        }
        float x = r->get_freg(d->rs1());
        r->set_ireg(d->rd(), FPU::float2int(x));
    }

    void fcvt_s_w(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
        if(d->rs2() != 0){
          error_dump("命令フォーマットがおかしいです(fcvt_w_sではrs2()は0になる)\n");
        }
        uint32_t x = r->get_ireg(d->rs1());
        r->set_freg(d->rd(), FPU::int2float(x));
    }

    void feq(Decoder *d)
    {
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_ireg(d->rd(),FPU::feq(x,y));
    }
    void flt(Decoder *d)
    {
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_ireg(d->rd(),FPU::flt(x,y));
    }
    void fle(Decoder *d)
    {
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_ireg(d->rd(),FPU::fle(x,y));
    }

    void fcomp(Decoder *d)
    {
        switch (static_cast<FComp_Inst>(d->funct3()))
        {
        case FComp_Inst::FEQ:
            feq(d);
            break;
        case FComp_Inst::FLT:
            flt(d);
            break;
        case FComp_Inst::FLE:
            fle(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void _fsgnj(Decoder *d) 
    {
        float x = r->get_freg(d->rs1());
        float y = r->get_freg(d->rs2());
        r->set_freg(d->rd(), x * y > 0 ? x : -x);
    }

    void fsgnj(Decoder *d) 
    {
        switch (static_cast<FSGNJ_Inst>(d->funct3())) 
        {
        case FSGNJ_Inst::FSGNJ:
            _fsgnj(d);
            break;
        case FSGNJ_Inst::FSGNJN:
        case FSGNJ_Inst::FSGNJX:
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void fpu(Decoder *d)
    {
        switch (static_cast<FPU_Inst>(d->funct5_fmt()))
        {
        case FPU_Inst::FADD:
            fadd(d);
            break;
        case FPU_Inst::FSUB:
            fsub(d);
            break;
        case FPU_Inst::FMUL:
            fmul(d);
            break;
        case FPU_Inst::FDIV:
            fdiv(d);
            break;
        case FPU_Inst::FSQRT:
            fsqrt(d);
            break;
        case FPU_Inst::FCOMP:
            fcomp(d);
            break;
        case FPU_Inst::FCVT_W_S:
            fcvt_w_s(d);
            break;
        case FPU_Inst::FCVT_S_W:
            fcvt_s_w(d);
            break;
        case FPU_Inst::FSGNJ:
            fsgnj(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
    }

    void run(Decoder *d)
    {
        switch (static_cast<Inst>(d->opcode()))
        {
        case Inst::LUI:
            lui(d);
            r->ip += 4;
            break;
        case Inst::AUIPC:
            auipc(d);
            r->ip += 4;
            break;
        case Inst::JAL:
            jal(d);
            break;
        case Inst::JALR:
            jalr(d);
            break;
        case Inst::BRANCH:
            branch(d);
            break;
        case Inst::LOAD:
            load(d);
            r->ip += 4;
            break;
        case Inst::STORE:
            store(d);
            r->ip += 4;
            break;
        case Inst::ALUI:
            alui(d);
            r->ip += 4;
            break;
        case Inst::ALU:
            alu(d);
            r->ip += 4;
            break;
        case Inst::FLOAD:
            fload(d);
            r->ip += 4;
            break;
        case Inst::FSTORE:
            fstore(d);
            r->ip += 4;
            break;
        case Inst::FPU:
            fpu(d);
            r->ip += 4;
            break;
        default:
            info();
            error_dump("対応していないopcodeが使用されました: %x\n", d->opcode());
            r->ip += 4;
            break;
        }
    }

  public:
    Core(std::string filename, Settings *settings)
    {
        r = new Register;
        io = new IO;
        m = new Memory(io);

        this->settings = settings;

        char buf[512];
        std::ifstream ifs(filename);
        uint32_t addr = instruction_load_address;
        while (!ifs.eof())
        {
            ifs.read(buf, 512);
            int read_bytes = ifs.gcount();
            m->mmap(addr, (uint8_t *)buf, read_bytes);
            addr += read_bytes;
        }
    }
    ~Core()
    {
        delete r;
        delete m;
        delete io;
    }
    void info() {
        r->info();
        m->show_data(r->get_ireg(default_stack_pointer), default_stack_dump_size);
        io->show_status();
    }
    void main_loop()
    {
        while (1)
        {
            uint32_t ip = r->ip;
            Decoder d = Decoder(m->get_inst(ip));
            run(&d);
            if (settings->show_inst_value) {
                printf("instr: %x\n", d.code);
            }
            if (settings->show_registers) {
                r->info();
            }
            if (settings->show_stack) {
                m->show_data(r->get_ireg(default_stack_pointer), default_stack_dump_size);
            }
            if (settings->show_io) {
                io->show_status();
            }
            if (settings->step_execution) {
                std::string s;
                std::getline(std::cin, s);
            }
        }
    }
};


int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " program file" << std::endl;
        return 0;
    }
    Settings s = Settings(argc == 2 ? "" : argv[2]);
    Core core((std::string(argv[1])), &s);
    core.main_loop();
    return 0;
}
