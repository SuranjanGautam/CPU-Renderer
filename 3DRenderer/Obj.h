#pragma once
#include "Mat4.h"
#include <Vector>
using namespace std;
class Obj
{
	
public:
	class Vertex {
	public: 
		Vec3<> Pos;
		Vec2<> TexC;
		Vec3<> Norm;

		Vertex() { Pos = Vec3<>(); Norm = Vec3<>();TexC = Vec2<>();};	
		Vertex(Vec3<> a) { Vertex(); Pos = a; }
		Vertex(Vec3<> a,Vec2<> b, Vec3<> c) :Pos(a),TexC(b),Norm(c) {}
		
		static Vertex interp(Vertex a, Vertex b, float d) {
			Vertex t;			
			t.Pos = Vec3<>::Interp(a.Pos, b.Pos, d);
			t.TexC = Vec2<>::Interp(a.TexC, b.TexC, d);
			t.Norm = Vec3<>::Interp(a.Norm, b.Norm, d);
			return t;
		}

		~Vertex() {};
	};

	void transC() {		

		
		transform = Mat4<>::rotateMat(Wrot, Pos);

		transform = Mat4<>::translateMat(Pos) * transform ;		
		
		transform = Mat4<>::scaleMat(Scale, Vec3<>::O()) * transform;
		
		transform = Mat4<>::rotateMat(rot, Vec3<>::O()) * transform;
		
	}

	Obj() {}

	Obj(vector<Vertex> a,vector<int> b):Vertices(a),Index(b) {}

	~Obj() {
		SDL_FreeSurface(texture);		
	}

public:
	vector<Vertex> Vertices;
	vector<int> Index;

	Vec3<> Pos = Vec3<>(1, 1, 1);
	Vec3<> rot = Vec3<>(0.0f, 0.0f, 0.0f);
	Vec3<> Scale = Vec3<>(1, 1, 1);
	Vec3<> Wrot = Vec3<>(0.0f, 0.0f, 0.0f);
	Mat4<> transform;
	SDL_Surface *texture=NULL;
	SDL_Surface *normal = NULL;
	
	
};

