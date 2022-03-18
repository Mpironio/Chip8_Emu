#include "chip8.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>


const uint16_t START_ADRESS = 0x200;
const uint16_t FONTSET_START_ADRESS = 0x050;
const uint16_t FONTSET[80] =
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
chip8::chip8() 
	: randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	_opcode = 0;
	_pc = START_ADRESS;
	_I = 0;
	_sp = 0;

	for (int i = 0; i < 16; i++) {
		_regs[i] = 0;
	}

	for (int i = 0; i < 16; i++) {
		_stack[i] = 0;
	}

	for (int i = 0; i < 4096; i++) {
		_memory[i] = 0;
	}

	for (int i = 0; i < 64 * 32; i++) {
		video[i] = 0;
	}

	for (int i = 0; i < 80; i++) {
		_memory[FONTSET_START_ADRESS + i] = FONTSET[i];
	}

	randByte = std::uniform_int_distribution<int>(0, 255U);

	_delayTimer = 0;
	_soundTimer = 0;

};

void chip8::loadGame(char const* gameName) {
	//Abrimos el archivo en binario
	std::ifstream file(gameName, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::streampos size = file.tellg(); //.tellg() nos da el offset en bytes del principio del archivo al último caracter

		file.seekg(0, std::ios::beg);
		file.read((char*)&_memory[START_ADRESS], size);
		file.close();
	}
	else {
		exit(1);
	}
}

