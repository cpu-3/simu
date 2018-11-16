//TODO sort
class Stat
{
  public:
    unsigned long long stat_lui = 0;
    unsigned long long stat_auipc = 0;
    unsigned long long stat_jal = 0;
    unsigned long long stat_jalr = 0;
    unsigned long long stat_beq = 0;
    unsigned long long stat_bne = 0;
    unsigned long long stat_blt = 0;
    unsigned long long stat_bge = 0;
    unsigned long long stat_bltu = 0;
    unsigned long long stat_bgeu = 0;
    unsigned long long stat_lb = 0;
    unsigned long long stat_lh = 0;
    unsigned long long stat_lw = 0;
    unsigned long long stat_lbu = 0;
    unsigned long long stat_lhu = 0;
    unsigned long long stat_sb = 0;
    unsigned long long stat_sh = 0;
    unsigned long long stat_sw = 0;
    unsigned long long stat_addi = 0;
    unsigned long long stat_slti = 0;
    unsigned long long stat_sltiu = 0;
    unsigned long long stat_xori = 0;
    unsigned long long stat_ori = 0;
    unsigned long long stat_andi = 0;
    unsigned long long stat_slli = 0;
    unsigned long long stat_srli = 0;
    unsigned long long stat_add = 0;
    unsigned long long stat_sub = 0;
    unsigned long long stat_sll = 0;
    unsigned long long stat_slt = 0;
    unsigned long long stat_sltu = 0;
    unsigned long long stat_xor = 0;
    unsigned long long stat_srl = 0;
    unsigned long long stat_sra = 0;
    unsigned long long stat_or = 0;
    unsigned long long stat_and = 0;
    
    unsigned long long stat_flw = 0;
    unsigned long long stat_fsw = 0;
    unsigned long long stat_fadd = 0;
    unsigned long long stat_fsub = 0;
    unsigned long long stat_fmul = 0;
    unsigned long long stat_fdiv = 0;
    unsigned long long stat_fsqrt = 0;
    unsigned long long stat_fsgnj = 0;
    unsigned long long stat_fsgnjn = 0;
    unsigned long long stat_fcvt_w_s = 0;
    unsigned long long stat_fcvt_s_w = 0;
    unsigned long long stat_feq = 0;
    unsigned long long stat_flt = 0;
    unsigned long long stat_fle = 0;

    unsigned long long all() {
      return stat_lui + stat_auipc + stat_jal + stat_jalr + stat_beq + stat_bne + stat_blt + 
             stat_bge + stat_bltu + stat_bgeu + stat_lb + stat_lh + stat_lw + stat_lbu + 
             stat_lhu + stat_sb + stat_sh + stat_sw + stat_addi + stat_slti + stat_sltiu + 
             stat_xori + stat_ori + stat_andi + stat_slli + stat_srli + stat_add + stat_sub +
             stat_sll + stat_slt + stat_sltu + stat_xor + stat_srl + stat_sra + stat_or +
             stat_and + stat_flw + stat_fsw + stat_fadd + stat_fsub + stat_fmul + stat_fdiv +
             stat_fsqrt + stat_fsgnj + stat_fsgnjn + stat_fcvt_s_w + stat_fcvt_w_s + stat_feq +
             stat_flt + stat_fle;
    }

    void show_stats(){
      std::cout <<"Stat:" << std::endl;
      std::cout << "lui: "   << stat_lui;
      std::cout << " auipc: " << stat_auipc;
      std::cout << " jal: "   << stat_jal;
      std::cout << " jalr: "  << stat_jalr  << std::endl;
      std::cout << "beq: "   << stat_beq;
      std::cout << " bne: "   << stat_bne;
      std::cout << " blt: "   << stat_blt;
      std::cout << " bge: "   << stat_bge   << std::endl;
      std::cout << "bltu: "  << stat_bltu;
      std::cout << " bgeu: "  << stat_bgeu;
      std::cout << " lb: "    << stat_lb;
      std::cout << " lh: "    << stat_lh    << std::endl;
      std::cout << "lw: "    << stat_lw;
      std::cout << " lbu: "   << stat_lbu;
      std::cout << " lhu: "   << stat_lhu;
      std::cout << " sb: "    << stat_sb    << std::endl;
      std::cout << "sh: "    << stat_sh;
      std::cout << " sw: "    << stat_sw;
      std::cout << " addi: "  << stat_addi;
      std::cout << " slti: "  << stat_slti  << std::endl;
      std::cout << "sltiu: " << stat_sltiu;
      std::cout << " xori: "  << stat_xori;
      std::cout << " ori: "   << stat_ori;
      std::cout << " andi: "  << stat_andi  << std::endl;
      std::cout << "slli: "  << stat_slli;
      std::cout << " srli: "  << stat_srli;
      std::cout << " add: "   << stat_add;
      std::cout << " sub: "   << stat_sub   << std::endl;
      std::cout << "sll: "   << stat_sll;
      std::cout << " slt: "   << stat_slt;
      std::cout << " sltu: "  << stat_sltu;
      std::cout << " xor: "   << stat_xor   << std::endl;
      std::cout << " srl: "   << stat_srl;
      std::cout << " sra: "   << stat_sra;
      std::cout << " or: "    << stat_or;
      std::cout << " and: "   << stat_and   << std::endl;

      std::cout << "flw: "   << stat_flw;
      std::cout << " fsw: "   << stat_fsw;
      std::cout << " fadd: "  << stat_fadd;
      std::cout << " fsub: "  << stat_fsub  << std::endl;
      std::cout << "fmul: "  << stat_fmul;
      std::cout << " fdiv: "  << stat_fdiv;
      std::cout << " fsqrt: " << stat_fsqrt << std::endl;
      std::cout << "fsgnj: " << stat_fsgnj;
      std::cout << " fsgnjn: " << stat_fsgnjn;
      std::cout << " fcvt_w_s: " << stat_fcvt_w_s;
      std::cout << " fcvt_s_w: " << stat_fcvt_s_w << std::endl;
      std::cout << "feq: "   << stat_feq;
      std::cout << " flt: "   << stat_flt;
      std::cout << " fle: "   << stat_fle   << std::endl;
      std::cout << "--> All: " << all() << std::endl;
 
      std::cout << std::endl;
    }
};

