<<<<<<< HEAD
#include "chip8.h"
#include <SDL.h>
#undef main
#include <SDL_video.h>


int window() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    else {

    }
    SDL_Quit();
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    
	return 0;
=======
#define SDL_MAIN_HANDLED
#include <SDL.h> 
#include <SDL_opengl.h>

bool CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}
	if (SDL_SetVideoMode(100, 100, 8, SDL_OPENGL) == NULL)
	{
		return false;
	}


	SDL_WM_SetCaption("CodeSlinger - Chip8 Emulator", NULL);
	return true;
}

int main() {
	CreateSDLWindow();
	return 0;
	
>>>>>>> 34d10fa2bbbc91206f64e8d28fce8ccf950a8e01
}