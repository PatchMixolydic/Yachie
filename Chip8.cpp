#include <algorithm>
#include <iterator>
#include <fstream>
#include <iostream>
#include "Chip8.h"

Chip8::Chip8() {
    initState();
}

void Chip8::initState() {
    // Clear registers
    state.delayTimer = 0;
    state.soundTimer = 0;
    state.sp = 0x0F; // Point to the top of the stack
    state.pc = PROGRAM_OFFSET; // Point to the start of the program
    // Clear memory
    std::fill(state.memory, state.memory + MEMORY_SIZE, 0);
    for (bool* row : state.vram) {
        std::fill(row, row + DISPLAY_WIDTH, 0);
    }
    // Put font into ROM
    std::copy(std::begin(FONT_SET), std::end(FONT_SET), std::begin(state.memory));
}

void Chip8::load(std::string filename) {
    std::ifstream romFile(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!romFile.is_open()) {
        std::cerr << "Couldn't load rom " << filename << std::endl;
        return;
    }
    std::streampos romSize = romFile.tellg(); // kind of a hack to get the rom's size
    romFile.seekg(0, std::ios::beg);
    // seems hacky, that's what I get for using uint8_t
    char c;
    int offset = PROGRAM_OFFSET;
    while (romFile.get(c)) {
        state.memory[offset] = (uint8_t)c;
        offset++;
    }
}
