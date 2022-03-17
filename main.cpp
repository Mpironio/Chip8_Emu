
#include "chip8.h"
#include "platform.h"
#include <SDL.h>
#undef main
#include <iostream>
#include <chrono>


int main() {
	
	int cycleDelay = 50;
	

	chip8 chip = chip8();
	chip.loadGame("test_opcode.ch8");

	
	Platform platform("emu", 64 * 10, 32 * 10, 64, 32);

	int videoPitch = sizeof(chip.video[0]) * 64;
	
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit) {
		quit = platform.ProcessInput(chip.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay) {
			lastCycleTime = currentTime;

			chip.emulateCycle();

			platform.Update(chip.video, videoPitch);
		}
	}
	return 0;
}