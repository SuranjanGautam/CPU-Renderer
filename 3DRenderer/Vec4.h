#pragma once
#include<math.h>
#include "Vec3.h"

template<typename T = float>
class Vec4 : public Vec3<T>
{
public:
	T w;
	
	static Vec4 O() {return Vec4(0,0,0,1)}

	Vec4():Vec3(){}

	Vec4(T a, T b, T c, T d) :Vec3(a,b,c), w(d) {}

	~Vec4() {}

	T dist() {
		T temp;
		temp = sqrt(*this * *this);
		return temp;
	}
	static Vec4 Normalize(Vec4 a) {
		Vec4 temp;
		temp = a;
		T d = temp.w;
		temp.x /= d;
		temp.y /= d;
		temp.z /= d;
		temp.w /= d;
		return temp;
	}

	Vec4 operator * (T a) {	
		Vec4 temp;
		temp.x = x *a;
		temp.y = y *a;
		temp.z = z *a;
		temp.w = w *a;
		return temp;
	}

	Vec4 operator + (Vec4 a) 
	{		
		Vec4 temp;
		temp.x = x + a.x;
		temp.y = y + a.y;
		temp.z = z + a.z;
		temp.w = w + a.w;
		return temp;
	}

	Vec4 operator - (Vec4 a)
	{
		Vec4 temp;
		temp.x = x - a.x;
		temp.y = y - a.y;
		temp.z = z - a.z;
		temp.w = w - a.w;
		return temp;
	}

	T operator * (Vec4 a) {
		T temp;
		temp = x * a.x + y * a.y + z * a.z + w * a.w;
		return temp;
	}

	Vec4 operator += (Vec4 a) {
		a = a + *this;
		x = a.x;
		y = a.y;
		z = a.z;
		w = a.w;
		return *this;
	}

	Vec4 operator =(Vec3<T> a) {		
		x = a.x;
		y = a.y;
		z = a.z;
		w = 1;
		return *this;
	}

	Vec4 operator -() {		
		Vec4 temp;
		temp.x = -x;
		temp.y = -y;
		temp.z = -z;
		temp.w = -w;
		return temp;
	}

	static Vec4 Interp(Vec4 a, Vec4 b, float d) {
		Vec4 temp;
		temp = a + (-a + b)*d;
		
		return temp;
	}
};

