#pragma once

#include <string>
#include <random>
#include <chrono>
#include <cstdint>


class chip8 {
    public:
        chip8();
        void loadGame(char const* gameName);
        void emulateCycle();
        uint32_t video[64 * 32]{};
        uint8_t keypad[16]{};
       
        
    private:
        uint16_t _opcode; //chip8 has 2byte opcodes
        uint8_t _memory[4096]; //4kb memory
        uint8_t _regs[16]; //16 registers, 15 8-bit general purpose, 1 for carry flag
        uint16_t _I;
        uint16_t _pc;
        uint16_t _stack[16];
        uint16_t _sp;
        uint8_t _delayTimer;
        uint8_t _soundTimer;

        std::default_random_engine randGen;
        std::uniform_int_distribution<int> randByte;
        
};

/*
    Chip8 Memory Map

    0x000-0x1FF: In the original hardware, the first 512 bytes are occupied by the Chip-8 interpreter.
    0x200: Programs usually start here
    0xEA0-0xEFF: Reserved for callstack and other stuff
    0xF00-0xFFF: Reserved for display refresh

*/