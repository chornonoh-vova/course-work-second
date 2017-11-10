#pragma once

#include<d3d11.h>
#include<DirectXMath.h>

using namespace DirectX;

class ArcCamera
{
public:
	ArcCamera();

	void SetDistance(float distance, float minDistance, float maxDistance);
	void SetRotation(float x, float y, float minY, float maxY);
	void SetTarget(XMFLOAT3& target);

	float GetDistance() { return this->distance_; };

	XMMATRIX GetViewMatrix();

private:
	XMFLOAT3 position_;
	XMFLOAT3 target_;

	float distance_, minDistance_, maxDistance_;
	float xRotation_, yRotation_, yMin_, yMax_;
};