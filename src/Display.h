#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <cstdint>
#include <SFML/Graphics.hpp>

constexpr int DISPLAY_WIDTH = 64;
constexpr int DISPLAY_HEIGHT = 32;
constexpr int DISPLAY_SCALE = 4;
const std::string WIN_TITLE = "Chip-8";

using vram_t = std::array<std::array<uint8_t, DISPLAY_WIDTH>, DISPLAY_HEIGHT>;

class Display {
public:
    Display();
    void draw(vram_t& vram);
    sf::RenderWindow window;
private:
    sf::Image dispImage;
    sf::Texture dispTexture;
    sf::Sprite dispSprite;
};


#endif //CHIP8_DISPLAY_H
