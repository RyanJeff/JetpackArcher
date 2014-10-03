#include "MainMenu.h"
#include <iostream>
#include "Vertex.h"
#include "JetpackArcher.h"

MainMenu::MainMenu() : mBG(0)
{
}


MainMenu::~MainMenu()
{
	if (mBG)
	{
		delete mBG;
	}
}

void MainMenu::Init(ID3D11Device* device, UINT16 clientW, UINT16 clientH)
{
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
		1024.0f, 768.0f, 1.0f, bgFrame, 0.25f, device, 0.0f);
	
}

void MainMenu::DrawScene(CXMMATRIX vp, ID3D11DeviceContext* context, LitTexEffect* texEffect)
{
	mBG->Draw(vp, context, texEffect);
}

void MainMenu::UpdateScene(float dt)
{
	mBG->Update(dt);
}