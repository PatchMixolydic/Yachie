#include <iostream>
#include "Display.h"

Display::Display() : window(sf::VideoMode(DISPLAY_WIDTH * DISPLAY_SCALE, DISPLAY_HEIGHT * DISPLAY_SCALE), WIN_TITLE) {
    dispImage.create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    dispTexture.create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    dispSprite.setTexture(dispTexture);
    dispSprite.setScale(DISPLAY_SCALE, DISPLAY_SCALE);
}

void Display::draw(vram_t& vram) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            sf::Color color = vram[y][x] == 0 ? sf::Color::Black : sf::Color::White;
            dispImage.setPixel(x, y, color);
        }
    }
    dispTexture.loadFromImage(dispImage);
    window.clear(sf::Color(255, 0, 0, 255));
    window.draw(dispSprite);
    window.display();
}
