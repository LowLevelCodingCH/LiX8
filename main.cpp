#include <iostream>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cmath>

#define PROGLEN 12
#define PROGADR 0

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
    L7,

    I0, // interrupt vector

    F0,
    F1,
};

enum inst {
    NOP,
    CPY,
    MOV,
    LOD,
    MOVI,
    INC,
    DEC,

    ADD,
    SUB,
    MUL,
    DIV,

    PUSH,
    POP,

    CALL,
    RCALL,
    RET,

    CMP,

    JMP,
    JZ,
    JL,
    JM,
    JNE,

    HLT,

    INT,
    RETI,

    OUT,
    IN,
};

#define cr(I) case I : return #I

std::string intoa(char i)
{
	switch(i) {
		cr(NOP);
		cr(INT);
		cr(CPY);
		cr(MOV);
		cr(MOVI);
		cr(INC);
		cr(DEC);
		cr(ADD);
		cr(SUB);
		cr(MUL);
		cr(DIV);
		cr(PUSH);
		cr(POP);
		cr(CALL);
		cr(RET);
		cr(RETI);
		cr(CMP);
		cr(JMP);
		cr(JZ);
		cr(JL);
		cr(JM);
		cr(JNE);
		cr(HLT);
		cr(OUT);
		cr(IN);
		default: return "UNKNOWN";
	}
}

struct lix {
    std::uint16_t registers[13];
    std::uint16_t inst;
    std::uint16_t arg0;
    std::uint16_t arg1;
    std::uint8_t rmemory[16384];  // Not included by def
    std::uint16_t *memory;

    void printprog() {
        for(int i = 0; i < PROGLEN*3; i++)
            std::cout << (int)(((char*)(this->memory))[PROGADR+i]) << " ";
        std::cout << std::endl;
    }

    void printinst() {
        std::cout << intoa(this->inst) << ", " << (int)this->arg0 << ", " << (int)this->arg1 << std::endl;
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
                        this->registers[reg::F1] = 1;
                if(this->registers[(reg)this->arg0]  <  this->registers[(reg)this->arg1])
                        this->registers[reg::F1] = 2;
                if(this->registers[(reg)this->arg0]  == this->registers[(reg)this->arg1])
                        this->registers[reg::F1] = 0;
            };                                                                                                                                                       break; // break out
            case inst::NOP:                                                                                                                                   break; // break out
	    case inst::HLT: exit(0);
	    case inst::PUSH: this->memory[this->registers[reg::SP]] = this->registers[(reg)this->arg0]; this->registers[reg::SP]++; break;
	    case inst::POP: this->registers[(reg)this->arg0] = this->memory[this->registers[reg::SP]]; this->registers[reg::SP]--; break;
	    case inst::RCALL: 
			    this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			    this->registers[reg::SP]++;
			    this->registers[reg::PC] = this->registers[(reg)this->arg0];
			    break;
	    case inst::CALL: 
			    this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			    this->registers[reg::SP]++;
			    this->registers[reg::PC] = this->arg0;
			    break;
	    case inst::RET:
			    this->registers[reg::PC] = this->memory[this->registers[reg::SP] - 1];
			    break;
            case inst::CPY: this->registers[(reg)this->arg0]                           = this->registers[(reg)this->arg1];                                    break; // break out
            case inst::JMP: this->registers[reg::PC]                                   = this->arg0;                                                          break; // break out
            case inst::JNE: if(this->registers[reg::F1] != 0) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::INT:
	    		    this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			    this->registers[reg::SP]++;
			    this->memory[this->registers[reg::SP]] = this->registers[reg::F0];
			    this->registers[reg::SP]++;
			    this->memory[this->registers[reg::SP]] = this->registers[reg::F1];
			    this->registers[reg::SP]++;
			    this->registers[reg::PC] = this->memory[this->registers[reg::I0] + this->arg0];
			    std::cout << "PC: " << this->registers[reg::PC] << " F0: " << this->registers[reg::F0] << " F1: " << this->registers[reg::F1] << std::endl;
			    break;
	    case inst::RETI:
			    this->registers[reg::F1] = this->memory[this->registers[reg::SP] - 1];
			    this->registers[reg::F0] = this->memory[this->registers[reg::SP] - 2];
			    this->registers[reg::PC] = this->memory[this->registers[reg::SP] - 3];
			    this->registers[reg::SP] -= 3;
			    std::cout << "PC: " << this->registers[reg::PC] << " F0: " << this->registers[reg::F0] << " F1: " << this->registers[reg::F1] << std::endl;
			    break;
	    case inst::JZ:  if(this->registers[reg::F1] == 0) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::JM:  if(this->registers[reg::F1] == 1) this->registers[reg::PC] = this->arg0;                                                          break; // break out
            case inst::JL:  if(this->registers[reg::F1] == 2) this->registers[reg::PC] = this->arg0;                                                          break; // break out
	    case inst::MOV: this->memory[this->registers[(reg)this->arg0]]             = this->registers[(reg)this->arg1];                                    break; // break out
	    case inst::LOD: this->registers[(reg)this->arg1] = this->memory[this->registers[(reg)this->arg0]]; break;
	    case inst::MOVI: this->registers[(reg)this->arg0]             = this->arg1;                                    break; // break out
            case inst::INC: this->registers[(reg)this->arg0]++; break; // break out
            case inst::DEC: this->registers[(reg)this->arg0]--; break; // break out
            case inst::ADD: this->registers[reg::F0]                                   = this->registers[(reg)this->arg1] + this->registers[(reg)this->arg0]; break; // break out
            case inst::MUL: this->registers[reg::F0]                                   = this->registers[(reg)this->arg1] * this->registers[(reg)this->arg0]; break; // break out
            case inst::SUB: this->registers[reg::F0]                                   = this->registers[(reg)this->arg1] - this->registers[(reg)this->arg0]; break; // break out
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
        for(int i = 0; i < 13; i++)
            this->registers[i] = 0;
    }
    
    void init() {
	this->memory = (unsigned short*)&(this->rmemory[0]);
        this->clearmem();
        this->clearreg();
    }

    void load(short *insts, int len) {
    	for(int i = 0; i < len; i++) {
		this->memory[PROGADR+i] = insts[i];
	}
    }
};

