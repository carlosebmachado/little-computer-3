#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <sstream>
#include <Windows.h>
#include <conio.h>
#include <thread>
#include <list>

// debug
#include "print.hpp"
#include "debug_program.h"


// DEFINES
#ifdef OUT
#undef OUT
#endif // OUT
#ifdef IN
#undef IN
#endif // IN

#define OFFSET1  0b1
#define OFFSET3  0b111
#define OFFSET5  0b11111
#define OFFSET6  0b111111
#define OFFSET7  0b1111111
#define OFFSET8  0b11111111
#define OFFSET9  0b111111111
#define OFFSET11 0b11111111111


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
	STR,	// store base offset
	RTI,	// return from interrupt ***
	NOT,	// bitwise complement
	LDI,	// load indirect
	STI,	// store indirect
	JMP,	// jump (also: RET - return from subroutine)
	RES,	// unused opcode (reserved)
	LEA,	// load effective address
	TRAP	// system call
};

// Trap operation codes.
enum Trap
{
	GETC	= 0x20, // get char
	OUT		= 0x21, // out char
	PUTS	= 0x22, // put string
	IN		= 0x23, // prompt in
	PUTSP	= 0x24, // put string double
	HALT	= 0x25
};

// Condition codes.
enum CC
{
	P = 1 << 0,
	Z = 1 << 1,
	N = 1 << 2
};

// Memory mapped registers.
enum MR
{
	KBSR = 0xFE00, // keyboard status
	KBDR = 0xFE02  // keyboard data
};


// GLOBAL VARS

// While true, the VM keep running.
bool running = true;

// Memória (65536 posições).
uint16_t mem[UINT16_MAX];

// Vetor de posições dos registradores.
uint16_t reg[NREG];

// 
HANDLE hStdin = INVALID_HANDLE_VALUE;


// FUNCTIONS

// Extends to 16 bits.
uint16_t sext(uint16_t val, int bits)
{
	// se o bit mais à esquerda for 1 é negativo
	if ((val >> (bits - 1)) & 0b1)
	{
		// extende os bits à esquerda
		val |= 0xFFFF << bits;
	}
	return val;
}

