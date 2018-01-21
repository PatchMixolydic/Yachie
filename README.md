# Yet Another Chip-8 Interpreter/Emulator (Yachie)
Chip-8 Emulator written in C++.

## Dependencies
* SFML
* CMake (or type up your own Makefile, I can't stop you)
	* CMake file not guaranteed to use best practices or even work.

## Usage
`yachie [rom]` will open a rom file.

`yachie` will open the emulator and prompt you to open a ROM.

Press CTRL+O to open a different ROM.

## Controls

The keypad:

| 1 | 2 | 3 | C |
|---|---|---|---|
| 4 | 5 | 6 | D |
| 7 | 8 | 9 | E |
| A | 0 | B | F |

is mapped to these keys:

| 1 | 2 | 3 | 4 |
|---|---|---|---|
| Q | W | E | R |
| A | S | D | F |
| Z | X | C | V |

## Thanks
Thanks to Cowgod for his [Chip-8 documentation](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) and to Guillaume Vareille for [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/).
