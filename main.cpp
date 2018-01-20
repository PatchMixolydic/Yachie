#include <iostream>
#include <SFML/Graphics.hpp>
#include "Chip8.h"

constexpr bool CLOSE_ON_UNKNOWN_OPCODE = false;

int main(int argc, char* argv[]) {
    sf::Window window(sf::VideoMode(DISPLAY_WIDTH, DISPLAY_HEIGHT), "Chip-8"); // TODO: placeholder, move to display class probably
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

    while (window.isOpen()) {
        sf::Event event; // NOLINT
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (delayTimer.getElapsedTime().asSeconds() > TIMER_FREQUENCY) {
            cpu.tickTimers();
            delayTimer.restart();
        }

        if (cpuTimer.getElapsedTime().asSeconds() > CPU_FREQUENCY) {
            try {
                cpu.step();
            } catch (const std::runtime_error& e) {
                std::cerr << e.what() << std::endl;
                if (CLOSE_ON_UNKNOWN_OPCODE) {
                    window.close();
                }
            }
            cpuTimer.restart();
        }
    }

    return 0;
}
