#pragma once
#include "Mat4.h"
#include "Window.h"
#include "Obj.h"
#include "DefaultVertexShader.h"
#include "Tri.h"
#include <ppl.h>
#include <thread>
#include <mutex>
#include <functional>
#include <algorithm>
#include "ThreadPool.h"
#include <stdio.h>


using namespace std;
typedef typename Obj::Vertex Vertex;
typedef typename DefaultVertexShader::Vertex SVertex;
class Renderer
{
public:

	Renderer() { 
		window = Window();
		Zbuffer = new float[window.w * window.h];
		PPAA = new int[window.w * window.h];
		InitZbuffer();
		fb = SDL_CreateRGBSurface(0, window.w, window.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);	
		defaultSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		fill32surface(defaultSurface,0xB4B4B4ff);
	}

	Renderer(Window x) :window(x) {
		Zbuffer = new float[window.w * window.h];
		PPAA = new int[window.w * window.h];
		InitZbuffer();
		fb = SDL_CreateRGBSurface(0, window.w, window.h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		defaultSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);	
		fill32surface(defaultSurface, 0xB4B4B4ff);
	}

	~Renderer() { window.closeWindow();
	delete Zbuffer;
	}

	static int fill32surface(SDL_Surface * s, Uint32 c) {
		if (SDL_MUSTLOCK(s)) { SDL_LockSurface(s); }
		Uint32 *j = (Uint32*)s->pixels;
		for (int i = 0; i < (s->h)*(s->w);i++) {
			*(j + i) = c;				
			}
		
		if (SDL_MUSTLOCK(s)) { SDL_UnlockSurface(s); }
		return 1;
	}

	static int PutPixel(SDL_Surface * s, int x, int y, Uint32 c) {	
		if (x < 0 || y < 0 || x >= s->w || y >= s->h||s==NULL) { return 0; }
			if (SDL_MUSTLOCK(s)) { SDL_LockSurface(s); }
			Uint32 *j = (Uint32*)s->pixels;
			int offset = (y * s->h + x);
			*(j + offset) = c;
			if (SDL_MUSTLOCK(s)) { SDL_UnlockSurface(s); }
			return 1;		
	}

	static Uint32 getpixel(SDL_Surface *surface, int x, int y)
	{
		if (x >= 0 && x < surface->w && y >= 0 && y < surface->h);
		int bpp = surface->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to retrieve */
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch (bpp) {
		case 1:
			return *p;
			break;

		case 2:
			return *(Uint16 *)p;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return (p[0] << 16 | p[1] << 8 | p[2])<<8;
			else
				return (p[0] | p[1] << 8 | p[2] << 16)<<8;
			break;

		case 4:
			return *(Uint32 *)p;
			break;

		default:
			return 0;   
		}
	}

	static int DrawLine(SDL_Surface * s, int x1, int y1, int x2, int y2, Uint32 c) {			
		int dx = x2 - x1;
		int dy = y2 - y1;
		if (dx == dy && dx==0) { return 0; }
		if (abs(dx) > abs(dy)) {
			float slope = (float) dy / dx;			
			if (x2 > x1) {
				for (int i = x1;i <= x2;i++) {
					float j = y1 + (slope*(i-x1));
					j = round(j);
					PutPixel(s, i, j, c);
				}
			}
			else {
				for (int i = x2;i <= x1;i++) {
					float j = y2 + (slope*(i-x2));
					j = round(j);
					PutPixel(s, i, j, c);
				}
			}

		}
		else {
			float slope = (float)dx / dy;			
			if (y2 > y1) {
				for (int i = y1;i <= y2;i++) {
					int j =x1+slope*(i-y1);
					j = round(j);
					PutPixel(s, j, i, c);
				}
			}
			else {
				for (int i = y2;i <= y1;i++) {
					int j = x2+slope*(i-y2);
					j = round(j);
					PutPixel(s, j, i, c);
				}
			}
		}
		return 1;
	}	

	

	int DrawTriangle(SDL_Surface *s) {		
		
		for (auto &t : final) {
			SVertex* v0 = &t.v1;
			SVertex* v1 = &t.v2;
			SVertex* v2 = &t.v3;

			if (v1->Pos.y > v0->Pos.y)  swap(v0, v1);
			if (v2->Pos.y > v0->Pos.y)  swap(v0, v2);
			if (v2->Pos.y > v1->Pos.y)  swap(v1, v2);

			if (v0->Pos.y == v1->Pos.y) {
				if (v0->Pos.x >= v1->Pos.y) {					
					DrawFB(*v0, *v1, *v2,s);
				}
				else {		
					
					DrawFB(*v1, *v0, *v2, s);
				}
			}
			else if (v1->Pos.y == v2->Pos.y) {
				if (v1->Pos.x >= v2->Pos.y) {						
					DrawFT(*v1, *v2, *v0, s);
				}
				else {
					
					DrawFT(*v2, *v1, *v0, s);
				}
			}

			else
			{
				float dd = (v1->Pos.y - v0->Pos.y) / (v2->Pos.y - v0->Pos.y);
				SVertex tv1 = SVertex::Interp(*v0, *v2, dd);			

				if (tv1.Pos.x > v1->Pos.x) {					
					DrawFB(tv1,*v1,*v2,s);
					DrawFT(tv1,*v1,*v0,s);
				}

				else {						
					DrawFB(*v1,tv1,*v2,s);
					DrawFT(*v1, tv1, *v0, s);
				}				
			}
		}			
				
		return 1;
	}

