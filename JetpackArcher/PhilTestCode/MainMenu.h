#pragma once

#include "Sprite.h"
#include "BoundingBoxes.h"
#include "JetpackArcher.h"

class MainMenu
{
public:
	MainMenu();
	~MainMenu();

	void Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH);

	void DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect);
	void UpdateScene(float dt);
	void InitBoundingBoxes();
	void CheckClick(POINT mousePos, JetpackArcher* instance);

private:
	Sprite* mBG;

	BoundingBoxes::BoundingBox playBB;
	BoundingBoxes::BoundingBox creditsBB;

};

