struct Statdata{
    unsigned long long stat;
    std::string name;
    bool operator<( const Statdata& right ) const {
        return stat == right.stat ? name < right.name : stat < right.stat;
    }
};

class Stat
{
  public:
    Statdata lui;
    Statdata auipc;
    Statdata jal;
    Statdata jalr;
    Statdata beq;
    Statdata bne;
    Statdata blt;
    Statdata bge;
    Statdata bltu;
    Statdata bgeu;
    Statdata lb;
    Statdata lh;
    Statdata lw;
    Statdata lbu;
    Statdata lhu;
    Statdata sb;
    Statdata sh;
    Statdata sw;
    Statdata addi;
    Statdata slti;
    Statdata sltiu;
    Statdata xori;
    Statdata ori;
    Statdata andi;
    Statdata slli;
    Statdata srli;
    Statdata add;
    Statdata sub;
    Statdata sll;
    Statdata slt;
    Statdata sltu;
    Statdata xor_;
    Statdata sra;
    Statdata srl;
    Statdata or_;
    Statdata and_;
    Statdata flw;
    Statdata fsw;
    Statdata fadd;
    Statdata fsub;
    Statdata fmul;
    Statdata fdiv;
    Statdata fsqrt;
    Statdata fsgnj;
    Statdata fsgnjn;
    Statdata fcvt_w_s;
    Statdata fcvt_s_w;
    Statdata feq;
    Statdata flt;
    Statdata fle;
    Stat(){
        lui.stat = 0;
        lui.name = "lui";
        auipc.stat = 0;
        auipc.name = "auipc";
        jal.stat = 0;
        jal.name = "jal";
        jalr.stat = 0;
        jalr.name = "jalr";
        beq.stat = 0;
        beq.name = "beq";
        bne.stat = 0;
        bne.name = "bne";
        blt.stat = 0;
        blt.name = "blt";
        bge.stat = 0;
        bge.name = "bge";
        bltu.stat = 0;
        bltu.name = "bltu";
        bgeu.stat = 0;
        bgeu.name = "bgeu";
        lb.stat = 0;
        lb.name = "lb";
        lh.stat = 0;
        lh.name = "lh";
        lw.stat = 0;
        lw.name = "lw";
        lbu.stat = 0;
        lbu.name = "lbu";
        lhu.stat = 0;
        lhu.name = "lhu";
        sb.stat = 0;
        sb.name = "sb";
        sh.stat = 0;
        sh.name = "sh";
        sw.stat = 0;
        sw.name = "sw";
        addi.stat = 0;
        addi.name = "addi";
        slti.stat = 0;
        slti.name = "slti";
        sltiu.stat = 0;
        sltiu.name = "sltiu";
        xori.stat = 0;
        xori.name = "xori";
        ori.stat = 0;
        ori.name = "ori";
        andi.stat = 0;
        andi.name = "andi";
        slli.stat = 0;
        slli.name = "slli";
        srli.stat = 0;
        srli.name = "srli";
        add.stat = 0;
        add.name = "add";
        sub.stat = 0;
        sub.name = "sub";
        sll.stat = 0;
        sll.name = "sll";
        slt.stat = 0;
        slt.name = "slt";
        sltu.stat = 0;
        sltu.name = "sltu";
        xor_.stat = 0;
        xor_.name = "xor";
        srl.stat = 0;
        srl.name = "srl";
        sra.stat = 0;
        sra.name = "sra";
        or_.stat = 0;
        or_.name = "or";
        and_.stat = 0;
        and_.name = "and";
        flw.stat = 0;
        flw.name = "flw";
        fsw.stat = 0;
        fsw.name = "fsw";
        fadd.stat = 0;
        fadd.name = "fadd";
        fsub.stat = 0;
        fsub.name = "fsub";
        fmul.stat = 0;
        fmul.name = "fmul";
        fdiv.stat = 0;
        fdiv.name = "fdiv";
        fsqrt.stat = 0;
        fsqrt.name = "fsqrt";
        fsgnj.stat = 0;
        fsgnj.name = "fsgnj";
        fsgnjn.stat = 0;
        fsgnjn.name = "fsgnjn";
        fcvt_w_s.stat = 0;
        fcvt_w_s.name = "fcvt_w_s";
        fcvt_s_w.stat = 0;
        fcvt_s_w.name = "fcvt_w_s";
        feq.stat = 0;
        feq.name = "feq";
        flt.stat = 0;
        flt.name = "flt";
        fle.stat = 0;
        fle.name = "fle";
    }

    unsigned long long all() {
      return lui.stat + auipc.stat + jal.stat + jalr.stat + beq.stat + bne.stat + blt.stat +
             bge.stat + bltu.stat + bgeu.stat + lb.stat + lh.stat + lw.stat + lbu.stat +
             lhu.stat + sb.stat + sh.stat + sw.stat + addi.stat + slti.stat + sltiu.stat +
             xori.stat + ori.stat + andi.stat + slli.stat + srli.stat + add.stat + sub.stat +
             sll.stat + slt.stat + sltu.stat + xor_.stat + srl.stat + sra.stat + or_.stat +
             and_.stat + flw.stat + fsw.stat + fadd.stat + fsub.stat + fmul.stat + fdiv.stat +
             fsqrt.stat + fsgnj.stat + fsgnjn.stat + fcvt_s_w.stat + fcvt_w_s.stat + feq.stat +
             flt.stat + fle.stat;
    }

    void show_stats(){
        std::vector<Statdata> stats =
            {lui, auipc, jal, jalr, beq, bne, blt,
             bge, bltu, bgeu, lb, lh, lw, lbu,
             lhu, sb, sh, sw, addi, slti, sltiu,
             xori, ori, andi, slli, srli, add, sub,
             sll, slt, sltu, xor_, srl, sra, or_,
             and_, flw, fsw, fadd, fsub, fmul, fdiv,
             fsqrt, fsgnj, fsgnjn, fcvt_s_w, fcvt_w_s, feq,
             flt, fle};

        std::sort(stats.begin(), stats.end());

        for(int i = stats.size() - 1; i >= 0; i--){
            std::cout << stats[i].name << ": " << stats[i].stat << " ";
            if(i % 5 == 4){
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        std::cout << "--> All: " << all() << std::endl;
        std::cout << std::endl;
    }
    
};

