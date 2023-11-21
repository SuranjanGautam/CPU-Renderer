#pragma once
#include "Mat4.h"
#include "Obj.h"

template <class T>
class Tri
{
public:
	Tri() {};
	~Tri() {};

	Tri(T a, T b, T c):v1(a),v2(b),v3(c) {}

	Vec3<> normal() {		
		Vec3<> a = -v1.Pos;
		Vec3<> b = -v1.Pos;		
		a = a + v2.Pos;
		b = b + v3.Pos;
		a = a^b;
		a = Vec3<>::Normalize(a);
		return a;
	}
	

public:
	T v1, v2, v3;
};

