#ifndef CHIP8_H
#define CHIP8_H

#include <string>
#include <random>
#include <chrono>

class CHIP8 {
private:
	uint16_t I;									// Index Register
	uint16_t pc;								// Program Counter

	uint8_t sp;									// Stack Pointer
	uint16_t stack[16];							// 16 level Stack

	uint8_t V[16];								// 16 registers V0-VF
	uint8_t memory[4096];						// 4K Byte Memory

	uint8_t soundTimer;							// Sound Timer
	uint8_t delayTimer;							// Delay Timer

	std::default_random_engine randGen;			// Random number generator
	std::uniform_int_distribution<unsigned int> randByte;

	int extract_nibble(int, int, int);

public:
	CHIP8();

	bool drawFlag;								// Indicates need to update display

	void loadGame(const std::string gameFilePath);	// Loads a ROM file given a file path
	void cycle();								// Emulate a single cycle

	uint8_t display[64 * 32];					// Display
	uint8_t keypad[16];							// Input keys

};



#endif // CHIP8_H