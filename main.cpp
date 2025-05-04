#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define HW_SUP
// #undef HW_SUP

/**
 * @author LowLevelCodingCH (Alex), 2025
 * @copyright BSD 2 Clause, (c) LowLevelCodingCH
 */

#define PROGLEN 800
#define PROGADR 0
// Not the vga buffer

char vgamem_at_the_end[1200] = {0};

enum excep {
	INV_OPCODE,
	DIV_BY_ZERO,
	DOUBLE_FLT,
	PROT_FLT,
};
/**
 * @brief PROT_HI_0 is the highest priv
 */
enum prot { PROT_HI_0, PROT_LO_1 };

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
	S3,
	S4,
};

enum inst {
	NOP,
	CPY,
	STR,
	STRB,
	LDR,
	LDRB,
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
	ADRUM,
	ADRBS,
	IRET,
	IRETRG,
};

std::string to_lower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

// #I "" so that it works. otherwise char* is a null ptr
#define cr(I)                                                                                                          \
	case I:                                                                                                        \
		return to_lower((std::string)((char *) (#I "")))
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
		cr(ADRUM);
		cr(ADRBS);
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
		cr(STRB);
		cr(LDR);
		cr(LDRB);
		cr(BIZ);
		cr(BIM);
		cr(BIL);
		cr(BNZ);
		cr(HLT);
		cr(IRETRG);
	default:
		return "UNKNOWN";
	}
}

void vgaputc(short c)
{
	static int i	       = 0;
	char ch		       = (char) c;
	char fm		       = (char) (c << 8); // i think
	vgamem_at_the_end[i++] = ch;		  // formatting gets ignored
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
	std::uint16_t registers[16];
	std::uint16_t inst;
	std::uint16_t arg0;
	std::uint16_t arg1;
	std::uint8_t rmemory[0xFFFF]; // Not included by def
	std::uint16_t *memory;

	/**
	 * @brief Prints a program
	 */
	void printprog()
	{
		for (int i = 0; i < PROGLEN * 3; i++)
			std::cout << (int) (((char *) (this->memory))[PROGADR + i]) << " ";
		std::cout << std::endl;
	}

	/**
	 * @brief Prints an instruction
	 */
	void printinst()
	{
		std::cout << intoa(this->inst) << " " << (int) this->arg0 << ", " << (int) this->arg1 << std::endl;
	}

	/**
	 * @brief Fetches an instruction
	 */
	void fetch()
	{
		this->inst = this->memory[this->registers[reg::PC]];
		this->arg0 = this->memory[this->registers[reg::PC] + 1];
		this->arg1 = this->memory[this->registers[reg::PC] + 2];
		this->registers[reg::PC] += 3;
		if (this->registers[reg::PC] % 3) {
			std::cerr << "PC is invalid. has to be divisible by 3" << std::endl;
			std::abort();
		}
	}

	/**
	 * @brief Executes the instruction in inst, arg0 and arg1
	 */
	void execute()
	{
		if (this->registers[reg::S2] == prot::PROT_HI_0) {
			switch (this->inst) {
			case inst::SVCSTR:
				this->registers[reg::LR] = this->arg0;
				return;
			case inst::SWI:
				this->registers[reg::S2] = this->arg0;
				return;
			case inst::ADRUM:
				this->registers[reg::S3] = this->arg0;
				return;
			case inst::ADRBS:
				this->registers[reg::S4] = this->arg0;
				return;
			case inst::IRET:
				this->registers[reg::SP]--;
				this->registers[reg::S4] = this->memory[this->registers[reg::SP]];
				this->registers[reg::SP]--;
				this->registers[reg::S1] = this->memory[this->registers[reg::SP]];
				this->registers[reg::SP]--;
				this->registers[reg::S0] = this->memory[this->registers[reg::SP]];
				this->registers[reg::SP]--;
				this->registers[reg::PC] = this->memory[this->registers[reg::SP]];
				this->registers[reg::S2] = prot::PROT_LO_1;
				return;
			}
		}

		if (this->registers[reg::S2] == prot::PROT_LO_1) {
			switch (this->inst) {
			case inst::SVCSTR:
			case inst::SWI:
			case inst::IRET:
			case inst::ADRUM:
			case inst::ADRBS:
				goto prot_fault;
			}
		}

		switch (this->inst) {
		case inst::CMP:
			if (this->registers[(reg) this->arg0] > this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 1;
			if (this->registers[(reg) this->arg0] < this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 2;
			if (this->registers[(reg) this->arg0] == this->registers[(reg) this->arg1])
				this->registers[reg::S1] = 0;
			break;
		case inst::NOP:
			break;
		case inst::HLT:
			if (this->registers[reg::S2] == PROT_HI_0)
				exit(0);
			else
				goto prot_fault;
		case inst::PUSH:
			this->memory[this->registers[reg::SP]] = this->registers[(reg) this->arg0];
			this->registers[reg::SP]++;
			break;
		case inst::POP:
			this->registers[reg::SP]--;
			this->registers[(reg) this->arg0] = this->memory[this->registers[reg::SP]];
			break;
		case inst::RBL:
			this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->registers[reg::PC] = this->registers[(reg) this->arg0];
			break;
		case inst::BL:
			this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->registers[reg::PC] = this->arg0;
			break;
		case inst::RET:
			this->registers[reg::SP]--;
			this->registers[reg::PC] = this->memory[this->registers[reg::SP]];
			break;
		case inst::CPY:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] = this->registers[(reg) this->arg1];
			else
				goto prot_fault;
			break;
		case inst::B:
			this->registers[reg::PC] = this->arg0;
			break;
		case inst::BNZ:
			if (this->registers[reg::S1] != 0) this->registers[reg::PC] = this->arg0;
			break;
		case inst::SVC:
			this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S1];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S4];
			this->registers[reg::SP]++;
			this->registers[reg::S2] = prot::PROT_HI_0;
			if (this->memory[this->registers[reg::LR] + this->arg0] == 0)
				this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::DOUBLE_FLT];
			this->registers[reg::PC] = this->memory[this->registers[reg::LR] + this->arg0];
			break;
		case inst::BIZ:
			if (this->registers[reg::S1] == 0) this->registers[reg::PC] = this->arg0;
			break;
		case inst::BIM:
			if (this->registers[reg::S1] == 1) this->registers[reg::PC] = this->arg0;
			break;
		case inst::BIL:
			if (this->registers[reg::S1] == 2) this->registers[reg::PC] = this->arg0;
			break;
		case inst::STR:
			if (this->registers[(reg) this->arg0] < this->registers[reg::S3]) goto prot_fault;

			this->memory[this->registers[(reg) this->arg0] + this->registers[reg::S4]] =
			    (char) this->registers[(reg) this->arg1];
#ifdef HW_SUP
			if (this->registers[(reg) this->arg0] >= 1200 && this->registers[(reg) this->arg0] <= 2400)
				vgaputc(this->registers[(reg) this->arg1]);
#endif
			break;
		case inst::STRB:
			if (this->registers[(reg) this->arg0] < this->registers[reg::S3]) goto prot_fault;

			this->rmemory[this->registers[(reg) this->arg0] + this->registers[reg::S4]] =
			    (char) this->registers[(reg) this->arg1];
#ifdef HW_SUP
			if (this->registers[(reg) this->arg0] >= 1200 && this->registers[(reg) this->arg0] <= 2400)
				vgaputc(this->registers[(reg) this->arg1]);
#endif
			break;
		case inst::LDR:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    this->memory[this->registers[(reg) this->arg1] + this->registers[reg::S4]];
			else
				goto prot_fault;
			break;
		case inst::LDRB:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    this->rmemory[this->registers[(reg) this->arg1] + this->registers[reg::S4]];
			else
				goto prot_fault;
			break;
		case inst::MOV:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] = this->arg1;
			else
				goto prot_fault;
			break;
		case inst::INC:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0]++;
			else
				goto prot_fault;
			break;
		case inst::DEC:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0]--;
			else
				goto prot_fault;
			break;
		case inst::ADD:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] + this->registers[(reg) this->arg1];
			else
				goto prot_fault;
			break;
		case inst::MUL:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] * this->registers[(reg) this->arg1];
			else
				goto prot_fault;
			break;
		case inst::SUB:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] - this->registers[(reg) this->arg1];
			else
				goto prot_fault;
			break;
		case inst::DIV:
			if (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			    this->arg0 != reg::S4)
				this->registers[(reg) this->arg0] =
				    std::round(this->registers[(reg) this->arg0] / this->registers[(reg) this->arg1]);
			else
				goto prot_fault;
			break;
		case inst::IRETRG:
			this->memory[this->registers[reg::SP]] = this->registers[(reg) this->arg0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S1];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S4];
			this->registers[reg::SP]++;
			break;
		default:
			this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S0];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S1];
			this->registers[reg::SP]++;
			this->memory[this->registers[reg::SP]] = this->registers[reg::S4];
			this->registers[reg::SP]++;
			this->registers[reg::S2] = prot::PROT_HI_0;
			this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::INV_OPCODE];
			break;
		}
		return;
	prot_fault:
		this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
		this->registers[reg::SP]++;
		this->memory[this->registers[reg::SP]] = this->registers[reg::S0];
		this->registers[reg::SP]++;
		this->memory[this->registers[reg::SP]] = this->registers[reg::S1];
		this->registers[reg::SP]++;
		this->memory[this->registers[reg::SP]] = this->registers[reg::S4];
		this->registers[reg::SP]++;
		this->registers[reg::S2] = prot::PROT_HI_0;
		if (this->memory[this->registers[reg::LR] + excep::PROT_FLT] == 0)
			this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::DOUBLE_FLT];
		this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::PROT_FLT];
	}

	void clearmem()
	{
		for (int i = 0; i < 8192; i++)
			this->memory[i] = 0;
	}

	void clearreg()
	{
		for (int i = 0; i < 16; i++)
			this->registers[i] = 0;
	}

	void init()
	{
		this->memory = (unsigned short *) &(this->rmemory[0]);
		this->clearmem();
		this->clearreg();
		this->registers[reg::PC] = PROGADR;
	}

	void load(short *insts, int len)
	{
		for (int i = 0; i < len; i++)
			this->memory[PROGADR + i] = insts[i];
	}
};

// Li X 16 CPU

int main()
{
	lix cpu = {0};
	cpu.init();

	short prog[0x1000] = {0};

	std::ifstream file("a.bin");
	std::ostringstream a;
	int i = 0;

	a << file.rdbuf();

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

	printf((char *) vgamem_at_the_end);
	return 0;
}
