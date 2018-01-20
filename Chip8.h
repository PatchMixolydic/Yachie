#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <cstdint>

constexpr int PROGRAM_OFFSET = 0x200;
constexpr int MEMORY_SIZE = 4096;
constexpr int DISPLAY_WIDTH = 64;
constexpr int DISPLAY_HEIGHT = 32;
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
    uint16_t stack[16];
    bool vram[DISPLAY_HEIGHT][DISPLAY_WIDTH]; // values are stored y, x (col, row)
};

class Chip8 {
public:
    Chip8();
    void initState();
    void load(std::string filename);
    void step();
    void tickTimers();
    Chip8State state;

private:
    // Convenience functions
    inline uint16_t opidx(uint16_t op) {return (op & 0xF000) >> 12;}
    inline uint16_t addr(uint16_t op) {return op & 0x0FFF;}
    inline uint16_t nibble(uint16_t op) {return op & 0x000F;}
    inline uint16_t x(uint16_t op) {return (op & 0x0F00) >> 8;}
    inline uint16_t y(uint16_t op) {return (op & 0x00F0) >> 4;}
    inline uint16_t lowByte(uint16_t op) {return op & 0x00FF;}
    inline uint16_t axxb(uint16_t op) {return op & 0xF00F;}
    inline uint16_t axbc(uint16_t op) {return op & 0xF0FF;}
};

#endif //CHIP8_CHIP8_H
