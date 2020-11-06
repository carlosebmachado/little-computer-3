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

// Operations.
enum
{
    BR,  // branch
    ADD, // addtion
    LD,  // load
    ST,  // store
    JSR, // jump register
    AND, // bitwise and
    LDR, // load register
    STR, // store register
    RTI, // return from interrupt
    NOT, // bitwise not
    LDI, // load indirect
    STI, // store indirect
    JMP, // jump
    RES, // reserved (unused)
    LEA, // load effective address
    TRAP // trap operation
};

// Trap operations.
enum
{
    GETC  = 0x20, // obtém o caractere do teclado (não printa no console)
    OUT   = 0x21, // printa um caractere
    TRAP_PUTS  = 0x22, // printa uma string palavra
    TRAP_IN    = 0x23, // obtém o caractere do teclado (printa no console)
    TRAP_PUTSP = 0x24, // printa uma string byte
    TRAP_HALT  = 0x25  // finaliza o programa
};

// Condition codes.
enum
{
    P = 0x1, // P
    Z = 0x2, // Z
    N = 0x4  // N
};

enum
{
    MR_KBSR = 0xFE00, // keyboard status
    MR_KBDR = 0xFE02  // keyboard data
};


// Load the program file and put it on memory.
bool load(const char* path)
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
        if (!load(argv[1]))
        {
            return EXIT_FAILURE;
        }
    }

    // TODO: compilar

    return EXIT_SUCCESS;
}

