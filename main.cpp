#include <iostream>
#include "Chip8.h"

int main() {
    Chip8 cpu;
    for (uint8_t mem : cpu.state.memory) {
        std::cout << (int)mem << " ";
    }
    return 0;
}
