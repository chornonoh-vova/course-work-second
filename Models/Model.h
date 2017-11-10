#pragma once

#include"DX11Base.h"
#include"ArcCamera.h"
#include"objLoader.h"
#include<DirectXmath.h>

using namespace DirectX;

class Model : public DX11Base
{
public:
	Model();
	virtual ~Model();

	bool LoadContent();
	void UnloadContent();

	void Update();
	void Render();

	void Rotate(float angleX, float angleY);
	void Translate(float offsetX);

	void CameraRotate(float angle);
	void CameraTranslate(float offset);

	ArcCamera camera_;

private:
	ID3D11VertexShader* textureMapVS_;
	ID3D11PixelShader* textureMapPS_;

	ID3D11InputLayout* inputLayout_;
	ID3D11Buffer* vertexBuffer_;
	int totalVerts_;

	ID3D11ShaderResourceView* colorMap_;
	ID3D11SamplerState* colorMapSampler_;

	ID3D11Buffer* viewCB_;
	ID3D11Buffer* projCB_;
	ID3D11Buffer* worldCB_;
	XMMATRIX projMatrix_;
	XMMATRIX worldMatrix_;

	float totalOffsetX;
	float totalOffsetY;
	bool translate;

	float cameraOffset;
	float cameraAngle;
};