// Li X 16 CPU

#define HW_SUP
#undef HW_SUP
#define SGA_ADR 0x20

int main() {
    lix cpu;
    cpu.init();

    cpu.memory[PROGADR+0]  = MOVI;     // state: l0 = 3, nothing else changes
    cpu.memory[PROGADR+1]  = reg::L0;
    cpu.memory[PROGADR+2]  = 3;

    cpu.memory[PROGADR+3]  = CALL; // state: l0 = 3, mem[pc] = pc + 3 (6)
    cpu.memory[PROGADR+4]  = 6;
    cpu.memory[PROGADR+5]  = 0;
    
    cpu.memory[PROGADR+6]  = ADD; // l0 = 3, l7 = 3 + l1, mem[pc] = 6
    cpu.memory[PROGADR+7]  = reg::L0;
    cpu.memory[PROGADR+8]  = reg::L1;
    
    cpu.memory[PROGADR+9]  = RET; // l0 = 3, l7 = 3 + l1, mem[pc] = 0
    cpu.memory[PROGADR+10] = 0;
    cpu.memory[PROGADR+11] = 0;

#define target L1
#define counter L0

	short prog[101] = {0};

std::ifstream file("a.bin");
std::ostringstream a;
a << file.rdbuf();
int i = 0;
for(i = 0; i < a.str().size() / 2; i++) {
	short b = ((short*)(a.str().data()))[i];
	prog[i] = b;
}

    cpu.load(prog, 30);
    cpu.printprog();

    for(int i = 0; i < PROGLEN; i++) {
        cpu.fetch();
	if(cpu.inst == HLT) goto endin;
        cpu.execute();
	cpu.printinst();
	std::cout << "L0: " << (int)cpu.registers[reg::L0] << "\n";
    std::cout << "L1: " << (int)cpu.registers[reg::L1] << "\n";
	std::cout << "F0: " << (int)cpu.registers[reg::F0] << "\n";
	std::cout << "PC: " << (int)cpu.registers[reg::PC] << "\n";

	std::cout << "-----------------------------------------------\n";
    }

endin:
	std::cout << "L0: " << (int)cpu.registers[reg::L0] << "\n";
    std::cout << "L1: " << (int)cpu.registers[reg::L1] << "\n";
	std::cout << "F0: " << (int)cpu.registers[reg::F0] << "\n";
	std::cout << "-----------------------------------------------\n";

    return 0;
}
