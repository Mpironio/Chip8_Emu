#include "chip8.h"

void chip8::initialize() {
    _opcode = 0;
    _pc = 0x200;
    _I = 0;
    _sp = 0;

    _regs[16] = {0};
    _stack[16] = {0};
    _memory[4096] = {0};
    //Here i should also clear the display

    for(int i = 0; i < 80; i++){
        _memory[i] = _chip8Fontset[i];
    }
}