	int DrawTriangleThreaded(SDL_Surface *s) {		
		for (auto &t : final) {
			SVertex* v0 = &t.v1;
			SVertex* v1 = &t.v2;
			SVertex* v2 = &t.v3;

			if (v1->Pos.y > v0->Pos.y)  swap(v0, v1);
			if (v2->Pos.y > v0->Pos.y)  swap(v0, v2);
			if (v2->Pos.y > v1->Pos.y)  swap(v1, v2);

			if (v0->Pos.y == v1->Pos.y) {
				if (v0->Pos.x >= v1->Pos.y) {
					auto something = bind(&Renderer::DrawFB, this, *v0, *v1, *v2, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);
					//something = bind(&Renderer::DrawFB, this, *v0, *v1, *v2, s);
					//something();
					//DrawFB(*v0, *v1, *v2, s);
				}
				else {	
					auto something = bind(&Renderer::DrawFB, this, *v1, *v0, *v2, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);					
					//DrawFB(*v1, *v0, *v2, s);
				}
			}
			else if (v1->Pos.y == v2->Pos.y) {
				if (v1->Pos.x >= v2->Pos.y) {
					auto something = bind(&Renderer::DrawFT, this, *v1, *v2, *v0, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);					
					//DrawFT(*v1, *v2, *v0, s);
				}
				else {
					auto something = bind(&Renderer::DrawFT, this, *v2, *v1, *v0, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);					
					//DrawFT(*v2, *v1, *v0, s);
				}
			}

			else
			{
				float dd = (v1->Pos.y - v0->Pos.y) / (v2->Pos.y - v0->Pos.y);
				SVertex tv1 = SVertex::Interp(*v0, *v2, dd);

				if (tv1.Pos.x > v1->Pos.x) {
					auto something = bind(&Renderer::DrawFB, this, tv1, *v1, *v2, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);
					something = bind(&Renderer::DrawFT, this, tv1, *v1, *v0, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);					
					//DrawFB(tv1, *v1, *v2, s);
					//DrawFT(tv1, *v1, *v0, s);
				}

				else {
					auto something = bind(&Renderer::DrawFB, this, *v1, tv1, *v2, s);
					
					tpmain.enqueue(something);
					something = bind(&Renderer::DrawFT, this, *v1, tv1, *v0, s);
					//sdltp.enqueue(something);
					tpmain.enqueue(something);
					//DrawFB(*v1, tv1, *v2, s);
					//DrawFT(*v1, tv1, *v0, s);
				}
			}
		}		
		tpmain.waitfinsh();
		return 1;
	}

	int DrawTriangleO(SDL_Surface *s) {
		for (auto &t : final) {
			SVertex* v0 = &t.v1;
			SVertex* v1 = &t.v2;
			SVertex* v2 = &t.v3;

			if (v1->Pos.y > v0->Pos.y)  swap(v0, v1);
			if (v2->Pos.y > v0->Pos.y)  swap(v0, v2);
			if (v2->Pos.y > v1->Pos.y)  swap(v1, v2);

			if (v0->Pos.y == v1->Pos.y) {
				if (v0->Pos.x >= v1->Pos.y) {
					DrawFBO(*v0, *v1, *v2, s);
				}
				else {
					DrawFBO(*v1, *v0, *v2, s);
				}
			}
			else if (v1->Pos.y == v2->Pos.y) {
				if (v1->Pos.x >= v2->Pos.y) {
					DrawFTO(*v1, *v2, *v0, s);
				}
				else {
					DrawFTO(*v1, *v2, *v0, s);
				}
			}

			else
			{
				float dd = (v1->Pos.y - v0->Pos.y) / (v2->Pos.y - v0->Pos.y);
				SVertex tv1 = SVertex::Interp(*v0, *v2, dd);

				if (tv1.Pos.x > v1->Pos.x) {
					DrawFBO(tv1, *v1, *v2, s);
					DrawFTO(tv1, *v1, *v0, s);
				}

				else {
					DrawFBO(*v1, tv1, *v2, s);
					DrawFTO(*v1, tv1, *v0, s);
				}
			}
		}

		return 1;
	}
	
	vector<Tri<SVertex>> formTri(vector<SVertex> a,Obj *b) {
		vector<Tri<SVertex>> t;
		for (int i = 0;i < b->Index.size();i+=3) {
			Tri<SVertex> v = Tri<SVertex>(a[b->Index[i]],a[b->Index[i+1]],a[b->Index[i+2]]);
			t.push_back(v);
		}
		
		return t;	
	}

	void prespectivetransform(vector<Tri<SVertex>> &a) {
		Mat4<> t = Mat4<>::prespectiveTrans(n,f,w,h);		
		for (auto &x : a) {
			x.v1.Pos = t*x.v1.Pos;
			x.v2.Pos = t*x.v2.Pos;
			x.v3.Pos = t*x.v3.Pos;
		}
	}

	void prespectivedivide(vector<Tri<SVertex>> &a) {
		for (auto &x : a) {	

			float zinv = (float)1.0 / x.v1.Pos.w;
			x.v1 = x.v1 * zinv;
			x.v1.Pos.w = zinv;

			zinv = 1.0 / x.v2.Pos.w;
			x.v2 = x.v2 * zinv;
			x.v2.Pos.w = zinv;

			zinv = 1.0 / x.v3.Pos.w;
			x.v3 = x.v3 * zinv;			
			x.v3.Pos.w = zinv;			
		}
	}