//TODO: 
/*
	- Refactorizar los cases
	- Terminar las operaciones
*/
void chip8::emulateCycle() {
	//Fetch
	_opcode = _memory[_pc] << 8 | _memory[_pc + 1];
	std::cout << "PC: " << _pc << std::endl;
	
	std::cout << std::hex << _opcode << " ";
	//Decode

	uint8_t Vx = (_opcode >> 8) & 0xF;
	uint8_t Vy = (_opcode >> 4) & 0xF;
	uint8_t Vf = 15;
	switch (_opcode & 0xF000) {

	case 0x0000:
		switch (_opcode & 0xF) {
		case 0x0000: {//Caso 00E0: CLS Clear 
			//(si haces la intersección de arriba el resultado es 0x0000 y entra en este caso)
			std::cout << "CLS Clear" << std::endl;
			for (int i = 0; i < 64 * 32; i++) {
				video[i] = 0;
			}
			_pc += 2;

		}
			break;
		case 0x000E: {//Caso 00EE: RET
			std::cout << "RET" << std::endl;
			_sp--;
			_pc = _stack[_sp] + 2;
			
		}
		   break;
		}
		break;

	case 0x1000:	//Caso 1nnn: JP addr
	{
		std::cout << "JP addr" << std::endl;
		_pc = _opcode & 0x0FFF;
	}
	break;

	case 0x2000:	//Caso 2nnn: CALL addr
	{
		std::cout << "CALL addr" << std::endl;
		_stack[_sp] = _pc;
		_sp++;
		_pc = _opcode & 0x0FFF;
	}
	break;

	case 0x3000:	//Caso 3xkk: SE Vx, byte
	{
		std::cout << "SE Vx, byte" << std::endl;
		_regs[Vx] == (_opcode & 0x00FF) ? _pc += 4 : _pc = _pc + 2;
	}
	break;

	case 0x4000:	//Caso 4xkk: SNE Vx, byte
	{
		std::cout << "SNE VX, byte" << std::endl;
		_regs[Vx] != (_opcode & 0x00FF) ? _pc += 4 : _pc = _pc + 2;
	}
	break;

	case 0x5000:	//Caso 5xy0: SE Vx, Vy
	{
		std::cout << "SE Vx, Vy" << std::endl;
		_regs[Vx] == _regs[Vy] ? _pc += 4 : _pc = _pc + 2;
	}
	break;

	case 0x6000:	//Caso 6xkk: LD Vx, byte
	{
		std::cout << "LD Vx, byte" << std::endl;
		_regs[Vx] = _opcode & 0xFF;
		_pc += 2;
	}
	break;

	case 0x7000:	//Caso 7xkk: ADD Vx, byte
	{
		std::cout << "ADD Vx, byte" << std::endl;
		_regs[Vx] += _opcode & 0xFF;
		_pc += 2;
	}
	break;

	case 0x8000:
	{
		switch (_opcode & 0xF) {
		case 0x0:	//Caso 8xy0: LD Vx, Vy
		{	
			std::cout << "LD Vx, Vy" << std::endl;
			_regs[Vx] = _regs[Vy];
			_pc += 2;
		}
		break;
		case 0x1:	//Caso 8xy1: OR Vx, Vy
		{
			std::cout << "OR Vx, Vy" << std::endl;
			_regs[Vx] |= _regs[Vy];
			_pc += 2;
		}
		break;
		case 0x2:	//Caso 8xy2: AND Vx, Vy
		{	
			std::cout << "AND Vx, Vy" << std::endl;
			_regs[Vx] &= _regs[Vy];
			_pc += 2;
		}
		break;
		case 0x3:	//Caso 8xy3: XOR Vx, Vy
		{
			std::cout << "XOR Vx, Vy" << std::endl;
			_regs[Vx] ^= _regs[Vy];
			_pc += 2;
		}
		break;
		case 0x4:	//Caso 8xy4: ADD Vx, Vy
		{	
			std::cout << "ADD Vx, Vy" << std::endl;
			_regs[Vx] + _regs[Vy] > 255 ? _regs[Vx] += _regs[Vy], _regs[Vf] = 1 : _regs[Vf] = 0;
			_pc += 2;
		}
		break;
		case 0x5: 	//Caso 8xy5: SUB Vx, Vy
		{	
			std::cout << "SUB Vx, Vy" << std::endl;
			_regs[Vx] > _regs[Vy] ? _regs[Vf] = 1 : _regs[Vf] = 0;
			_regs[Vx] -= _regs[Vy];
			_pc += 2;
		}
		break;
		case 0x6:	//Caso 8xy6: SHR Vx {, Vy} HACER
		{
			std::cout << "SHR Vx, {, Vy}" << std::endl;
			//HACER
			//HACER
			//HACER
			//HACER
			(_regs[Vx] >> 7 & 0x1) == 1 ? _regs[Vf] = 1 : _regs[Vf] = 0;
			_regs[Vx] /= 2;
			_pc += 2;
		}
		break;
		case 0x7:	//Caso 8xy7: SUBN Vx, Vy
		{	
			std::cout << "SUBN Vx, Vy" << std::endl;
			_regs[Vy] > _regs[Vx] ? _regs[Vf] = 1 : _regs[Vf] = 0;
			_regs[Vx] = _regs[Vy] - _regs[Vx];
			_pc += 2;
		}
		break;
		case 0xE:	//Caso 8xyE: SHL Vx {, Vy} Chequea el bit más significativo de Vx
		{	
			std::cout << "SHL Vx {, Vy}" << std::endl;
			(Vx >> 3 & 0xF) == 1 ? _regs[Vf] = 1 : _regs[Vf] = 0;
			_regs[Vx] *= 2;
			_pc += 2;
		}
		break;
		}
	}
	break;

	case 0x9000:	//Caso 9xy0: SNE Vx, Vy
	{	
		std::cout << "SNE Vx, Vy" << std::endl;
		_regs[Vx] != _regs[Vy] ? _pc += 4 : _pc = _pc + 2;
	}
	break;

	case 0xA000:	//Caso Annn: LD I, addr
	{	
		std::cout << "LD I, addr" << std::endl;
		_I = _opcode & 0x0FFF;
		_pc += 2;
	}
	break;

	case 0xB000:	//Caso Bnnn: JP V0, addr
	{	
		std::cout << "JP V0, addr" << std::endl;
		_pc = (_opcode & 0xFFF) + _regs[0];
	}
	break;

	case 0xC000:	//Caso Cxkk: RND Vx, byte
		
	{	
		std::cout << "RND Vx, byte" << std::endl;
		uint8_t byte = _opcode & 0x00FF;
		_regs[Vx] = randByte(randGen) & byte;
		_pc += 2;
	}
	break;

	case 0xD000: //Caso Dxyn: DRW Vx, Vy, nibble
	{	
		std::cout << "CLS DRW Vx, Vy, nibble" << std::endl;
		uint8_t height = _opcode & 0x000F;
		uint8_t xPos = _regs[Vx] % 64;
		uint8_t	yPos = _regs[Vy] % 32;

		_regs[0xF] = 0;
		for (int row = 0; row < height; row++) {
			uint8_t spriteByte = _memory[_I + row];
			for (int col = 0; col < 8; col++) {

				uint8_t spritePixel = spriteByte & (0x80 >> col);
				uint32_t* screenPixel = &video[(yPos + row) * 64 + (xPos + col)];

				if (spritePixel) {
					if (*screenPixel == 0xFFFFFFFF) {
						_regs[0xF] = 1;
					}

					*screenPixel ^= 0xFFFFFFFF;
				}
			}
		}
		_pc += 2;
	}
	break;

	case 0xE000:
	{
		switch (_opcode & 0xF) {
		case 0x000E: { //Caso Ex9E: SKP Vx
			std::cout << "SKP Vx" << std::endl;
			keypad[_regs[Vx]] != 0 ? _pc += 4 : _pc += 2;
		}
			break;
		case 0x0001: { //Caso ExA1: SKNP Vx
			std::cout << "SKNP Vx" << std::endl;
			keypad[_regs[Vx]] == 0 ? _pc += 4 : _pc += 2;
		}
			break;
		}
	}
	break;
	case 0xF000:
	{
		switch (_opcode & 0xFF) {
		case 0x0007: //Caso Fx07: LD Vx, DT
		{
			std::cout << "LD Vx, DT" << std::endl;
			_regs[Vx] = _delayTimer;
			_pc += 2;
		}
		break;
		case 0x000A: //Caso Fx0A: LD Vx, K
		{
			std::cout << "LD Vx, K" << std::endl;
			keypad[(_opcode & 0xF00) >> 8] ? _regs[Vx] = (_opcode & 0xF00) >> 8, _pc += 2 : _pc -= 2; //la forma de que una instrucción se repita hasta que se haga lo pedido es que el pc vuelva al lugar donde empezaba esa instrucción
		}
		break;
		case 0x0015: //Caso Fx15: LD DT, Vx
		{
			std::cout << "LD DT, Vx" << std::endl;
			_delayTimer = _regs[Vx];
			_pc += 2;
		}
		break;
		case 0x0018: //Caso Fx18: LD ST, Vx
		{	
			std::cout << "LD ST, Vx" << std::endl;
			_soundTimer = _regs[Vx];
			_pc += 2;
		}
		break;
		case 0x001E: //Caso Fx1E: ADD I, Vx
		{
			std::cout << "ADD I, Vx" << std::endl;
			_I += _regs[Vx];
			_pc += 2;
		}
		break;
		case 0x0029: //Caso Fx29: LD F, Vx	//Se guarda en _I la posición de memoria donde se encuentra el carácter almacenado en Vx. El fontset arranca en 0x50
					 //Cada caracter se representa con 5 líneas de 1 byte, por lo que tendremos un caracter distinto cada 5 bytes
		{	
			std::cout << "LD F, Vx" << std::endl;
			_I = FONTSET_START_ADRESS + (5 * _regs[Vx]);
			_pc += 2;
		}
		break;
		case 0x0033: //Caso Fx33: LD B, Vx
		{
			std::cout << "LD B, Vx" << std::endl;
			uint8_t value = _regs[Vx];

			_memory[_I + 4] = value % 10;
			value /= 10;

			_memory[_I + 2] = value % 10;
			value /= 10;

			_memory[_I] = value % 10;
			value /= 10;

			_pc += 2;
		}
		break;
		case 0x0055: //Caso Fx55: LD [I], Vx
		{
			std::cout << "LD [I], Vx" << std::endl;
			for (int i = 0; i < Vx; i++) {
				_memory[_I + i] = _regs[i];
			}
			_pc += 2;
		}
		break;
		case 0x0065: //Caso LD Vx, [I]
		{	
			std::cout << "LD Vx, [I]" << std::endl;
			for (int i = 0; i < Vx; i++) {
				_regs[i] = _memory[_I + i];
			}
			_pc += 2;
		}
		break;
		}

	}
	break;

	}




	if (_delayTimer > 0) {
		_delayTimer--;
	}

	if (_soundTimer > 0) {
		_soundTimer--;
	}

}


