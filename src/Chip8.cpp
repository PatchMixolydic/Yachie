#include <algorithm>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
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
    clearVRAM();
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

void Chip8::step() {
    if (state.pc > MEMORY_SIZE) {
        std::stringstream message;
        message << std::hex;
        message << "PC went out of bounds at 0x";
        message << (state.pc);
        throw std::out_of_range(message.str());
    }
    uint16_t opcode = state.memory[state.pc] << 8 | state.memory[state.pc + 1];
    state.pc += OPCODE_SIZE;
    if (opcode == 0x00E0) {
        clearVRAM(); // CLS
    } else if (opcode == 0x00EE) {
        std::cout << "RET" << std::endl; // RET TODO
    } else if (opidx(opcode) == 0x0) {
        ; // SYS, deprecated, just nop
    } else if (opidx(opcode) == 0x1) {
        std::cout << "JP " << std::hex << addr(opcode) << std::endl; // JMP TODO
    } else if (opidx(opcode) == 0x2) {
        std::cout << "CALL " << std::hex << addr(opcode) << std::endl; // JSR TODO
    } else if (opidx(opcode) == 0x3) {
        // Skip next instruction if Vx = byte
        if (state.v[x(opcode)] == lowByte(opcode)) {
            state.pc += OPCODE_SIZE;
        }
    } else if (opidx(opcode) == 0x4) {
        // Skip next instruction if Vx != byte
        if (state.v[x(opcode)] != lowByte(opcode)) {
            state.pc += OPCODE_SIZE;
        }
    } else if (opidx(opcode) == 0x5 && nibble(opcode) == 0x0) {
        // Skip next instruction if Vx = Vy
        if (state.v[x(opcode)] == state.v[y(opcode)]) {
            state.pc += OPCODE_SIZE;
        }
    } else if (opidx(opcode) == 0x6) {
        // load byte into register
        state.v[x(opcode)] = lowByte(opcode);
    } else if (opidx(opcode) == 0x7) {
        // add byte to register
        state.v[x(opcode)] += lowByte(opcode);
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x0) {
        // load Vy into Vx
        state.v[x(opcode)] = state.v[y(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x1) {
        // Vx | Vy
        state.v[x(opcode)] |= state.v[y(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x2) {
        // Vx & Vy
        state.v[x(opcode)] &= state.v[y(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x3) {
        // Vx ^ Vy
        state.v[x(opcode)] ^= state.v[y(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x4) {
        // add Vy to Vx, VF = carry
        uint16_t res = state.v[x(opcode)] + state.v[y(opcode)];
        state.v[0xF] = uint8_t(res & 0xFF00 != 0);
        state.v[x(opcode)] = uint8_t(res & 0x00FF);
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x5) {
        // sub Vy from Vx, VF is 1 if Vx > Vy
        state.v[0xF] = state.v[x(opcode)] > state.v[y(opcode)];
        state.v[x(opcode)] -= state.v[y(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x6) {
        // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
        state.v[0xF] = state.v[x(opcode)] & 1;
        state.v[x(opcode)] >>= 1;
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x7) {
        // sub Vx from Vy and store result in Vx, VF is 1 if Vy > Vx
        state.v[0xF] = state.v[y(opcode)] > state.v[x(opcode)];
        state.v[x(opcode)] = state.v[y(opcode)] - state.v[x(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x6) {
        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        state.v[0xF] = (state.v[x(opcode)] & 0b10000000) >> 7;
        state.v[x(opcode)] <<= 1;
    } else if (opidx(opcode) == 0x9 && nibble(opcode) == 0x0) {
        // Skip next instruction if Vx != Vy
        if (state.v[x(opcode)] != state.v[y(opcode)]) {
            state.pc += OPCODE_SIZE;
        }
    } else if (opidx(opcode) == 0xA) {
        std::cout << std::hex << "LD I, $" << addr(opcode) << std::endl; // load ram at addr into I TODO
    } else if (opidx(opcode) == 0xB) {
        std::cout << std::hex << "JP V0, $" << addr(opcode) << std::endl; // jump to addr + v0 TODO
    } else if (opidx(opcode) == 0xC) {
        std::cout << std::hex << "RND V" << x(opcode) << ", " << lowByte(opcode) << std::endl; // Random uint8 & Vx TODO
    } else if (opidx(opcode) == 0xD) {
        std::cout << std::hex << "DRW V" << x(opcode) << ", V" << y(opcode) << ", " << nibble(opcode) << std::endl;
        // Read [nibble] bytes from RAM starting at $[register I] and XOR them into VRAM at (Vx, Vy), wrapping on OOB TODO
    } else if (opidx(opcode) == 0xE && lowByte(opcode) == 0x9E) {
        std::cout << std::hex << "SKP V" << x(opcode) << std::endl; // Skip next instruction if key [Vx] is pressed TODO
    } else if (opidx(opcode) == 0xE && lowByte(opcode) == 0xA1) {
        std::cout << std::hex << "SKNP V" << x(opcode) << std::endl; // Skip next instruction if key [Vx] is not pressed TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x07) {
        std::cout << std::hex << "LD V" << x(opcode) << ", DT" << std::endl; // Load the value of the delay timer into Vx TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x0A) {
        std::cout << std::hex << "LD V" << x(opcode) << ", K" << std::endl; // Halt execution until next keypress, store keypress in Vx TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x15) {
        std::cout << std::hex << "LD DT, V" << x(opcode) << std::endl; // Load Vx into the delay timer TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x18) {
        std::cout << std::hex << "LD ST, V" << x(opcode) << std::endl; // Load Vx into the sound timer TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x1E) {
        std::cout << std::hex << "ADD I, V" << x(opcode) << std::endl; // Add Vx into I TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x29) {
        std::cout << std::hex << "LD F, V" << x(opcode) << std::endl; // Load the address of digit Vx into I TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x33) {
        std::cout << std::hex << "LD B, V" << x(opcode) << std::endl; // Load BCD version of Vx into I, I+1, I+2 TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x55) {
        std::cout << std::hex << "LD I, V" << x(opcode) << std::endl; // Load V0-Vx into memory at $I TODO
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x65) {
        std::cout << std::hex << "LD V" << x(opcode) << ", I" << std::endl; // Load registers V0-Vx from $I TODO
    } else {
        std::stringstream message;
        message << std::hex;
        message << "Unknown opcode ";
        message << opcode;
        message << " at 0x";
        message << (state.pc - OPCODE_SIZE);
        throw std::runtime_error(message.str());
    }
}

void Chip8::tickTimers() {
    if (state.delayTimer > 0) {
        state.delayTimer--;
    }
    if (state.soundTimer > 0) {
        state.soundTimer--;
        // TODO: audio
    }
}

void Chip8::clearVRAM() {
    for (auto& row : state.vram) {
        std::fill(row.begin(), row.end(), 0);
    }
}
