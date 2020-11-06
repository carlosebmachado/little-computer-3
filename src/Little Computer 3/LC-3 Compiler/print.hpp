#ifndef PRINT_HPP
#define PRINT_HPP

#include <iostream>
#include <bitset>

namespace hxe
{
    void println(const char* str = "", ...)
    {
        std::cout << str << std::endl;
    }

    void print(const char* str = "")
    {
        std::cout << str;
    }

    template<typename T>
    void printHex(T n)
    {
        std::cout << "Hex: ";
        std::cout << std::hex << n << std::dec << std::endl;
    }

    template<typename T>
    void printBin(T n)
    {
        std::cout << "Bin: ";
        std::cout << std::bitset<16>(n) << std::endl;
    }

    template<typename T>
    void printOct(T n)
    {
        std::cout << "Oct: ";
        std::cout << std::oct << n << std::dec << std::endl;
    }

    template<typename T>
    void printDec(T n)
    {
        std::cout << "Dec: ";
        std::cout << n << std::endl;
    }

    template<typename T>
    void printCustom(T n, std::string format)
    {
        for (char c : format)
        {
            switch (c)
            {
            case 'd':
                printDec(n);
                break;
            case 'b':
                printBin(n);
                break;
            case 'h':
                printHex(n);
                break;
            case 'o':
                printOct(n);
                break;
            default:
                std::cout << "unknown format: " << c << std::endl;
                break;
            }
        }
    }
}

#endif // PRINT_HPP
