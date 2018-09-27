enum struct Inst {
    LUI     = 0b0110111,
    AUIPC   = 0b0010111,
    JAL     = 0b1101111,
    JALR    = 0b1100111,
    BRANCH  = 0b1100011,
    LOAD    = 0b0000011,
    STORE   = 0b0100011,
    ALUI    = 0b0010011,
    ALU     = 0b0110011,
    FENCE   = 0b0001111,
    ECALL   = 0b1110011,
    EBREAK  = 0b1110011,
};

enum struct ALUI_Inst {
    ADDI  = 0b000,
    SLTI  = 0b010,    
    SLTIU = 0b011,    
    XORI  = 0b100,    
    ORI   = 0b110,    
    ANDI  = 0b111,    
    SLLI  = 0b001,    
    SRL_LA  = 0b101,    
};

enum struct ALUI_SRL_Inst {
    SRLI = 0b0000000,
    SRAI = 0b0100000,
};

enum struct ALU_Inst {
    ADD_SUB = 0,
    SLL,
    SLT,
    SLTU,
    XOR,
    SR,
    OR,
    AND,
};

enum struct ALU_ADD_SUB_Inst {
    ADD = 0b0000000,
    SUB = 0b0100000,
};

enum struct ALU_SR_Inst {
    SRL = 0b0000000,
    SRA = 0b0100000,
};

enum struct Branch_Inst {
    BEQ     = 0b000,
    BNE     = 0b001,
    BLT     = 0b100,
    BGE     = 0b101,
    BLTU    = 0b110,
    BGEU    = 0b111,
};

enum struct Fence_Inst {
    FENCE   = 0b000,
    FENCEI  = 0b001,
};

enum struct Load_Inst {
    LB  = 0b0,
    LH  = 0b1,
    LW  = 0b10,
    LBU = 0b100,
};

enum struct Store_Inst {
    SB = 0,
    SH,
    SW,
};