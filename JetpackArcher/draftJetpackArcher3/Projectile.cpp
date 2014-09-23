#include "Projectile.h"

//Projectile::Projectile() : MAX_DISTANCE(2000.0f)
//{
//}

Projectile::~Projectile()
{
}

void Projectile::AddForce(FXMVECTOR force)
{
	XMVECTOR vel = XMLoadFloat3(&mVelocity);
	vel = vel + force;
	XMStoreFloat3(&mVelocity, vel);
}

void Projectile::Update(float dt)
{
	XMVECTOR pos = XMLoadFloat2(&mPos);
	XMVECTOR vel = XMLoadFloat3(&mVelocity);
	
	mDistanceTravelled += XMVector3Length(vel).m128_f32[0] * dt;

	pos = pos + (vel * dt);

	XMStoreFloat2(&mPos, pos);
	XMStoreFloat3(&mVelocity, vel);
	Sprite::Update(dt);
}