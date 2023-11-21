#include "Window.h"

Window::Window()
{	
	SDL_Init(SDL_INIT_VIDEO);
	rWindow = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_SHOWN);
	rSurface = SDL_GetWindowSurface(rWindow);
	w =h= 512;
	SDL_SetSurfaceBlendMode(rSurface, SDL_BLENDMODE_NONE);
}

Window::Window(int x, int y)
{
	SDL_Init(SDL_INIT_VIDEO);
	w = x;
	h = y;
	rWindow = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	rSurface = SDL_GetWindowSurface(rWindow);
	SDL_SetSurfaceBlendMode(rSurface, SDL_BLENDMODE_NONE);
}

void Window::UpdateSurface()
{
	SDL_UpdateWindowSurface(rWindow);
}



Window::~Window()
{	
}

void Window::Events()
{
	while (SDL_PollEvent(&wEvent) !=0)
	{
		if (wEvent.type == SDL_QUIT) {
			exit = true;
		}		
	}
}

void Window::closeWindow() {
	SDL_FreeSurface(rSurface);
	SDL_DestroyWindow(rWindow);
	SDL_Quit();
}




