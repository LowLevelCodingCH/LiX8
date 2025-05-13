#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
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
	L8,
	L9,
	L10,
	L11,
	L12,
	L13,
	L14,
	L15,

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
	OR,
	XOR,
	AND,
	NOT,
	IRET,
	IRETRG,
};

namespace lixasm
{
/**
 * @param token Name of the token
 * @return number to be put rawly into the executable file
 * @note uses yoda notation (lit == var) instead of "normal" notation (var ==
 * lit), To prevent this: (var = lit) from accidentally happening
 */
short get_inst(std::string token, std::unordered_map<std::string, int> lbls)
{
	if ("nop" == token)
		return NOP;
	else if ("cpy" == token)
		return CPY;
	else if ("mov" == token)
		return MOV;
	else if ("str" == token)
		return STR;
	else if ("ldr" == token)
		return LDR;
	else if ("strb" == token)
		return STRB;
	else if ("ldrb" == token)
		return LDRB;
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
	else if ("and" == token)
		return AND;
	else if ("or" == token)
		return OR;
	else if ("not" == token)
		return NOT;
	else if ("xor" == token)
		return XOR;
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
	else if ("iretrg" == token)
		return IRETRG;
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
	else if ("adrum" == token)
		return ADRUM;
	else if ("adrbs" == token)
		return ADRBS;
	else if ("pc" == token)
		return PC;
	else if ("sp" == token)
		return SP;
	else if ("r0" == token)
		return L0;
	else if ("r1" == token)
		return L1;
	else if ("r2" == token)
		return L2;
	else if ("r3" == token)
		return L3;
	else if ("r4" == token)
		return L4;
	else if ("r5" == token)
		return L5;
	else if ("r6" == token)
		return L6;
	else if ("r7" == token)
		return L7;
	else if ("r8" == token)
		return L8;
	else if ("r9" == token)
		return L9;
	else if ("r10" == token)
		return L10;
	else if ("r11" == token)
		return L11;
	else if ("r12" == token)
		return L12;
	else if ("r13" == token)
		return L13;
	else if ("r14" == token)
		return L14;
	else if ("r15" == token)
		return L15;
	else if ("lr" == token)
		return LR;
	else if ("s1" == token)
		return S1;
	else if ("s0" == token)
		return S0;
	else if ("" == token || "#import" == token)
		return 0;
	if (token.back() == ':') {
		auto lbl = lbls.find(token);
		if (lbl == lbls.end()) {
			std::cerr << "Error: Label not found " << token << std::endl;
			std::exit(1);
			return -1;
		} else
			return lbl->second;
	}
	try {
		return std::stoi(token);
	} catch (...) {
		std::cerr << "Error: Unknown token " << token << std::endl;
		std::exit(1);
	}
}
} // namespace lixasm

bool instr(char c, std::string s)
{
	for (auto d : s)
		if (c == d) return true;
	return false;
}

bool startswith(std::string is, std::string st)
{
	return strncmp((char *) &is[0], (char *) &st[0], strlen((char *) &st[0]));
}

std::vector<std::string> split(std::string string, std::string delimiter)
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

int main(int argc, char *argv[])
{
	if (argc != 2) exit(1);

	std::ifstream file(argv[1]);
	close(open("lxsm.pproc.obj.s", O_CREAT | O_TRUNC | O_RDWR, 0777));
	std::ofstream ifile("lxsm.pproc.obj.s");
	std::string line, incln;
	std::stringstream text;
	std::vector<short> output;
	std::ofstream outfile("a.bin");
	int i = 0, tokmnt = 0, j = 0;
	std::unordered_map<std::string, int> lbls;
	std::vector<std::string> gtokens;

	std::cout << "Pass 0: Preprocessing" << std::endl;
	while (std::getline(file, line)) {
		if (!startswith(line, "#import !")) {
			std::ifstream included(split(line, "!")[1]);

			while(std::getline(included, incln)) {
				if (incln[0] == '#') continue;
				if ("" == incln) continue;
				ifile << incln << "\n";
			}

			continue;
		} else
			ifile << line << "\n";
	}

	ifile.close();
	std::ifstream ifil("lxsm.pproc.obj.s");
	std::cout << "Pass 1: Resolving labels and symbols" << std::endl;
	while (std::getline(ifil, line)) {
		auto tokens = split(line, " \t,[]{}()");

		if (!tokens[0].empty()) {
			if (tokens[0].back() == ':') {
				lbls.insert(std::pair<std::string, int>(tokens[0], i));
				continue;
			}
		}

		for (auto token : tokens) {
			if ("" == token) continue;

			gtokens.push_back(token);
			++i;
		}
	}

	i = 0;

	std::cout << "Pass 2: Resolving tokens" << std::endl;
	for (auto token : gtokens) {
		if ("" == token) continue;

		if (token.back() == ':') {
			auto lbl = lbls.find(token);
			if (lbl == lbls.end()) {
				std::cout << "Could not find symbol " << token << std::endl;
				std::exit(1);
			}
			output.push_back(lbl->second);
			++i;
			continue;
		} else if ("$" == token) {
			output.push_back(i - 1);
			continue;
		}

		output.push_back(lixasm::get_inst(token, lbls));
		++tokmnt;
		++i;
	}

	file.close();

	outfile.write(reinterpret_cast<char *>(output.data()), output.size() * sizeof(short));
	outfile.close();
	ifil.close();
//	unlink("lxsm.pproc.obj.s");
}
