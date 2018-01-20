#include <iostream>
#include "Chip8.h"

int main(int argc, char* argv[]) {
    Chip8 cpu;
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: chip8 [rom]" << std::endl;
            exit(0);
        } else {
            cpu.load(arg);
        }
    }
    for (uint8_t byte : cpu.state.memory) {
        std::cout << (int)byte << " ";
    }
    return 0;
}
