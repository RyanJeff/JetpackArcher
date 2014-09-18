#pragma once

#include "GraphicalObject.h"

class Projectile : public GraphicalObject
{
protected:
	XMFLOAT3 mVelocity;
	float mDamage;
	float mDistanceTravelled;

public:
	Projectile();
	Projectile(FXMVECTOR velocity, FXMVECTOR pos, FXMVECTOR look, CXMVECTOR up, float damage, BasicModel& model)
		: GraphicalObject(pos, look, up, model),
		mDamage(damage),
		MAX_DISTANCE(200.0f),
		mDistanceTravelled(0.0f)
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

