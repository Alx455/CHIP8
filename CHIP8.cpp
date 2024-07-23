#include "CHIP8.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>

const int FONTSET_SIZE = 80;
const int START_ADRESS = 0x200;
const int MAX_FILE_SIZE = 3584;

uint8_t fontset[FONTSET_SIZE] =
{
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

CHIP8::CHIP8() : randGen(std::random_device()()), randByte(0, 255) {
	pc = START_ADRESS;
	I = 0;
	sp = 0;
	soundTimer = 0;
	delayTimer = 0;

	std::fill(std::begin(V),std::end(V), 0);
	std::fill(std::begin(stack), std::end(stack), 0);
	std::fill(std::begin(memory), std::end(memory), 0);
	std::fill(std::begin(display), std::end(display), 0);
	std::fill(std::begin(keypad), std::end(keypad), 0);

	for (int i = 0; i < FONTSET_SIZE; i++) {
		memory[i] = fontset[i];
	}

	drawFlag = false;
	std::srand(time(NULL));
}

void CHIP8::loadGame(std::string gameFilePath) {
	std::ifstream gameFile(gameFilePath, std::ios::binary, std::ios::ate);
	if (gameFile.is_open()) {
		std::streamsize gameFileSize = gameFile.tellg();				// Retrieving size of file
		gameFile.seekg(0, std::ios::beg);								// Moving back to beginning of file
		if (gameFileSize > MAX_FILE_SIZE) {								// Exit if a file is too large(exceeds avaiable memory)
			std::cout << "Game file is too large(May be an invalid file)" << std::endl;
			return;
		}

		char* buffer = new char[gameFileSize];							// Creating buffer to load file into memory
		gameFile.read(buffer, gameFileSize);
		gameFile.close();
		
		for (int i = 0; i < gameFileSize; i++) {						// Loading file into memory
			memory[START_ADRESS + i] = buffer[i];
		}

		delete[] buffer;												// Clearing allocated buffer
	}
	else {
		std::cout << "Game file failed to open" << std::endl;
	}
}

void cycle() {

}