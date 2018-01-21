#include <iostream>
#include "Chip8.h"
#include "tinyfiledialogs.h"

void openROM(Chip8& cpu) {
    const char* filename = tinyfd_openFileDialog("Open ROM", nullptr, 0, nullptr, nullptr, 0); // Sorry about the default location...
    if (filename != nullptr) {
        cpu.load(filename);
    }
}

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
    } else {
        openROM(cpu);
    }

    while (display.window.isOpen()) {
        sf::Event event; // NOLINT
        while (display.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                display.window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.control && event.key.code == sf::Keyboard::O) {
                    openROM(cpu);
                }
            }
        }

        if (cpu.running) {
            if (delayTimer.getElapsedTime().asSeconds() > TIMER_FREQUENCY) {
                cpu.tickTimers();
                display.draw(cpu.state.vram);
                delayTimer.restart();
            }

            if (cpuTimer.getElapsedTime().asSeconds() > CPU_FREQUENCY) {
                try {
                    cpu.step();
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
