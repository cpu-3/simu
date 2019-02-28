class Core
{
    const uint32_t instruction_load_address = 0;
    const int default_stack_pointer = 2;
    const int default_stack_dump_size = 48;
    Memory *m;
    Register *r;
    IO *io;
    Stat *stat;
    Disasm *disasm;

    Settings *settings;

    void lui(Decoder *d)
    {
        uint32_t imm = d->u_type_imm();
        r->set_ireg(d->rd(), imm);
        (stat->lui.stat)++;
        disasm->type = "u";
        disasm->inst_name = "lui";
        disasm->dest = d->rd();
        disasm->imm = d->u_type_imm();
    }
    void auipc(Decoder *d)
    {
        // sign extended
        int32_t imm = d->u_type_imm();
        imm += (int32_t)(r->ip);
        r->set_ireg(d->rd(), imm);
        (stat->auipc.stat)++;
        disasm->type = "u";
        disasm->inst_name = "auipc";
        disasm->dest = d->rd();
        disasm->imm = d->u_type_imm();
    }

    void jal(Decoder *d)
    {
        int32_t imm = d->jal_imm();
        r->set_ireg(d->rd(), r->ip + 1);
        r->ip = (int32_t)r->ip + imm;
        (stat->jal.stat)++;
        disasm->type = "j";
        disasm->inst_name = "jal";
        disasm->dest = d->rd();
        disasm->imm = d->jal_imm();
    }
    void jalr(Decoder *d)
    {
        // sign extended
        int32_t imm = d->i_type_imm();
        int32_t s = r->get_ireg(d->rs1());
        r->set_ireg(d->rd(), r->ip + 1);
        r->ip = s + imm;
        (stat->jalr.stat)++;
        disasm->type = "i";
        disasm->inst_name = "jalr";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }

    void branch_inner(Decoder *d, int flag)
    {
        if (flag)
        {
            r->ip = (int32_t)r->ip + d->b_type_imm();
        }
        else
        {
            r->ip += 1;
        }
    }
    void beq(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) == r->get_ireg(d->rs2()));
        (stat->beq.stat)++;
        disasm->type = "b";
        disasm->inst_name = "beq";
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
        disasm->imm = d->b_type_imm();
    }
    void bne(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) != r->get_ireg(d->rs2()));
        (stat->bne.stat)++;
        disasm->type = "b";
        disasm->inst_name = "bne";
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
        disasm->imm = d->b_type_imm();
    }
    void blt(Decoder *d)
    {
        branch_inner(d, (int32_t)r->get_ireg(d->rs1()) < (int32_t)r->get_ireg(d->rs2()));
        (stat->blt.stat)++;
        disasm->type = "b";
        disasm->inst_name = "blt";
        disasm->src1 = (int32_t)d->rs1();
        disasm->src2 = (int32_t)d->rs2();
        disasm->imm = d->b_type_imm();
    }
    void bge(Decoder *d)
    {
        branch_inner(d, (int64_t)r->get_ireg(d->rs1()) >= (int64_t)r->get_ireg(d->rs2()));
        (stat->bge.stat)++;
        disasm->type = "b";
        disasm->inst_name = "bge";
        disasm->src1 = (int64_t)d->rs1();
        disasm->src2 = (int64_t)d->rs2();
        disasm->imm = d->b_type_imm();
    }
    void bltu(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) < r->get_ireg(d->rs2()));
        (stat->bltu.stat)++;
        disasm->type = "b";
        disasm->inst_name = "bltu";
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
        disasm->imm = d->b_type_imm();
    }
    void bgeu(Decoder *d)
    {
        branch_inner(d, r->get_ireg(d->rs1()) >= r->get_ireg(d->rs2()));
        (stat->bgeu.stat)++;
        disasm->type = "b";
        disasm->inst_name = "bgeu";
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
        disasm->imm = d->b_type_imm();
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
        (stat->lb.stat)++;
        disasm->type = "i";
        disasm->inst_name = "lb";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
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
        (stat->lh.stat)++;
        disasm->type = "i";
        disasm->inst_name = "lh";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
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
        (stat->lw.stat)++;
        disasm->type = "i";
        disasm->inst_name = "lw";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void lbu(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t offset = d->i_type_imm();
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_1(addr);
        r->set_ireg(d->rd(), val);
        (stat->lbu.stat)++;
        disasm->type = "i";
        disasm->inst_name = "lbu";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void lhu(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        uint32_t offset = d->i_type_imm();
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_2(addr);
        r->set_ireg(d->rd(), val);
        (stat->lhu.stat)++;
        disasm->type = "i";
        disasm->inst_name = "lhu";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
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
        (stat->sb.stat)++;
        disasm->type = "s";
        disasm->inst_name = "sb";
        disasm->src = d->rs2();
        disasm->base = d->rs1();
        disasm->imm = d->s_type_imm();
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
        (stat->sh.stat)++;
        disasm->type = "s";
        disasm->inst_name = "sh";
        disasm->src = d->rs2();
        disasm->base = d->rs1();
        disasm->imm = d->s_type_imm();
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
        (stat->sw.stat)++;
        disasm->type = "s";
        disasm->inst_name = "sw";
        disasm->src = d->rs2();
        disasm->base = d->rs1();
        disasm->imm = d->s_type_imm();
    }

    void addi(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::add(x, y));
        (stat->addi.stat)++;
        disasm->type = "i";
        disasm->inst_name = "addi";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void slti(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::slt(x, y));
        (stat->slti.stat)++;
        disasm->type = "i";
        disasm->inst_name = "slti";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void sltiu(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::sltu(x, y));
        (stat->sltiu.stat)++;
        disasm->type = "i";
        disasm->inst_name = "sltiu";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void xori(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::xor_(x, y));
        (stat->xori.stat)++;
        disasm->type = "i";
        disasm->inst_name = "xori";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void ori(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::or_(x, y));
        (stat->ori.stat)++;
        disasm->type = "i";
        disasm->inst_name = "ori";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void andi(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::and_(x, y));
        (stat->andi.stat)++;
        disasm->type = "i";
        disasm->inst_name = "andi";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void slli(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::sll(x, y));
        (stat->slli.stat)++;
        disasm->type = "i";
        disasm->inst_name = "slli";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    void srli(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = d->i_type_imm();
        r->set_ireg(d->rd(), ALU::srl(x, y));
        (stat->srli.stat)++;
        disasm->type = "i";
        disasm->inst_name = "srli";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
    }
    // void srai

    void add(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::add(x, y));
        (stat->add.stat)++;
        disasm->type = "r";
        disasm->inst_name = "add";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void sub(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sub(x, y));
        (stat->sub.stat)++;
        disasm->type = "r";
        disasm->inst_name = "sub";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void sll(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sll(x, y));
        (stat->sll.stat)++;
        disasm->type = "r";
        disasm->inst_name = "sll";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void slt(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::slt(x, y));
        (stat->slt.stat)++;
        disasm->type = "r";
        disasm->inst_name = "slt";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void sltu(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sltu(x, y));
        (stat->sltu.stat)++;
        disasm->type = "r";
        disasm->inst_name = "sltu";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void xor_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::xor_(x, y));
        (stat->xor_.stat)++;
        disasm->type = "r";
        disasm->inst_name = "xor";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void srl(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::srl(x, y));
        (stat->srl.stat)++;
        disasm->type = "r";
        disasm->inst_name = "srl";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void sra(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::sra(x, y));
        (stat->sra.stat)++;
        disasm->type = "r";
        disasm->inst_name = "sra";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void or_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::or_(x, y));
        (stat->or_.stat)++;
        disasm->type = "r";
        disasm->inst_name = "or";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void and_(Decoder *d)
    {
        uint32_t x = r->get_ireg(d->rs1());
        uint32_t y = r->get_ireg(d->rs2());
        r->set_ireg(d->rd(), ALU::and_(x, y));
        (stat->and_.stat)++;
        disasm->type = "r";
        disasm->inst_name = "and";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
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
            r->ip += 1;
            break;
        }
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
            r->ip += 1;
            break;
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

    void flw(Decoder *d)
    {
        uint32_t base = r->get_ireg(d->rs1());
        int32_t offset = d->i_type_imm();
        offset <<= 20;
        offset >>= 20;
        uint32_t addr = base + offset;
        uint32_t val = m->read_mem_4(addr);
        r->set_freg_raw(d->rd(), val);
        (stat->flw.stat)++;
        disasm->type = "fi";
        disasm->inst_name = "flw";
        disasm->dest = d->rd();
        disasm->base = d->rs1();
        disasm->imm = d->i_type_imm();
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
        (stat->fsw.stat)++;
        disasm->type = "fs";
        disasm->inst_name = "fsw";
        disasm->src = d->rs2();
        disasm->base = d->rs1();
        disasm->imm = d->s_type_imm();
    }

    void fadd(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fadd(x, y));
        (stat->fadd.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fadd";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fsub(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fsub(x, y));
        (stat->fsub.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fsub";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fmul(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fmul(x, y));
        (stat->fmul.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fmul";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fdiv(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fdiv(x, y));
        (stat->fdiv.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fdiv";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fsqrt(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        if (d->rs2() != 0)
        {
            error_dump("命令フォーマットがおかしいです(fsqrtではrs2()は0になる)\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        r->set_freg_raw(d->rd(), FPU::fsqrt(x));
        (stat->fsqrt.stat)++;
        disasm->type = "fR";
        disasm->inst_name = "fsqrt";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
    }

    void _fsgnj(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fsgnj(x, y));
        (stat->fsgnj.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fsgnj";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fsgnjn(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fsgnjn(x, y));
        (stat->fsgnjn.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fsgnjn";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }

    void fsgnjx(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_freg_raw(d->rd(), FPU::fsgnjx(x, y));
        (stat->fsgnjn.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fsgnjx";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }

    void fcvt_w_s(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        if (d->rs2() != 0)
        {
            error_dump("命令フォーマットがおかしいです(fcvt_w_sではrs2()は0になる)\n");
        }
        uint32_t x = r->get_freg_raw(d->rs1());
        r->set_ireg(d->rd(), FPU::float2int(x));
        (stat->fcvt_w_s.stat)++;
        disasm->type = "fR";
        disasm->inst_name = "fcvt_w_s";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
    }
    void fcvt_s_w(Decoder *d)
    {
        if (d->rm() != 0)
        {
            error_dump("丸め型がおかしいです\n");
        }
        if (d->rs2() != 0)
        {
            error_dump("命令フォーマットがおかしいです(fcvt_w_sではrs2()は0になる)\n");
        }
        uint32_t x = r->get_ireg(d->rs1());
        r->set_freg_raw(d->rd(), FPU::int2float(x));
        (stat->fcvt_s_w.stat)++;
        disasm->type = "fR";
        disasm->inst_name = "fcvt_s_w";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
    }

    void feq(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_ireg(d->rd(), FPU::feq(x, y));
        (stat->feq.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "feq";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void flt(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_ireg(d->rd(), FPU::flt(x, y));
        (stat->flt.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "flt";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
    }
    void fle(Decoder *d)
    {
        uint32_t x = r->get_freg_raw(d->rs1());
        uint32_t y = r->get_freg_raw(d->rs2());
        r->set_ireg(d->rd(), FPU::fle(x, y));
        (stat->fle.stat)++;
        disasm->type = "fr";
        disasm->inst_name = "fle";
        disasm->dest = d->rd();
        disasm->src1 = d->rs1();
        disasm->src2 = d->rs2();
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
            r->ip += 1;
            break;
        }
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
            r->ip += 1;
            break;
        }
    }

    void fsgnj(Decoder *d)
    {
        switch (static_cast<FSgnj_Inst>(d->funct3()))
        {
        case FSgnj_Inst::FSGNJ:
            _fsgnj(d);
            break;
        case FSgnj_Inst::FSGNJN:
            fsgnjn(d);
            break;
        case FSgnj_Inst::FSGNJX:
            fsgnjx(d);
            break;
        default:
            error_dump("対応していないfunct3が使用されました: %x\n", d->funct3());
        }
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
            r->ip += 1;
            break;
        case Inst::AUIPC:
            auipc(d);
            r->ip += 1;
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
            r->ip += 1;
            break;
        case Inst::STORE:
            store(d);
            r->ip += 1;
            break;
        case Inst::ALUI:
            alui(d);
            r->ip += 1;
            break;
        case Inst::ALU:
            alu(d);
            r->ip += 1;
            break;
        case Inst::FLOAD:
            fload(d);
            r->ip += 1;
            break;
        case Inst::FSTORE:
            fstore(d);
            r->ip += 1;
            break;
        case Inst::FPU:
            fpu(d);
            r->ip += 1;
            break;
        default:
            error_dump("対応していないopcodeが使用されました: %x\n", d->opcode());
            r->ip += 1;
            break;
        }
    }

  public:
    Core(std::string filename, Settings *settings)
    {
        r = new Register;
        io = new IO;
        m = new Memory(io);
        stat = new Stat;
        disasm = new Disasm;

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
        delete stat;
        delete disasm;
    }
    void show_stack_from_top()
    {
        std::cout << "Stack" << std::endl;
        m->show_data(r->get_ireg(default_stack_pointer), default_stack_dump_size);
    }
    void info()
    {
        if (!settings->hide_error_dump)
        {
            r->info();
            printf("inst_count: %llx\n", inst_count);
            io->show_status();
            stat->show_stats();
        }
    }
    unsigned long long inst_count = 0; //pipe line
    void main_loop()
    {
        while (1)
        {
            uint32_t ip = r->ip;
            Decoder d = Decoder(m->get_inst(ip));
            run(&d);
            if (inst_count < settings->wait)
            {
                inst_count++;
                continue;
            }
            if (settings->show_inst_value)
            {
                printf("inst_count: %llx\n", inst_count);
                printf("ip: %x\n", ip * 4);
                std::cout << "inst: " << std::bitset<32>(d.code) << std::endl;
                disasm->print_inst(disasm->type);
            }
            inst_count++;
            if (settings->show_registers)
            {
                r->info();
            }
            if (settings->show_stack)
            {
                show_stack_from_top();
            }
            if (settings->show_io)
            {
                io->show_status();
            }
            if (settings->step_execution)
            {
                std::string s;
                std::cout << "enter s(next step), c(continue) > ";
                std::getline(std::cin, s);
                if (settings->break_point && s == "c")
                {
                    settings->step_execution = false;
                }
            }
            if (settings->break_point)
            {
                if (ip == settings->ip)
                {
                    std::cout << "enter s(next step), c(continue) > ";
                    std::string s;
                    std::getline(std::cin, s);
                    if (s != "c")
                    {
                        settings->step_execution = true;
                    }
                }
            }
        }
    }
};
