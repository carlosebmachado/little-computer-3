#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <sstream>
#include "print.hpp"


// ENUMS

// Registers.
enum
{
    R_R0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_CC,
    R_COUNT
};

// Operations.
enum
{
    OP_BR,  // branch
    OP_ADD, // addtion
    OP_LD,  // load
    OP_ST,  // store
    OP_JSR, // jump register
    OP_AND, // bitwise and
    OP_LDR, // load register
    OP_STR, // store register
    OP_RTI, // return from interrupt
    OP_NOT, // bitwise not
    OP_LDI, // load indirect
    OP_STI, // store indirect
    OP_JMP, // jump
    OP_RES, // reserved (unused)
    OP_LEA, // load effective address
    OP_TRAP // trap operation
};

// Trap operations.
enum
{
    TRAP_GETC  = 0x20, // obtém o caractere do teclado (não printa no console)
    TRAP_OUT   = 0x21, // printa um caractere
    TRAP_PUTS  = 0x22, // printa uma string palavra
    TRAP_IN    = 0x23, // obtém o caractere do teclado (printa no console)
    TRAP_PUTSP = 0x24, // printa uma string byte
    TRAP_HALT  = 0x25  // finaliza o programa
};

// Condition codes.
enum
{
    CC_P = 0x1, // P
    CC_Z = 0x2, // Z
    CC_N = 0x4  // N
};

enum
{
    MR_KBSR = 0xFE00, // keyboard status
    MR_KBDR = 0xFE02  // keyboard data
};


// Load the program file and put it on memory.
bool compile(const char* path)
{
    // debug file
    std::stringstream np;
    np << path;
    np << "add_pos.asm";
    std::fstream file(np.str().c_str());
    // ---------
    //std::fstream file(path);



    return true;
}

int main(int argc, const char* argv[])
{
    // verifica se possui argumentos
    if (argc == 1)
    {
        std::cout << "lc3 \"obj prog path\"" << std::endl;
        return EXIT_FAILURE;
    }

    // verifica se possui argumentos
    if (argc > 1)
    {
        //hxe::print(argv[1]);
        if (!compile(argv[1]))
        {
            return EXIT_FAILURE;
        }
    }

    // TODO: compilar

    return EXIT_SUCCESS;
}

