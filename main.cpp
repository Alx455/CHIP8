#include <SDL.h>
#include "CHIP8.h"
#include <iostream>

const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;
const int SCALE = 10; // Scale the CHIP-8 display to make it larger

uint8_t keymap[16]{
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

int main(int argc, char* argv[]) {
    const char* romPath = "ROMs/test_opcode.ch8";

    CHIP8 firstDevice;
    firstDevice.loadGame(romPath);

    int height = 500;
    int width = 1000;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    return 0;
}
