/*
Model - основной класс для работы с моделью.
*/

#include"Model.h"

#define SHADER_FILE "TextureMap.fx"
#define OBJECT_FILE "Sphere.obj"
#define TEXTURE_FILE "decall.dds"

struct VertexPos
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};


Model::Model() : textureMapVS_(0),
textureMapPS_(0),
inputLayout_(0),
vertexBuffer_(0),
colorMap_(0),
colorMapSampler_(0),
viewCB_(0),
projCB_(0),
worldCB_(0),
totalVerts_(0),
totalOffsetX(0),
totalOffsetY(0),
translate(false),
cameraAngle(0.0f),
cameraOffset(5.0f)
{

}


Model::~Model()
{

}


bool Model::LoadContent()
{
	ID3DBlob* vsBuffer = 0;

	bool compileResult = CompileD3DShader(SHADER_FILE, "VS_Main", "vs_5_0", &vsBuffer);

	if (!compileResult)
	{
		return false;
	}

	HRESULT d3dResult;

	d3dResult = d3dDevice_->CreateVertexShader(vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), 0, &textureMapVS_);

	if (FAILED(d3dResult))
	{

		if (vsBuffer)
			vsBuffer->Release();

		return false;
	}

	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

	d3dResult = d3dDevice_->CreateInputLayout(solidColorLayout, totalLayoutElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout_);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		return false;
	}

	ID3DBlob* psBuffer = 0;

	compileResult = CompileD3DShader(SHADER_FILE, "PS_Main", "ps_5_0", &psBuffer);

	if (!compileResult)
	{
		return false;
	}

	d3dResult = d3dDevice_->CreatePixelShader(psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(), 0, &textureMapPS_);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		return false;
	}


	// Load the models from the file.
	ObjModel objModel;

	if (!(objModel.LoadOBJ(OBJECT_FILE)))
	{
		return false;
	}

	totalVerts_ = objModel.GetTotalVerts();

	VertexPos* vertices = new VertexPos[totalVerts_];
	float* vertsPtr = objModel.GetVertices();
	float* texCPtr = objModel.GetTexCoords();

	for (int i = 0; i < totalVerts_; i++)
	{
		vertices[i].pos = XMFLOAT3(*(vertsPtr + 0), *(vertsPtr + 1), *(vertsPtr + 2));
		vertsPtr += 3;

		vertices[i].tex0 = XMFLOAT2(*(texCPtr + 0), *(texCPtr + 1));
		texCPtr += 2;
	}

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * totalVerts_;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	d3dResult = d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer_);

	if (FAILED(d3dResult))
	{
		return false;
	}

	delete[] vertices;
	objModel.Release();


	d3dResult = D3DX11CreateShaderResourceViewFromFile(d3dDevice_,
		TEXTURE_FILE, 0, 0, &colorMap_, 0);
	if (FAILED(d3dResult))
	{
		return false;
	}

	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	d3dResult = d3dDevice_->CreateSamplerState(&colorMapDesc, &colorMapSampler_);

	if (FAILED(d3dResult))
	{
		return false;
	}


	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dResult = d3dDevice_->CreateBuffer(&constDesc, 0, &viewCB_);

	if (FAILED(d3dResult))
	{
		return false;
	}

	d3dResult = d3dDevice_->CreateBuffer(&constDesc, 0, &projCB_);

	if (FAILED(d3dResult))
	{
		return false;
	}

	d3dResult = d3dDevice_->CreateBuffer(&constDesc, 0, &worldCB_);

	if (FAILED(d3dResult))
	{
		return false;
	}

	this->worldMatrix_ = XMMatrixIdentity();
	this->projMatrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.01f, 100.0f);
	this->projMatrix_ = XMMatrixTranspose(projMatrix_);

	this->camera_.SetDistance(cameraOffset, 0.0f, 1000.0f);
	this->camera_.SetRotation(cameraAngle, 0.0f, 0.0f, 1.0f);

	return true;
}


