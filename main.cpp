#include <SDL.h>
#include "CHIP8.h"
#include <iostream>
#include <chrono>
#include <thread>


const int KEYMAP_SIZE = 16;

uint8_t keymap[KEYMAP_SIZE]{ // KEYBOARD_KEY = CHIP8_KEY
        SDLK_x,     // x = 0
        SDLK_1,     // 1 = 1
        SDLK_2,     // 2 = 2
        SDLK_3,     // 3 = 3
        SDLK_q,     // q = 4
        SDLK_w,     // w = 5
        SDLK_e,     // e = 6
        SDLK_a,     // a = 7
        SDLK_s,     // s = 8
        SDLK_d,     // d = 9
        SDLK_z,     // z = A
        SDLK_c,     // c = B
        SDLK_4,     // 4 = C
        SDLK_r,     // r = D
        SDLK_f,     // f = E
        SDLK_v,     // v = F
};

int main(int argc, char* argv[]) {
    std::string gameFilePath = "C:/Users/LENOVO/source/repos/CHIP8/ROMs/TETRIS.ch8";

    CHIP8 chip8;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL could not initialize. Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    const int WINDOW_HEIGHT = 320, WINDOW_WIDTH = 640;

    SDL_Window* window = nullptr;
    window = SDL_CreateWindow("CHIP8 Emulator Screen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Window could not initialize. Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = nullptr;
    renderer = SDL_CreateRenderer(window, -1, 0);  // Alternatively use SDL_RENDERER_ACCELERATED in third argument for fastest rendering(GPU)
    if (renderer == nullptr) {
        std::cout << "Renderer could not initialize. Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    SDL_Texture* texture = nullptr;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (texture == nullptr) {
        std::cout << "Texture could not initialize. Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    chip8.loadGame(gameFilePath);
    uint32_t SDLpixelBuffer[WINDOW_WIDTH * WINDOW_HEIGHT];

    while (true) {
        chip8.cycle();
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            SDL_Keycode key;
            if (event.type == SDL_KEYDOWN) {
                key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) {
                    SDL_Quit();
                    exit(0);
                }
                for (int i = 0; i < KEYMAP_SIZE; ++i) {
                    if (key == keymap[i])
                        chip8.keypad[i] = 1;
                }
            }
            else if (event.type == SDL_KEYUP) {
                key = event.key.keysym.sym;
                for (int i = 0; i < KEYMAP_SIZE; ++i) {
                    if (key == keymap[i]) {
                        chip8.keypad[i] = 0;
                    }
                }
            }
            else if (event.type == SDL_QUIT) {
                SDL_Quit();
                exit(0);
            }

        }

        if (chip8.drawFlag) {
            chip8.drawFlag = false;

            for (int i = 0; i < (WINDOW_WIDTH * WINDOW_HEIGHT); ++i) {
                uint8_t pixel = chip8.display[i];
                if (pixel != 0)
                    SDLpixelBuffer[i] = 0xFFFFFFFF;
                else
                    SDLpixelBuffer[i] = 0x00000000;
            }

            SDL_UpdateTexture(texture, NULL, SDLpixelBuffer, 64 * sizeof(Uint32));    // updating SDL display
            SDL_RenderClear(renderer);                          // clear previous data in renderer to prepare for current frame
            SDL_RenderCopy(renderer, texture, NULL, NULL);      // draws current framne(texture)
            SDL_RenderPresent(renderer);                        // displays the frame on the display window
        }
        std::this_thread::sleep_for(std::chrono::microseconds(2000));
    }
    SDL_Quit();
}