	void BFCALT(vector<Tri<SVertex>> &a) {			
		Concurrency::combinable<vector<Tri<SVertex>>> xx;		
		concurrency::parallel_for_each(a.begin(), a.end(), [&](Tri<SVertex> h) {
			bool x = true;
			Vec3<> g = -h.v1.Pos;
			if (g * h.normal() <= 0) {
				x = false;
			}
			else if (h.v1.Pos.z > f || h.v2.Pos.z > f || h.v3.Pos.z >f) {
				x = false;
			}
			if (x) { xx.local().push_back(h); }
		});
			
		vector<Tri<SVertex>> e = xx.combine([](vector<Tri<SVertex>> h, vector<Tri<SVertex>> &j) {			
			h.insert(h.end(), j.begin(), j.end());
			return h;
		});

		a = e;		
	}

	void BFC(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> e;

		for (auto &h : a) {
			Vec3<> g = -h.v1.Pos;
			if (g * h.normal() <= 0) {
				continue;
			}
			else if (h.v1.Pos.z > f || h.v2.Pos.z > f || h.v3.Pos.z > f) {
				continue;
			}
			e.push_back(h);
		}
			a = e;		
	}


	void ClipL(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> t;
		
		for(auto &x : a){	
			vector<int> c;
			if (x.v1.Pos.x < -x.v1.Pos.w) {
				c.push_back(1);
			}
			if (x.v2.Pos.x < -x.v2.Pos.w) {
				c.push_back(2);
			}
			if (x.v3.Pos.x < -x.v3.Pos.w) {
				c.push_back(3);
			}
			if (c.size() < 3) {
				t.push_back(x);
			}				
		}
		a = t;
	}

	void ClipR(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> t;

		for (auto &x : a) {
			vector<int> c;
			if (x.v1.Pos.x > x.v1.Pos.w) {
				c.push_back(1);
			}
			if (x.v2.Pos.x > x.v2.Pos.w) {
				c.push_back(2);
			}
			if (x.v3.Pos.x > x.v3.Pos.w) {
				c.push_back(3);
			}			

			if (c.size()< 3) {
				t.push_back(x);
			}
		}
		a = t;
	}

	void ClipT(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> t;

		for (auto &x : a) {
			vector<int> c;
			if (x.v1.Pos.y > x.v1.Pos.w) {
				c.push_back(1);
			}
			if (x.v2.Pos.y > x.v2.Pos.w) {
				c.push_back(2);
			}
			if (x.v3.Pos.y > x.v3.Pos.w) {
				c.push_back(3);
			}
			
			if (c.size() < 3) {
				t.push_back(x);
			}

		}
		a=t;
	}
	
	void ClipB(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> t;

		for (auto &x : a) {
			vector<int> c;
			if (x.v1.Pos.y < -x.v1.Pos.w) {
				c.push_back(1);
			}
			if (x.v2.Pos.y < -x.v2.Pos.w) {
				c.push_back(2);
			}
			if (x.v3.Pos.y < -x.v3.Pos.w) {
				c.push_back(3);
			}

			if (c.size() < 3) {
				t.push_back(x);
			}			

		}
		a = t;
	}
	
	void ClipF(vector<Tri<SVertex>> &a) {
		vector<Tri<SVertex>> t;

		for (auto &x : a) {
			vector<int> c;
			if (x.v1.Pos.z < 0) {
				c.push_back(1);
			}
			if (x.v2.Pos.z < 0) {
				c.push_back(2);
			}
			if (x.v3.Pos.z < 0) {
				c.push_back(3);
			}
			if (c.size() == 0) {
				t.push_back(x);
			}
			else if (c.size() == 1) {
				SVertex v1, v2;
				float dd = 0;
				switch (c[0])
				{
				case 1:

					dd = (x.v3.Pos.z) / (x.v3.Pos.z - x.v1.Pos.z);
					v1 = SVertex::Interp(x.v3, x.v1, dd);

					dd = (x.v2.Pos.z) / (x.v2.Pos.z - x.v1.Pos.z);
					v2 = SVertex::Interp(x.v2, x.v1, dd);

					t.push_back(Tri<SVertex>(x.v2, x.v3, v1));
					t.push_back(Tri<SVertex>(x.v2, v1, v2));

					break;
				case 2:

					dd = (x.v1.Pos.z) / (x.v1.Pos.z - x.v2.Pos.z);
					v1 = SVertex::Interp(x.v1, x.v2, dd);


					dd = (x.v3.Pos.z) / (x.v3.Pos.z - x.v2.Pos.z);
					v2 = SVertex::Interp(x.v3, x.v2, dd);

					t.push_back(Tri<SVertex>(x.v3, x.v1, v1));
					t.push_back(Tri<SVertex>(x.v3, v1, v2));

					break;
				case 3:
					dd = (x.v2.Pos.z) / (x.v2.Pos.z - x.v3.Pos.z);
					v1 = SVertex::Interp(x.v2, x.v3, dd);

					dd = (x.v1.Pos.z) / (x.v1.Pos.z - x.v3.Pos.z);
					v2 = SVertex::Interp(x.v1, x.v3, dd);

					t.push_back(Tri<SVertex>(x.v1, x.v2, v1));
					t.push_back(Tri<SVertex>(x.v1, v1, v2));
					break;
				default:
					break;
				}

			}
			else if (c.size() == 2) {
				SVertex v1, v2;
				float dd=0;
				switch (c[0] + c[1])
				{
				case 5:
					dd = (x.v1.Pos.z) / (x.v1.Pos.z - x.v2.Pos.z);
					
					v1 = SVertex::Interp(x.v1, x.v2, dd);


					dd = (x.v1.Pos.z) / (x.v1.Pos.z - x.v3.Pos.z);

					v2 = SVertex::Interp(x.v1, x.v3, dd);

					t.push_back(Tri<SVertex>(x.v1, v1, v2));

					break;
				case 4:
					dd = (x.v2.Pos.z)  / (x.v2.Pos.z - x.v3.Pos.z);

					v1 = SVertex::Interp(x.v2, x.v3, dd);

					dd = (x.v2.Pos.z)  / (x.v2.Pos.z - x.v1.Pos.z);

					v2 = SVertex::Interp(x.v2, x.v1, dd);

					t.push_back(Tri<SVertex>(x.v2, v1, v2));
					break;
				case 3:
					dd = (x.v3.Pos.z)  / (x.v3.Pos.z - x.v1.Pos.z);

					v1 = SVertex::Interp(x.v3, x.v1, dd);


					dd = (x.v3.Pos.z)  / (x.v3.Pos.z - x.v2.Pos.z);

					v2 = SVertex::Interp(x.v3, x.v2, dd);

					t.push_back(Tri<SVertex>(x.v3, v1, v2));
					break;
				default:
					break;
				}

			}
			

		}
		a=t;
	}

