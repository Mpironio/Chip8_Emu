#include <string>

typedef unsigned char BYTE;
typedef unsigned short WORD;

class chip8 {
    public:
        void initialize();
        void loadGame(std::string game_name);
        void emulateCycle();
    
    private:
        WORD _opcode; //chip8 has 2byte opcodes
        BYTE _memory[4096]; //4kb memory
        BYTE _regs[16]; //16 registers, 15 8-bit general purpose, 1 for carry flag
        WORD _I;
        WORD _pc;
        WORD _stack[16];
        WORD _sp;

};

/*
    Chip8 Memory Map

    0x000-0x1FF: In the original hardware, the first 512 bytes are occupied by the Chip-8 interpreter.
    0x200: Programs usually start here
    0xEA0-0xEFF: Reserved for callstack and other stuff
    0xF00-0xFFF: Reserved for display refresh

*/