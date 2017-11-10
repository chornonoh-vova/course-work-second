/*
ArcCamera - камера проекта
*/

#include"ArcCamera.h"


ArcCamera::ArcCamera() : target_(XMFLOAT3(0.0f, 0.0f, 0.0f)), position_(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	SetDistance(2.0f, 1.0f, 10.0f);
	SetRotation(0.0f, 0.0f, -XM_PIDIV2, XM_PIDIV2);
}


void ArcCamera::SetDistance(float distance, float minDistance, float maxDistance)
{
	distance_ = distance;
	minDistance_ = minDistance;
	maxDistance_ = maxDistance;

	if (distance_ < minDistance_) distance_ = minDistance_;
	if (distance_ > maxDistance_) distance_ = maxDistance_;
}


void ArcCamera::SetRotation(float x, float y, float minY, float maxY)
{
	xRotation_ = x;
	yRotation_ = y;
	yMin_ = minY;
	yMax_ = maxY;

	if (yRotation_ < yMin_) yRotation_ = yMin_;
	if (yRotation_ > yMax_) yRotation_ = yMax_;
}


void ArcCamera::SetTarget(XMFLOAT3& target)
{
	target_ = target;
}

XMMATRIX ArcCamera::GetViewMatrix()
{
	XMVECTOR zoom = XMVectorSet(0.0f, 0.0f, distance_, 1.0f);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(xRotation_, -yRotation_, 0.0f);

	zoom = XMVector3Transform(zoom, rotation);

	XMVECTOR pos = XMLoadFloat3(&position_);
	XMVECTOR lookAt = XMLoadFloat3(&target_);

	pos = lookAt + zoom;
	XMStoreFloat3(&position_, pos);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	up = XMVector3Transform(up, rotation);

	XMMATRIX viewMat = XMMatrixLookAtLH(pos, lookAt, up);

	return viewMat;
}