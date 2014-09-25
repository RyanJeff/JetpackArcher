#include "Player_phil_jetpack.h"

Player::~Player(void)
{
}

void Player::Jump()
{
	if(mGrounded)
	{
		XMVECTOR vel = XMLoadFloat3(&mVelocity);
		vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JUMP_FORCE, vel.m128_f32[2], 0.0f);
		XMStoreFloat3(&mVelocity, vel);
		mGrounded = false;
	}
}

float timer = 0.0f;
void Player::UseJetpack(float dt)
{
	//Check to see if jet pack is recharged and resets the useage timer.
	if (jetPackRecharge == 0.0f)
	{
		jetPackUseage = 0.0f;
	}
	if (jetPackUseage < 3.0f)
	{
		jetPackRecharge += 1.0f;
		while (jetPackUseage < 3.0f)
		{
			jetPackUseage += dt;
			XMVECTOR vel = XMLoadFloat3(&mVelocity);
			vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JUMP_FORCE, vel.m128_f32[2], 0.0f);
			XMStoreFloat3(&mVelocity, vel);
		}
		
	}
}

float Player::GetRechangeTime()
{
	return jetPackRecharge;
}

float Player::GetUsage()
{
	return jetPackUseage;
}

void Player::RechargeJetPack(float dt)
{
	if (jetPackRecharge == 0.0f)
	{
		return;
	}
	if (jetPackRecharge >= 8.0f)
	{
		jetPackRecharge = 0.0f;
		return;
	}
	jetPackRecharge += dt;
}