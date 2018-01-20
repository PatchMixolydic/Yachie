#include <iostream>
#include <SFML/Graphics.hpp>
#include "Chip8.h"

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
            cpu.step();
            cpuTimer.restart();
        }
    }

    return 0;
}
