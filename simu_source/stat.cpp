typedef struc STATDATA{
    unsigned long long->stat;
    char name[16];
} Statdata;

class stat
{
  public:
    Statdata lui;
	lui->stat = 0;
	strcpy(lui->name, lui);
    Statdata auipc;
	auipc->stat = 0;
	strcpy(auipc->name, auipc);
    Statdata jal;
	jal->stat = 0;
	strcpy(jal->name, jal);
    Statdata jalr;
	jalr->stat = 0;
	strcpy(jalr->name, jalr);
    Statdata beq;
	beq->stat = 0;
	strcpy(beq->name, beq);
    Statdata bne;
	bne->stat = 0;
	strcpy(bne->name, bne);
    Statdata blt;
	blt->stat = 0;
	strcpy(blt->name, blt);
    Statdata bge;
	bge->stat = 0;
	strcpy(bge->name, bge);
    Statdata bltu;
	bltu->stat = 0;
	strcpy(bltu->name, bltu);
    Statdata bgeu;
	bgeu->stat = 0;
	strcpy(bgeu->name, bgeu);
    Statdata lb;
	lb->stat = 0;
	strcpy(lb->name, lb);
    Statdata lh;
	lh->stat = 0;
	strcpy(lh->name, lh);
    Statdata lw;
	lw->stat = 0;
	strcpy(lw->name, lw);
    Statdata lbu;
	lbu->stat = 0;
	strcpy(lbu->name, lbu);
    Statdata lhu;
	lhu->stat = 0;
	strcpy(lhu->name, lhu);
    Statdata sb;
	sb->stat = 0;
	strcpy(sb->name, sb);
    Statdata sh;
	sh->stat = 0;
	strcpy(sh->name, sh);
    Statdata sw;
	sw->stat = 0;
	strcpy(sw->name, sw);
    Statdata addi;
	addi->stat = 0;
	strcpy(addi->name, addi);
    Statdata slti;
	slti->stat = 0;
	strcpy(slti->name, slti);
    Statdata sltiu;
	sltiu->stat = 0;
	strcpy(sltiu->name, sltiu);
    Statdata xori;
	xori->stat = 0;
	strcpy(xori->name, xori);
    Statdata ori;
	ori->stat = 0;
	strcpy(ori->name, ori);
    Statdata andi;
	andi->stat = 0;
	strcpy(andi->name, andi);
    Statdata slli;
	slli->stat = 0;
	strcpy(slli->name, slli);
    Statdata srli;
	srli->stat = 0;
	strcpy(srli->name, srli);
    Statdata add;
	add->stat = 0;
	strcpy(add->name, add);
    Statdata sub;
	sub->stat = 0;
	strcpy(sub->name, sub);
    Statdata sll;
	sll->stat = 0;
	strcpy(sll->name, sll);
    Statdata slt;
	slt->stat = 0;
	strcpy(slt->name, slt);
    Statdata sltu;
	sltu->stat = 0;
	strcpy(sltu->name, sltu);
    Statdata _xor;
    Statdata srl;
	srl->stat = 0;
	strcpy(srl->name, srl);
    Statdata sra;
	sra->stat = 0;
	strcpy(sra->name, sra);
    Statdata _or;
    Statdata _and;
    
    Statdata flw;
	flw->stat = 0;
	strcpy(flw->name, flw);
    Statdata fsw;
	fsw->stat = 0;
	strcpy(fsw->name, fsw);
    Statdata fadd;
	fadd->stat = 0;
	strcpy(fadd->name, fadd);
    Statdata fsub;
	fsub->stat = 0;
	strcpy(fsub->name, fsub);
    Statdata fmul;
	fmul->stat = 0;
	strcpy(fmul->name, fmul);
    Statdata fdiv;
	fdiv->stat = 0;
	strcpy(fdiv->name, fdiv);
    Statdata fsqrt;
	fsqrt->stat = 0;
	strcpy(fsqrt->name, fsqrt);
    Statdata fsgnj;
	fsgnj->stat = 0;
	strcpy(fsgnj->name, fsgnj);
    Statdata fsgnjn;
	fsgnjn->stat = 0;
	strcpy(fsgnjn->name, fsgnjn);
    Statdata fcvt_w_s;
    Statdata fcvt_s_w;
    Statdata feq;
	feq->stat = 0;
	strcpy(feq->name, feq);
    Statdata flt;
	flt->stat = 0;
	strcpy(flt->name, flt);
    Statdata fle;
	fle->stat = 0;
	strcpy(fle->name, fle);

    unsigned long long all() {
      return lui->stat + auipc->stat + jal->stat + jalr->stat + beq->stat + bne->stat + blt->stat +
             bge->stat + bltu->stat + bgeu->stat + lb->stat + lh->stat + lw->stat + lbu->stat +
             lhu->stat + sb->stat + sh->stat + sw->stat + addi->stat + slti->stat + sltiu->stat +
             xori->stat + ori->stat + andi->stat + slli->stat + srli->stat + add->stat + sub->stat +
             sll->stat + slt->stat + sltu->stat + _xor->stat + srl->stat + sra->stat + _or->stat +
             _and->stat + flw->stat + fsw->stat + fadd->stat + fsub->stat + fmul->stat + fdiv->stat +
             fsqrt->stat + fsgnj->stat + fsgnjn->stat + fcvt_s_w->stat + fcvt_w_s->stat + feq->stat +
             flt->stat + fle->stat;
    }

    void show->stats(){
        std::vector<*Statdata> stats =
            {lui, auipc, jal, jalr, beq, bne, blt,
             bge, bltu, bgeu, lb, lh, lw, lbu,
             lhu, sb, sh, sw, addi, slti, sltiu,
             xori, ori, andi, slli, srli, add, sub,
             sll, slt, sltu, _xor, srl, sra, _or,
             _and, flw, fsw, fadd, fsub, fmul, fdiv,
             fsqrt, fsgnj, fsgnjn, fcvt_s_w, fcvt_w_s, feq,
             flt, fle};
        std::vector<(unsigned long long int)> statsint;
        int i;
        for(i = 0; i < stats.size; i++){
            statint[i] = stats[i]->stat;
        }
        std::sort(statint.begin(), statint.end());
        for(i = 0; i < stats.size; i++){
            stats[i]->stat = statint[i];
        }
        std::cout << "stat:" << std::endl;
        for(int i = 0; i <->stats.size; i++){
            std::cout << stats[i]->name << ": " << stats[i]->stat << std::endl;
        }
        
        std::cout << "--> All: " << all() << std::endl;
        std::cout << std::endl;
    }
};