void Model::UnloadContent()
{
	if (colorMapSampler_) colorMapSampler_->Release();
	if (colorMap_) colorMap_->Release();
	if (textureMapVS_) textureMapVS_->Release();
	if (textureMapPS_) textureMapPS_->Release();
	if (inputLayout_) inputLayout_->Release();
	if (vertexBuffer_) vertexBuffer_->Release();
	if (viewCB_) viewCB_->Release();
	if (projCB_) projCB_->Release();
	if (worldCB_) worldCB_->Release();

	this->colorMapSampler_ = 0;
	this->colorMap_ = 0;
	this->textureMapVS_ = 0;
	this->textureMapPS_ = 0;
	this->inputLayout_ = 0;
	this->vertexBuffer_ = 0;
	this->viewCB_ = 0;
	this->projCB_ = 0;
	this->worldCB_ = 0;
}


void Model::Update()
{
	if (this->totalOffsetY <= 0.0f)
		this->translate = true;
	if (this->totalOffsetY >= 5.0f)
		this->translate = false;
	//this->worldMatrix_ *= XMMatrixRotationX(-0.001f) * XMMatrixRotationY(-0.001f);
	if (this->translate)
	{
		this->totalOffsetY += 0.01f;
		this->worldMatrix_ *= XMMatrixTranslation(-(this->totalOffsetX), 0.0f, 0.0f) *
			XMMatrixTranslation(0.0f, 0.01f, 0.0f) *
			XMMatrixRotationY(-0.005f) *
			XMMatrixTranslation(this->totalOffsetX, 0.0f, 0.0f);
	}
	if (!this->translate)
	{
		this->totalOffsetY -= 0.01f;
		this->worldMatrix_ *= XMMatrixTranslation(-(this->totalOffsetX), 0.0f, 0.0f) *
			XMMatrixTranslation(0.0f, -0.01f, 0.0f) *
			XMMatrixRotationY(0.005f) *
			XMMatrixTranslation(this->totalOffsetX, 0.0f, 0.0f);
	}
}


void Model::Render()
{
	if (d3dContext_ == 0)
		return;

	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	d3dContext_->ClearRenderTargetView(backBufferTarget_, clearColor);
	d3dContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);

	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;

	d3dContext_->IASetInputLayout(inputLayout_);
	d3dContext_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	d3dContext_->VSSetShader(textureMapVS_, 0, 0);
	d3dContext_->PSSetShader(textureMapPS_, 0, 0);
	d3dContext_->PSSetShaderResources(0, 1, &colorMap_);
	d3dContext_->PSSetSamplers(0, 1, &colorMapSampler_);

	XMMATRIX worldMat = XMMatrixTranspose(worldMatrix_);

	XMMATRIX viewMat = camera_.GetViewMatrix();
	viewMat = XMMatrixTranspose(viewMat);

	d3dContext_->UpdateSubresource(worldCB_, 0, 0, &worldMat, 0, 0);
	d3dContext_->UpdateSubresource(viewCB_, 0, 0, &viewMat, 0, 0);
	d3dContext_->UpdateSubresource(projCB_, 0, 0, &projMatrix_, 0, 0);

	d3dContext_->VSSetConstantBuffers(0, 1, &worldCB_);
	d3dContext_->VSSetConstantBuffers(1, 1, &viewCB_);
	d3dContext_->VSSetConstantBuffers(2, 1, &projCB_);

	d3dContext_->Draw(totalVerts_, 0);

	swapChain_->Present(0, 0);
}

void Model::Rotate(float angleX, float angleY)
{
	this->worldMatrix_ *= XMMatrixTranslation(-(this->totalOffsetX), 0.0f, 0.0f) *
		XMMatrixRotationY(angleX) *
		XMMatrixTranslation(this->totalOffsetX, 0.0f, 0.0f);

	return;
}

void Model::Translate(float offsetX)
{
	this->totalOffsetX += offsetX;
	this->worldMatrix_ *= XMMatrixTranslation(offsetX, 0.0f, 0.0f);

	return;
}

void Model::CameraRotate(float angle)
{
	this->cameraAngle += angle;
	this->camera_.SetRotation(this->cameraAngle, 0.0f, 0.0f, 0.0f);
}

void Model::CameraTranslate(float offset)
{
	this->cameraOffset += offset;
	this->camera_.SetDistance(cameraOffset, 0.0f, 1000.0f);
}
