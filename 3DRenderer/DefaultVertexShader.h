#pragma once
#include"Obj.h"
#include"Mat4.h"
#include <ppl.h>
class DefaultVertexShader
{
	
public:
	DefaultVertexShader() {};
	~DefaultVertexShader() {};

	

	class Vertex {
	public:
		Vertex() {					
		}
		~Vertex() {}

		static Vertex Interp(Vertex x,Vertex y,float d) {
			Vertex t;			
			t.Pos = Vec4<>::Interp(x.Pos, y.Pos, d);
			t.Texc = Vec2<>::Interp(x.Texc,y.Texc, d);
			t.Norm = Vec3<>::Interp(x.Norm, y.Norm, d);
			t.Wpos = Vec3<>::Interp(x.Wpos, y.Wpos, d);
			return t;
		}	

		static Vertex PrespectiveDivide(Vertex x) {
			Vertex t;
			float tt = (1.0f / x.Pos.w);
			t.Texc = x.Texc * tt;
			t.Norm = x.Norm * tt;
			t.Wpos = x.Wpos * tt;
			t.Pos = Vec4<>::Normalize(x.Pos);			
			return t;
		}

		Vertex operator * (float x) {
			Vertex temp;
			temp.Pos = this->Pos * x;
			temp.Texc = this->Texc * x;
			temp.Norm = this->Norm * x;
			temp.Wpos = this->Wpos * x;
			return temp;
		}

		Vertex operator / (float x) {
			Vertex temp;
			float tempf = 1.0f / x;
			temp.Pos = Pos * tempf;
			temp.Texc = Texc * tempf;
			temp.Norm = Norm * tempf;
			temp.Wpos = Wpos * tempf;
			return temp;
		}

		Vertex operator - (Vertex x) {
			Vertex temp=*this;
			temp.Pos = Pos - x.Pos;
			temp.Texc = Texc - x.Texc;
			temp.Norm = Norm - x.Norm;
			temp.Wpos = Wpos - x.Wpos;
			return temp;
			
		}

		Vertex operator + (Vertex x) {
			Vertex temp;
			temp.Pos = Pos + x.Pos;
			temp.Texc = Texc + x.Texc;
			temp.Norm = Norm + x.Norm;
			temp.Wpos = Wpos + x.Wpos;
			return temp;
		}

		Vertex& operator += (Vertex x) {			
			this->Pos = Pos + x.Pos;
			this->Texc = Texc + x.Texc;
			this->Norm = Norm + x.Norm;
			this->Wpos = Wpos + x.Wpos;
			return *this;
		}
		

		Vec4<> Pos;
		Vec2<> Texc;
		Vec3<> Norm;
		Vec3<> Wpos;
		
	};

	vector<Vertex> apply(vector<Obj::Vertex> x, Obj *j) {
		j->transC();
		vector<Vertex> tt;
		tt.reserve(x.size());
		concurrency::parallel_transform(x.begin(), x.end(), begin(tt), [&](Obj::Vertex &h)-> Vertex {
			Vertex t;
			Vec4<> t1(0, 0, 0, 1);
			t.Texc = h.TexC;			
			t1 = h.Pos;
			t.Pos = j->transform * t1;
			t.Wpos = t.Pos;
			t1 = h.Norm;
			t1.w = 0;
			t.Norm = j->transform * t1;
			
			return t;
		});
		return tt;
	}

	vector<Vertex> applyTP(vector<Obj::Vertex> x, Obj *j) {
		j->transC();
		vector<Vertex> tt;
		tt.reserve(x.size());
		concurrency::parallel_transform(x.begin(), x.end(), begin(tt), [&](Obj::Vertex &h)-> Vertex {
			Vertex t;
			Vec4<> t1(0, 0, 0, 1);
			t1 = h.Pos;
			t.Pos = j->transform * t1;
			t.Wpos = t.Pos;
			t1 = h.Norm;
			t1.w = 0;
			t.Norm = j->transform * t1;
			return t;
		});
		return tt;
	}

	vector<Vertex> applyO(vector<Obj::Vertex> x, Obj *j) {
		j->transC();
		vector<Vertex> tt;
		for (auto h : x) {
			Vertex t;
			Vec4<> t1(0, 0, 0, 1);
			t1 = h.Pos;
			t.Pos = j->transform * t1;
			t.Wpos = t.Pos;
			t1 = h.Norm;
			t1.w = 0;
			t.Norm = j->transform * t1;
			tt.push_back(t);
		}		
		return tt;
	}

};

