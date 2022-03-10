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
	
}