	void screenspacetransformation(int w, int h, vector<Tri<SVertex>> &a) {
		for (auto &r : a) {		

			//r.v1.Pos.DisplayVec();

			auto xx = w / 2;
			auto yy = h / 2;

			r.v1.Pos.x = (r.v1.Pos.x * xx) + xx;
			r.v1.Pos.y = (-r.v1.Pos.y * yy) + yy;		

			r.v2.Pos.x = (r.v2.Pos.x * xx) + xx;
			r.v2.Pos.y = (-r.v2.Pos.y * yy) + yy;

			r.v3.Pos.x = (r.v3.Pos.x * xx) + xx;
			r.v3.Pos.y = (-r.v3.Pos.y * yy) + yy;
			
		}
	}

	void Clip(vector<Tri<SVertex>> &a) {
		ClipF(a);
		ClipL(a);
		ClipR(a);
		ClipT(a);
		ClipB(a);

	}

	void wireframeDraw(SDL_Surface *s) {
		for (auto &b : final) {
			DrawLine(s, b.v1.Pos.x, b.v1.Pos.y, b.v2.Pos.x, b.v2.Pos.y, 0x0000FFFF);
			DrawLine(s, b.v1.Pos.x, b.v1.Pos.y, b.v3.Pos.x, b.v3.Pos.y, 0x0000FFFF);
			DrawLine(s, b.v3.Pos.x, b.v3.Pos.y, b.v2.Pos.x, b.v2.Pos.y, 0x0000FFFF);
		}
	}

	int Render(Obj *a,int Mode,int Modeaa) {
		modeaa = Modeaa;
		mode = Mode;
		currentObj = a;	

		final = formTri(dvs.apply(a->Vertices, a), a);

		BFC(final);		

		prespectivetransform(final);	

		Clip(final);	

		prespectivedivide(final);	

		screenspacetransformation(window.w, window.h, final);		
		
		if (mode == 0) {
			DrawTriangleThreaded(fb);
			PutPixel(fb, fb->w-1, 0,0xff0000ff);
		}
		else {
			DrawTriangle(fb);			
		}
		if (!modeaa) {
			PutPixel(fb, fb->w - 2, 0, 0x00ff00ff);
		}

		//wireframeDraw(fb);

		
		
		for (int i = 0;i < aa.size();i += 4) {
			Uint32 t;
			if ((int)aa[i + 3]) {
				t = BlurR(fb, aa[i], aa[i + 1]);				
			}
			else {				
				t = BlurL(fb, aa[i], aa[i + 1]);
			}
			aa[i + 3] = (float)t;
		}		
		for (int i = 0;i < aa.size();i += 4) {
			AA(fb,aa[i],aa[i+1],aa[i+2],(Uint32)aa[i + 3]);
		}
		aa.clear();		
		return 1;		
	}

	void InitZbuffer() {
		for (int i = 0;i < window.w;i++) {
			for (int j = 0;j<window.h;j++) {
				*(Zbuffer + (i*window.w) + j) = INFINITY;
				*(PPAA + (i*window.w) + j) = 0;			
			}
		}
	}

	
	void DrawFT(SVertex r, SVertex l, SVertex b, SDL_Surface *s) {
		float dy = b.Pos.y - l.Pos.y;
		auto ldv = (b - l)/dy;
		auto rdv = (b - r)/dy;

		int Ystart = (int)ceil(l.Pos.y - 0.5f);
		int Yend = (int)ceil(b.Pos.y - 0.5f);

		Ystart = min(max(Ystart, 0), window.h);
		Yend = max(min(Yend, window.h), 0);

		DrawTri(ldv,rdv,l,r,b,s,Ystart,Yend);
		
	}

	void DrawFB(SVertex r, SVertex l, SVertex b, SDL_Surface *s) {
		float dy = l.Pos.y - b.Pos.y;
		auto ldv = (l - b) / dy;
		auto rdv = (r - b) / dy;

		int Ystart = (int)ceil(b.Pos.y - 0.5f);
		int Yend = (int)ceil(l.Pos.y - 0.5f);
		Ystart = min(max(Ystart, 0), window.h);
		Yend = max(min(Yend, window.h), 0);

		DrawTri(ldv, rdv, l, r, b, s, Ystart, Yend);
		
	}

