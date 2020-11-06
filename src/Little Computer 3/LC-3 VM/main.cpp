#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <sstream>
#include "print.hpp"


// ENUMS

// Registers.
enum Reg
{
	R0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	PC,
	CC,
	NREG
};

// Operation codes.
enum Op
{
	BR,		// conditional branch
	ADD,	// addtion
	LD,		// load
	ST,		// store
	JSR,	// jump to subroutine (also: JSRR - register)
	AND,	// bitwise logical and
	LDR,	// load base + offset
	STR,
	RTI,	// return from interrupt
	NOT,	// bitwise complement
	LDI,	// load indirect
	STI,
	JMP,	// jump (also: RET - return from subroutine)
	RES,
	LEA,	// load effective address
	TRAP
};

// Trap operation codes.
enum Trap
{
	GETC  = 0x20,
	OUT   = 0x21,
	PUTS  = 0x22,
	IN    = 0x23,
	PUTSP = 0x24,
	HALT  = 0x25
};

// Condition codes.
enum CC
{
	P = 1 << 0,
	Z = 1 << 1,
	N = 1 << 2
};


// GLOBAL VARS

// While true, the VM keep running.
bool running = true;

// Memória (65536 posições).
uint16_t mem[UINT16_MAX];

// Vetor de posições dos registradores.
uint16_t reg[NREG];


// FUNCTIONS


// Extends to 16 bits.
uint16_t sext(uint16_t val, int bits)
{
	// se o bit mais à esquerda for 1 é negativo
	if ((val >> bits - 1) & 1)
	{
		// extende os bits à esquerda
		val |= 0xFFFF << bits;
	}
	return val;
}

void setcc(uint16_t val)
{
	if ((val >> 15) & 1)
	{
		// negativo
		reg[CC] = N;
	}
	else if (val == 0)
	{
		// zero
		reg[CC] = Z;
	}
	else
	{
		// positivo
		reg[CC] = P;
	}
}

// It fetch and evaluate the instruction.
void eval()
{
	// fetch
	uint16_t instr = mem[reg[PC]++];
	uint16_t op = instr >> 12;

	//hxe::printHex(instruction);
	//hxe::printHex(op);
	//if (instruction == 0) {
	//    running = false;
	//}

	switch (op)
	{
		case ADD:
		{
			// dest reg
			uint16_t dr = (instr >> 9) & 0x7;
			// source reg 1
			uint16_t sr1 = (instr >> 6) & 0x7;

			// verifica se o bit 5 é 0
			if (!((instr >> 5) & 0x1))
			{
				// se for sr1 é somado com sr2
				uint16_t sr2 = instr & 0x7;
				reg[dr] = reg[sr1] + reg[sr2];
			}
			else
			{
				// senão é somado com o immediate 5 bits
				uint16_t imm5 = instr & 0x1F;
				reg[dr] = reg[sr1] + sext(imm5, 5);
			}

			setcc(reg[dr]);

			break;
		}
		case AND:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t sr1 = (instr >> 6) & 0x7;

			// verifica se o bit 5 é 0
			if (!((instr >> 5) & 0x1))
			{
				uint16_t sr2 = instr & 0x7;
				reg[dr] = reg[sr1] & reg[sr2];
			}
			else
			{
				uint16_t imm5 = instr & 0x1F;
				reg[dr] = reg[sr1] & sext(imm5, 5);
			}

			setcc(reg[dr]);

			break;
		}
		case BR:
		{
			uint16_t n = (instr >> 11) & 0x1;
			uint16_t z = (instr >> 10) & 0x1;
			uint16_t p = (instr >>  9) & 0x1;
			uint16_t pcoffset9 = instr & 0x1FF;

			// verifica se alguma flag foi recebida
			// e se foi, se ela está ativa no registrador
			if ((n & ((reg[CC] >> 2) & 0x1)) ||
				(z & ((reg[CC] >> 1) & 0x1)) ||
				(p & ((reg[CC] >> 0) & 0x1)))
			{
				// caso alguma se verdade é atribuido pcoffset 9 bits ao PC
				reg[PC] += sext(pcoffset9, 9);
			}
			
			break;
		}
		case JMP:
		{
			// faz um pulo incondicional atravez do reg lido
			// ou faz o retorno da função, onde é atribuido o reg 7
			uint16_t baser = (instr >> 6) & 0x7;

			reg[PC] = reg[baser];
			
			break;
		}
		case JSR:
		{
			// salva estado do PC em R7
			reg[R7] = reg[PC];

			// verifica se o bit 11 é 0
			if (!((instr >> 11) & 0x1))
			{
				// se for ele atribui para PC o valor do registrador base
				uint16_t baser = (instr >> 6) & 0x7;
				reg[PC] = reg[baser];
			}
			else
			{
				// senão ele atribui o offset extendido
				uint16_t pcoffset11 = instr & 0x7FF;
				reg[PC] += sext(pcoffset11, 11);
			}

			break;
		}
		case LD:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t pcoffset9 = instr & 0x1FF;

			reg[dr] = mem[reg[PC] + sext(pcoffset9, 9)];

			setcc(reg[dr]);

			break;
		}
		case LDI:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t pcoffset9 = instr & 0x1FF;

			reg[dr] = mem[mem[reg[PC] + sext(pcoffset9, 9)]];

			setcc(reg[dr]);

			break;
		}
		case LDR:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t baser = (instr >> 6) & 0x7;
			uint16_t pcoffset6 = instr & 0x3F;

			reg[dr] = mem[reg[baser] + sext(pcoffset6, 6)];

			setcc(reg[dr]);

			break;
		}
		case LEA:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t pcoffset9 = instr & 0x1FF;

			reg[dr] = reg[PC] + sext(pcoffset9, 9);

			setcc(reg[dr]);

			break;
		}
		case NOT:
		{
			uint16_t dr = (instr >> 9) & 0x7;
			uint16_t sr = (instr >> 6) & 0x7;

			reg[dr] = ~reg[sr];

			setcc(reg[dr]);

			break;
		}
		case RTI:
		case RES:
		default:
		{
			break;
		}
	}
}

