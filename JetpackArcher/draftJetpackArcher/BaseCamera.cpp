#include "BaseCamera.h"


BaseCamera::BaseCamera(void) : 
mPos(0.0f, 0.0f, 0.0f),
mLook(0.0f, 0.0f, 1.0f),
mUp(0.0f, 1.0f, 0.0f),
mRight(1.0f, 0.0f, 0.0f),
mViewUpdated(true)
{
}

BaseCamera::BaseCamera(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up) : 
mViewUpdated(true)
{
	XMVECTOR right = XMVector3Cross(up, look);

	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mPos, pos);
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);
}


BaseCamera::~BaseCamera(void)
{
}


void BaseCamera::SetPos(FXMVECTOR pos)
{
	XMStoreFloat3(&mPos, pos);
	mViewUpdated = true;
}

void BaseCamera::SetFacing(FXMVECTOR look, FXMVECTOR up)
{
	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mUp, up);

	XMVECTOR right = XMVector3Cross(up, look);
	XMStoreFloat3(&mRight, right);
	mViewUpdated = true;
}

FXMVECTOR BaseCamera::GetPos() const
{
	return XMLoadFloat3(&mPos);
}

FXMVECTOR BaseCamera::GetLook() const
{
	return XMLoadFloat3(&mLook);
}

FXMVECTOR BaseCamera::GetUp() const
{
	return XMLoadFloat3(&mUp);
}

XMMATRIX BaseCamera::GetView() const
{
	return XMLoadFloat4x4(&mView);
}

void BaseCamera::Update()
{
	if(mViewUpdated)
	{
		mViewUpdated = false;
		
		XMVECTOR look = XMLoadFloat3(&mLook);
		XMVECTOR pos = XMLoadFloat3(&mPos);
		XMVECTOR up = XMLoadFloat3(&mUp);

		XMMATRIX V = XMMatrixLookAtLH(pos, pos + look, up);
		XMStoreFloat4x4(&mView, V);
	}
}