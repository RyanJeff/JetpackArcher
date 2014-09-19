#pragma once
#include "Sprite.h"

class Player :
	public Sprite
{
private:
	const float JUMP_FORCE;

public:
	Player(void) : JUMP_FORCE(20.0f) {}

	Player(FXMVECTOR pos, float speed = 0.0f, float health = 0.0f) : 
		JUMP_FORCE(20.0f),
		mSpeed(0.0f),
		mHealth(0.0f),
		//mVelocity(0.0f, 0.0f, 0.0f),
		mGrounded(true)
	{
	}

	~Player(void);

	void Jump();

	float GetSpeed() const
	{
		return mSpeed;
	}

protected:
	float mSpeed;
	float mHealth;
	//XMFLOAT3 mVelocity;
	bool mGrounded;
};

