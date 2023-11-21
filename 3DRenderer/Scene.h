#pragma once
#include "Obj.h"
#include "Light.h"

class Scene
{
public:
	Scene() {
	
	}
	~Scene() {}
public:
	vector<Obj> objlist;
	vector<Light> lightlist;
	


};

