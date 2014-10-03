#pragma once

#include "Sprite.h"

class MainMenu
{
public:
	MainMenu();
	~MainMenu();

	void Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH);

	void DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect);
	void UpdateScene(float dt);

	

private:
	Sprite* mBG;

};

