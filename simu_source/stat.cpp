//TODO sort
class Stat
{
  public:
    int stat_lui = 0;
    int stat_auipc = 0;
    int stat_jal = 0;
    int stat_jalr = 0;
    int stat_beq = 0;
    int stat_bne = 0;
    int stat_blt = 0;
    int stat_bge = 0;
    int stat_bltu = 0;
    int stat_bgeu = 0;
    int stat_lb = 0;
    int stat_lh = 0;
    int stat_lw = 0;
    int stat_lbu = 0;
    int stat_lhu = 0;
    int stat_sb = 0;
    int stat_sh = 0;
    int stat_sw = 0;
    int stat_addi = 0;
    int stat_slti = 0;
    int stat_sltiu = 0;
    int stat_xori = 0;
    int stat_ori = 0;
    int stat_andi = 0;
    int stat_slli = 0;
    int stat_srli = 0;
    int stat_add = 0;
    int stat_sub = 0;
    int stat_sll = 0;
    int stat_slt = 0;
    int stat_sltu = 0;
    int stat_xor = 0;
    int stat_srl = 0;
    int stat_sra = 0;
    int stat_or = 0;
    int stat_and = 0;
    
    int stat_flw = 0;
    int stat_fsw = 0;
    int stat_fadd = 0;
    int stat_fsub = 0;
    int stat_fmul = 0;
    int stat_fdiv = 0;
    int stat_fsqrt = 0;
    int stat_fsgnj = 0;
    int stat_fsgnjn = 0;
    int stat_fcvt_w_s = 0;
    int stat_fcvt_s_w = 0;
    int stat_feq = 0;
    int stat_flt = 0;
    int stat_fle = 0;

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
      std::cout << "xor: "   << stat_xor;
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
      std::cout << " fdiv: "  << stat_fdiv;
      std::cout << " fsqrt: " << stat_fsqrt << std::endl;
      std::cout << "fsgnj: " << stat_fsgnj;
      std::cout << " fsgnjn: " << stat_fsgnjn;
      std::cout << " fcvt_w_s: " << stat_fcvt_w_s;
      std::cout << " fcvt_s_w: " << stat_fcvt_s_w << std::endl;
      std::cout << "feq: "   << stat_feq;
      std::cout << " flt: "   << stat_flt;
      std::cout << " fle: "   << stat_fle   << std::endl;
 
      std::cout << std::endl;
    }
};

