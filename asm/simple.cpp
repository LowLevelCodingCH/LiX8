#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

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

    OUT,
    IN,
};

std::vector<std::string> split(std::string string, char delimiter)
{
	std::string buffer;
	std::vector<std::string> result;

	for(char c : string) {
		if(c == delimiter) {
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
	if(argc != 2)
		exit(1);

	std::ifstream       file(argv[1]);
	std::string         line;
	std::stringstream   text;
	std::vector<short>  output;
	short               *out;

	while(std::getline(file, line))
		text << line << ' ';

	file.close();

	auto htoks = split(text.str(), ' ');

	for(auto htok : htoks) {
		if("nop" == htok)
			output.push_back((short)inst::NOP);
		else if("cpy" == htok)
			output.push_back((short)inst::CPY);
		else if("mov" == htok)
			output.push_back((short)inst::MOV);
		else if("movi" == htok)
			output.push_back((short)inst::MOVI);
		else if("lod" == htok)
			output.push_back((short)inst::LOD);
		else if("inc" == htok)
			output.push_back((short)inst::INC);
		else if("dec" == htok)
			output.push_back((short)inst::DEC);
		else if("add" == htok)
			output.push_back((short)inst::ADD);
		else if("sub" == htok)
			output.push_back((short)inst::SUB);
		else if("mul" == htok)
			output.push_back((short)inst::MUL);
		else if("div" == htok)
			output.push_back((short)inst::DIV);
		else if("push" == htok)
			output.push_back((short)inst::PUSH);
		else if("pop" == htok)
			output.push_back((short)inst::POP);
		else if("ret" == htok)
			output.push_back((short)inst::RET);
		else if("call" == htok)
			output.push_back((short)inst::CALL);
		else if("rcall" == htok)
			output.push_back((short)inst::RCALL);
		else if("cmp" == htok)
			output.push_back((short)inst::CMP);
		else if("jmp" == htok)
			output.push_back((short)inst::JMP);
		else if("jz" == htok)
			output.push_back((short)inst::JZ);
		else if("jl" == htok)
			output.push_back((short)inst::JL);
		else if("jm" == htok)
			output.push_back((short)inst::JM);
		else if("jne" == htok)
			output.push_back((short)inst::JNE);
		else if("hlt" == htok)
			output.push_back((short)inst::HLT);
		else if("out" == htok)
			output.push_back((short)inst::OUT);
		else if("in" == htok)
			output.push_back((short)inst::IN);
		else if("sp" == htok)
			output.push_back((short)reg::SP);
		else if("l0" == htok)
			output.push_back((short)reg::L0);
		else if("l1" == htok)
			output.push_back((short)reg::L1);
		else if("l2" == htok)
			output.push_back((short)reg::L2);
		else if("l3" == htok)
			output.push_back((short)reg::L3);
		else if("l4" == htok)
			output.push_back((short)reg::L4);
		else if("l5" == htok)
			output.push_back((short)reg::L5);
		else if("l6" == htok)
			output.push_back((short)reg::L6);
		else if("l7" == htok)
			output.push_back((short)reg::L7);
		else {
			try {
				output.push_back((short)std::stoi(htok));
			} catch (...) {
				output.push_back((short)0);
			}
		}
	}

	out = output.data();

	std::ofstream ofile("a.bin");
	ofile.write(reinterpret_cast<char*>(output.data()), output.size() * sizeof(short));
	std::cout << out << "\n"; 
	ofile.close();
}
