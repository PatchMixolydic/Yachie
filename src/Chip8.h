#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <array>
#include <cstdint>
#include "Display.h"

constexpr int PROGRAM_OFFSET = 0x200;
constexpr int MEMORY_SIZE = 4096;
constexpr int OPCODE_SIZE = 2;
constexpr int STACK_SIZE = 16;
constexpr float TIMER_FREQUENCY = 1.f / 60.f; // Sound and delay timers are 60Hz
constexpr float CPU_FREQUENCY = 1.f / 1000.f; // CPU frequency is ill defined, using 1KHz here
constexpr uint8_t FONT_SET[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct Chip8State {
    uint8_t memory[MEMORY_SIZE];
    uint8_t v[16]; // registers
    uint8_t soundTimer;
    uint8_t delayTimer;
    uint16_t pc;
    uint16_t sp;
    uint16_t i;
    uint16_t stack[STACK_SIZE];
    vram_t vram; // values are stored y, x (col, row)
};

class Chip8 {
public:
    Chip8();
    void initState();
    void load(std::string filename);
    void step();
    void tickTimers();
    void clearVRAM();
    Chip8State state;
    bool running = false;

private:
    void pushToStack(uint16_t address);
    uint16_t popFromStack();
    // Convenience functions
    inline uint16_t opidx(uint16_t op) {return (op & 0xF000) >> 12;} // X000
    inline uint16_t addr(uint16_t op) {return op & 0x0FFF;} // 0XXX
    inline uint8_t nibble(uint16_t op) {return uint8_t(op & 0x000F);} // 000X
    inline uint16_t x(uint16_t op) {return (op & 0x0F00) >> 8;} // 0X00
    inline uint16_t y(uint16_t op) {return (op & 0x00F0) >> 4;} // 00X0
    inline uint8_t lowByte(uint16_t op) {return uint8_t(op & 0x00FF);} // 00XX
};

#endif //CHIP8_CHIP8_H