	void phongshading(SDL_Surface *s, int x, int y, SVertex vert) {

		//getting the reuqired vectors
		Vec3<float> pos = -vert.Wpos;
		Vec3<float> l(1, 1, -1);
		Vec3<float> n = vert.Norm;
		Vec3 <float> t;

		//looking up texture color and normal map
		Uint32 pointcolor;
		Uint32 normalcolor;
		if (currentObj->texture == NULL) {
			pointcolor = texturelookup(defaultSurface, vert.Texc.x, vert.Texc.y,vert.Pos.z);
		}
		else
		{
			pointcolor = texturelookup(currentObj->texture, vert.Texc.x, vert.Texc.y, vert.Pos.z);
		}

		//getting tangent normal map
		if(currentObj->normal!=NULL){
			normalcolor = texturelookup(currentObj->normal, vert.Texc.x, vert.Texc.y, vert.Pos.z);
			Uint8 red = (normalcolor & 0xFF000000) >> 24;
			Uint8 blue = (normalcolor & 0x00ff0000) >> 16;
			Uint8 green = (normalcolor & 0x0000ff00) >> 8;
			float xn, yn, zn;
			xn = (float)red / 255.0f;
			yn = (float)blue / 255.0f;
			zn = (float)green / 255.0f;
			auto nn = Vec3<>(xn, yn, zn);			
			t = Vec3<>::Normalize(nn);
			n.x = n.x * t.x;
			n.y = n.y * t.y;
			n.z = n.z * t.z;
		}

		//normalizing the vectors
		l = Vec3<float>::Normalize(l);
		pos = Vec3<float>::Normalize(pos);
		n = Vec3<float>::Normalize(n);

		//calculating cos
		float dot = l * n ;
		dot =max(dot, 0.0f);		

		//calculating reflected light vector
		auto r = (n * dot * 2) - l;
		


		//extracting each channel r,g,b
		Uint8 red = (pointcolor & 0xFF000000)>>24;
		Uint8 blue = (pointcolor & 0x00ff0000)>>16;
		Uint8 green = (pointcolor & 0x0000ff00)>>8;

		//making a vec3 with normalized values of each channel 0-1 for easier calculation
		Vec3<> pcolor((float)red/255, (float)blue / 255,(float)green / 255);

		//phong calculations
		float Ifinal = 0.1f + (fmaxf(fminf(dot, 1), 0) * 0.8f);
		auto spec = (fmaxf(fminf(powf( max(r * pos,0.0f), 32), 1), 0) *0.8f);
		
		
		pcolor = pcolor * clamp<float>(Ifinal,0,1);
		pcolor = pcolor + (Vec3<>(1, 1, 1) * spec);	
		pcolor.x = clamp<float> (pcolor.x, 0, 1);
		pcolor.y = clamp<float>(pcolor.y, 0, 1);
		pcolor.z = clamp<float>(pcolor.z, 0, 1);	

		//color apply the phong illumination to each color channel
		red = 255 * pcolor.x;
		blue = 255 * pcolor.y;
		green = 255 * pcolor.z;
		
		//Finally merging the channels to make a 32bit color with max alpha				
		Uint32 Color = 0x00000000;		
		Color = (Color | red) << 8;
		Color = (Color | blue) << 8;		
		Color = (Color | green) << 8;		
		Color = Color | 0x000000FF;

		//applying the color to pixel
		PutPixel(s, x, y, Color);
		
	}
	
	//Texture Lookup function
	Uint32 texturelookup(SDL_Surface *s,float u,float v,float z) {				
		int x = (int)round(u *(s->w - 1)) % s->w;
		int y = (int)round(v* (s->h -1)) % s->h;

		//flipping y value
		y = s->h - y - 1;

		//x = x%s->w;
		//y = y%s->h;	
		
		return getpixel(s, x, y);
	}

	template <typename T>
	constexpr T clamp(T a, T left, T right) {
		return max(min(a, right), left);
	}

public:
	Window window;
	float* Zbuffer;
	int* PPAA;
	DefaultVertexShader dvs;

	float n=1, f=100,w=1,h=1;
	float fov;	
	vector<Tri<SVertex>> final;
	SDL_Surface * fb;
	int count=0;
	SDL_Surface *defaultSurface;
	vector<function<void()>> postfn;
	vector<float> aa;
	SDL_Surface * aab;
	

private:	
	bool mode;
	bool modeaa;
	Obj* currentObj;
	ThreadPool tpmain;	
	mutex mu;


	void DrawFBO(SVertex v1, SVertex v0, SVertex v2, SDL_Surface *s) {
			float SL = (v2.Pos.x - v0.Pos.x) / (v2.Pos.y - v0.Pos.y);
			float SR = (v2.Pos.x - v1.Pos.x) / (v2.Pos.y - v1.Pos.y);


			int Ystart = ceil(v2.Pos.y - 0.5f);
			int Yend = ceil(v0.Pos.y - 0.5f);

			for (int i = Ystart;i < Yend;i++) {
				int xstart = (int)ceil(((SL * (i + 0.5 - v0.Pos.y)) + v0.Pos.x) + 0.5);
				int xend = (int)ceil(((SR * (i + 0.5 - v1.Pos.y)) + v1.Pos.x) + 0.5);
				xstart = (int)min(max(xstart, 0), window.w);
				xend = (int)max(min(xend, window.w), 0);
				
				for (int c = xstart;c<xend;c++)
				{
					PutPixel(s, c, i, 0x00FFFFFF);
				}
			}			
		}

