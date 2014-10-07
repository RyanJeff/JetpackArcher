#pragma once
#include "Sprite.h"

class JetpackArcher;
class GameWon
{
public:
	GameWon();
	~GameWon();

	void Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH);
	void DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect);
	void UpdateScene(float dt);

private:
	Sprite* mBG;
};

