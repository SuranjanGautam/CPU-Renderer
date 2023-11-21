#pragma once
#include<math.h>
#include "Vec4.h"

template<typename T = float>
class Mat4
{
	
public:
	T V[4][4];


	Mat4() {
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				V[i][j] = 0;
			}
		}
	};

	Mat4(int a) {
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				if (i == j)
				{
					V[i][j] = 1;
				}
				else {
					V[i][j] = 0;
				}
			}
		}
	}

	Mat4 operator * (Mat4 a) {
		Mat4<T> t;
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				T l = 0;
				for (int k = 0;k < 4;k++) {
					l += V[i][k] * a.V[k][j];
				}
				t.V[i][j] = l;
			}
		}
		return t;
	}

	Mat4 operator + (Mat4 a) {
		Mat4<T> t;
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				t.V[i][j] = t.V[i][j]+ a.V[i][j];
			}
		}
		return t;
	}
	
	Vec4<T> operator * (Vec4<T> a) {
		Vec4<T> temp;
		temp.x = a.x * V[0][0] + a.y * V[1][0] + a.z * V[2][0] + a.w * V[3][0];
		temp.y = a.x * V[0][1] + a.y * V[1][1] + a.z * V[2][1] + a.w * V[3][1];
		temp.z = a.x * V[0][2] + a.y * V[1][2] + a.z * V[2][2] + a.w * V[3][2];
		temp.w = a.x * V[0][3] + a.y * V[1][3] + a.z * V[2][3] + a.w * V[3][3];
		return temp;
	}

	Mat4 operator -() {
		Mat4 t = *this;
		for (int i = 0;i < 4;i++) {
			for (int j = 0;j < 4;j++) {
				V[i][j] = t.V[j][i];
			}
		}
		return *this;
	}

	


	static Mat4<T> translateMat(T x, T y, T z) {
		Mat4<T> t(0);
		t.V[3][0] = x;
		t.V[3][1] = y;
		t.V[3][2] = z;
		t.V[3][3] = 1;
		return t;
	}

	static Mat4<T> translateMat(Vec3<> x) {
		return  translateMat(x.x, x.y, x.z);
	}

	static Mat4<T> scaleMat(T x, T y, T z, T u, T v,T w) {
		Mat4<T> t;		
		t.V[0][0] = x;
		t.V[1][1] = y;
		t.V[2][2] = z;
		t.V[3][3] = 1;
		if (u == v == w == 0) {
			return t;
		}
		else {
			t = t * translateMat(u, v, w);
			t = translateMat(-u, -v, -w)*t;
			return t;
		}
	}
	static Mat4<T> scaleMat(Vec3<> a, Vec3<> b) {
		return scaleMat(a.x, a.y, a.z, b.x, b.y, b.z);
	}

	static Mat4<T> rotateMat(T x, T y, T z, T u, T v, T w) {
		Mat4<T> t(0);
		
		Mat4<T> r1(0);
		if (x != 0) {
			r1.V[1][1] = cos(x);
			r1.V[2][2] = cos(x);
			r1.V[1][2] = -sin(x);
			r1.V[2][1] = +sin(x);
			r1.V[0][0] = 1;
			r1.V[3][3] = 1;
			
		}		
		Mat4<T> r2(0);
		if (y != 0) {
			r2.V[0][0] = cos(y);
			r2.V[2][2] = cos(y);
			r2.V[0][2] = sin(y);
			r2.V[2][0] = -sin(y);
			r2.V[1][1] = 1;
			r2.V[3][3] = 1;
		}			

		Mat4<T> r3(0);
		if (z != 0) {
			r3.V[0][0] = cos(z);
			r3.V[1][1] = cos(z);
			r3.V[1][0] = sin(z);
			r3.V[0][1] = -sin(z);
			r3.V[2][2] = 1;
			r3.V[3][3] = 1;
		}		

		r1 = -r1;
		r2 = -r2;
		r3 = -r3;	

		if ((u == v) &&(v == w) && u == 0) {
			t = r2 * r3;			
			t = r1 * t;				
			return t;			
		}
		else {			
			t = r3 * translateMat(u, v, w);
			t = r2 * t;
			t = r1 * t;
			t = translateMat(-u, -v, -w) * t;

			return t;
		}
	}

	static Mat4<T> rotateMat(Vec3<> a, Vec3<> b) {
		return rotateMat(a.x, a.y, a.z, b.x, b.y, b.z);
	}

	static Mat4<> prespectiveTrans(float n,float f,float w,float h) {
		Mat4 t;
		t.V[0][0] = (2* n / w);
		t.V[1][1] = ( 2*n / h);
		t.V[2][2] = f / (f - n);
		t.V[3][2] = -(f*n) / (f - n);
		t.V[2][3] = 1.0;		
		return t;
	}

	void DisplayMat() {
		for (int i = 0;i < 4;i++) {
			cout << "\n";
			for (int j = 0;j < 4;j++) {
				cout << V[i][j] << " ";
			}
		}
	}

	~Mat4() {}
};

