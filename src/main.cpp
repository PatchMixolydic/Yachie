#include <iostream>
#include "Chip8.h"
#include "tinyfiledialogs.h"

constexpr sf::Keyboard::Key KEYMAP[] = {
    sf::Keyboard::X, sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3,    // 0 1 2 3
    sf::Keyboard::Q, sf::Keyboard::W, sf::Keyboard::E, sf::Keyboard::A,             // 4 5 6 7
    sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::Z, sf::Keyboard::C,             // 8 9 A B
    sf::Keyboard::Num4, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::V,          // C D E F
};

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
                } else if (cpu.state.acceptingInputInto != -1) { // might be a bit of a hack having this here
                    for (int i = 0; i < NUMBER_OF_KEYS; i++) {
                        if (event.key.code == KEYMAP[i]) {
                            cpu.keyInput(i);
                        }
                    }
                }
            }
        }

        if (cpu.state.running) {
            if (delayTimer.getElapsedTime().asSeconds() > TIMER_FREQUENCY) {
                cpu.tickTimers();
                display.draw(cpu.state.vram);
                delayTimer.restart();
            }

            if (cpuTimer.getElapsedTime().asSeconds() > CPU_FREQUENCY) {
                try {
                    for (int i = 0; i < NUMBER_OF_KEYS; i++) {
                        cpu.state.input[i] = sf::Keyboard::isKeyPressed(KEYMAP[i]); // Setup input
                    }
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