	void DrawFTO(SVertex v1, SVertex v0, SVertex v2, SDL_Surface *s) {
			float SL = (v2.Pos.x - v0.Pos.x) / (v2.Pos.y - v0.Pos.y);
			float SR = (v2.Pos.x - v1.Pos.x) / (v2.Pos.y - v1.Pos.y);

			int Ystart = ceil(v0.Pos.y - 0.5f);
			int Yend = ceil(v2.Pos.y - 0.5f);
			Ystart = min(max(Ystart, 0), window.h);
			Yend = max(min(Yend, window.h), 0);


			for (int i = Ystart;i < Yend;i++) {
				int xstart = (int)ceil(((SL * (i + 0.5 - v0.Pos.y)) + v0.Pos.x) + 0.5);
				int xend = (int)ceil(((SR * (i + 0.5 - v1.Pos.y)) + v1.Pos.x) + 0.5);
				xstart = (int)min(max(xstart, 0), window.w);
				xend = (int)max(min(xend, window.w), 0);

				for (int c = xstart;c<xend;c++)
				{
					if (*(Zbuffer + (c*window.w) + i) > 0) {}
					PutPixel(s, c, i, 0xFF00FFFF);
				}
			}
		}


	void DrawTri(SVertex ldv ,SVertex rdv,SVertex l , SVertex r, SVertex b , SDL_Surface *s, int Ystart, int Yend) {
		l = l + (ldv * ((float)Ystart + 0.5 - l.Pos.y));
		r = r + (rdv * ((float)Ystart + 0.5 - r.Pos.y));
				
		
		float ls = (l.Pos.y - b.Pos.y) / (l.Pos.x - b.Pos.x);
		float rs = (r.Pos.y - b.Pos.y) / (r.Pos.x - b.Pos.x);
		float lb =  l.Pos.y- (ls * l.Pos.x);
		float rb =  r.Pos.y- (rs * r.Pos.x);
				

		for (int y = Ystart; y < Yend; y++, l += ldv, r += rdv) {
			int Xstart = (int)ceil(l.Pos.x - 0.5f);
			int Xend = (int)ceil(r.Pos.x - 0.5f);
			Xstart = (int)min(max(Xstart, 0), window.w);
			Xend = (int)max(min(Xend, window.w), 0);

			float dx = r.Pos.x - l.Pos.x;
			auto dv = (r - l) /dx;
			auto lv = l;			
			lv = lv + (dv * (0.5f + (float)Xstart-lv.Pos.x));
			


			for (int x = Xstart;x < Xend;x++) {
				if (ZbufferCheck(lv, x, y)) {
					float zinv = 1.0f / lv.Pos.w;
					auto vert = lv * zinv;
					phongshading(s, x, y, vert);
					//PutPixel(s, x, y, 0x00FF00FF);

					//Pependicular Algorithm
					if (Xend - Xstart > 1) {
						if (x == Xstart && !modeaa) {
							int xt = Xstart-1;
							float dist;
							while (xt >= 0) {
								dist = abs(perpdist(xt, y, ls, -1, lb));
								if (dist < 0.6f) {
									lock_guard<mutex> l(mu);
									aa.push_back(xt);
									aa.push_back(y);
									aa.push_back(dist);
									aa.push_back(1);
									//postfn.push_back(bind(&Renderer::AA, this, xt, y, dist, s, true));
								}
								else {
									break;
								}
								xt -= 1;
							}
						}
						if (x == Xend - 1 && !modeaa) {
							int xt = Xend;
							float dist;
							while (xt <= window.w - 1) {
								dist = abs(perpdist(xt, y, rs, -1, rb));
								if (dist < 0.6f) {
									lock_guard<mutex> l(mu);
									aa.push_back(xt);
									aa.push_back(y);
									aa.push_back(dist);
									aa.push_back(0);
									//postfn.push_back(bind(&Renderer::AA, this, xt, y, dist, s, false));
								}
								else {
									break;
								}
								xt += 1;
							}
						}
					}
				}
					lv.Pos = lv.Pos + dv.Pos;
					lv.Norm = lv.Norm + dv.Norm;
					lv.Texc = lv.Texc + dv.Texc;
					lv.Wpos = lv.Wpos + dv.Wpos;
				
			}
		}
	}

	float perpdist(float x,float y,float a,float b,float c) {
		x = x + 0.5f;
		y = y + 0.5f;
		float t = (a*x) + (b*y) + (c);
		t = abs(t);
		t = t / hypotf(a,b);
		return t;
	}

