#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * @author LowLevelCodingCH (Alex), 2025
 * @copyright BSD 2 Clause, (c) LowLevelCodingCH
 */

#define PROGLEN 100
#define PROGADR 0

enum excep {
	INV_OPCODE,
	DIV_BY_ZERO,
	DOUBLE_FLT,
	PROT_FLT
};
/**
 * @brief PROT_HI_0 is the highest priv
 */
enum prot {
	PROT_HI_0,
	PROT_LO_1
};

enum reg {
	PC,
	SP,
	L0,
	L1,
	L2,
	L3,
	L4,
	L5,
	L6,
	L7,
	LR,
	S0,
	S1,
	S2,
};

enum inst {
	NOP,
	CPY,
	STR,
	LDR,
	MOV,
	INC,
	DEC,
	ADD,
	SUB,
	MUL,
	DIV,
	PUSH,
	POP,
	RBL,
	RET,
	CMP,
	B,
	BIZ,
	BL,
	BIM,
	BNZ,
	BIL,
	HLT,
	SWI,
	SVC,
	SVCSTR,
	IRET,
	OUT,
	IN,
};

#define cr(I)                                                         \
	case I:                                                       \
		return #I
/**
 * @brief Returns string name of an instruction
 * @param i Instruction to return the name of
 */
std::string intoa(short i)
{
	switch (i) {
		cr(NOP);
		cr(SVC);
		cr(SVCSTR);
		cr(CPY);
		cr(STR);
		cr(MOV);
		cr(INC);
		cr(DEC);
		cr(ADD);
		cr(SUB);
		cr(MUL);
		cr(DIV);
		cr(PUSH);
		cr(POP);
		cr(BL);
		cr(RET);
		cr(IRET);
		cr(CMP);
		cr(SWI);
		cr(B);
		cr(BIZ);
		cr(BIM);
		cr(BIL);
		cr(BNZ);
		cr(HLT);
		cr(OUT);
		cr(IN);
	default:
		return "UNKNOWN";
	}
}

/**
 * @struct lix
 * @brief A structure representing a simple virtual machine with
 * registers, memory, and instruction execution capabilities.
 *
 * @var lix::registers
 * Array of 13 16-bit registers used for general-purpose and
 * special-purpose operations.
 *
 * @var lix::inst
 * The current instruction to be executed.
 *
 * @var lix::arg0
 * The first operand of the current instruction.
 *
 * @var lix::arg1
 * The second operand of the current instruction.
 *
 * @var lix::rmemory
 * A raw memory buffer of 16 KB (16384 bytes).
 *
 * @var lix::memory
 * A pointer to lix::rmemory but it is 16 bits per entry because PC
 * increments in words not bytes.
 *
 * @fn lix::printprog()
 * @brief Prints the program loaded in memory starting from the program
 * address.
 *
 * @fn lix::printinst()
 * @brief Prints the current instruction and its operands in a readable
 * format.
 *
 * @fn lix::fetch()
 * @brief Fetches the next instruction and its operands from memory and
 * updates the program counter (PC).
 *
 * @fn lix::execute()
 * @brief Executes the current instruction.
 *
 * @fn lix::clearmem()
 * @brief Clears the memory by setting all memory locations to zero.
 *
 * @fn lix::clearreg()
 * @brief Clears all registers by setting them to zero.
 *
 * @fn lix::init()
 * @brief Clears the memory and registers.
 *
 * @fn lix::load(short* insts, int len)
 * @brief Loads a program (array of instructions) into memory at the
 * program address.
 * @param insts Pointer to the array of instructions to load.
 * @param len The number of instructions to load.
 */
struct lix {
	std::uint16_t registers[14];
	std::uint16_t inst;
	std::uint16_t arg0;
	std::uint16_t arg1;
	std::uint8_t rmemory[16384]; // Not included by def
	std::uint16_t *memory;

	/**
	 * @brief Prints a program
	 */
	void printprog()
	{
		for (int i = 0; i < PROGLEN * 3; i++)
			std::cout
			    << (int) ((
				   (char *) (this->memory))[PROGADR +
							    i])
			    << " ";
		std::cout << std::endl;
	}

