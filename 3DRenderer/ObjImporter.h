#pragma once
#include "Obj.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

using namespace std;
class ObjImporter
{
public:
	ObjImporter() {};
	~ObjImporter() {};
	static Obj Import(string path)
	{
		vector<Obj::Vertex> vertices;	
		vector<Vec3<>> pos;
		vector<Vec3<>> norm;
		vector<Vec2<>> tex;
		vector<int> indices;
		Obj temp;
		string extension = path.substr(path.length() - 4, 4);
		if (extension != ".obj") {
			
			return temp;
		}
		else {	
			
			fstream in;
			in.open(path.c_str());
			
			string line;			
			vector<string> tokens;				
			while (in) {
				getline(in, line);				
				tokens = split(line, ' ');
				if (line.length() == 0) { break; }
				if (tokens[0] == "#") {
					continue;
				}
				else if (tokens[0] == "v") {
										
					float x, y, z;
					x = stod(tokens[1]);
					y = stod(tokens[2]);
					z = stod(tokens[3]);
					pos.push_back(Vec3<>(x, y, z));					
				}
				else if (tokens[0] == "vt") {
					float x, y;
					x = stod(tokens[1]);
					y = stod(tokens[2]);					
					tex.push_back(Vec2<>(x, y));
				}
				else if (tokens[0] == "vn") {
					float x, y, z;
					x = stod(tokens[1]);
					y = stod(tokens[2]);
					z = stod(tokens[3]);
					norm.push_back(Vec3<>(x, y, z));
				}
				else if (tokens[0] == "f") {
					vector<string> t;					
					for (string x : tokens) {
						
						if (x == "f") { continue; }
						Obj::Vertex vtemp;
						t = split(x, '/'); 						
						vtemp.Pos = pos[stoi(t[0])-1];										
						vtemp.TexC = tex[stoi(t[1])-1];
						vtemp.Norm = norm[stoi(t[2])-1];
						vertices.push_back(vtemp);
						indices.push_back(vertices.size()-1);						
						t.clear();
					}	
				}
				else {
					continue;
				}
				
			}
		}		
		temp = Obj(vertices,indices);
		return temp;
	}


private:
	static vector<string> split(string s, char c) {
		stringstream ss(s);
		vector<string> temp;
		string st;
		while (getline(ss, st, c)) {
			temp.push_back(st);
		}
		return temp;
	}
};




