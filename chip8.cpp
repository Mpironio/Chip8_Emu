#include "chip8.h"
#include <fstream>

const WORD START_ADRESS = 0x200;
const WORD FONTSET_START_ADRESS = 0x050;
const WORD FONTSET[80] =
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
chip8::chip8() {
	_opcode = 0;
	_pc = START_ADRESS;
	_I = 0;
	_sp = 0;

	_regs[16] = { 0 };
	_stack[16] = { 0 };
	_memory[4096] = { 0 };
	_graphics[64 * 32] = { 0 };

	for (int i = 0; i < 80; i++) {
		_memory[FONTSET_START_ADRESS + i] = FONTSET[i];
	}

};

void chip8::loadGame(char const* gameName) {
	//Abrimos el archivo en binario
	std::ifstream file(gameName, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg(); //.tellg() nos da el offset en bytes del principio del archivo al último caracter
		char* buffer = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (unsigned int i = 0; i < size; i++) {
			_memory[START_ADRESS + i] = buffer[i];
		}
		delete[] buffer;
	}

	
}

void chip8::emulateCycle() {
	//Fetch
	_opcode = _memory[_pc] << 8 | _memory[_pc + 1];

	//Decode
	switch (_opcode & 0xF000) {
	case 0x0000:
		switch (_opcode & 0x000F) {
		case 0x0000: //Caso 00E0: CLS Clear 
			//(si haces la intersección de arriba el resultado es 0x0000 y entra en este caso)
			for (int i = 0; i < 64 * 32; i++) {
				_graphics[i] = 0;
			}
			_pc += 2;
		case 0x000E: //Caso 00EE: RET
			_sp--;
			_pc = _stack[_sp];
		}
	}

}

