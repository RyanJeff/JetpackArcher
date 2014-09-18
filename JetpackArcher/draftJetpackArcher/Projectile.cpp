#include "Projectile.h"


Projectile::Projectile() : MAX_DISTANCE(2000.0f)
{


}

Projectile::~Projectile()
{
	mModelMesh = NULL;
}

void Projectile::AddForce(FXMVECTOR force)
{
	XMVECTOR vel = XMLoadFloat3(&mVelocity);

	vel = vel + force;

	XMStoreFloat3(&mVelocity, vel);
}

void Projectile::Update(float dt)
{
	XMVECTOR pos = XMLoadFloat3(&mPos);
	XMVECTOR vel = XMLoadFloat3(&mVelocity);

	mDistanceTravelled += XMVector3Length(vel).m128_f32[0] * dt;

	pos = pos + (vel * dt);
	mWorldUpdated = true;

	XMStoreFloat3(&mPos, pos);
	XMStoreFloat3(&mVelocity, vel);
	GraphicalObject::Update();
}

