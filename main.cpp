#include <SDL.h>
#include "CHIP8.h"
#include <iostream>
#include <chrono>
#include <thread>

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
    std::string gameFilePath = "C:/Users/LENOVO/source/repos/CHIP8/ROMs/IBM_Logo.ch8";

    CHIP8 chip8;

    int height = 200;
    int width = 400;

    
}
