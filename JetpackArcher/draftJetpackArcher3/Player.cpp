#include "Player.h"

Player::~Player(void)
{
}

void Player::Jump()
{
	if(mGrounded)
	{
		XMVECTOR vel = XMLoadFloat3(&mVelocity);
		vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JUMP_FORCE, 0.0f, 0.0f);
		XMStoreFloat3(&mVelocity, vel);
		mGrounded = false;
	}
}

void Player::UseJetpack(float dt)
{
	//Check to see if jetpack is recharged and resets the usage timer.
	if (jetpackRecharge == 0.0f)
	{
		jetpackUsage = 0.0f;
	}
	if (jetpackUsage <= 3.0f)
	{
		jetpackRecharge += 1.0f;
		while (jetpackUsage < 0.9f)
		{
			jetpackUsage += dt;
			XMVECTOR vel = XMLoadFloat3(&mVelocity);
			if (vel.m128_f32[1] < JUMP_FORCE)
			{
				vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JETPACK_FORCE, 0.0f, 0.0f);
				XMStoreFloat3(&mVelocity, vel);
			}
		}
	}
	else if (jetpackUsage > 3.0f)
	{
		XMVECTOR vel = XMLoadFloat3(&mVelocity);
		vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] - JETPACK_FORCE, 0.0f, 0.0f);
		XMStoreFloat3(&mVelocity, vel);
	}
}

float Player::GetRechargeTime()
{
	return jetpackRecharge;
}

float Player::GetUsage()
{
	return jetpackUsage;
}

void Player::RechargeJetpack(float dt)
{
	if (jetpackRecharge == 0.0f)
	{
		return;
	}
	if (jetpackRecharge >= 8.0f)
	{
		jetpackRecharge = 0.0f;
		return;
	}
	jetpackRecharge += dt;
}