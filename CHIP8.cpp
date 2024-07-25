#include "CHIP8.h"
#include <iostream>
#include <fstream>
#include <algorithm>

#include <filesystem>


const int FONTSET_SIZE = 80;
const int START_ADRESS = 0x200;
const int MAX_FILE_SIZE = 3584;

const int SHIFT_FIRST_NIBBLE = 12;
const int SHIFT_SECOND_NIBBLE = 8;
const int SHIFT_THIRD_NIBBLE = 4;
const int SHIFT_LAST_NIBBLE = 0;


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

// Isolates nibbles(4 bits) for use in opcode processing
int CHIP8::extract_nibble(int val, int shiftAmt, int mask = 0xFFFF) {
	int masked_val = val & mask;
	return (masked_val >> shiftAmt);
}


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
	std::srand(static_cast<unsigned int>(time(NULL)));
}

void CHIP8::loadGame(const std::string gameFilePath) {
	std::ifstream gameFile(gameFilePath, std::ios::in | std::ios::binary | std::ios::ate);
	if (!gameFile.is_open()) {
		std::cout << "Game file failed to open" << std::endl;
	}

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

void CHIP8::cycle() {
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];					// Fetch opcode by getting first byte
																		// shifting 8 bits to make room for second
																		// byte and combining them by logical OR
	uint8_t opcodeFirstNibble = extract_nibble(opcode, SHIFT_FIRST_NIBBLE, 0xF000);
	uint8_t opcodeLastNibble = extract_nibble(opcode, SHIFT_LAST_NIBBLE, 0x000F);
	int opcodeLastTwoNibbles = extract_nibble(opcode, SHIFT_LAST_NIBBLE, 0x00FF);
	uint8_t opcodeSecondNibble = extract_nibble(opcode, SHIFT_SECOND_NIBBLE, 0x0F00);
	uint8_t opcodeThirdNibble = extract_nibble(opcode, SHIFT_THIRD_NIBBLE, 0x00F0);


	// Opcode divided into 4 nibbles 0xNNNN, where portions are used accordingly in specific instruction
	switch (opcodeFirstNibble) {
	case 0X0:  // Possible instructions: 0x00E0, 0X00EE
		switch (opcode) {
			case 0x00E0:  // instruciton: 0x00E0
				std::fill(std::begin(display), std::end(display), 0);
				drawFlag = true;
				pc += 2;
				break;

			case 0x00EE:  // instruction: 0X00EE
				sp--;
				pc = stack[sp];
				pc += 2;
				break;
		}
		break;

	case 0X1:  // instruction: 0x1NNN
		pc = opcode & 0x0FFF;
		break;

	case 0X2:  // instruction: 0x2NNN
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFF;
		break;

	case 0X3:  // instruction: 0x3XNN

		break;

	case 0x4:  // instruction: 0x4XNN

		break;

	case 0x5:  // instruction: 0x5XY0

		break;

	case 0x6:  // instruction: 0x6XNN
		V[opcodeSecondNibble] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7:  // instruction: 0x7XNN
		V[opcodeSecondNibble] += (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x8:  // Possible instructions: 0x8XY0, 0x8XY1, 0x8XY2, 0x8XY3, 0x8XY4, 0x8XY5, 0x8XY6, 0x8XY7, 0x8XYE
		switch (opcodeLastNibble) {
			case 0x0:  // instruction: 0x8XY0
				
				break;

			case 0x1:  // instruction: 0x8XY1

				break;

			case 0x2:  // instruction: 0x8XY2

				break;

			case 0x3:  // instruction: 0x8XY3

				break;

			case 0x4:  // instruction: 0x8XY4

				break;

			case 0x5:  // instruction: 0x8XY5

				break;

			case 0x6:  // instruction: 0x8XY6

				break;

			case 0x7:  // instruction: 0x8XY7

				break;

			case 0xE:  // instruction: 0x8XYE

				break;

			
		}
		break;

	case 0x9:  // instruction: 0x9XY0

		break;

	case 0xA:  // instruction: 0xANNN
		I = (opcode & 0x0FFF);
		pc += 2;
		break;

	case 0xB:  // instruction: 0xBNNN

		break;

	case 0xC:  // instruction: 0xCXNN

		break;

	case 0xD:  // instruction: 0xDXYN
		{
		// Draws sprite at coordinates (x, y) with dimensions of 8 pixel width and N pixel height
			int x = V[opcodeSecondNibble];
			int y = V[opcodeThirdNibble];
			int width = 8;					// chip 8 pixels are 8 bits wide
			int height = opcodeLastNibble;	// height pixels determined by value in N(last nibble)
			V[0xF] = 0;						// VF flag register set to 0. If collision occurs(pixel in display buffer
			// attempts to write over a pixel that is already turned on, 
			int rowPixelValues;				// pixel value to be displayed
			for (int i = 0; i < height; i++) {  // i tracks current row
				rowPixelValues = memory[I + i]; // sprite begins at location memory[I], and is stored in rows of
												// 8 pixels. So there is N rows of 8 width
				for (int j = 0; j < width; j++) {  // j tracks current column
					int mask = 0x80 >> j;	 // creates a mask for the jth pixel(count bits(pixels) from right to left)
					if ((rowPixelValues & mask) != 0) { // checks if jth pixel is on(1)
						int coordinate = ((x + j) + (y + i) * 64) % 2048;  // calculating 2D coordinate (x, y) as 1D value in display
						if (display[coordinate] == 1)
							V[0xF] = 1;		// Collision occured, Vf flag set to 1
						display[coordinate] = display[coordinate] ^ 1;
					}
				}

			}
			drawFlag = true;
			pc += 2;
		}
		break;

	case 0xE:  // Possible instruction: 0xEX9E, 0xEXA1
		switch (opcodeLastTwoNibbles) {
			case 0x9E:  // instruction: 0xEX9E

				break;

			case 0xA1:  // instruction: 0xEXA1

				break;
		}
		break;

	case 0xF:  // Possible instructions: 0xFX07, 0xFX15, 0xFX18, 0xFX1E, 0xFX0A, 0xFX29, 0xFX33, 0xFX55, 0xFX65
		switch (opcodeLastTwoNibbles) {
			case 0x07:  // instruction: 0xFX07

				break;

			case 0x15:  // instruction: 0xFX15

				break;

			case 0x18:  // instruction: 0xFX18

				break;

			case 0x1E:  // instruction: 0xFX1E

				break;

			case 0x0A:  // instruction: 0xFX0A

				break;

			case 0x29:  // instruction: 0xFX29

				break;

			case 0x33:  // instruction: 0xFX33

				break;

			case 0x55:  // instruction: 0xFX55

				break;

			case 0x65:  // instruction: 0xFX65

				break;
		}
		break;

	default:
		std::cout << "Error: CHIP8 is attempting to process an invalid opcode" << std::endl;
	}			
}

