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
    std::ifstream romFile(filename, std::ios::in | std::ios::binary);
    if (!romFile.is_open()) {
        std::cerr << "Couldn't load rom " << filename << std::endl;
        return;
    }
    initState();
    // seems hacky, that's what I get for using uint8_t
    char c;
    int offset = PROGRAM_OFFSET;
    while (romFile.get(c)) {
        state.memory[offset] = (uint8_t)c;
        offset++;
    }
}

int Chip8::step() {
    uint16_t opcode = getOpcode();
    state.pc += 2;
    if (opcode == 0x00E0) {
        std::cout << "CLS" << std::endl; // CLS
    } else if (opcode == 0x00EE) {
        std::cout << "RET" << std::endl; // RET
    } else if (opidx(opcode) == 0x0) {
        std::cout << "SYS " << std::hex << addr(opcode) << std::endl; // SYS, deprecated, just nop
    } else if (opidx(opcode) == 0x1) {
        std::cout << "JP " << std::hex << addr(opcode) << std::endl; // JMP
    } else if (opidx(opcode) == 0x2) {
        std::cout << "CALL " << std::hex << addr(opcode) << std::endl; // JSR
    } else if (opidx(opcode) == 0x3) {
        std::cout << "SE V" << std::hex << x(opcode) << ", " << lowByte(opcode) << std::endl; // Skip next instruction if Vx = byte
    } else {
        return ERR_INVALID_OPCODE;
    }
    return ERR_SUCCESS;
}

void Chip8::tickTimers() {
    if (state.delayTimer > 0) state.delayTimer--;
    if (state.soundTimer > 0) {
        state.soundTimer--;
        // TODO: audio
    }
}

uint16_t Chip8::getOpcode() {
    return state.memory[state.pc] << 8 | state.memory[state.pc + 1];
}
