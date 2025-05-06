#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>
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
	else if ("lr" == token)
		return LR;
	else if ("s1" == token)
		return S1;
	else if ("s0" == token)
		return S0;
	else if ("" == token)
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

bool instr(char c, char *s)
{
	for (int i = 0; i < strlen(s); i++)
		if (c == s[i]) return true;
	return false;
}

std::vector<std::string> split(std::string string, char *delimiter)
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

enum lhtype {
	TINVAL,
	LIX_8_16,
};

struct lxe_hdr {
	short lh_magic[3];
	short lh_type,
	      lh_sec_code_begin,
	      lh_sec_data_begin,
	      lh_sec_symtab_begin,
	      lh_symtb_entries;
};

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3) exit(1);

	std::ifstream file(argv[1]);
	std::string line;
	std::stringstream text;
	std::vector<short> output;
	std::vector<short> hdr;
	lxe_hdr ehdr;
	std::ofstream outfile("a.bin");
	int i = 0, tokmnt = 0, j = 0;
	bool pseudoclean = false;
	bool data	 = false;
	std::unordered_map<std::string, int> lbls;
	std::vector<std::string> gtokens;
	int datsec = 0;
	int codsec = 0;
	bool indat = false;
	codsec	   = sizeof(lxe_hdr) / 2;

	if (argc == 3)
		if (!strcasecmp(argv[2], "-Pc")) pseudoclean = true;

	// Pass 1: resolve labels
	std::cout << "Pass 1: Resolving labels and symbols" << std::endl;
	while (std::getline(file, line)) {
		if (line[0] == '#') continue;
		auto tokens = split(line, " \t,[]{}()");

		if (!tokens[0].empty()) {
			if (tokens[0].back() == ':') {
				if (indat)
					lbls.insert(std::pair<std::string, int>(tokens[0], i + datsec));
				else
					lbls.insert(std::pair<std::string, int>(tokens[0], i + codsec));
				continue;
			}
		}

		for (auto token : tokens) {
			if ("" == token) continue;
			if (".data" == token) {
				datsec = i + (sizeof(lxe_hdr) / 2);
				indat  = true;
				continue;
			}

			gtokens.push_back(token);
			++i;
			if (indat) j++;
		}
	}

	indat = false;

	i = 0;

	if (pseudoclean) {
		for (auto a : "lxe")
			if (a) hdr.push_back((short) a);

		hdr.push_back((short) lhtype::LIX_8_16);

		// section .code begin
		hdr.push_back(sizeof(lxe_hdr) / 2);
		codsec = sizeof(lxe_hdr) / 2;
	}

	// Pass 2: resolve tokens
	std::cout << "Pass 2: Resolving tokens" << std::endl;
	for (auto token : gtokens) {
		if ("" == token) continue;

		if (".data" == token) {
			hdr.push_back(i + (sizeof(lxe_hdr) / 2));
			datsec = i + (sizeof(lxe_hdr) / 2);
			indat  = true;
			continue;
		}

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

	if (pseudoclean) {
		std::cout << "Pass 3" << std::endl;

		// That if it ever gets here it stops
		output.push_back((short) HLT);

		i += (sizeof(lxe_hdr) / 2) + 1;
		hdr.push_back(datsec);
		// section .symtab begin
		hdr.push_back(i);

		short j = 0;

		for (auto a : lbls) {
			for (auto c : a.first)
				output.push_back((short) c);
			for (i = 0; i < 256 - a.first.size(); i++)
				output.push_back(0);
			output.push_back(0);
			output.push_back(a.second);
			j++;
		}

		// symtab entries
		hdr.push_back(j);
	}

	file.close();

	for (auto e : output)
		hdr.push_back(e);

	outfile.write(reinterpret_cast<char *>(hdr.data()), hdr.size() * sizeof(short));
	outfile.close();
}
