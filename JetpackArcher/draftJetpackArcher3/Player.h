#pragma once
#include "Sprite.h"

class Player : public Sprite
{
private:
	const float JUMP_FORCE;

public:
	Player(void) : JUMP_FORCE(560.0f)
	{}

	Player(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, const std::vector<Frame*>& frames,
		float frameRate, ID3D11Device* device) :
			Sprite(pos2D, scale2D, frameWidth, frameHeight, depth, frames, frameRate, device),
			JUMP_FORCE(560.0f)
	{
	}

	~Player(void);

	void Jump();

	void UseJetpack(float dt);
};