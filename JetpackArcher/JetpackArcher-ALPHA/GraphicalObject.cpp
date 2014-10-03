#include "GraphicalObject.h"

GraphicalObject::GraphicalObject(void) : 
	mPos(0.0f, 0.0f, 0.0f),
	mLook(0.0f, 0.0f, 1.0f),
	mUp(0.0f, 1.0f, 0.0f),
	mRight(1.0f, 0.0f, 0.0f),
	mWorldUpdated(true)
	{
	}

GraphicalObject::GraphicalObject(FXMVECTOR pos, FXMVECTOR look, FXMVECTOR up, BasicModel& model) :
	BasicModel(model), mWorldUpdated(true)
{
	XMVECTOR right = XMVector3Cross(up, look);

	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mPos, pos);
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);
}

GraphicalObject::~GraphicalObject(void)
{
	mModelMesh = NULL;
}

void GraphicalObject::SetPos(FXMVECTOR pos)
{
	XMStoreFloat3(&mPos, pos);
	mWorldUpdated = true;
}

void GraphicalObject::SetFacing(FXMVECTOR look, FXMVECTOR up)
{
	XMStoreFloat3(&mLook, look);
	XMStoreFloat3(&mUp, up);

	XMVECTOR right = XMVector3Cross(up, look);
	XMStoreFloat3(&mRight, right);
	mWorldUpdated = true;
}

FXMVECTOR GraphicalObject::GetPos() const
{
	return XMLoadFloat3(&mPos);
}

FXMVECTOR GraphicalObject::GetLook() const
{
	return XMLoadFloat3(&mLook);
}

FXMVECTOR GraphicalObject::GetUp() const
{
	return XMLoadFloat3(&mUp);
}

FXMVECTOR GraphicalObject::GetRight() const
{
	return XMLoadFloat3(&mRight);
}

void GraphicalObject::Pitch(float angle)
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

	mWorldUpdated = true;
}

void GraphicalObject::YawLocal(float angle)
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

	mWorldUpdated = true;
}

void GraphicalObject::YawGlobal(float angle)
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

	mWorldUpdated = true;
}

void GraphicalObject::Roll(float angle)
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

	mWorldUpdated = true;
}

void GraphicalObject::MoveLook(float amt)
{
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR pos = XMLoadFloat3(&mPos);
	pos += look * amt;

	XMStoreFloat3(&mPos, pos);
	mWorldUpdated = true;
}

void GraphicalObject::MoveStrafe(float amt)
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR pos = XMLoadFloat3(&mPos);
	pos += right * amt; 

	XMStoreFloat3(&mPos, pos);
	mWorldUpdated = true;
}

void GraphicalObject::Update()
{
	if(mWorldUpdated)
	{
		mWorldUpdated = false;
		
		XMVECTOR look = XMLoadFloat3(&mLook);
		XMVECTOR pos = XMLoadFloat3(&mPos);
		pos.m128_f32[3] = 1.0f;
		XMVECTOR up = XMLoadFloat3(&mUp);
		XMVECTOR right = XMLoadFloat3(&mRight);
 
		XMStoreFloat4x4(&mWorld, XMMATRIX(right, up, look, pos));
	}
}

void GraphicalObject::Draw(ID3D11DeviceContext* context, CXMMATRIX vp)
{
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX invTrn = MathHelper::InverseTranspose(world);

	mModelMesh->Draw(context, world, invTrn, vp);
}