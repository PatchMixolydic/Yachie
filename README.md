# Yet Another Chip-8 Emulator/Interpreter (Yachie)
Chip-8 Emulator written in C++. Not done yet.

## Dependencies
* SFML
* CMake (or type up your own Makefile, I can't stop you)
	* CMake file not guaranteed to use best practices or even work. Be sure to change INCLUDE_DIR and LIBS_DIR to your directories (or nothing if you're on Linux)

## Usage
`yachie [rom]` will open a rom file.
`yachie` will just open the emulator. Right now there is no way to load a rom file from this state.

## Todo
* Implement opcodes
* Add support for loading ROMs from within the emulator

## Thanks
Thanks to Cowgod for his [Chip-8 documentation](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM).
