#include "Player.h"

Player::~Player()
{
}

void Player::Jump()
{
	if (mGrounded)
	{
		//XMVECTOR vel = XMLoadFloat3(&mVelocity);
		//vel = XMVectorSet(vel.m128_f32[0], vel.m128_f32[1] + JUMP_FORCE, vel.m128_f32[2], 0.0f);
		//XMStoreFloat3(&mVelocity, vel);
		mGrounded = false;
	}
}