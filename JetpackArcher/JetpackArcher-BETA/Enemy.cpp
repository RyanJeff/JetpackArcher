#include "Enemy.h"

void Enemy::Chase(std::vector<Enemy*>& chaser, Sprite* target, float dt)
{
	for (int i = 0; i < chaser.size(); ++i)
	{
		XMVECTOR toPlayer = XMVectorSet(target->GetPos().m128_f32[0] - chaser[i]->GetPos().m128_f32[0], 
			target->GetPos().m128_f32[1] - chaser[i]->GetPos().m128_f32[1], 0.0f, 0.0f);

		velocity = XMVector2Normalize(toPlayer) * maxVelocity;
		
		XMVECTOR pos = XMVectorSet(chaser[i]->GetPos().m128_f32[0] + velocity.m128_f32[0] * dt, chaser[i]->GetPos().m128_f32[1] + 
			velocity.m128_f32[1] * dt, 0.0f, 0.0f);

		chaser[i]->SetPos(pos);
	}
}

void Enemy::Update(float dt)
{
	bbox.pos.x = mPos.x;
	bbox.pos.y = mPos.y;
	bbox.height = mWidth;
	bbox.width = mHeight;

	Sprite::Update(dt);
}