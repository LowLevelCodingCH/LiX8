#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*
 * @author LowLevelCodingCH (Alex), 2025
 * @copyright BSD 2 Clause, (c) LowLevelCodingCH
 */

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

	I0,

	F0,
	F1,
};

enum inst {
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
	IRET,

	OUT,
	IN,
};

namespace lixasm
{
/*
 * @param String of the token
 * @return number to be put rawly into the executable file
 * @note uses yoda notation (lit == var) instead of "normal" notation (var == lit),
 *       To prevent this: (var = lit) from accidentally happening
 */
short get_inst(std::string htok)
{
	if ("nop" == htok)
		return (short) inst::NOP;
	else if ("cpy" == htok)
		return (short) inst::CPY;
	else if ("movm" == htok)
		return (short) inst::MOVM;
	else if ("mov" == htok)
		return (short) inst::MOV;
	else if ("lod" == htok)
		return (short) inst::LOD;
	else if ("inc" == htok)
		return (short) inst::INC;
	else if ("dec" == htok)
		return (short) inst::DEC;
	else if ("add" == htok)
		return (short) inst::ADD;
	else if ("sub" == htok)
		return (short) inst::SUB;
	else if ("mul" == htok)
		return (short) inst::MUL;
	else if ("div" == htok)
		return (short) inst::DIV;
	else if ("push" == htok)
		return (short) inst::PUSH;
	else if ("pop" == htok)
		return (short) inst::POP;
	else if ("ret" == htok)
		return (short) inst::RET;
	else if ("swi" == htok)
		return (short) inst::SWI;
	else if ("iret" == htok)
		return (short) inst::IRET;
	else if ("bl" == htok)
		return (short) inst::BL;
	else if ("bil" == htok)
		return (short) inst::BIL;
	else if ("rbl" == htok)
		return (short) inst::RBL;
	else if ("cmp" == htok)
		return (short) inst::CMP;
	else if ("b" == htok)
		return (short) inst::B;
	else if ("biz" == htok)
		return (short) inst::BIZ;
	else if ("bim" == htok)
		return (short) inst::BIM;
	else if ("bnz" == htok)
		return (short) inst::BNZ;
	else if ("hlt" == htok)
		return (short) inst::HLT;
	else if ("out" == htok)
		return (short) inst::OUT;
	else if ("in" == htok)
		return (short) inst::IN;
	else if ("sp" == htok)
		return (short) reg::SP;
	else if ("l0" == htok || "r0" == htok)
		return (short) reg::L0;
	else if ("l1" == htok || "r1" == htok)
		return (short) reg::L1;
	else if ("l2" == htok || "r2" == htok)
		return (short) reg::L2;
	else if ("l3" == htok || "r3" == htok)
		return (short) reg::L3;
	else if ("l4" == htok || "r4" == htok)
		return (short) reg::L4;
	else if ("l5" == htok || "r5" == htok)
		return (short) reg::L5;
	else if ("l6" == htok || "r6" == htok)
		return (short) reg::L6;
	else if ("l7" == htok || "r7" == htok)
		return (short) reg::L7;
	else if ("i0" == htok)
		return (short) reg::I0;
	else {
		try {
			return (short) std::stoi(htok);
		} catch (...) {
			return (short) 0;
		}
	}
}
} // namespace lixasm

std::vector<std::string> split(std::string string, char delimiter)
{
	std::string buffer;
	std::vector<std::string> result;

	for (char c : string) {
		if (c == delimiter) {
			result.push_back(buffer);
			buffer.clear();
		} else
			buffer.push_back(c);
	}

	result.push_back(buffer);
	return result;
}

// fuck this test

int main(int argc, char* argv[])
{
	if (argc != 2) exit(1);

	std::ifstream file(argv[1]);
	std::string line;
	std::stringstream text;
	std::vector<short> output;

	while (std::getline(file, line))
		text << line << ' ';

	file.close();

	auto htoks = split(text.str(), ' ');

	for (auto htok : htoks)
		output.push_back(lixasm::get_inst(htok));

	std::ofstream ofile("a.bin");
	ofile.write(reinterpret_cast<char*>(output.data()), output.size() * sizeof(short));
	ofile.close();
}
