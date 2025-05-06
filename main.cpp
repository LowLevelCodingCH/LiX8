#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
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
	// ffff * 2 because sizeof memory is just sizeof rmemory / 2. because 16 is 2x8.
	std::uint8_t rmemory[0xFFFF * 2]; // Not included by def
	std::uint16_t *memory;

	/**
	 * @brief Prints a program
	 */
	void printprog()
	{
		for (int i = 0; i < PROGLEN * 3; i++)
			std::cout << (int) (((short *) (this->memory))[PROGADR + i]) << " ";
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
		this->registers[reg::PC]++;

		switch (this->inst) {
		case inst::ADD:
		case inst::DIV:
		case inst::SUB:
		case inst::MUL:
		case inst::LDR:
		case inst::STR:
		case inst::LDRB:
		case inst::STRB:
		case inst::CPY:
		case inst::CMP:
		case inst::MOV:
			this->registers[reg::PC] += 2;
			break;
		case inst::INC:
		case inst::DEC:
		case inst::SWI:
		case inst::SVC:
		case inst::SVCSTR:
		case inst::PUSH:
		case inst::POP:
		case inst::B:
		case inst::BIZ:
		case inst::BL:
		case inst::RBL:
		case inst::BIM:
		case inst::BNZ:
		case inst::BIL:
		case inst::ADRBS:
		case inst::ADRUM:
		case inst::IRETRG:
			this->registers[reg::PC] += 1;
		case inst::NOP:
		case inst::HLT:
		case inst::IRET:
		case inst::RET:
			break;
		}
	}

	bool push(reg v)
	{
		if (this->isclean()) {
			this->memory[this->registers[reg::SP]] = this->registers[v];
			this->registers[reg::SP]++;
		} else
			return false;
		return true;
	}

	bool pop(reg v)
	{
		this->registers[reg::SP]--;
		if (this->isclean())
			this->registers[v] = this->memory[this->registers[reg::SP]];
		else
			return false;
		return true;
	}

	void iretstub()
	{
		this->pop(reg::S1);
		this->pop(reg::S0);
		this->pop(reg::PC);
	}

	void intstub()
	{
		this->push(reg::PC);
		this->push(reg::S0);
		this->push(reg::S1);
	}

	void exec_svc()
	{
		this->intstub();
		this->registers[reg::S2] = prot::PROT_HI_0;
		if (this->memory[this->registers[reg::LR] + this->arg0] == 0)
			this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::DOUBLE_FLT];
		this->registers[reg::PC] = this->memory[this->registers[reg::LR] + this->arg0];
	}

	void exec_iretrg()
	{
		this->push((reg) this->arg1);
		this->push(reg::S0);
		this->push(reg::S1);
	}

	bool exec_str()
	{
		if (this->registers[(reg) this->arg0] < this->registers[reg::S3]) return false;

		this->memory[this->registers[(reg) this->arg0] + this->registers[reg::S4]] =
		    (char) this->registers[(reg) this->arg1];
#ifdef HW_SUP
		if (this->registers[(reg) this->arg0] >= 1200 && this->registers[(reg) this->arg0] <= 2400)
			vgaputc(this->registers[(reg) this->arg1]);
#endif
		return true;
	}

	bool exec_strb()
	{
		if (this->registers[(reg) this->arg0] < this->registers[reg::S3]) return false;

		this->rmemory[this->registers[(reg) this->arg0] + this->registers[reg::S4]] =
		    (char) this->registers[(reg) this->arg1];
#ifdef HW_SUP
		if (this->registers[(reg) this->arg0] >= 1200 && this->registers[(reg) this->arg0] <= 2400)
			vgaputc(this->registers[(reg) this->arg1]);
#endif
		return true;
	}

	bool exec_ldrb()
	{
		if (this->isclean())
			this->registers[(reg) this->arg0] =
			    this->rmemory[this->registers[(reg) this->arg1] + this->registers[reg::S4]];
		else
			return false;
		return true;
	}

	bool exec_ldr()
	{
		if (this->isclean())
			this->registers[(reg) this->arg0] =
			    this->memory[this->registers[(reg) this->arg1] + this->registers[reg::S4]];
		else
			return false;
		return true;
	}

	void fault(excep e)
	{
		this->intstub();
		this->registers[reg::S2] = prot::PROT_HI_0;
		if (this->memory[this->registers[reg::LR] + e] == 0)
			this->registers[reg::PC] = this->memory[this->registers[reg::LR] + excep::DOUBLE_FLT];
		this->registers[reg::PC] = this->memory[this->registers[reg::LR] + e];
	}

	void exec_iret()
	{
		this->iretstub();
		this->registers[reg::S2] = prot::PROT_LO_1;
	}

	void exec_cmp()
	{
		if (this->registers[(reg) this->arg0] > this->registers[(reg) this->arg1]) this->registers[reg::S1] = 1;
		if (this->registers[(reg) this->arg0] < this->registers[(reg) this->arg1]) this->registers[reg::S1] = 2;
		if (this->registers[(reg) this->arg0] == this->registers[(reg) this->arg1])
			this->registers[reg::S1] = 0;
	}

	void exec_push() { this->push((reg) this->arg0); }

	void exec_pop() { this->pop((reg) this->arg0); }

	void exec_ret()
	{
		this->registers[reg::SP]--;
		this->registers[reg::PC] = this->memory[this->registers[reg::SP]];
	}

	void exec_bl()
	{
		this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
		this->registers[reg::SP]++;
		this->registers[reg::PC] = this->registers[reg::S4] + this->arg0;
	}

	void exec_rbl()
	{
		this->memory[this->registers[reg::SP]] = this->registers[reg::PC];
		this->registers[reg::SP]++;
		this->registers[reg::PC] = this->registers[reg::S4] + this->registers[(reg) this->arg0];
	}

	bool isclean()
	{
		return (this->arg0 != reg::S2 && this->arg0 != reg::LR && this->arg0 != reg::S3 &&
			this->arg0 != reg::S4);
	}

	void exec_b() { this->registers[reg::PC] = this->registers[reg::S4] + this->arg0; }

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
				this->registers[reg::S3] = this->memory[this->arg0];
				return;
			case inst::ADRBS:
				this->registers[reg::S4] = this->memory[this->arg0];
				return;
			case inst::IRET:
				this->exec_iret();
				return;
			case inst::HLT:
				exit(1);
			}
		} else if (this->registers[reg::S2] == prot::PROT_LO_1) {
			switch (this->inst) {
			case inst::SVCSTR:
			case inst::SWI:
			case inst::IRET:
			case inst::HLT:
			case inst::ADRUM:
			case inst::ADRBS:
				goto prot_fault;
			}
		}

		switch (this->inst) {
		case inst::CMP:
			this->exec_cmp();
			return;
		case inst::NOP:
			return;
		case inst::PUSH:
			this->exec_push();
			return;
		case inst::POP:
			this->exec_pop();
			return;
		case inst::RBL:
			this->exec_rbl();
			return;
		case inst::BL:
			this->exec_bl();
			return;
		case inst::RET:
			this->exec_ret();
			return;
		case inst::CPY:
			if (this->isclean())
				this->registers[(reg) this->arg0] = this->registers[(reg) this->arg1];
			else
				break;
			return;
		case inst::SVC:
			this->exec_svc();
			return;
		case inst::B:
			this->exec_b();
			return;
		case inst::BNZ:
			if (this->registers[reg::S1] != 0) this->exec_b();
			return;
		case inst::BIZ:
			if (this->registers[reg::S1] == 0) this->exec_b();
			return;
		case inst::BIM:
			if (this->registers[reg::S1] == 1) this->exec_b();
			return;
		case inst::BIL:
			if (this->registers[reg::S1] == 2) this->exec_b();
			return;
		case inst::STR:
			if (!this->exec_str()) break;
			return;
		case inst::STRB:
			if (!this->exec_strb()) break;
			return;
		case inst::LDR:
			if (!this->exec_ldr()) break;
			return;
		case inst::LDRB:
			if (!this->exec_ldrb()) break;
			return;
		case inst::MOV:
			if (this->isclean())
				this->registers[(reg) this->arg0] = this->arg1;
			else
				break;
			return;
		case inst::INC:
			if (this->isclean())
				this->registers[(reg) this->arg0]++;
			else
				break;
			return;
		case inst::DEC:
			if (this->isclean())
				this->registers[(reg) this->arg0]--;
			else
				break;
			return;
		case inst::ADD:
			if (this->isclean())
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] + this->registers[(reg) this->arg1];
			else
				break;
			return;
		case inst::MUL:
			if (this->isclean())
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] * this->registers[(reg) this->arg1];
			else
				break;
			return;
		case inst::SUB:
			if (this->isclean())
				this->registers[(reg) this->arg0] =
				    this->registers[(reg) this->arg0] - this->registers[(reg) this->arg1];
			else
				break;
			return;
		case inst::DIV:
			if (this->isclean()) {
				this->registers[reg::L6] =
				    std::round(this->registers[(reg) this->arg0] / this->registers[(reg) this->arg1]);
				this->registers[reg::L7] =
				    this->registers[(reg) this->arg0] % this->registers[(reg) this->arg1];
			} else
				break;
			return;
		case inst::IRETRG:
			this->exec_iretrg();
			return;
		default:
			this->fault(excep::INV_OPCODE);
			return;
		}
	prot_fault:
		this->fault(excep::PROT_FLT);
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
