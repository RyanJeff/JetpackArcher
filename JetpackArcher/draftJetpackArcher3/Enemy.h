#pragma once
#include "Sprite.h"

class Enemy : public Sprite
{
private:
	

public:
	Enemy()
	{}

	Enemy(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, const std::vector<Frame*>& frames,
		float frameRate, ID3D11Device* device) :
		Sprite(pos2D, scale2D, frameWidth, frameHeight, depth, frames, frameRate, device)
	{}

	//~Enemy();

	void Chase();
};