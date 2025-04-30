#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#define PROGLEN 51
#define PROGADR 0

enum reg
{
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

	F0, // unused
	F1,
};

enum inst
{
	NOP,
	CPY,
	MOVM,
	LOD,
	MOV,
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

#define cr(I)                                                                                         \
	case I:                                                                                       \
		return #I

std::string intoa(short i)
{
	switch (i)
	{
		cr(NOP);
		cr(INT);
		cr(CPY);
		cr(MOVM);
		cr(MOV);
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
		default:
			return "UNKNOWN";
	}
}

struct lix
{
	std::uint16_t registers[13];
	std::uint16_t inst;
	std::uint16_t arg0;
	std::uint16_t arg1;
	std::uint8_t rmemory[16384]; // Not included by def
	std::uint16_t *memory;

	void printprog()
	{
		for (int i = 0; i < PROGLEN * 3; i++)
			std::cout << (int) (((char *) (this->memory))[PROGADR + i]) << " ";
		std::cout << std::endl;
	}

	void printinst()
	{
		std::cout << intoa(this->inst) << " " << (int) this->arg0 << ", " << (int) this->arg1
			  << std::endl;
	}

	void fetch()
	{
		this->inst = this->memory[PROGADR + this->registers[reg::PC]];
		this->arg0 = this->memory[PROGADR + this->registers[reg::PC] + 1];
		this->arg1 = this->memory[PROGADR + this->registers[reg::PC] + 2];
		this->registers[reg::PC] += 3;
	}

	void execute()
	{
		switch (this->inst)
		{
			case inst::CMP:
			{
				if (this->registers[(reg) this->arg0] >
				    this->registers[(reg) this->arg1])
					this->registers[reg::F1] = 1;
				if (this->registers[(reg) this->arg0] <
				    this->registers[(reg) this->arg1])
					this->registers[reg::F1] = 2;
				if (this->registers[(reg) this->arg0] ==
				    this->registers[(reg) this->arg1])
					this->registers[reg::F1] = 0;
			};
			break; // break out
			case inst::NOP:
				break; // break out
			case inst::HLT:
				exit(0);
			case inst::PUSH:
				this->memory[this->registers[reg::SP]] =
				    this->registers[(reg) this->arg0];
				this->registers[reg::SP]++;
				break;
			case inst::POP:
				this->registers[(reg) this->arg0] =
				    this->memory[this->registers[reg::SP]];
				this->registers[reg::SP]--;
				break;
			case inst::RCALL:
				this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->registers[reg::PC] = this->registers[(reg) this->arg0];
				break;
			case inst::CALL:
				this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->registers[reg::PC] = this->arg0;
				break;
			case inst::RET:
				this->registers[reg::PC] = this->memory[this->registers[reg::SP] - 1];
				break;
			case inst::CPY:
				this->registers[(reg) this->arg0] = this->registers[(reg) this->arg1];
				break; // break out
			case inst::JMP:
				this->registers[reg::PC] = this->arg0;
				break; // break out
			case inst::JNE:
				if (this->registers[reg::F1] != 0)
					this->registers[reg::PC] = this->arg0;
				break; // break out
			case inst::INT:
				this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory[this->registers[reg::SP]] = this->registers[reg::F0];
				this->registers[reg::SP]++;
				this->memory[this->registers[reg::SP]] = this->registers[reg::F1];
				this->registers[reg::SP]++;
				this->registers[reg::PC] =
				    this->memory[this->registers[reg::I0] + this->arg0];
				std::cout << "PC: " << this->registers[reg::PC]
					  << " F0: " << this->registers[reg::F0]
					  << " F1: " << this->registers[reg::F1] << std::endl;
				break;
			case inst::RETI:
				this->registers[reg::F1] = this->memory[this->registers[reg::SP] - 1];
				this->registers[reg::F0] = this->memory[this->registers[reg::SP] - 2];
				this->registers[reg::PC] = this->memory[this->registers[reg::SP] - 3];
				this->registers[reg::SP] -= 3;
				std::cout << "PC: " << this->registers[reg::PC]
					  << " F0: " << this->registers[reg::F0]
					  << " F1: " << this->registers[reg::F1] << std::endl;
				break;
			case inst::JZ:
				if (this->registers[reg::F1] == 0)
					this->registers[reg::PC] = this->arg0;
				break; // break out
			case inst::JM:
				if (this->registers[reg::F1] == 1)
					this->registers[reg::PC] = this->arg0;
				break; // break out
			case inst::JL:
				if (this->registers[reg::F1] == 2)
					this->registers[reg::PC] = this->arg0;
				break; // break out
			case inst::MOVM:
				this->memory[this->registers[(reg) this->arg0]] =
				    this->registers[(reg) this->arg1];
				break; // break out
			case inst::LOD:
				this->registers[(reg) this->arg1] =
				    this->memory[this->registers[(reg) this->arg0]];
				break;
			case inst::MOV:
				this->registers[(reg) this->arg0] = this->arg1;
				break; // break out
			case inst::INC:
				this->registers[(reg) this->arg0]++;
				break; // break out
			case inst::DEC:
				this->registers[(reg) this->arg0]--;
				break; // break out
			case inst::ADD:
				this->registers[(reg) this->arg1] = this->registers[(reg) this->arg1] +
								    this->registers[(reg) this->arg0];
				break; // break out
			case inst::MUL:
				this->registers[(reg) this->arg1] = this->registers[(reg) this->arg1] *
								    this->registers[(reg) this->arg0];
				break; // break out
			case inst::SUB:
				this->registers[(reg) this->arg1] = this->registers[(reg) this->arg1] -
								    this->registers[(reg) this->arg0];
				break; // break out
			case inst::IN:
				break; // break out
			case inst::OUT:
				break; // break out
			default:
				break; // break out
		}
	}

	void clearmem()
	{
		for (int i = 0; i < 4096; i++)
			this->memory[i] = 0;
	}

	void clearreg()
	{
		for (int i = 0; i < 13; i++)
			this->registers[i] = 0;
	}

	void init()
	{
		this->memory = (unsigned short *) &(this->rmemory[0]);
		this->clearmem();
		this->clearreg();
	}

	void load(short *insts, int len)
	{
		for (int i = 0; i < len; i++)
		{
			this->memory[PROGADR + i] = insts[i];
		}
	}
};

// Li X 16 CPU

#define HW_SUP
#undef HW_SUP
#define SGA_ADR 0x20

int main()
{
	lix cpu = {0};
	cpu.init();

	short prog[101] = {0};

	std::ifstream file("a.bin");
	std::ostringstream a;
	a << file.rdbuf();
	int i = 0;
	for (i = 0; i < a.str().size() / 2; i++)
	{
		short b = ((short *) (a.str().data()))[i];
		prog[i] = b;
	}

	cpu.load(prog, 51);
	cpu.printprog();

	for (int i = 0; i < PROGLEN; i++)
	{
		cpu.fetch();
		if (cpu.inst == HLT)
			goto endin;
		cpu.execute();
		cpu.printinst();
	}

endin:
	return 0;
}
