#include <iostream>
#include "Chip8.h"

constexpr bool CLOSE_ON_UNKNOWN_OPCODE = false;

int main(int argc, char* argv[]) {
    Display display;
    Chip8 cpu;
    sf::Clock cpuTimer;
    sf::Clock delayTimer;

    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: chip8 [rom]" << std::endl;
            exit(0);
        } else {
            cpu.load(arg);
        }
    }

    while (display.window.isOpen()) {
        sf::Event event; // NOLINT
        while (display.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                display.window.close();
            }
        }

        if (cpu.romLoaded) {
            if (delayTimer.getElapsedTime().asSeconds() > TIMER_FREQUENCY) {
                cpu.tickTimers();
                display.draw(cpu.state.vram);
                delayTimer.restart();
            }

            if (cpuTimer.getElapsedTime().asSeconds() > CPU_FREQUENCY) {
                try {
                    cpu.step();
                } catch (const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                    if (CLOSE_ON_UNKNOWN_OPCODE) {
                        display.window.close();
                    }
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                    display.window.close();
                }
                cpuTimer.restart();
            }
        }
    }

    return 0;
}
