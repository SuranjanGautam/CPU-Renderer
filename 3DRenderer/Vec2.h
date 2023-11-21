#pragma once
#include<math.h>
#include<Algorithm>

template<typename T = float>
class Vec2
{
public:
	T x, y;

	static Vec2 o() { return Vec2(0, 0, 0); }

	Vec2(){};
	Vec2(T a, T b) :x(a), y(b) {};

	T dist() {
		return sqrt(*this * *this);
	}

	T operator * (Vec2 a) {
		T temp;
		temp = x * a.x + y * a.y;
		return temp;
	}

	Vec2 operator * (T a) {		
		Vec2 temp;
		temp.x = x*a;
		temp.y = y*a;
		return temp;
	}

	Vec2 operator + (Vec2 a) {	
		Vec2 temp;
		temp.x = x + a.x;
		temp.y = y + a.y;
		return temp;
	}

	Vec2 operator -(){	
		Vec2 temp;
		temp.x = -x;
		temp.y = -y;
		return temp;
	}

	Vec2 operator - (Vec2 a) {
		Vec2 temp;
		temp.x = x - a.x;
		temp.y = y - a.y;
		return temp;
	}

	static Vec2 Normalize(Vec2 a) {
		Vec2 temp;
		float d = a.dist();
		temp.x = a.x / d;
		temp.y = a.y / d;
		return temp;
	}

	static Vec2 Interp(Vec2 a, Vec2 b, float d) {
		Vec2 temp;
		temp = a + ((-a + b)*d);
		return temp;
	}

	~Vec2() {}
};

