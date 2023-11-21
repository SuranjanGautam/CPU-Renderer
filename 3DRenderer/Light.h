#pragma once
#include "Vec4.h"

class Light
{
public:
	Light() {
		Position = Vec3<>(0,0,0);
		intensity = 10;
	}
	Light(Vec3<> pos, int itn) {
		Position = pos;
		intensity = itn;
	}
	~Light() {}


public:
	Vec3<> Position;
	float intensity;
};