	/**
	 * @brief Prints an instruction
	 */
	void printinst()
	{
		std::cout << intoa(this->inst) << " "
			  << (int) this->arg0 << ", "
			  << (int) this->arg1 << std::endl;
	}

	/**
	 * @brief Fetches an instruction
	 */
	void fetch()
	{
		this->inst = this->memory[this->registers[reg::PC]];
		this->arg0 =
		    this->memory[this->registers[reg::PC] + 1];
		this->arg1 =
		    this->memory[this->registers[reg::PC] + 2];
		this->registers[reg::PC] += 3;
	}

	/**
	 * @brief Executes the instruction in inst, arg0 and arg1
	 */
	void execute()
	{
		excep cexcp;
		if (this->registers[reg::S2] == prot::PROT_HI_0) {
			switch (this->inst) {
			case inst::SVCSTR:
				this->registers[reg::LR] = this->arg0;
				goto dfe;
			case inst::SWI:
				this->registers[reg::S2] = this->arg0;
				goto dfe;
			case inst::IN:
				goto dfe;
			case inst::OUT:
				goto dfe;
			case inst::IRET:
				this->registers[reg::S1] =
				    this->memory
					[this->registers[reg::SP] - 1];
				this->registers[reg::S0] =
				    this->memory
					[this->registers[reg::SP] - 2];
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::SP] - 3];
				this->registers[reg::SP] -= 3;
				this->registers[reg::S2] =
				    prot::PROT_LO_1;
				goto dfe;
			}
		} else { // see if low priv user tries to access high
			 // priv func
			switch (this->inst) {
			case inst::SVCSTR:
				cexcp = excep::PROT_FLT;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S0];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S1];
				this->registers[reg::SP]++;
				if (this->memory
					[this->registers[reg::LR] +
					 cexcp] == 0)
					cexcp = excep::DOUBLE_FLT;
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::LR] +
					 cexcp];
				goto dfe;
			case inst::SWI:
				cexcp = excep::PROT_FLT;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S0];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S1];
				this->registers[reg::SP]++;
				if (this->memory
					[this->registers[reg::LR] +
					 cexcp] == 0)
					cexcp = excep::DOUBLE_FLT;
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::LR] +
					 cexcp];
				goto dfe;
			case inst::IN:
				cexcp = excep::PROT_FLT;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S0];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S1];
				this->registers[reg::SP]++;
				if (this->memory
					[this->registers[reg::LR] +
					 cexcp] == 0)
					cexcp = excep::DOUBLE_FLT;
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::LR] +
					 cexcp];
				goto dfe;
			case inst::OUT:
				cexcp = excep::PROT_FLT;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S0];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S1];
				this->registers[reg::SP]++;
				if (this->memory
					[this->registers[reg::LR] +
					 cexcp] == 0)
					cexcp = excep::DOUBLE_FLT;
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::LR] +
					 cexcp];
				goto dfe;
			case inst::IRET:
				cexcp = excep::PROT_FLT;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::PC];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S0];
				this->registers[reg::SP]++;
				this->memory
				    [this->registers[reg::SP]] =
				    this->registers[reg::S1];
				this->registers[reg::SP]++;
				this->registers[reg::S2] =
				    prot::PROT_LO_1;
				if (this->memory
					[this->registers[reg::LR] +
					 cexcp] == 0)
					cexcp = excep::DOUBLE_FLT;
				this->registers[reg::PC] =
				    this->memory
					[this->registers[reg::LR] +
					 cexcp];
				goto dfe;
			}
		}

		switch (this->inst) {
		case inst::CMP:
			if (this->registers[(reg) this->arg0] >
			    this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 1;
			if (this->registers[(reg) this->arg0] <
			    this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 2;
			if (this->registers[(reg) this->arg0] ==
			    this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 0;
			break;
		case inst::NOP:
			break;
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
		case inst::RBL:
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->registers[reg::PC] =
			    this->registers[(reg) this->arg0];
			break;
		case inst::BL:
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->registers[reg::PC] = this->arg0;
			break;
		case inst::RET:
			this->registers[reg::PC] =
			    this->memory[this->registers[reg::SP] - 1];
			break;
		case inst::CPY:
			this->registers[(reg) this->arg0] =
			    this->registers[(reg) this->arg1];
			break;
		case inst::B:
			this->registers[reg::PC] = this->arg0;
			break;
		case inst::BNZ:
			if (this->registers[reg::S1] != 0)
				this->registers[reg::PC] = this->arg0;
			break;
		case inst::SVC:
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::S0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::S1];
			this->registers[reg::SP]++;
			this->registers[reg::S2] = prot::PROT_HI_0;
			this->registers[reg::PC] =
			    this->memory[this->registers[reg::LR] +
					 this->arg0];
			break;
		case inst::BIZ:
			if (this->registers[reg::S1] == 0)
				this->registers[reg::PC] = this->arg0;
			break;
		case inst::BIM:
			if (this->registers[reg::S1] == 1)
				this->registers[reg::PC] = this->arg0;
			break;
		case inst::BIL:
			if (this->registers[reg::S1] == 2)
				this->registers[reg::PC] = this->arg0;
			break;
		case inst::STR:
			this->rmemory
			    [this->registers[(reg) this->arg0]] =
			    this->registers[(reg) this->arg1];
			break;
		case inst::LDR:
			this->registers[(reg) this->arg1] =
			    this->rmemory
				[this->registers[(reg) this->arg0]];
			break;
		case inst::MOV:
			if (this->arg0 != reg::S2 &&
			    this->arg0 != reg::LR)
				this->registers[(reg) this->arg0] =
				    this->arg1;
			break;
		case inst::INC:
			this->registers[(reg) this->arg0]++;
			break;
		case inst::DEC:
			this->registers[(reg) this->arg0]--;
			break;
		case inst::ADD:
			this->registers[(reg) this->arg0] =
			    this->registers[(reg) this->arg1] +
			    this->registers[(reg) this->arg0];
			break;
		case inst::MUL:
			this->registers[(reg) this->arg0] =
			    this->registers[(reg) this->arg1] *
			    this->registers[(reg) this->arg0];
			break;
		case inst::SUB:
			this->registers[(reg) this->arg0] =
			    this->registers[(reg) this->arg1] -
			    this->registers[(reg) this->arg0];
			break;
		default:
			cexcp = excep::INV_OPCODE;
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::S0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] =
			    this->registers[reg::S1];
			this->registers[reg::SP]++;
			if (this->memory[this->registers[reg::LR] +
					 cexcp] == 0)
				cexcp = excep::DOUBLE_FLT;
			this->registers[reg::PC] =
			    this->memory[this->registers[reg::LR] +
					 cexcp];
			break;
		}
	dfe:
	}

	void clearmem()
	{
		for (int i = 0; i < 8192; i++)
			this->memory[i] = 0;
	}

	void clearreg()
	{
		for (int i = 0; i < 14; i++)
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
		for (int i = 0; i < len; i++) {
			this->memory[PROGADR + i] = insts[i];
		}
	}
};

// Li X 16 CPU

#define HW_SUP
// #undef HW_SUP
#define SGA_ADR                                                       \
	0x2000 /* "Video card"'s ram would be attached there (MMIO.   \
		  read more) */

int main()
{
	lix cpu = {0};
	cpu.init();

	short prog[0x1000] = {0};

	std::ifstream file("a.bin");
	std::ostringstream a;
	a << file.rdbuf();
	int i = 0;
	for (i = 0; i < a.str().size() / 2; ++i) {
		short b = ((short *) (a.str().data()))[i];
		prog[i] = b;
	}

	cpu.load(prog, 0x1000);
	cpu.printprog();

	for (int i = 0; i < PROGLEN; ++i) {
		cpu.fetch();
		if (cpu.inst == HLT) break;
		cpu.execute();
		cpu.printinst();
	}

	return 0;
}
