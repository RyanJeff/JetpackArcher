#include "FPSCamera.h"


FPSCamera::~FPSCamera(void)
{
}


void FPSCamera::Pitch(float angle)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR up = XMLoadFloat3(&mUp);

	XMMATRIX rotate = XMMatrixRotationAxis(right, angle);

	look = XMVector3TransformNormal(look, rotate);
	up = XMVector3Cross(look, right);
	up = XMVector3Normalize(up);

	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mUp, up);

	mViewUpdated = true;
}

void FPSCamera::YawLocal(float angle)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR up = XMLoadFloat3(&mUp);

	XMMATRIX rotate = XMMatrixRotationAxis(up, angle);

	right = XMVector3TransformNormal(right, rotate);
	look = XMVector3Cross(up, right);
	look = XMVector3Normalize(look);

	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mRight, right);

	mViewUpdated = true;
}

void FPSCamera::YawGlobal(float angle)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR up = XMLoadFloat3(&mUp);

	XMMATRIX rotate = XMMatrixRotationY(angle);

	right = XMVector3TransformNormal(right, rotate);
	look = XMVector3TransformNormal(look, rotate);

	up = XMVector3Cross(look, right);
	up = XMVector3Normalize(up);

	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);

	mViewUpdated = true;
}


void FPSCamera::Roll(float angle)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR up = XMLoadFloat3(&mUp);

	XMMATRIX rotate = XMMatrixRotationAxis(look, angle);

	up = XMVector3TransformNormal(up, rotate);
	right = XMVector3Cross(up, look);
	right = XMVector3Normalize(right);

	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mRight, right);

	mViewUpdated = true;
}

void FPSCamera::MoveLook(float amt)
{
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR pos = XMLoadFloat3(&mPos);
	pos += look * amt;

	XMStoreFloat3(&mPos, pos);
	mViewUpdated = true;
}

void FPSCamera::MoveStrafe(float amt)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR pos = XMLoadFloat3(&mPos);
	pos += right * amt; 

	XMStoreFloat3(&mPos, pos);
	mViewUpdated = true;
}



