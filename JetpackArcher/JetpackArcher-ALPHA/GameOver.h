#pragma once

#include "Sprite.h"
#include "BoundingBoxes.h"
class JetpackArcher;

class GameOver
{
public:
	GameOver();
	~GameOver();

	void Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH);
	void DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect);
	void UpdateScene(float dt);
	void InitBoundingBoxes();
	void CheckClick(POINT mousePos, JetpackArcher* instance);

private:
	Sprite* mBG;
	BoundingBoxes::BoundingBox tryAgainBB;
	BoundingBoxes::BoundingBox quitBB;
};

