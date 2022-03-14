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
	WORD Vx = _regs[(_opcode >> 8) & 0xF];
	WORD Vy = _regs[(_opcode >> 4) & 0xF];
	WORD Vf = _regs[15];
	switch (_opcode & 0xF000) {
	case 0x0000:
		switch (_opcode & 0x000F) {
		case 0x0000: //Caso 00E0: CLS Clear 
			//(si haces la intersección de arriba el resultado es 0x0000 y entra en este caso)
			for (int i = 0; i < 64 * 32; i++) {
				_graphics[i] = 0;
			}
			_pc += 2;
			break;
		case 0x000E: //Caso 00EE: RET
			_sp--;
			_pc = _stack[_sp];
			break;
		}
	case 0x1000:	//Caso 1nnn: JP addr
		_pc = _opcode & 0x0FFF;
		break;

	case 0x2000:	//Caso 2nnn: CALL addr
		_sp++;
		_stack[_sp] = _pc;
		_pc = _opcode & 0x0FFF;
		break;

	case 0x3000:	//Caso 3xkk: SE Vx, byte
		Vx == _opcode & 0x00FF ? _pc += 4 : _pc = _pc + 2;
		break;

	case 0x4000:	//Caso 4xkk: SNE Vx, byte
		_regs[(_opcode >> 8) & 0xF] != _opcode & 0x00FF ? _pc += 4 : _pc = _pc + 2;
		break;

	case 0x5000:	//Caso 5xy0: SE Vx, Vy
		Vx == Vy ? _pc += 4 : _pc = _pc + 2;
		break;

	case 0x6000:	//Caso 6xkk: LD Vx, byte
		Vx = _opcode & 0xFF;
		_pc += 2;
		break;

	case 0x7000:	//Caso 7xkk: ADD Vx, byte
		Vx += _opcode & 0xFF;
		_pc += 2;
		break;

	case 0x8000:	
		switch (_opcode & 0xF) {
		case 0x0:	//Caso 8xy0: LD Vx, Vy
			Vx = Vy;
			_pc += 2;
			break;
		case 0x1:	//Caso 8xy1: OR Vx, Vy
			Vx = Vx | Vy;
			_pc += 2;
			break;
		case 0x2:	//Caso 8xy2: AND Vx, Vy
			Vx = Vx & Vy;
			_pc += 2;
			break;
		case 0x3:	//Caso 8xy3: XOR Vx, Vy
			Vx = Vx ^ Vy;
			_pc += 2;
			break;
		case 0x4:	//Caso 8xy4: ADD Vx, Vy
			Vx + Vy > 255 ? Vx = Vx + Vy, Vf = 1 : Vf = 0;
			_pc += 2;
			break;
		case 0x5: 	//Caso 8xy5: SUB Vx, Vy
			Vx > Vy ? Vf = 1 : Vf = 0;
			Vx = Vx - Vy;
			_pc += 2;
		
			break;
		case 0x6:	//Caso 8xy6: SHR Vx {, Vy}
			Vx & 0xF == 1 ? Vf = 1 : Vf = 0;
			Vx /= 2;
			_pc += 2;
			break;
		case 0x7:	//Caso 8xy7: SUBN Vx, Vy
			Vy > Vx ? Vf = 1 : Vf = 0;
			Vx = Vy - Vx;
			_pc += 2;
			break;
		case 0xE:	//Caso 8xyE: SHL Vx {, Vy}
			Vx & 0xF == 1 ? Vf = 1 : Vf = 0;
			Vx *= 2;
			_pc += 2;
			break;


		}
	case 0x9000:	//Caso 9xy0: SNE Vx, Vy
		Vx != Vy ? _pc += 4 : _pc = _pc + 2;
		break;

	case 0xA000:	//Caso Annn: LD I, addr
		_I = _opcode & 0x0FFF;
		_pc += 2;
		break;
	
	case 0xB000:	//Caso Bnnn: JP V0, addr
		_pc = (_opcode & 0xFFF) + _regs[0];
		break;

	case 0xC000:	//Caso Cxkk: RND Vx, byte
		// Vx = número random ( 0 - 255 ) & kk
		_pc += 2;
		break;

	case 0xD000:	//Caso Dxyn: DRW Vx, Vy, nibble
		_pc += 2;
		break;
	
	case 0xE000:
		switch (_opcode & 0xF) {
		case 0x000E: //Caso Ex9E: SKP Vx
			_key[_regs[(_opcode & 0xF00) >> 8]] != 0 ? _pc += 4 : _pc += 2;
			break;
		case 0x0001: //Caso ExA1: SKNP Vx
			_key[_regs[(_opcode & 0xF00) >> 8]] == 0 ? _pc += 4 : _pc += 2;
			break;
		}

	case 0xF000:
		switch (_opcode & 0xFF) {
		case 0x0007: //Caso Fx07: LD Vx, DT
			Vx = _delayTimer;
			_pc += 2;
			break;
		case 0x000A: //Caso Fx0A: LD Vx, K
			_key[(_opcode & 0xF00) >> 8] ? _regs[Vx] = (_opcode & 0xF00) >> 8, _pc += 2 : _pc -= 2; //la forma de que una instrucción se repita hasta que se haga lo pedido es que el pc vuelva al lugar donde empezaba esa instrucción
			break;
		case 0x0015: //Caso Fx15: LD DT, Vx
			_delayTimer = Vx;
			_pc += 2;
			break;
		case 0x0018: //Caso Fx18: LD ST, Vx
			_soundTimer = Vx;
			_pc += 2;
			break;
		case 0x001E: //Caso Fx1E: ADD I, Vx
			_I += Vx;
			_pc += 2;
			break;
		case 0x0029: //Caso Fx29: LD F, Vx	//Se guarda en _I la posición de memoria donde se encuentra el carácter almacenado en Vx. El fontset arranca en 0x50
					 //Cada caracter se representa con 5 líneas de 1 byte, por lo que tendremos un caracter distinto cada 5 bytes
			_I = FONTSET_START_ADRESS + (5 * _regs[Vx]);
			_pc += 2;
			break;
		case 0x0033: //Caso Fx33: LD B, Vx
			BYTE value = _regs[Vx];

			_memory[_I + 4] = value % 10;
			value /= 10;

			_memory[_I + 2] = value % 10;
			value /= 10;

			_memory[_I] = value % 10;
			value /= 10;

			_pc += 2;
			break;
		case 0x0055: //Caso Fx55: LD [I], Vx
			for (int i = 0; i < Vx; i++) {
				_memory[_I + i] = _regs[i];
			}
			_pc += 2;
			break;
		case 0x0065: //Caso LD Vx, [I]
			for (int i = 0; i < Vx; i++) {
				_regs[i] = _memory[_I + i];
			}
			_pc += 2;
			break;
		}
		
	}


}

