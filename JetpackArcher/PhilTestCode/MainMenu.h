#pragma once

#include "d3dApp.h"
#include "Sprite.h"
#include "BaseCamera.h"

class MainMenu
{
public:
	MainMenu();
	~MainMenu();

	bool Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH);

	void DrawScene(ID3D11DeviceContext* context);

	

private:
	D3DApp* theApp;
	Sprite* mBG;
	LitTexEffect* mLitTexEffect;
	BaseCamera* m2DCam;
	XMFLOAT4 mAmbientColour;
	XMFLOAT4X4 mProj;

	PointLightOptimized mPointLight;
	SpotLightOptimized mSpotLight;
};

