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

float timer = 0.0f;
void Player::UseJetpack(float dt)
{
	XMVECTOR vel = XMLoadFloat3(&mVelocity);
	timer += dt;
	if (timer < 3.0f)
	{
		vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JETPACK_FORCE, 0.0f, 0.0f);
		XMStoreFloat3(&mVelocity, vel);
	}
	else
	{
		//if (vel.m128_f32[1] > 200.0f)
		//{
			vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] - JP_GRAVITY, 0.0f, 0.0f);
			XMStoreFloat3(&mVelocity, vel);
			timer = 0.0f;
		//}

	}
}

//float Player::GetRechargeTime()
//{
//	return jetpackRecharge;
//}
//
//float Player::GetUsage()
//{
//	return jetpackUsage;
//}
//
//void Player::RechargeJetpack(float dt)
//{
//	if (jetpackRecharge == 0.0f)
//	{
//		return;
//	}
//	if (jetpackRecharge >= 8.0f)
//	{
//		jetpackRecharge = 0.0f;
//		return;
//	}
//	jetpackRecharge += dt;
//}