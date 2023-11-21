#include <SDL.h>
#include <iostream>
#include "Window.h"
#include "Mat4.h"
#include"Obj.h"
#include "Renderer.h"
#include "ObjImporter.h"
#include <filesystem>

using namespace std;

int totaltick;

int main(int argc, char ** argv)
{	
	totaltick = SDL_GetTicks();

	Window mainWindow(512, 512);

	Renderer r(mainWindow);
	/*Vec3<> a[] = {Vec3<>(-1.0, -1.0, 1.0),
		Vec3<>(1.0, -1.0, 1.0),
		Vec3<>(1.0, 1.0, 1.0),
		Vec3<>(-1.0, 1.0, 1.0),
		Vec3<>(-1.0, -1.0, -1.0),
		Vec3<>(1.0, -1.0, -1.0),
		Vec3<>(1.0, 1.0, -1.0),
		Vec3<>(-1.0, 1.0, -1.0)};
	

	vector<Obj::Vertex> b;
	Obj::Vertex t(a[0]);	
	for each (Vec3<> v in a) {
		t = Obj::Vertex(v);
		b.push_back(t);
	}

	vector<int> c = { 
		0, 1, 2,
		2, 3, 0,		
		1, 5, 6,
		6, 2, 1,		
		7, 6, 5,
		5, 4, 7,		
		4, 0, 3,
		3, 7, 4,		
		4, 5, 1,
		1, 0, 4,		
		3, 2, 6,
		6, 7, 3 };

	
	
	Obj cube(b,c);*/		
	//cube = ObjImporter::Import("X://Desktop//3dmodels//monkeysmooth.obj");	
	//cube.texture = SDL_LoadBMP("monkey.bmp");
	Obj cube;
	if (argv[1] != NULL) {
		cube = ObjImporter::Import(argv[1]);	
		if (argv[2] != NULL) {
			cube.texture = SDL_LoadBMP(argv[2]);	
			if (argv[3] != NULL)
			{
				cube.normal = SDL_LoadBMP(argv[3]);
			}
		}
		
	}
	else {
		cube = ObjImporter::Import("default.obj");
	}

	//SDL_ConvertSurfaceFormat(cube.texture, SDL_PIXELFORMAT_RGBA32, 0);
	//cout << cube.texture->format->BitsPerPixel<<"-"<<r.fb->format->BitsPerPixel;	
	
	//cube2 = ObjImporter::Import("X:\\Desktop\\venus.obj");
	//cube = cube2;

	cube.Pos = Vec3<>(0, -2, 10);
	
	cube.rot = Vec3<>(0.0,0.0,0.0);		
	
	
	bool rmode = true;
	bool rmode2 = true;
	bool rot = false;
	const Uint8 * k;	
	bool temp = false;
	bool temp2 = 0;
	bool temp3 = 0;
	float movespeed = 1;
	float rotationspeed = 1;

	float deltatime = 0;


	while (!mainWindow.exit)
	{		
		int initialtick = SDL_GetTicks();
		mainWindow.Events();
		
		k = SDL_GetKeyboardState(NULL);
		if (k[SDL_SCANCODE_UP]) { cube.Pos.y = cube.Pos.y + (movespeed * deltatime); }
		if (k[SDL_SCANCODE_DOWN]) { cube.Pos.y = cube.Pos.y - (movespeed * deltatime); }
		if (k[SDL_SCANCODE_LEFT]) { cube.Pos.x = cube.Pos.x - (movespeed * deltatime); }
		if (k[SDL_SCANCODE_RIGHT]) { cube.Pos.x = cube.Pos.x + (movespeed * deltatime); }
		if (k[SDL_SCANCODE_W]) { cube.Pos.z = cube.Pos.z + (movespeed * deltatime); }
		if (k[SDL_SCANCODE_S]) { cube.Pos.z = fmax(cube.Pos.z - (movespeed * deltatime), 0); }
		if (k[SDL_SCANCODE_A]) { cube.Wrot.y = fmod(cube.Wrot.y + 0.1, 2.0 * 3.1415); }
		if (k[SDL_SCANCODE_D]) { cube.Wrot.y = fmod(cube.Wrot.y - 0.1, 2.0 * 3.1415); }
		if (k[SDL_SCANCODE_Y]) {
			r.h = r.w = r.w - 0.01;
		r.h = r.w = fmax(r.w, 0.1);
		}
		if (k[SDL_SCANCODE_T]) {r.h= r.w = r.w + 0.01; }
		if (k[SDL_SCANCODE_R]) { temp = true; }
		if (!k[SDL_SCANCODE_R] && temp) { rot = !rot; temp = false; }

		if (k[SDL_SCANCODE_H]) { temp2 = true; }
		if (!k[SDL_SCANCODE_H] && temp2) { rmode = !rmode; temp2 = false; }
		if (k[SDL_SCANCODE_I]) { temp3 = true; }
		if (!k[SDL_SCANCODE_I] && temp3) { rmode2 = !rmode2; temp3 = false; }

		if (rot) {
			cube.rot = cube.rot + Vec3<>(0.01f, 0.01f, 0.01f);
			cube.rot.x = fmod(cube.rot.x, 2 * 3.1415f);
			cube.rot.y = fmod(cube.rot.y, 2 * 3.1415f);
			cube.rot.z = fmod(cube.rot.z, 2 * 3.1415f);
		}		

		r.fill32surface(r.fb,0x000000ff);

		r.Render(&cube,rmode,rmode2);		

		r.window.SetSurface(r.fb);	
		r.window.UpdateSurface();

		r.InitZbuffer();
		
		//framerate handling
		int b = SDL_GetTicks() - initialtick;
		cout << (float)1000 / b << std::flush;
		deltatime = (float)b / 1000;	
	}

	mainWindow.closeWindow();	
	return 1;	
}