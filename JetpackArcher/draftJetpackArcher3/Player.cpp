#include "Player.h"

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

//float timer = 0.0f;
//void Player::UseJetpack(float dt)
//{
//	timer++;
//	XMVECTOR vel = XMLoadFloat3(&mVelocity);
//	vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + 30, vel.m128_f32[2], 0.0f);
//	XMStoreFloat3(&mVelocity, vel);
//	mGrounded = false;
//
//	if (timer > 1000)
//	{
//		mVelocity.y = 0.0f;
//		timer = 0.0f;
//	}
//}