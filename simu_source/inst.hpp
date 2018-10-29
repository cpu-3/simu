enum struct Inst : uint8_t {
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
    FLOAD   = 0b0000111,
    FSTORE  = 0b0100111,
    FPU     = 0b1010011,
};

enum struct ALUI_Inst : uint8_t{
    ADDI  = 0b000,
    SLTI  = 0b010,
    SLTIU = 0b011,
    XORI  = 0b100,
    ORI   = 0b110,
    ANDI  = 0b111,
    SLLI  = 0b001,
    SRLI  = 0b101,
};

enum struct ALUI_SRL_Inst : uint8_t {
    SRLI = 0b0000000,
    SRAI = 0b0100000,
};

enum struct ALU_Inst : uint8_t{
    ADD_SUB = 0b000,
    SLL     = 0b001,
    SLT     = 0b010,
    SLTU    = 0b011,
    XOR     = 0b100,
    SR      = 0b101,
    OR      = 0b110,
    AND     = 0b111,
};

enum struct FPU_Inst : uint8_t{
    FADD  = 0b0000000,
    FSUB  = 0b0000100,
    FMUL  = 0b0001000,
    FDIV  = 0b0001100,
    FSQRT = 0b0101100,
    FCVT_W_S = 0b1100000,
    FCOMP    = 0b1010000,
    FMV_X_W  = 0b1110000,
    FCVT_S_W = 0b1101000,
    FMV_W_X  = 0b1111000,
    FSGNJ = 0b0010000,
};

enum struct FLoad_Inst : uint8_t{
    FLW = 0b010,
};

enum struct FStore_Inst : uint8_t{
    FSW = 0b010,
};

enum struct FComp_Inst : uint8_t{
    FEQ = 0b010,
    FLT = 0b001,
    FLE = 0b000,
};

enum struct ALU_ADD_SUB_Inst : uint8_t {
    ADD = 0b0000000,
    SUB = 0b0100000,
};

enum struct ALU_SR_Inst : uint8_t {
    SRL = 0b0000000,
    SRA = 0b0100000,
};

enum struct Branch_Inst : uint8_t {
    BEQ     = 0b000,
    BNE     = 0b001,
    BLT     = 0b100,
    BGE     = 0b101,
    BLTU    = 0b110,
    BGEU    = 0b111,
};

enum struct Fence_Inst : uint8_t {
    FENCE   = 0b000,
    FENCEI  = 0b001,
};

enum struct Load_Inst : uint8_t {
    LB  = 0b000,
    LH  = 0b001,
    LW  = 0b010,
    LBU = 0b100,
    LHU = 0b101,
};

enum struct Store_Inst : uint8_t {
    SB = 0b000,
    SH = 0b001,
    SW = 0b010
};

enum struct FSGNJ_Inst : uint8_t {
    FSGNJ = 0b000,
    FSGNJN = 0b001,
    FSGNJX = 0b010
};
