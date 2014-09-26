#pragma once
#include "Sprite.h"
#include "BoundingBoxes.h"

class Enemy : public Sprite
{
public:
	BoundingBoxes::BoundingBox bbox;

public:
	Enemy(FXMVECTOR pos2D, FXMVECTOR scale2D, uint16_t frameWidth, uint16_t frameHeight, float depth, const std::vector<Frame*>& frames,
		float frameRate, ID3D11Device* device) :
		Sprite(pos2D, scale2D, frameWidth, frameHeight, depth, frames, frameRate, device),
		bbox()
	{}

	void Chase(std::vector<Enemy*>& chaser, Sprite* target, float dt);

	void Update(float dt);
};