	void AA(int x, int y,float g, SDL_Surface *s,bool d) {	
		
		g = 0.5f - g;		
		if (*(PPAA + (x*window.w) + y) ==0) {
			Uint32 cr;
			if (d) {
				cr = BlurRR(s, x, y);
				PutPixel(s, x, y, Blur(s, x, y));
				return;
			}
			else
			{
				cr = BlurLL(s, x, y);
				PutPixel(s, x, y, Blur(s, x, y));
				return;
			}
				Uint8 redr = (cr & 0xFF000000) >> 24;
				Uint8 bluer = (cr & 0x00ff0000) >> 16;
				Uint8 greenr = (cr & 0x0000ff00) >> 8;
				
				redr = (float)redr * g;
				bluer = (float)bluer * g;
				greenr = (float)greenr * g;
				

				auto cc = getpixel(s, x, y);
				Uint8 redc = (cr & 0xFF000000) >> 24;
				Uint8 bluec = (cr & 0x00ff0000) >> 16;
				Uint8 greenc = (cr & 0x0000ff00) >> 8;

				redc = (float)redc * (1.0f-g);
				bluec = (float)bluec * (1.0f - g);
				greenc = (float)greenc * (1.0f - g);

				float red = (float)redr + (float)redc;
				float blue = (float)bluer + (float)bluec;
				float green = (float)greenr + (float)greenc;

				Uint32 Color = 0x00000000;
				Color = (Color | (Uint8)red) << 8;
				Color = (Color | (Uint8)blue) << 8;
				Color = (Color | (Uint8)green) << 8;
				Color = Color | 0x000000FF;

				PutPixel(s, x, y, Color);	

				*(PPAA + (x*window.w) + y) == 1;
		}
	}

	void AA(SDL_Surface *s,int x,int y,float g,Uint32 c) {
		if (g > 0.5f) { return; }
		g = 0.5f - g;

		if (*(PPAA + (x*window.w) + y) == 0) {
			Uint32 cr = c;
			
			Uint8 redr = (cr & 0xFF000000) >> 24;
			Uint8 bluer = (cr & 0x00ff0000) >> 16;
			Uint8 greenr = (cr & 0x0000ff00) >> 8;

			float rr = ((float)redr/255) * g;
			float br = (float)((float)bluer / 255) * g;
			float gr = (float)((float)greenr / 255) * g;


			auto cc = getpixel(s, x, y);
			Uint8 redc = (cr & 0xFF000000) >> 24;
			Uint8 bluec = (cr & 0x00ff0000) >> 16;
			Uint8 greenc = (cr & 0x0000ff00) >> 8;

			float g2 = 1.0f - g;
			g2 *= g2;
			float rc = ((float)redc / 255) * g2;
			float bc = ((float)bluec / 255) * g2;
			float gc = ((float)greenc / 255) * g2;

			float red = (float)rr + (float)rc;
			float blue = (float)br + (float)bc;
			float green = (float)gr + (float)gc;
			red /= 1-(g*(1.0f-g));
			blue /= 1 - (g*(1.0f - g));
			green /= 1 - (g*(1.0f - g));
			red *= 255;
			blue *= 255;
			green *= 255;

			Uint32 Color = 0x00000000;
			Color = (Color | (Uint8)red) << 8;
			Color = (Color | (Uint8)blue) << 8;
			Color = (Color | (Uint8)green) << 8;
			Color = Color | 0x000000FF;

			PutPixel(s, x, y, Color);

			*(PPAA + (x*window.w) + y) == 1;
		}
	}

	Uint32 BlurL(SDL_Surface *s, int xx, int yy) {
		Uint32 c = getpixel(s, xx-1, yy);
		Uint32 c1 = getpixel(s, xx - 1, yy-1);
		Uint32 c2 = getpixel(s, xx - 1, yy+1);
		Uint32 c4 = getpixel(s, xx , yy);

		float y = (float)((c & 0xFF000000) >> 24) + (float)((c1 & 0xFF000000) >> 24) + (float)((c2 & 0xFF000000) >> 24) + (float)((c4 & 0xFF000000) >> 24);
		y /= 4;
		y = roundf(y);
		float x = (float)((c & 0x00ff0000) >> 16) + (float)((c1 & 0x00ff0000) >> 16) + (float)((c2 & 0x00ff0000) >> 16) + (float)((c4 & 0x00ff0000) >> 16);
		x /= 4;
		x = roundf(x);
		float z = (float)((c & 0x0000ff00) >> 8) + (float)((c1 & 0x0000ff00) >> 8) + (float)((c2 & 0x0000ff00) >> 8) + (float)((c4 & 0x0000ff00) >> 8);
		z /= 4;
		z = roundf(z);

		Uint32 Color = 0x00000000;
		Color = (Color | (Uint8)y) << 8;
		Color = (Color | (Uint8)x) << 8;
		Color = (Color | (Uint8)z) << 8;
		return Color;
	}
	Uint32 BlurR(SDL_Surface *s, int xx, int yy) {
		Uint32 c = getpixel(s, xx+1, yy);
		Uint32 c1 = getpixel(s, xx + 1, yy-1);
		Uint32 c2 = getpixel(s, xx + 1, yy+1);
		Uint32 c4 = getpixel(s, xx, yy);
		float y = (float)((c & 0xFF000000) >> 24) + (float)((c1 & 0xFF000000) >> 24) + (float)((c2 & 0xFF000000) >> 24) + (float)((c4 & 0xFF000000) >> 24);
		y /= 4;
		y = roundf(y);
		float x = (float)((c & 0x00ff0000) >> 16) + (float)((c1 & 0x00ff0000) >> 16) + (float)((c2 & 0x00ff0000) >> 16) + (float)((c4 & 0x00ff0000) >> 16);
		x /= 4;
		x = roundf(x);
		float z = (float)((c & 0x0000ff00) >> 8) + (float)((c1 & 0x0000ff00) >> 8) + (float)((c2 & 0x0000ff00) >> 8) + (float)((c4 & 0x0000ff00) >> 8);
		z /= 4;
		z = roundf(z);

		Uint32 Color = 0x00000000;
		Color = (Color | (Uint8)y) << 8;
		Color = (Color | (Uint8)x) << 8;
		Color = (Color | (Uint8)z) << 8;
		return Color;
	}

