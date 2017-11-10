/*
ObjModel -используется для представления модели OBJ.
*/

#include"objLoader.h"

ObjModel::ObjModel()
{
	vertices_ = 0;
	normals_ = 0;
	texCoords_ = 0;
	totalVerts_ = 0;
}


ObjModel::~ObjModel()
{
	Release();
}


void ObjModel::Release()
{
	totalVerts_ = 0;

	if (vertices_ != 0) delete[] vertices_;
	if (normals_ != 0) delete[] normals_;
	if (texCoords_ != 0) delete[] texCoords_;

	vertices_ = 0;
	normals_ = 0;
	texCoords_ = 0;
}


bool ObjModel::LoadOBJ(char *fileName)
{
	std::ifstream fileStream;
	int fileSize = 0;

	fileStream.open(fileName, std::ifstream::in);

	if (fileStream.is_open() == false)
		return false;

	fileStream.seekg(0, std::ios::end);
	fileSize = (int)fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);

	if (fileSize <= 0)
		return false;

	char *buffer = new char[fileSize];

	if (buffer == 0)
		return false;

	memset(buffer, '\0', fileSize);

	TokenStream tokenStream, lineStream, faceStream;
	std::string tempLine, token;

	fileStream.read(buffer, fileSize);
	tokenStream.SetTokenStream(buffer);

	delete[] buffer;

	tokenStream.ResetStream();

	std::vector<float> verts, norms, texC;
	std::vector<int> faces;

	char lineDelimiters[2] = { '\n', ' ' };

	while (tokenStream.MoveToNextLine(&tempLine))
	{
		lineStream.SetTokenStream((char*)tempLine.c_str());
		tokenStream.GetNextToken(0, 0, 0);

		if (!lineStream.GetNextToken(&token, lineDelimiters, 2))
			continue;

		if (strcmp(token.c_str(), "v") == 0)
		{
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			verts.push_back((float)atof(token.c_str()));
		}
		else if (strcmp(token.c_str(), "vn") == 0)
		{
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			norms.push_back((float)atof(token.c_str()));
		}
		else if (strcmp(token.c_str(), "vt") == 0)
		{
			lineStream.GetNextToken(&token, lineDelimiters, 2);
			texC.push_back((float)atof(token.c_str()));

			lineStream.GetNextToken(&token, lineDelimiters, 2);
			texC.push_back((float)atof(token.c_str()));
		}
		else if (strcmp(token.c_str(), "f") == 0)
		{
			char faceTokens[3] = { '\n', ' ', '/' };
			std::string faceIndex;

			faceStream.SetTokenStream((char*)tempLine.c_str());
			faceStream.GetNextToken(0, 0, 0);

			for (int i = 0; i < 3; i++)
			{
				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));

				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));

				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
				faces.push_back((int)atoi(faceIndex.c_str()));
			}
		}
		else if (strcmp(token.c_str(), "#") == 0)
		{
			int a = 0;
			int b = a;
		}

		token[0] = '\0';
	}

	// "Unroll" the loaded obj information into a list of triangles.

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	totalVerts_ = numFaces * 3;

	vertices_ = new float[totalVerts_ * 3];

	if ((int)norms.size() != 0)
	{
		normals_ = new float[totalVerts_ * 3];
	}

	if ((int)texC.size() != 0)
	{
		texCoords_ = new float[totalVerts_ * 2];
	}

	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		vertices_[vIndex + 0] = verts[(faces[f + 0] - 1) * 3 + 0];
		vertices_[vIndex + 1] = verts[(faces[f + 0] - 1) * 3 + 1];
		vertices_[vIndex + 2] = verts[(faces[f + 0] - 1) * 3 + 2];
		vIndex += 3;

		if (texCoords_)
		{
			texCoords_[tIndex + 0] = texC[(faces[f + 1] - 1) * 2 + 0];
			texCoords_[tIndex + 1] = texC[(faces[f + 1] - 1) * 2 + 1];
			tIndex += 2;
		}

		if (normals_)
		{
			normals_[nIndex + 0] = norms[(faces[f + 2] - 1) * 3 + 0];
			normals_[nIndex + 1] = norms[(faces[f + 2] - 1) * 3 + 1];
			normals_[nIndex + 2] = norms[(faces[f + 2] - 1) * 3 + 2];
			nIndex += 3;
		}
	}

	verts.clear();
	norms.clear();
	texC.clear();
	faces.clear();

	return true;
}