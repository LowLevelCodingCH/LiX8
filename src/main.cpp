#include <iostream>
#include <cstdint>
#include <cmath>

#define PROGLEN 85
#define PROGADR 8

enum reg {
    PC, // program counter
    SP, // stack pointer

    L0,
    L1,
    L2,
    L3,
    L4,
    L5,
    L6,
    L7
};

enum inst {
    NOP,
    CPY,
    MOV,

    ADD,
    SUB,
    MUL,
    DIV,

    CMP,

    JMP,
    JZ,
    JL,
    JM,
    JNE,

    OUT,
    IN,
};

struct lix {
    std::uint8_t registers[10];
    std::uint8_t inst;
    std::uint8_t arg0;
    std::uint8_t arg1;
    std::uint8_t memory[4096];  // Not included by def

    void printprog() {
        for(int i = 0; i < PROGLEN*3; i++)
            std::cout << (int)this->memory[PROGADR+i] << " ";
        std::cout << std::endl;
    }

    void printinst() {
        std::cout << (int)this->inst << ", " << (int)this->arg0 << ", " << (int)this->arg1 << std::endl;
    }

    void fetch() {
        this->inst = this->memory[PROGADR+this->registers[reg::PC]];
        this->arg0 = this->memory[PROGADR+this->registers[reg::PC]+1];
        this->arg1 = this->memory[PROGADR+this->registers[reg::PC]+2];
        this->registers[reg::PC]+=3;
    }

    void execute() {
        switch(this->inst) {
            case inst::CMP: {
                if(this->registers[(reg)this->arg0]  >  this->registers[(reg)this->arg1])
                        this->registers[reg::L7] = 1;
                if(this->registers[(reg)this->arg0]  <  this->registers[(reg)this->arg1])
                        this->registers[reg::L7] = 2;
                if(this->registers[(reg)this->arg0]  == this->registers[(reg)this->arg1])
                        this->registers[reg::L7] = 0;
            };                                                                                                                                                       break; // break out
            case inst::NOP:                                                                                                                                   break; // break out
            case inst::CPY: this->registers[(reg)this->arg0]                           = this->registers[(reg)this->arg1];                                    break; // break out
            case inst::JMP: this->registers[reg::PC]                                   = this->arg0;                                                          break; // break out
            case inst::JNE: if(this->registers[reg::L7] != 0) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::JZ:  if(this->registers[reg::L7] == 0) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::JM:  if(this->registers[reg::L7] == 1) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::JL:  if(this->registers[reg::L7] == 2) this->registers[reg::PC] = this->arg0;                                                          break; // break out
	    case inst::MOV: this->memory[this->registers[(reg)this->arg0]]             = this->registers[(reg)this->arg1];                                    break; // break out
            case inst::ADD: this->registers[reg::L7]                                   = this->registers[(reg)this->arg1] + this->registers[(reg)this->arg0]; break; // break out
            case inst::MUL: this->registers[reg::L7]                                   = this->registers[(reg)this->arg1] * this->registers[(reg)this->arg0]; break; // break out
            case inst::SUB: this->registers[reg::L7]                                   = this->registers[(reg)this->arg1] - this->registers[(reg)this->arg0]; break; // break out
            case inst::IN:                                                                                                                                    break; // break out
            case inst::OUT:                                                                                                                                   break; // break out
            default:                                                                                                                                                 break; // break out
        }
    }

    void clearmem() {
        for(int i = 0; i < 4096; i++)
            this->memory[i] = 0;
    }

    void clearreg() {
        for(int i = 0; i < 10; i++)
            this->registers[i] = 0;
    }

    void init() {
        this->clearmem();
        this->clearreg();
    }
};

// Li X 8 CPU

#define HW_SUP
#undef HW_SUP
#define SGA_ADR 0x20

int main() {
    lix cpu;
    cpu.init();

    cpu.memory[PROGADR+0]  = 0;
    cpu.memory[PROGADR+1]  = 0;
    cpu.memory[PROGADR+2]  = 0;
    
    cpu.memory[PROGADR+3]  = 0;
    cpu.memory[PROGADR+4]  = 0;
    cpu.memory[PROGADR+5]  = 0;
    
    cpu.memory[PROGADR+6]  = 0;
    cpu.memory[PROGADR+7]  = 0;
    cpu.memory[PROGADR+8]  = 0;

    cpu.memory[PROGADR+9]  = 0;
    cpu.memory[PROGADR+10] = 0;
    cpu.memory[PROGADR+11] = 0;

    for(int i = 0; i < PROGLEN; i++) {
        cpu.fetch();
        cpu.execute();
    }
}