// Load the program file and put it on memory.
bool load(const char* path)
{
	std::ifstream is;
	std::vector<uint16_t> rawFileBuffer;

	// debug file
	//hxe::println(path);
	std::stringstream np;
	np << path;
	np << "add_pos.obj";
	is.open(np.str().c_str(), std::ios::binary);
	//hxe::println(np.str().c_str());
	// ---------
	//is.open(path, std::ios::binary);

	if (!is.is_open())
	{
		std::cout << "not possible to load file" << std::endl;
		return false;
	}

	is.seekg(0, std::ios::end);
	size_t filesize = is.tellg();
	is.seekg(0, std::ios::beg);

	rawFileBuffer.resize(filesize / sizeof(uint16_t));

	is.read((char*)rawFileBuffer.data(), filesize);

	//hxe::println("File:");
	//for (size_t i = 0; i < rawFileBuffer.size(); i++) {
	//    if (rawFileBuffer[i] > 0) {
	//        hxe::print("Pos ");
	//        hxe::printHex(i);
	//        hxe::printHex(rawFileBuffer[i]);
	//    }
	//}

	uint16_t iaddr = (rawFileBuffer[0] >> 8) | (rawFileBuffer[0] << 8);
	if (iaddr < 0x3000)
	{
		std::cout << "initial address should be equal or greather than 0x3000" << std::endl;
		return false;
	}
	// atribui o endereço inicial ao pc
	reg[PC] = iaddr;
	// faz o swap dos bytes e carrega o programa para a memória
	uint16_t memi = iaddr;
	for (size_t i = 1; i < rawFileBuffer.size(); i++)
	{
		// swap bytes
		mem[memi] = (rawFileBuffer[i] >> 8) | (rawFileBuffer[i] << 8);
		memi++;
		if(memi > 0xFDFF)
		{
			std::cout << "user program memory overflow" << std::endl;
			return false;
		}
	}

	//hxe::println("Memory swaped:");
	//for (uint16_t i = 0; i < UINT16_MAX; i++) {
	//    if (memory[i] > 0) {
	//        hxe::print("Pos ");
	//        hxe::printHex(i);
	//        hxe::printHex(memory[i]);
	//    }
	//}

	return true;
}

int main(int argc, const char* argv[])
{
	// verifica se possui argumentos
	if (argc == 1)
	{
		std::cout << "lc3 \"obj program path\"" << std::endl;
		return EXIT_FAILURE;
	}

	// verifica se possui argumentos
	if (argc > 1)
	{
		//hxe::print(argv[1]);
		if (!load(argv[1]))
		{
			return EXIT_FAILURE;
		}
	}

	// TODO: inicializar variáveis

	while (running)
	{
		eval();
	}

	return EXIT_SUCCESS;
}
