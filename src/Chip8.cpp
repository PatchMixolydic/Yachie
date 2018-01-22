#include <algorithm>
#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Chip8.h"

Chip8::Chip8() : rng(device()), randomDistribution(0, 255) {
    initState();
}

void Chip8::initState() {
    // Clear registers
    state.delayTimer = 0;
    state.soundTimer = 0;
    state.sp = STACK_SIZE; // Point to the top of the stack
    state.pc = PROGRAM_OFFSET; // Point to the start of the program
    // Clear memory
    std::fill(state.memory, state.memory + MEMORY_SIZE, 0);
    std::fill(state.stack, state.stack + STACK_SIZE, 0);
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
    state.running = true;
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
        // RET
        state.pc = popFromStack();
    } else if (opidx(opcode) == 0x0) {
        ; // SYS, deprecated, just nop
    } else if (opidx(opcode) == 0x1) {
        // JMP
        state.pc = addr(opcode);
    } else if (opidx(opcode) == 0x2) {
        // JSR
        pushToStack(state.pc); // already points to next opcode
        state.pc = addr(opcode);
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
        state.v[0xF] = state.v[y(opcode)] & 1;
        state.v[x(opcode)] = state.v[y(opcode)] >> 1;
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0x7) {
        // sub Vx from Vy and store result in Vx, VF is 1 if Vy > Vx
        state.v[0xF] = state.v[y(opcode)] > state.v[x(opcode)];
        state.v[x(opcode)] = state.v[y(opcode)] - state.v[x(opcode)];
    } else if (opidx(opcode) == 0x8 && nibble(opcode) == 0xE) {
        // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        state.v[0xF] = (state.v[y(opcode)] & 0b10000000) >> 7;
        state.v[x(opcode)] = state.v[y(opcode)] << 1;
    } else if (opidx(opcode) == 0x9 && nibble(opcode) == 0x0) {
        // Skip next instruction if Vx != Vy
        if (state.v[x(opcode)] != state.v[y(opcode)]) {
            state.pc += OPCODE_SIZE;
        }
    } else if (opidx(opcode) == 0xA) {
        // load addr into I
        state.i = addr(opcode);
    } else if (opidx(opcode) == 0xB) {
        // jump to addr + v0
        state.pc = addr(opcode) + state.v[0];
    } else if (opidx(opcode) == 0xC) {
        // Random uint8 & Vx
        state.v[x(opcode)] = randomDistribution(rng) & lowByte(opcode);
    } else if (opidx(opcode) == 0xD) {
        // Read [nibble] bytes from RAM starting at $[register I] and XOR them into VRAM at (Vx, Vy), wrapping on OOB
        state.v[0xf] = 0; // set on sprite collision
        for (int yIdx = 0; yIdx < nibble(opcode); yIdx++) {
            uint8_t row = state.memory[state.i + yIdx];
            int yCoord = (state.v[y(opcode)] + yIdx) % DISPLAY_HEIGHT;
            for (int xIdx = 0; xIdx <= 8; xIdx++) {
                int xCoord = (state.v[x(opcode)] + xIdx) % DISPLAY_WIDTH;
                if ((row & (0x80 >> xIdx)) != 0) {
                    if (state.vram[yCoord][xCoord] != 0) {
                        state.v[0xf] = 1;
                    }
                    state.vram[yCoord][xCoord] ^= 1;
                }
            }
        }
    } else if (opidx(opcode) == 0xE && lowByte(opcode) == 0x9E) {
        // Skip next instruction if key [Vx] is pressed
        if (state.input[state.v[x(opcode)]]) {
            state.pc += 2;
        }
    } else if (opidx(opcode) == 0xE && lowByte(opcode) == 0xA1) {
        // Skip next instruction if key [Vx] is not pressed
        if (!state.input[state.v[x(opcode)]]) {
            state.pc += 2;
        }
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x07) {
        // Load the value of the delay timer into Vx
        state.v[x(opcode)] = state.delayTimer;
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x0A) {
        // Halt execution until next keypress, store keypress in Vx
        state.running = false;
        state.acceptingInputInto = x(opcode);
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x15) {
        // Load Vx into the delay timer
        state.delayTimer = state.v[x(opcode)];
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x18) {
        // Load Vx into the sound timer
        state.soundTimer = state.v[x(opcode)];
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x1E) {
        // Add Vx into I
        state.i += state.v[x(opcode)];
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x29) {
        // Load the address of digit Vx into I
        state.i = 0x5 * state.v[x(opcode)]; // 0x5 is the size of a character in bytes
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x33) {
        // Load BCD version of Vx into I, I+1, I+2
        uint8_t vx = state.v[x(opcode)];
        for (int i = 2; i >= 0; i--) {
            state.memory[state.i + i] = vx % 10; // 240 -> 0
            vx /= 10; // 240 -> 24
        }
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x55) {
        // Load V0-Vx into memory at $I
        for (int reg = 0; reg <= x(opcode); reg++) {
            state.memory[state.i + reg] = state.v[reg];
        }
    } else if (opidx(opcode) == 0xF && lowByte(opcode) == 0x65) {
        // Load registers V0-Vx from $I
        for (int reg = 0; reg <= x(opcode); reg++) {
            state.v[reg] = state.memory[state.i + reg];
        }
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

void Chip8::pushToStack(uint16_t address) {
    if (state.sp <= 0) { // Stack is full
        std::cerr << "Tried to push with a full stack, ignoring" << std::endl;
        return;
    } else if (state.sp > STACK_SIZE) { // Stack pointer out of bounds
        std::cerr << "Stack pointer out of bounds, resetting to sane value" << std::endl;
        state.sp = STACK_SIZE;
    }
    state.sp--;
    state.stack[state.sp] = address;
}

uint16_t Chip8::popFromStack() {
    if (state.sp <= 0) { // Stack pointer out of bounds
        std::cerr << "Stack pointer out of bounds, resetting to sane value" << std::endl;
        state.sp = 0;
    } else if (state.sp >= STACK_SIZE) { // Stack is empty
        std::cerr << "Tried to pop with an empty stack, returning 0" << std::endl;
        return 0;
    }
    uint16_t res = state.stack[state.sp];
    state.sp++;
    return res;
}

void Chip8::keyInput(uint8_t keyId) {
    if (state.acceptingInputInto == -1) {
        return;
    }
    state.v[state.acceptingInputInto] = keyId;
    state.acceptingInputInto = -1;
    state.running = true;
}
