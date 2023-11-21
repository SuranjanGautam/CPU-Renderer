#pragma once
#include <SDL.h>

class Window
{
public:
	SDL_Window* rWindow;
	SDL_Surface* rSurface;
	SDL_Event wEvent;	
	int w, h;

	bool exit = false;
	
	Window();
	Window(int, int);

	void UpdateSurface();

	void SetSurface(SDL_Surface * s) {
		SDL_SetSurfaceBlendMode(s, SDL_BLENDMODE_NONE);
		//if (SDL_MUSTLOCK(rSurface)) { SDL_LockSurface(rSurface); }		
		SDL_BlitSurface(s, NULL, rSurface, NULL);
		//if (SDL_MUSTLOCK(rSurface)) { SDL_UnlockSurface(rSurface); }
	}
	
	~Window();

	void Events();

	void closeWindow();
};

