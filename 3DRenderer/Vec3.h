#pragma once
#include"Vec2.h"

template<typename T = float>
class Vec3 : public Vec2<T>
{

public:
	T z;
	static Vec3 O() {
		return Vec3(0, 0, 0);
	}

	Vec3() :Vec2() {}
	Vec3(T a, T b, T c) :Vec2(a, b), z(c) {}

	T dist() {
		return sqrt(*this * *this);
	}

	T operator * (Vec3 a) {
		T temp;
		temp = x * a.x + y * a.y + z * a.z;
		return temp;
	}

	Vec3 operator * (T a) {	
		Vec3 temp;
		temp.x = x*a;
		temp.y = y*a;
		temp.z = z*a;
		return temp;
	}

	Vec3 operator + (Vec3 a) {	
		Vec3 temp;
		temp.x = x + a.x;
		temp.y = y + a.y;
		temp.z = z + a.z;
		return temp;
	}

	Vec3 operator -() {	
		Vec3 temp;
		temp.x = -x;
		temp.y = -y;
		temp.z = -z;
		return temp;
	}

	Vec3 operator - (Vec3 a) {
		Vec3 temp;
		temp.x = x - a.x;
		temp.y = y - a.y;
		temp.z = z - a.z;
		return temp;
	}

	static Vec3 Normalize(Vec3 a) {
		Vec3 temp;
		float d = a.dist();
		temp.x = a.x / d;
		temp.y = a.y / d;
		temp.z = a.z / d;
		return temp;
	}

	Vec3 operator ^(Vec3 a) {
		Vec3 temp;
		temp.x = (y*a.z)-(a.y*z);
		temp.y = (z*a.x)-(x*a.z);
		temp.z = (x*a.y)-(a.x*y);
		return temp;
	}

	static Vec3 Interp(Vec3 a, Vec3 b, float d) {
		Vec3 temp;
		temp = a + ((-a + b)*d);				
		return temp;
	}	

	void DisplayVec() {
		cout << x << " " << y << " " << z << "-- \n";
	}

	~Vec3() {}
};

