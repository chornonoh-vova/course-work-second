#pragma once

#include<fstream>
#include<vector>
#include<string>
#include"TokenStream.h"

class ObjModel
{
public:
	ObjModel();
	~ObjModel();

	void Release();
	bool LoadOBJ(char *fileName);

	float *GetVertices() { return vertices_; }
	float *GetNormals() { return normals_; }
	float *GetTexCoords() { return texCoords_; }
	int    GetTotalVerts() { return totalVerts_; }

private:
	float *vertices_;
	float *normals_;
	float *texCoords_;
	int totalVerts_;
};
