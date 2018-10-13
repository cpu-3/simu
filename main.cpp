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
    uint8_t memory[memory_size];

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

class FPU
{
    static float add(float x, float y)
    {
        return x + y;
    }
    static float sub(float x, float y)
    {
        return x - y;
    }
    static float mul(float x, float y)
    {
        return x * y;
    }
    static float div(float x, float y)
    {
        return x / y;
    }
    static float sqrt(float x)
    {
        return std::sqrt(x);
    }
};

class Register
{
    static const int ireg_size = 32;
    static const int freg_size = 32;
    uint32_t i_registers[ireg_size] = {0};
    float f_registers[freg_size] = {0.0f};

    static void check_ireg_name(int name, int write)
    {
        if (name < 0 || name > ireg_size)
        {
            error_dump("レジスタの番号が不正です: %d\n", name);
        }

        if (write && name == 0)
        {
            warn_dump("レジスタ0に書き込もうとしていますが\n");
        }
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
    void info()
    {
        std::cout << "ip: " << ip << std::endl;
        for (int i = 0; i < ireg_size; i++)
        {
            std::cout << "x" << i << ": " << i_registers[i] << " ";
            if (i % 6 == 5)
            {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }
};

class Decoder
{
    // get val's [l, r) bit value
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
        ret <<= 19;
        ret >>= 19;
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

class Core
{
    const uint32_t instruction_load_address = 0;
    Memory *m;
    Register *r;

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
    
    //TODO fsub, fmul, fdiv, fsqrt
    void fadd(Decoder *d)
    {
        if(d->rm() != 0){
          error_dump("丸め型がおかしいです\n");
        }
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
        branch_inner(d, (int64_t)r->get_ireg(d->rs1()) < (int64_t)r->get_ireg(d->rs2()));
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
        r->set_ireg(d->rd(), ALU::sltu(x, y));
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
        
//TODO implement fpu(d)
/*
        case Inst::FPU:
            fpu(d);
            r->ip += 4;
            break;
*/
        default:
            error_dump("対応していないopcodeが使用されました: %x\n", d->opcode());
            r->ip += 4;
            break;
        }
    }

  public:
    Core(std::string filename)
    {
        r = new Register;
        m = new Memory;

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
    }
    void main_loop()
    {
        while (1)
        {
            uint32_t ip = r->ip;
            r->info();
            Decoder d = Decoder(m->get_inst(ip));
            printf("instr: %x\n", d.code);
            run(&d);
            //std::string s;
            //std::getline(std::cin, s);
        }
    }
};

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cout << "Usage: " << argv[0] << " program file" << std::endl;
        return 0;
    }
    Core core((std::string(argv[1])));
    core.main_loop();
    return 0;
}
