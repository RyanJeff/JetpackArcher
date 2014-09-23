#pragma once
#include "Sprite.h"

class Projectile : public Sprite
{
protected:
	XMFLOAT3 mVelocity;
	float mDamage;
	float mDistanceTravelled;

public:
	//Projectile();

	Projectile(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, const std::vector<Frame*>& frames,
		float frameRate, ID3D11Device* device, XMVECTOR velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)) :
			Sprite(pos2D, scale2D, frameWidth, frameHeight, depth, frames, frameRate, device),
			MAX_DISTANCE(1000.0f),
			mDistanceTravelled(0.0f),
			mDamage(1.0f),
			mVelocity(0.0f, 0.0f, 0.0f)
	{
		XMStoreFloat3(&mVelocity, velocity);
	}

	virtual ~Projectile();

	void AddForce(FXMVECTOR force);

	virtual void Update(float dt);

	float GetDamage()
	{
		return mDamage;
	}

	float GetDistanceTravelled()
	{
		return mDistanceTravelled;
	}

	const float MAX_DISTANCE;
};