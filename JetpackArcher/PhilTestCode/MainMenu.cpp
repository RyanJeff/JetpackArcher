#include "MainMenu.h"
#include "Sprite.h"
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include <iostream>
#include "Vertex.h"
#include "BaseCamera.h"


MainMenu::MainMenu() : mBG(0)
{
	
}


MainMenu::~MainMenu()
{
}

bool MainMenu::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
	if (!theApp->Init())
		return false;

	Sprite::Frame* BGFrame = new Sprite::Frame();
	ID3D11ShaderResourceView* BGimage;
	D3DX11CreateShaderResourceViewFromFile(device, L"Textures/MenuScreen.png", 0, 0, &BGimage, 0);
	//bg frame
	BGFrame->imageWidth = 1024;
	BGFrame->imageHeight = 768;
	BGFrame->x = 0;
	BGFrame->y = 0;
	BGFrame->image = BGimage;
	std::vector<Sprite::Frame*> bgFrame;
	bgFrame.push_back(BGFrame);

	mBG = new Sprite(XMVectorSet(clientW / 2.0f, clientH / 2.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
		1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, device);
	mAmbientColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	return true;
}

void MainMenu::DrawScene(ID3D11DeviceContext* context)
{
	//Draw Background
	
	XMVECTOR ambient = XMLoadFloat4(&mAmbientColour);

	XMVECTOR eyePos = XMVectorSet(m2DCam->GetPos().m128_f32[0], m2DCam->GetPos().m128_f32[1], m2DCam->GetPos().m128_f32[2], 0.0f);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX view = m2DCam->GetView();

	mLitTexEffect->SetPerFrameParams(ambient, eyePos, mPointLight, mSpotLight);

	XMMATRIX vp = view * proj;
	mBG->Draw(vp, context, mLitTexEffect);
}