void setcc(uint16_t val)
{
	if ((val >> 15) & 0b1)
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

void kbread()
{
	while (running)
	{
		if (_kbhit())
		{
			mem[KBSR] = (1 << 15);
			mem[KBDR] = _getch();
		}
		else
		{
			mem[KBSR] = 0;
		}
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	}
}

// It fetch and evaluate the instruction.
void eval()
{
	while (running)
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
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t sr1 = (instr >> 6) & 0b111;
			uint16_t bit5 = (instr >> 5) & 0b1;

			if (!bit5)
			{
				uint16_t sr2 = instr & 0b111;
				reg[dr] = reg[sr1] + reg[sr2];
			}
			else
			{
				uint16_t imm5 = instr & 0b11111;
				reg[dr] = reg[sr1] + sext(imm5, 5);
			}

			setcc(reg[dr]);

			break;
		}
		case AND:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t sr1 = (instr >> 6) & 0b111;
			uint16_t bit5 = (instr >> 5) & 0b1;

			if (!bit5)
			{
				uint16_t sr2 = instr & 0b111;
				reg[dr] = reg[sr1] & reg[sr2];
			}
			else
			{
				uint16_t imm5 = instr & 0b11111;
				reg[dr] = reg[sr1] & sext(imm5, 5);
			}

			setcc(reg[dr]);

			break;
		}
		case BR:
		{
			uint16_t nzp = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			uint16_t NZP = reg[CC] & 0b111;

			if (nzp & NZP)
			{
				reg[PC] += sext(pcoffset9, 9);
			}

			break;
		}
		case JMP:
		{
			uint16_t baser = (instr >> 6) & 0b111;

			reg[PC] = reg[baser];

			break;
		}
		case JSR:
		{
			reg[R7] = reg[PC];

			uint16_t bit11 = (instr >> 11) & 0b1;

			if (!bit11)
			{
				uint16_t baser = (instr >> 6) & 0b111;
				reg[PC] = reg[baser];
			}
			else
			{
				uint16_t pcoffset11 = instr & 0b11111111111;
				reg[PC] += sext(pcoffset11, 11);
			}

			break;
		}
		case LD:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			uint16_t pos = reg[PC] + sext(pcoffset9, 9);

			reg[dr] = mem[pos];

			setcc(reg[dr]);

			break;
		}
		case LDI:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			uint16_t pos = reg[PC] + sext(pcoffset9, 9);

			reg[dr] = mem[mem[pos]];

			setcc(reg[dr]);

			break;
		}
		case LDR:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t baser = (instr >> 6) & 0b111;
			uint16_t pcoffset6 = instr & 0b111111;

			uint16_t pos = reg[baser] + sext(pcoffset6, 6);

			reg[dr] = mem[pos];

			setcc(reg[dr]);

			break;
		}
		case LEA:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			reg[dr] = reg[PC] + sext(pcoffset9, 9);

			setcc(reg[dr]);

			break;
		}
		case NOT:
		{
			uint16_t dr = (instr >> 9) & 0b111;
			uint16_t sr = (instr >> 6) & 0b111;

			reg[dr] = ~reg[sr];

			setcc(reg[dr]);

			break;
		}
		case RTI:
		{
			std::cout << "privilege mode exception" << std::endl;
			abort();
			break;
		}
		case ST:
		{
			uint16_t sr = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			uint16_t pos = reg[PC] + sext(pcoffset9, 9);

			mem[pos] = reg[sr];

			break;
		}
		case STI:
		{
			uint16_t sr = (instr >> 9) & 0b111;
			uint16_t pcoffset9 = instr & 0b111111111;

			uint16_t pos = reg[PC] + sext(pcoffset9, 9);

			mem[mem[pos]] = reg[sr];

			break;
		}
		case STR:
		{
			uint16_t sr = (instr >> 9) & 0b111;
			uint16_t baser = (instr >> 6) & 0b111;
			uint16_t pcoffset6 = instr & 0b111111;

			uint16_t pos = reg[baser] + sext(pcoffset6, 6);

			mem[pos] = reg[sr];

			break;
		}
		case TRAP:
		{
			uint16_t trapvect8 = instr & 0b11111111;

			switch (trapvect8)
			{
			case GETC:
			{
				reg[R0] = (uint16_t)_getch();
				break;
			}
			case OUT:
			{
				_putch(reg[R0]);
				break;
			}
			case PUTS:
			{
				uint16_t pos = reg[R0];
				while (mem[pos])
				{
					_putch(mem[pos]);
					pos++;
				}
				break;
			}
			case IN:
			{
				std::cout << "$>";
				uint16_t c = (uint16_t)_getch();
				std::cout << (char)c << std::endl;
				reg[R0] = c;
				break;
			}
			case PUTSP:
			{
				uint16_t pos = reg[R0];
				while (mem[pos])
				{
					_putch(mem[pos] & 0b11111111);
					char c = mem[pos] >> 8;
					if (c)
					{
						_putch(c);
					}
					pos++;
				}
				break;
			}
			case HALT:
			{
				std::cout << "HALT" << std::endl;
				running = 0;
				break;
			}
			default:
			{
				break;
			}
			}

			break;
		}
		case RES:
		{
			std::cout << "RES bad opcode..." << std::endl;
			abort();
			break;
		}
		default:
		{
			std::cout << "unknown opcode..." << std::endl;
			abort();
			break;
		}
		}
	}
}

uint16_t swap16(uint16_t val)
{
	return (val >> 8) | (val << 8);
}

// Load the program file and put it on memory.
bool load(const char* path)
{
	std::ifstream is;
	std::vector<uint16_t> rawFileBuffer;

	is.open(path, std::ios::binary);

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
	//	std::stringstream ns;
	//	ns << "Pos " << i+1 << "\n";
	//	hxe::print(ns.str().c_str());
	//	hxe::printBin(rawFileBuffer[i]);
	//	hxe::printBin(swap16(rawFileBuffer[i]));
	//}


	// debug program
	//rawFileBuffer = std::vector<uint16_t>();
	//int size = sizeof(debug_program) / sizeof(debug_program[0]);
	//for (int i = 0; i < size; i++) {
	//	rawFileBuffer.push_back(debug_program[i]);
	//}
	// -------------------------


	uint16_t iaddr = swap16(rawFileBuffer[0]);
	//if (iaddr < 0x3000)
	//{
	//	std::cout << "initial address should be equal or greather than 0x3000" << std::endl;
	//	return false;
	//}
	// atribui o endereço inicial ao pc
	reg[PC] = iaddr;
	// faz o swap dos bytes e carrega o programa para a memória
	uint16_t memi = iaddr;
	for (size_t i = 1; i < rawFileBuffer.size(); i++)
	{
		// swap bytes
		mem[memi] = swap16(rawFileBuffer[i]);
		memi++;
		if(memi > 0xFDFF)
		{
			std::cout << "user program memory overflow" << std::endl;
			return false;
		}
	}

	//hxe::println("\nMemory swaped:");
	//for (uint16_t i = 0; i < UINT16_MAX; i++) {
	//    if (mem[i] > 0) {
	//        hxe::print("Pos ");
	//        hxe::printHex(i);
	//        hxe::printBin(mem[i]);
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

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

	std::list<std::thread> threads;
	threads.push_back(std::thread(kbread));
	threads.push_back(std::thread(eval));

	for (auto& thread :threads)
	{
		thread.join();
	}

	return EXIT_SUCCESS;
}