	Uint32 BlurLL(SDL_Surface *s, int xx, int yy) {
		Uint32 c = getpixel(s, xx - 1, yy);
		Uint32 c1 = getpixel(s, xx - 1, yy - 1);
		Uint32 c2 = getpixel(s, xx - 1, yy + 1);
		Uint32 c4 = getpixel(s, xx, yy);

		float y = (float)((c & 0xFF000000) >> 24) + (float)((c1 & 0xFF000000) >> 24) + (float)((c2 & 0xFF000000) >> 24);
		y /= 3;
		y = roundf(y);
		float x = (float)((c & 0x00ff0000) >> 16) + (float)((c1 & 0x00ff0000) >> 16) + (float)((c2 & 0x00ff0000) >> 16);
		x /= 3;
		x = roundf(x);
		float z = (float)((c & 0x0000ff00) >> 8) + (float)((c1 & 0x0000ff00) >> 8) + (float)((c2 & 0x0000ff00) >> 8);
		z /= 3;
		z = roundf(z);

		Uint32 Color = 0x00000000;
		Color = (Color | (Uint8)y) << 8;
		Color = (Color | (Uint8)x) << 8;
		Color = (Color | (Uint8)z) << 8;
		return Color;
	}
	Uint32 BlurRR(SDL_Surface *s, int xx, int yy) {
		Uint32 c = getpixel(s, xx + 1, yy);
		Uint32 c1 = getpixel(s, xx + 1, yy - 1);
		Uint32 c2 = getpixel(s, xx + 1, yy + 1);
		Uint32 c4 = getpixel(s, xx, yy);
		float y = (float)((c & 0xFF000000) >> 24) + (float)((c1 & 0xFF000000) >> 24) + (float)((c2 & 0xFF000000) >> 24);
		y /= 3;
		y = roundf(y);
		float x = (float)((c & 0x00ff0000) >> 16) + (float)((c1 & 0x00ff0000) >> 16) + (float)((c2 & 0x00ff0000) >> 16);
		x /= 3;
		x = roundf(x);
		float z = (float)((c & 0x0000ff00) >> 8) + (float)((c1 & 0x0000ff00) >> 8) + (float)((c2 & 0x0000ff00) >> 8);
		z /= 3;
		z = roundf(z);

		Uint32 Color = 0x00000000;
		Color = (Color | (Uint8)y) << 8;
		Color = (Color | (Uint8)x) << 8;
		Color = (Color | (Uint8)z) << 8;
		return Color;
	}


	Uint32 Blur(SDL_Surface *s,int xx,int yy) {
		Uint32 c = getpixel(s, xx, yy);
		Uint32 c1 = getpixel(s, xx+1, yy);
		Uint32 c2 = getpixel(s, xx-1, yy);
		Uint32 c3 = getpixel(s, xx, yy-1);
		Uint32 c4 = getpixel(s, xx, yy+1);
		Uint32 c5 = getpixel(s, xx + 1, yy+1);
		Uint32 c6 = getpixel(s, xx - 1, yy+1);
		Uint32 c7 = getpixel(s, xx + 1, yy-1);
		Uint32 c8 = getpixel(s, xx - 1, yy-1);
		
		float y = (float)((c & 0xFF000000) >> 24) + (float)((c1 & 0xFF000000) >> 24) + (float)((c2 & 0xFF000000) >> 24) + (float)((c3 & 0xFF000000) >> 24) + (float)((c4 & 0xFF000000) >> 24) + (float)((c5 & 0xFF000000) >> 24) + (float)((c6 & 0xFF000000) >> 24) + (float)((c7 & 0xFF000000) >> 24) + (float)((c8 & 0xFF000000) >> 24);
		y /= 9;
		y = roundf(y);
		float x = (float)((c & 0x00ff0000) >> 16) + (float)((c1 & 0x00ff0000) >> 16) + (float)((c2 & 0x00ff0000) >> 16) + (float)((c3 & 0x00ff0000) >> 16) + (float)((c4 & 0x00ff0000) >> 16) + (float)((c5 & 0x00ff0000) >> 16) + (float)((c6 & 0x00ff0000) >> 16) + (float)((c7 & 0x00ff0000) >> 16) + (float)((c8 & 0x00ff0000) >> 16);
		x /= 9;
		x = roundf(x);
		float z = (float)((c & 0x0000ff00) >> 8) + (float)((c1 & 0x0000ff00) >> 8) + (float)((c2 & 0x0000ff00) >> 8) + (float)((c3 & 0x0000ff00) >> 8) + (float)((c4 & 0x0000ff00) >> 8) + (float)((c5 & 0x0000ff00) >> 8) + (float)((c6 & 0x0000ff00) >> 8) + (float)((c7 & 0x0000ff00) >> 8) + (float)((c8 & 0x0000ff00) >> 8);
		z /= 9;
		z = roundf(z);

		Uint32 Color = 0x00000000;
		Color = (Color | (Uint8)y) << 8;
		Color = (Color | (Uint8)x) << 8;
		Color = (Color | (Uint8)z) << 8;
		return Color;
	}
	
	bool ZbufferCheck(SVertex &v,int x,int y) {			
		if (v.Pos.z <= *(Zbuffer + (x*window.w) + y)) {				
			*(Zbuffer + (x*window.w) + y) = v.Pos.z;		
			return true;
		}
		else
		{
			return false;
		}		
	}
};

