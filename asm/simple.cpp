#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @author LowLevelCodingCH (Alex), 2025
 * @copyright BSD 2 Clause, (c) LowLevelCodingCH
 */

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

namespace lixasm
{
/**
 * @param token Name of the token
 * @return number to be put rawly into the executable file
 * @note uses yoda notation (lit == var) instead of "normal" notation (var == lit),
 *       To prevent this: (var = lit) from accidentally happening
 */
short get_inst(std::string token)
{
	if ("nop" == token)
		return NOP;
	else if ("cpy" == token)
		return CPY;
	else if ("str" == token)
		return STR;
	else if ("mov" == token)
		return MOV;
	else if ("ldr" == token)
		return LDR;
	else if ("inc" == token)
		return INC;
	else if ("dec" == token)
		return DEC;
	else if ("add" == token)
		return ADD;
	else if ("sub" == token)
		return SUB;
	else if ("mul" == token)
		return MUL;
	else if ("div" == token)
		return DIV;
	else if ("push" == token)
		return PUSH;
	else if ("pop" == token)
		return POP;
	else if ("ret" == token)
		return RET;
	else if ("svc" == token)
		return SVC;
	else if ("svcstr" == token)
		return SVCSTR;
	else if ("iret" == token)
		return IRET;
	else if ("bl" == token)
		return BL;
	else if ("bil" == token)
		return BIL;
	else if ("rbl" == token)
		return RBL;
	else if ("cmp" == token)
		return CMP;
	else if ("swi" == token)
		return SWI;
	else if ("b" == token)
		return B;
	else if ("biz" == token)
		return BIZ;
	else if ("bim" == token)
		return BIM;
	else if ("bnz" == token)
		return BNZ;
	else if ("hlt" == token)
		return HLT;
	else if ("out" == token)
		return OUT;
	else if ("in" == token)
		return IN;
	else if ("pc" == token)
		return PC;
	else if ("sp" == token)
		return SP;
	else if ("l0" == token || "r0" == token)
		return L0;
	else if ("l1" == token || "r1" == token)
		return L1;
	else if ("l2" == token || "r2" == token)
		return L2;
	else if ("l3" == token || "r3" == token)
		return L3;
	else if ("l4" == token || "r4" == token)
		return L4;
	else if ("l5" == token || "r5" == token)
		return L5;
	else if ("l6" == token || "r6" == token)
		return L6;
	else if ("l7" == token || "r7" == token)
		return L7;
	else if ("lr" == token)
		return LR;
	else if ("" == token)
		return 0;
	else {
		try {
			return std::stoi(token);
		} catch (...) {
			std::cerr << "Error: Unknown token " << token << std::endl;
			std::exit(1);
			return -1;
		}
	}
}
} // namespace lixasm

bool instr(char c, char* s)
{
	for (int i = 0; i < strlen(s); i++)
		if (c == s[i]) return true;
	return false;
}

std::vector<std::string> split(std::string string, char* delimiter)
{
	std::string buffer;
	std::vector<std::string> result;

	for (char c : string) {
		if (instr(c, delimiter)) {
			result.push_back(buffer);
			buffer.clear();
		} else
			buffer.push_back(c);
	}

	result.push_back(buffer);
	return result;
}

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

	auto tokens = split(text.str(), " \t,.");

	for (auto token : tokens)
		if ("" != token) output.push_back(lixasm::get_inst(token));

	std::ofstream outfile("a.bin");
	outfile.write(reinterpret_cast<char*>(output.data()), output.size() * sizeof(short));
	outfile.close();
}
